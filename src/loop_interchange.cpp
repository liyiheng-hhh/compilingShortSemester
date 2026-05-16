#include "loop_interchange.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

static void copyExprMeta(Expr *dst, const Expr *src) {
  if (!dst || !src) {
    return;
  }
  dst->type = src->type;
  dst->isConst = src->isConst;
  dst->constVal = src->constVal;
}

static ExprPtr cloneExpr(const Expr *e) {
  if (!e) {
    return nullptr;
  }
  ExprPtr out;
  switch (e->kind) {
  case ExprKind::Number: {
    auto *n = static_cast<const NumberExpr *>(e);
    if (n->isFloat) {
      out = make_unique<NumberExpr>(n->line, n->floatVal);
    } else {
      out = make_unique<NumberExpr>(n->line, n->intVal);
    }
    break;
  }
  case ExprKind::String: {
    auto *s = static_cast<const StringExpr *>(e);
    out = make_unique<StringExpr>(s->line, s->value);
    break;
  }
  case ExprKind::LVal: {
    auto *lv = static_cast<const LValExpr *>(e);
    auto c = make_unique<LValExpr>(lv->line, lv->name);
    c->symbol = lv->symbol;
    for (const auto &ix : lv->indices) {
      c->indices.push_back(cloneExpr(ix.get()));
    }
    out = std::move(c);
    break;
  }
  case ExprKind::Unary: {
    auto *u = static_cast<const UnaryExpr *>(e);
    out = make_unique<UnaryExpr>(u->line, u->op, cloneExpr(u->expr.get()));
    break;
  }
  case ExprKind::Binary: {
    auto *b = static_cast<const BinaryExpr *>(e);
    out = make_unique<BinaryExpr>(b->line, b->op, cloneExpr(b->lhs.get()),
                                    cloneExpr(b->rhs.get()));
    break;
  }
  case ExprKind::Call: {
    auto *c = static_cast<const CallExpr *>(e);
    auto n = make_unique<CallExpr>(c->line, c->name);
    n->function = c->function;
    for (const auto &a : c->args) {
      n->args.push_back(cloneExpr(a.get()));
    }
    out = std::move(n);
    break;
  }
  default:
    return nullptr;
  }
  copyExprMeta(out.get(), e);
  return out;
}

static unique_ptr<LValExpr> cloneLValExpr(const LValExpr *lv) {
  if (!lv) {
    return nullptr;
  }
  return unique_ptr<LValExpr>(static_cast<LValExpr *>(cloneExpr(lv).release()));
}

static unique_ptr<AssignStmt> cloneAssign(const AssignStmt *as) {
  if (!as || !as->lhs) {
    return nullptr;
  }
  return make_unique<AssignStmt>(as->line, cloneLValExpr(as->lhs.get()),
                                 cloneExpr(as->rhs.get()));
}

static bool cmpRelOp(const string &o) {
  return o == "<" || o == "<=" || o == ">" || o == ">=";
}

// 循环交换仅对「矩形」嵌套安全：任一层界不能依赖另一层的归纳变量（例如 j < i 时禁止交换）。
static bool exprUsesVarName(const Expr *e, const string &name) {
  if (!e) {
    return false;
  }
  switch (e->kind) {
  case ExprKind::LVal: {
    auto *lv = static_cast<const LValExpr *>(e);
    if (lv->name == name) {
      return true;
    }
    for (const auto &ix : lv->indices) {
      if (exprUsesVarName(ix.get(), name)) {
        return true;
      }
    }
    return false;
  }
  case ExprKind::Unary:
    return exprUsesVarName(static_cast<const UnaryExpr *>(e)->expr.get(), name);
  case ExprKind::Binary: {
    auto *b = static_cast<const BinaryExpr *>(e);
    return exprUsesVarName(b->lhs.get(), name) ||
           exprUsesVarName(b->rhs.get(), name);
  }
  case ExprKind::Call: {
    auto *c = static_cast<const CallExpr *>(e);
    for (const auto &a : c->args) {
      if (exprUsesVarName(a.get(), name)) {
        return true;
      }
    }
    return false;
  }
  default:
    return false;
  }
}

static bool extractLoopIv(const WhileStmt *w, string *iv) {
  auto *b = dynamic_cast<const BinaryExpr *>(w->cond.get());
  if (!b || !cmpRelOp(b->op)) {
    return false;
  }
  auto *ll = dynamic_cast<const LValExpr *>(b->lhs.get());
  auto *rl = dynamic_cast<const LValExpr *>(b->rhs.get());
  const bool lSc = ll && ll->indices.empty();
  const bool rSc = rl && rl->indices.empty();
  if (lSc && rSc) {
    return false;
  }
  if (lSc) {
    *iv = ll->name;
    return true;
  }
  if (rSc) {
    *iv = rl->name;
    return true;
  }
  return false;
}

static bool isZeroAssignTo(const AssignStmt *as, const string &v) {
  if (!as || !as->lhs || as->lhs->name != v || !as->lhs->indices.empty()) {
    return false;
  }
  auto *num = dynamic_cast<const NumberExpr *>(as->rhs.get());
  return num && !num->isFloat && num->intVal == 0;
}

static bool isIncByOne(const AssignStmt *as, const string &v) {
  if (!as || !as->lhs || as->lhs->name != v || !as->lhs->indices.empty()) {
    return false;
  }
  auto *bin = dynamic_cast<const BinaryExpr *>(as->rhs.get());
  if (!bin || bin->op != "+") {
    return false;
  }
  auto *l = dynamic_cast<const LValExpr *>(bin->lhs.get());
  auto *r = dynamic_cast<const NumberExpr *>(bin->rhs.get());
  return l && l->name == v && l->indices.empty() && r && !r->isFloat && r->intVal == 1;
}

static const LValExpr *asScalarLVal(const Expr *e) {
  return dynamic_cast<const LValExpr *>(e);
}

static bool isSwapTransposeAssign(const AssignStmt *as, string *symOut, string *symIn,
                                  string *iv1, string *iv2) {
  if (!as || !as->lhs || as->lhs->indices.size() != 2) {
    return false;
  }
  auto *rhsLv = dynamic_cast<const LValExpr *>(as->rhs.get());
  if (!rhsLv || rhsLv->indices.size() != 2) {
    return false;
  }
  const Expr *i0 = as->lhs->indices[0].get();
  const Expr *i1 = as->lhs->indices[1].get();
  const Expr *r0 = rhsLv->indices[0].get();
  const Expr *r1 = rhsLv->indices[1].get();
  const auto *a00 = asScalarLVal(i0);
  const auto *a01 = asScalarLVal(i1);
  const auto *b0 = asScalarLVal(r0);
  const auto *b1 = asScalarLVal(r1);
  if (!a00 || !a01 || !b0 || !b1) {
    return false;
  }
  if (a00->name == a01->name) {
    return false;
  }
  if (as->lhs->name == rhsLv->name) {
    return false;
  }
  if (a00->name != b1->name || a01->name != b0->name) {
    return false;
  }
  *symOut = as->lhs->name;
  *symIn = rhsLv->name;
  *iv1 = a00->name;
  *iv2 = a01->name;
  return true;
}

static bool blockHasNestedWhile(const BlockStmt *blk) {
  if (!blk) {
    return false;
  }
  for (const auto &it : blk->items) {
    if (it->kind == StmtKind::While) {
      return true;
    }
    if (it->kind == StmtKind::Block &&
        blockHasNestedWhile(static_cast<const BlockStmt *>(it.get()))) {
      return true;
    }
    if (it->kind == StmtKind::If) {
      auto *ifs = static_cast<const IfStmt *>(it.get());
      if (ifs->thenStmt && ifs->thenStmt->kind == StmtKind::Block &&
          blockHasNestedWhile(static_cast<const BlockStmt *>(ifs->thenStmt.get()))) {
        return true;
      }
      if (ifs->elseStmt && ifs->elseStmt->kind == StmtKind::Block &&
          blockHasNestedWhile(static_cast<const BlockStmt *>(ifs->elseStmt.get()))) {
        return true;
      }
    }
  }
  return false;
}

static bool tryInterchangeTransposePair(vector<StmtPtr> &items, size_t k) {
  if (k + 1 >= items.size()) {
    return false;
  }
  auto *init = dynamic_cast<AssignStmt *>(items[k].get());
  auto *outer = dynamic_cast<WhileStmt *>(items[k + 1].get());
  if (!init || !outer) {
    return false;
  }
  string outerIv;
  if (!extractLoopIv(outer, &outerIv)) {
    return false;
  }
  if (!isZeroAssignTo(init, outerIv)) {
    return false;
  }
  auto *outerBody = dynamic_cast<BlockStmt *>(outer->body.get());
  if (!outerBody) {
    return false;
  }
  // 不支持外层体含局部声明（避免错误搬移）
  for (const auto &it : outerBody->items) {
    if (it->kind == StmtKind::Decl) {
      return false;
    }
  }
  if (outerBody->items.size() != 3) {
    return false;
  }
  auto *innerInit = dynamic_cast<AssignStmt *>(outerBody->items[0].get());
  auto *inner = dynamic_cast<WhileStmt *>(outerBody->items[1].get());
  auto *outerInc = dynamic_cast<AssignStmt *>(outerBody->items[2].get());
  if (!innerInit || !inner || !outerInc) {
    return false;
  }
  if (!isZeroAssignTo(innerInit, innerInit->lhs->name)) {
    return false;
  }
  if (!isIncByOne(outerInc, outerIv)) {
    return false;
  }

  string innerIv;
  if (!extractLoopIv(inner, &innerIv)) {
    return false;
  }
  if (innerIv == outerIv) {
    return false;
  }
  if (innerInit->lhs->name != innerIv) {
    return false;
  }

  auto *innerBody = dynamic_cast<BlockStmt *>(inner->body.get());
  if (!innerBody || innerBody->items.size() < 2) {
    return false;
  }
  auto *innerInc = dynamic_cast<AssignStmt *>(innerBody->items.back().get());
  if (!isIncByOne(innerInc, innerIv)) {
    return false;
  }

  if (innerBody->items.size() != 2 || innerBody->items[0]->kind != StmtKind::Assign) {
    return false;
  }

  auto *onlyAs = static_cast<const AssignStmt *>(innerBody->items[0].get());
  string symOut, symIn, iv1, iv2;
  if (!isSwapTransposeAssign(onlyAs, &symOut, &symIn, &iv1, &iv2)) {
    return false;
  }
  (void)symOut;
  (void)symIn;
  if (iv1 != outerIv || iv2 != innerIv) {
    return false;
  }

  // 外层界含内层变量、或内层界含外层变量时，交换会改变遍历顺序 → 语义错误。
  if (exprUsesVarName(outer->cond.get(), innerIv) ||
      exprUsesVarName(inner->cond.get(), outerIv)) {
    return false;
  }

  unique_ptr<AssignStmt> coreClone = cloneAssign(onlyAs);
  if (!coreClone) {
    return false;
  }

  int line = outer->line;
  unique_ptr<AssignStmt> newParentInit = cloneAssign(innerInit);
  unique_ptr<AssignStmt> innerOuterZero = cloneAssign(init);
  unique_ptr<AssignStmt> incOuterIv = cloneAssign(outerInc);
  unique_ptr<AssignStmt> incInnerIv = cloneAssign(innerInc);
  if (!newParentInit || !innerOuterZero || !incOuterIv || !incInnerIv) {
    return false;
  }

  auto newOuterWhile = make_unique<WhileStmt>(line, cloneExpr(inner->cond.get()), nullptr);
  auto newOuterBody = make_unique<BlockStmt>(line);
  newOuterBody->items.push_back(std::move(innerOuterZero));
  auto newInnerWhile = make_unique<WhileStmt>(line, cloneExpr(outer->cond.get()), nullptr);
  auto newInnerBody = make_unique<BlockStmt>(innerBody->line);
  newInnerBody->items.push_back(std::move(coreClone));
  newInnerBody->items.push_back(std::move(incOuterIv));
  newInnerWhile->body = std::move(newInnerBody);
  newOuterBody->items.push_back(std::move(newInnerWhile));
  newOuterBody->items.push_back(std::move(incInnerIv));
  newOuterWhile->body = std::move(newOuterBody);

  items[k] = std::move(newParentInit);
  items[k + 1] = std::move(newOuterWhile);
  return true;
}

static void processBlock(BlockStmt *blk) {
  if (!blk) {
    return;
  }
  for (auto &it : blk->items) {
    if (it->kind == StmtKind::Block) {
      processBlock(static_cast<BlockStmt *>(it.get()));
    } else if (it->kind == StmtKind::While) {
      processBlock(dynamic_cast<BlockStmt *>(static_cast<WhileStmt *>(it.get())->body.get()));
    } else if (it->kind == StmtKind::If) {
      auto *ifs = static_cast<IfStmt *>(it.get());
      if (ifs->thenStmt && ifs->thenStmt->kind == StmtKind::Block) {
        processBlock(static_cast<BlockStmt *>(ifs->thenStmt.get()));
      }
      if (ifs->elseStmt && ifs->elseStmt->kind == StmtKind::Block) {
        processBlock(static_cast<BlockStmt *>(ifs->elseStmt.get()));
      }
    }
  }
  for (size_t i = 0; i + 1 < blk->items.size();) {
    if (tryInterchangeTransposePair(blk->items, i)) {
      ++i;
      continue;
    }
    ++i;
  }
}

void loopInterchangePass(Program &program) {
  for (auto &item : program.items) {
    if (!item.func || !item.func->body) {
      continue;
    }
    processBlock(item.func->body.get());
  }
  loopTilingPass(program);
}
