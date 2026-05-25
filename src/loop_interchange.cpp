#include "loop_interchange.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

static void lixCopyExprMeta(Expr *dst, const Expr *src) {
  if (!dst || !src) {
    return;
  }
  dst->type = src->type;
  dst->isConst = src->isConst;
  dst->constVal = src->constVal;
}

static ExprPtr lixCloneExpr(const Expr *e) {
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
      c->indices.push_back(lixCloneExpr(ix.get()));
    }
    out = std::move(c);
    break;
  }
  case ExprKind::Unary: {
    auto *u = static_cast<const UnaryExpr *>(e);
    out = make_unique<UnaryExpr>(u->line, u->op, lixCloneExpr(u->expr.get()));
    break;
  }
  case ExprKind::Binary: {
    auto *b = static_cast<const BinaryExpr *>(e);
    out = make_unique<BinaryExpr>(b->line, b->op, lixCloneExpr(b->lhs.get()),
                                    lixCloneExpr(b->rhs.get()));
    break;
  }
  case ExprKind::Call: {
    auto *c = static_cast<const CallExpr *>(e);
    auto n = make_unique<CallExpr>(c->line, c->name);
    n->function = c->function;
    for (const auto &a : c->args) {
      n->args.push_back(lixCloneExpr(a.get()));
    }
    out = std::move(n);
    break;
  }
  default:
    return nullptr;
  }
  lixCopyExprMeta(out.get(), e);
  return out;
}

static unique_ptr<LValExpr> lixCloneLValExpr(const LValExpr *lv) {
  if (!lv) {
    return nullptr;
  }
  return unique_ptr<LValExpr>(static_cast<LValExpr *>(lixCloneExpr(lv).release()));
}

static unique_ptr<AssignStmt> lixCloneAssign(const AssignStmt *as) {
  if (!as || !as->lhs) {
    return nullptr;
  }
  return make_unique<AssignStmt>(as->line, lixCloneLValExpr(as->lhs.get()),
                                 lixCloneExpr(as->rhs.get()));
}

static bool lixCmpRelOp(const string &o) {
  return o == "<" || o == "<=" || o == ">" || o == ">=";
}

// 循环交换仅对「矩形」嵌套安全：任一层界不能依赖另一层的归纳变量（例如 j < i 时禁止交换）。
static bool lixExprUsesVarName(const Expr *e, const string &name) {
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
      if (lixExprUsesVarName(ix.get(), name)) {
        return true;
      }
    }
    return false;
  }
  case ExprKind::Unary:
    return lixExprUsesVarName(static_cast<const UnaryExpr *>(e)->expr.get(), name);
  case ExprKind::Binary: {
    auto *b = static_cast<const BinaryExpr *>(e);
    return lixExprUsesVarName(b->lhs.get(), name) ||
           lixExprUsesVarName(b->rhs.get(), name);
  }
  case ExprKind::Call: {
    auto *c = static_cast<const CallExpr *>(e);
    for (const auto &a : c->args) {
      if (lixExprUsesVarName(a.get(), name)) {
        return true;
      }
    }
    return false;
  }
  default:
    return false;
  }
}

static bool lixExtractLoopIv(const WhileStmt *w, string *iv) {
  auto *b = dynamic_cast<const BinaryExpr *>(w->cond.get());
  if (!b || !lixCmpRelOp(b->op)) {
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

static bool lixIsZeroAssignTo(const AssignStmt *as, const string &v) {
  if (!as || !as->lhs || as->lhs->name != v || !as->lhs->indices.empty()) {
    return false;
  }
  auto *num = dynamic_cast<const NumberExpr *>(as->rhs.get());
  return num && !num->isFloat && num->intVal == 0;
}

static bool lixParseZeroInit(const Stmt *s, string *name) {
  if (auto *as = dynamic_cast<const AssignStmt *>(s)) {
    if (as->lhs && as->lhs->indices.empty() &&
        lixIsZeroAssignTo(as, as->lhs->name)) {
      *name = as->lhs->name;
      return true;
    }
  }
  if (auto *d = dynamic_cast<const DeclStmt *>(s)) {
    if (d->defs.size() == 1 && d->defs[0].init && d->defs[0].init->expr) {
      auto *n = dynamic_cast<const NumberExpr *>(d->defs[0].init->expr.get());
      if (n && !n->isFloat && n->intVal == 0) {
        *name = d->defs[0].name;
        return true;
      }
    }
  }
  return false;
}

static unique_ptr<AssignStmt> lixMakeZeroAssign(int line, const string &v) {
  auto lhs = make_unique<LValExpr>(line, v);
  return make_unique<AssignStmt>(line, std::move(lhs),
                                 make_unique<NumberExpr>(line, 0));
}

static unique_ptr<AssignStmt> lixCloneZeroInitAsAssign(const Stmt *s) {
  if (auto *as = dynamic_cast<const AssignStmt *>(s)) {
    return lixCloneAssign(as);
  }
  string name;
  if (auto *d = dynamic_cast<const DeclStmt *>(s)) {
    if (lixParseZeroInit(d, &name)) {
      return lixMakeZeroAssign(d->line, name);
    }
  }
  return nullptr;
}

static bool lixExtractLtBound(const Expr *cond, string *iv, ExprPtr *limit) {
  auto *b = dynamic_cast<const BinaryExpr *>(cond);
  if (!b || b->op != "<") {
    return false;
  }
  if (auto *lv = dynamic_cast<const LValExpr *>(b->lhs.get())) {
    if (!lv->indices.empty()) {
      return false;
    }
    *iv = lv->name;
    *limit = lixCloneExpr(b->rhs.get());
    return true;
  }
  if (auto *lv = dynamic_cast<const LValExpr *>(b->rhs.get())) {
    if (!lv->indices.empty()) {
      return false;
    }
    *iv = lv->name;
    *limit = lixCloneExpr(b->lhs.get());
    return true;
  }
  return false;
}

struct LixOuterLoopHead {
  string iv;
  ExprPtr limit;
  bool ivFromDecl = false;
};

static bool lixMatchOuterLoopHead(const Stmt *initStmt, const WhileStmt *outerW,
                               LixOuterLoopHead *out) {
  if (!initStmt || !outerW || !out) {
    return false;
  }
  if (!lixExtractLtBound(outerW->cond.get(), &out->iv, &out->limit)) {
    return false;
  }
  out->ivFromDecl = false;
  if (auto *as = dynamic_cast<const AssignStmt *>(initStmt)) {
    return lixIsZeroAssignTo(as, out->iv);
  }
  if (auto *d = dynamic_cast<const DeclStmt *>(initStmt)) {
    if (d->defs.size() != 1 || d->base != BaseType::Int) {
      return false;
    }
    string name;
    if (!lixParseZeroInit(d, &name) || name != out->iv) {
      return false;
    }
    out->ivFromDecl = true;
    return true;
  }
  return false;
}

static void lixStripDeclZeroInit(Stmt *s) {
  if (auto *d = dynamic_cast<DeclStmt *>(s)) {
    if (d->defs.size() == 1) {
      d->defs[0].init.reset();
    }
  }
}

static bool lixIsIncByOne(const AssignStmt *as, const string &v) {
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

static const LValExpr *lixAsScalarLVal(const Expr *e) {
  return dynamic_cast<const LValExpr *>(e);
}

static bool lixIsSwapTransposeAssign(const AssignStmt *as, string *symOut, string *symIn,
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
  const auto *a00 = lixAsScalarLVal(i0);
  const auto *a01 = lixAsScalarLVal(i1);
  const auto *b0 = lixAsScalarLVal(r0);
  const auto *b1 = lixAsScalarLVal(r1);
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

static bool lixBlockHasNestedWhile(const BlockStmt *blk) {
  if (!blk) {
    return false;
  }
  for (const auto &it : blk->items) {
    if (it->kind == StmtKind::While) {
      return true;
    }
    if (it->kind == StmtKind::Block &&
        lixBlockHasNestedWhile(static_cast<const BlockStmt *>(it.get()))) {
      return true;
    }
    if (it->kind == StmtKind::If) {
      auto *ifs = static_cast<const IfStmt *>(it.get());
      if (ifs->thenStmt && ifs->thenStmt->kind == StmtKind::Block &&
          lixBlockHasNestedWhile(static_cast<const BlockStmt *>(ifs->thenStmt.get()))) {
        return true;
      }
      if (ifs->elseStmt && ifs->elseStmt->kind == StmtKind::Block &&
          lixBlockHasNestedWhile(static_cast<const BlockStmt *>(ifs->elseStmt.get()))) {
        return true;
      }
    }
  }
  return false;
}

static bool lixTryInterchangeTransposePair(vector<StmtPtr> &items, size_t k) {
  if (k + 1 >= items.size()) {
    return false;
  }
  auto *outer = dynamic_cast<WhileStmt *>(items[k + 1].get());
  if (!outer) {
    return false;
  }
  string outerIv;
  string outerInitIv;
  if (!lixExtractLoopIv(outer, &outerIv) || !lixParseZeroInit(items[k].get(), &outerInitIv) ||
      outerInitIv != outerIv) {
    return false;
  }
  auto *outerBody = dynamic_cast<BlockStmt *>(outer->body.get());
  if (!outerBody || outerBody->items.size() != 3) {
    return false;
  }
  for (size_t idx = 0; idx < outerBody->items.size(); ++idx) {
    if (outerBody->items[idx]->kind == StmtKind::Decl && idx != 0) {
      return false;
    }
  }
  string innerIv;
  if (!lixParseZeroInit(outerBody->items[0].get(), &innerIv)) {
    return false;
  }
  auto *inner = dynamic_cast<WhileStmt *>(outerBody->items[1].get());
  auto *outerInc = dynamic_cast<AssignStmt *>(outerBody->items[2].get());
  if (!inner || !outerInc || !lixIsIncByOne(outerInc, outerIv)) {
    return false;
  }
  string innerIvLoop;
  if (!lixExtractLoopIv(inner, &innerIvLoop) || innerIvLoop != innerIv) {
    return false;
  }
  if (innerIv == outerIv) {
    return false;
  }

  auto *innerBody = dynamic_cast<BlockStmt *>(inner->body.get());
  if (!innerBody || innerBody->items.size() < 2) {
    return false;
  }
  auto *innerInc = dynamic_cast<AssignStmt *>(innerBody->items.back().get());
  if (!lixIsIncByOne(innerInc, innerIv)) {
    return false;
  }

  if (innerBody->items.size() != 2 || innerBody->items[0]->kind != StmtKind::Assign) {
    return false;
  }

  auto *onlyAs = static_cast<const AssignStmt *>(innerBody->items[0].get());
  string symOut, symIn, iv1, iv2;
  if (!lixIsSwapTransposeAssign(onlyAs, &symOut, &symIn, &iv1, &iv2)) {
    return false;
  }
  (void)symOut;
  (void)symIn;
  if (iv1 != outerIv || iv2 != innerIv) {
    return false;
  }

  // 外层界含内层变量、或内层界含外层变量时，交换会改变遍历顺序 → 语义错误。
  if (lixExprUsesVarName(outer->cond.get(), innerIv) ||
      lixExprUsesVarName(inner->cond.get(), outerIv)) {
    return false;
  }

  unique_ptr<AssignStmt> coreClone = lixCloneAssign(onlyAs);
  if (!coreClone) {
    return false;
  }

  int line = outer->line;
  unique_ptr<AssignStmt> newParentInit = lixCloneZeroInitAsAssign(outerBody->items[0].get());
  unique_ptr<AssignStmt> innerOuterZero = lixCloneZeroInitAsAssign(items[k].get());
  unique_ptr<AssignStmt> incOuterIv = lixCloneAssign(outerInc);
  unique_ptr<AssignStmt> incInnerIv = lixCloneAssign(innerInc);
  if (!newParentInit || !innerOuterZero || !incOuterIv || !incInnerIv) {
    return false;
  }

  auto newOuterWhile = make_unique<WhileStmt>(line, lixCloneExpr(inner->cond.get()), nullptr);
  auto newOuterBody = make_unique<BlockStmt>(line);
  newOuterBody->items.push_back(std::move(innerOuterZero));
  auto newInnerWhile = make_unique<WhileStmt>(line, lixCloneExpr(outer->cond.get()), nullptr);
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

static unique_ptr<LValExpr> lixMakeArray2DLVal(int line, const string &arr,
                                            const string &iv0, const string &iv1) {
  auto lv = make_unique<LValExpr>(line, arr);
  lv->indices.push_back(make_unique<LValExpr>(line, iv0));
  lv->indices.push_back(make_unique<LValExpr>(line, iv1));
  return lv;
}

static bool lixIndexIsIv(const Expr *e, const string &iv) {
  auto *lv = dynamic_cast<const LValExpr *>(e);
  return lv && lv->name == iv && lv->indices.empty();
}

static bool lixMatchCik(const LValExpr *lv, const string &iIv, const string &kIv,
                     string *sym) {
  if (!lv || lv->indices.size() != 2) {
    return false;
  }
  if (!lixIndexIsIv(lv->indices[0].get(), iIv) ||
      !lixIndexIsIv(lv->indices[1].get(), kIv)) {
    return false;
  }
  *sym = lv->name;
  return true;
}

static bool lixMatchAkj(const LValExpr *lv, const string &kIv, const string &jIv,
                     string *sym) {
  if (!lv || lv->indices.size() != 2) {
    return false;
  }
  if (!lixIndexIsIv(lv->indices[0].get(), kIv) ||
      !lixIndexIsIv(lv->indices[1].get(), jIv)) {
    return false;
  }
  *sym = lv->name;
  return true;
}

static bool lixMatchAij(const LValExpr *lv, const string &iIv, const string &jIv,
                     string *sym) {
  if (!lv || lv->indices.size() != 2) {
    return false;
  }
  if (!lixIndexIsIv(lv->indices[0].get(), iIv) ||
      !lixIndexIsIv(lv->indices[1].get(), jIv)) {
    return false;
  }
  *sym = lv->name;
  return true;
}

// sum = sum + C[i][k] * A[k][j]（many_mat_cal 内层 k 循环）
static bool lixMatchGemmSumAccum(const AssignStmt *as, const string &sumIv,
                              const string &iIv, const string &jIv,
                              const string &kIv, string *cSym, string *aSym) {
  if (!as || !as->lhs || as->lhs->name != sumIv || !as->lhs->indices.empty()) {
    return false;
  }
  auto *add = dynamic_cast<const BinaryExpr *>(as->rhs.get());
  if (!add || add->op != "+") {
    return false;
  }
  auto *sumL = dynamic_cast<const LValExpr *>(add->lhs.get());
  if (!sumL || sumL->name != sumIv || !sumL->indices.empty()) {
    return false;
  }
  auto *mul = dynamic_cast<const BinaryExpr *>(add->rhs.get());
  if (!mul || mul->op != "*") {
    return false;
  }
  auto *l0 = dynamic_cast<const LValExpr *>(mul->lhs.get());
  auto *l1 = dynamic_cast<const LValExpr *>(mul->rhs.get());
  if (!l0 || !l1) {
    return false;
  }
  string c0, a0;
  if (lixMatchCik(l0, iIv, kIv, &c0) && lixMatchAkj(l1, kIv, jIv, &a0)) {
    *cSym = c0;
    *aSym = a0;
    return true;
  }
  if (lixMatchCik(l1, iIv, kIv, &c0) && lixMatchAkj(l0, kIv, jIv, &a0)) {
    *cSym = c0;
    *aSym = a0;
    return true;
  }
  return false;
}

// i-j-k 点积 → i-k-j：每行先清零 A[i][*]，再对 k 扫 j 累加（A[k][j] 行优先）
static bool lixTryInterchangeGemmIjk(vector<StmtPtr> &items, size_t k) {
  (void)items;
  (void)k;
  // i-k-j 交换在 k==i 项上须保留原 A[i][j]；当前实现仍 WA（many_mat_cal），先关闭保正确性。
  return false;
#if 0
  if (k + 1 >= items.size()) {
    return false;
  }
  auto *outerW = dynamic_cast<WhileStmt *>(items[k + 1].get());
  LixOuterLoopHead head;
  if (!lixMatchOuterLoopHead(items[k].get(), outerW, &head)) {
    return false;
  }
  const string &iIv = head.iv;
  const ExprPtr &iLimit = head.limit;
  auto *outerBody = dynamic_cast<BlockStmt *>(outerW->body.get());
  if (!outerBody || outerBody->items.size() != 3) {
    return false;
  }
  string jIv;
  if (!lixParseZeroInit(outerBody->items[0].get(), &jIv)) {
    return false;
  }
  auto *midW = dynamic_cast<WhileStmt *>(outerBody->items[1].get());
  auto *outerInc = dynamic_cast<AssignStmt *>(outerBody->items[2].get());
  if (!midW || !lixIsIncByOne(outerInc, iIv)) {
    return false;
  }
  string jIv2;
  ExprPtr jLimit;
  if (!lixExtractLtBound(midW->cond.get(), &jIv2, &jLimit) || jIv2 != jIv) {
    return false;
  }
  auto *midBody = dynamic_cast<BlockStmt *>(midW->body.get());
  if (!midBody || midBody->items.size() != 5) {
    return false;
  }
  string kIv;
  if (!lixParseZeroInit(midBody->items[0].get(), &kIv)) {
    return false;
  }
  string sumIv;
  if (!lixParseZeroInit(midBody->items[1].get(), &sumIv)) {
    return false;
  }
  auto *innerW = dynamic_cast<WhileStmt *>(midBody->items[2].get());
  if (!innerW) {
    return false;
  }
  string kIv2;
  ExprPtr kLimit;
  if (!lixExtractLtBound(innerW->cond.get(), &kIv2, &kLimit) || kIv2 != kIv) {
    return false;
  }
  auto *innerBody = dynamic_cast<BlockStmt *>(innerW->body.get());
  if (!innerBody || innerBody->items.size() != 2 ||
      innerBody->items[0]->kind != StmtKind::Assign ||
      !lixIsIncByOne(dynamic_cast<const AssignStmt *>(innerBody->items[1].get()), kIv)) {
    return false;
  }
  auto *accumAs = dynamic_cast<const AssignStmt *>(innerBody->items[0].get());
  string cSym, aSym;
  if (!lixMatchGemmSumAccum(accumAs, sumIv, iIv, jIv, kIv, &cSym, &aSym)) {
    return false;
  }
  auto *storeAs = dynamic_cast<const AssignStmt *>(midBody->items[3].get());
  string aOut;
  if (!storeAs || !storeAs->lhs || storeAs->lhs->indices.size() != 2) {
    return false;
  }
  if (!lixMatchAij(storeAs->lhs.get(), iIv, jIv, &aOut) || aOut != aSym) {
    return false;
  }
  auto *sumRhs = dynamic_cast<const LValExpr *>(storeAs->rhs.get());
  if (!sumRhs || sumRhs->name != sumIv || !sumRhs->indices.empty()) {
    return false;
  }
  auto *midInc = dynamic_cast<const AssignStmt *>(midBody->items[4].get());
  if (!lixIsIncByOne(midInc, jIv)) {
    return false;
  }
  if (lixExprUsesVarName(jLimit.get(), iIv) || lixExprUsesVarName(kLimit.get(), iIv) ||
      lixExprUsesVarName(kLimit.get(), jIv) || lixExprUsesVarName(iLimit.get(), jIv)) {
    return false;
  }

  const int line = outerW->line;
  // k==i 项须用改写前的 A[i][j]：A[i][j] = C[i][i] * A[i][j]（不能先清零再累加）
  auto diagBody = make_unique<BlockStmt>(line);
  {
    auto alhs = lixMakeArray2DLVal(line, aSym, iIv, jIv);
    auto cii = lixMakeArray2DLVal(line, cSym, iIv, iIv);
    auto arhs = lixMakeArray2DLVal(line, aSym, iIv, jIv);
    auto mul = make_unique<BinaryExpr>(line, "*", std::move(cii), std::move(arhs));
    diagBody->items.push_back(
        make_unique<AssignStmt>(line, std::move(alhs), std::move(mul)));
    diagBody->items.push_back(lixCloneAssign(midInc));
  }
  auto diagWhile = make_unique<WhileStmt>(line, lixCloneExpr(midW->cond.get()), nullptr);
  diagWhile->body = std::move(diagBody);

  // k!=i：A[i][j] = A[i][j] + C[i][k] * A[k][j]
  auto accBody = make_unique<BlockStmt>(line);
  {
    auto alhs = lixMakeArray2DLVal(line, aSym, iIv, jIv);
    auto arhs = lixMakeArray2DLVal(line, aSym, iIv, jIv);
    auto cik = lixMakeArray2DLVal(line, cSym, iIv, kIv);
    auto akj = lixMakeArray2DLVal(line, aSym, kIv, jIv);
    auto mul = make_unique<BinaryExpr>(line, "*", std::move(cik), std::move(akj));
    auto add = make_unique<BinaryExpr>(line, "+", std::move(arhs), std::move(mul));
    accBody->items.push_back(
        make_unique<AssignStmt>(line, std::move(alhs), std::move(add)));
    accBody->items.push_back(lixCloneAssign(midInc));
  }
  auto accWhile = make_unique<WhileStmt>(line, lixCloneExpr(midW->cond.get()), nullptr);
  accWhile->body = std::move(accBody);

  auto kNeiBody = make_unique<BlockStmt>(line);
  if (StmtPtr jInit = lixCloneZeroInitAsAssign(outerBody->items[0].get())) {
    kNeiBody->items.push_back(std::move(jInit));
  } else {
    return false;
  }
  kNeiBody->items.push_back(std::move(accWhile));
  auto kNe = make_unique<BinaryExpr>(line, "!=",
                                     make_unique<LValExpr>(line, kIv),
                                     make_unique<LValExpr>(line, iIv));
  auto kSkipI = make_unique<IfStmt>(line, std::move(kNe), std::move(kNeiBody), nullptr);

  auto kBody = make_unique<BlockStmt>(line);
  kBody->items.push_back(std::move(kSkipI));
  if (unique_ptr<AssignStmt> kInc = lixCloneAssign(
          dynamic_cast<const AssignStmt *>(innerBody->items[1].get()))) {
    kBody->items.push_back(std::move(kInc));
  } else {
    return false;
  }
  auto kWhile = make_unique<WhileStmt>(line, lixCloneExpr(innerW->cond.get()), nullptr);
  kWhile->body = std::move(kBody);

  auto newOuterBody = make_unique<BlockStmt>(line);
  if (auto *jd = dynamic_cast<const DeclStmt *>(outerBody->items[0].get())) {
    auto nd = make_unique<DeclStmt>(jd->line, jd->isConst, jd->base);
    VarDef vj;
    vj.name = jIv;
    vj.line = jd->line;
    nd->defs.push_back(std::move(vj));
    newOuterBody->items.push_back(std::move(nd));
  } else if (StmtPtr jz = lixCloneZeroInitAsAssign(outerBody->items[0].get())) {
    newOuterBody->items.push_back(std::move(jz));
  } else {
    return false;
  }
  newOuterBody->items.push_back(std::move(diagWhile));
  if (dynamic_cast<const DeclStmt *>(midBody->items[0].get()) != nullptr) {
    auto kd = make_unique<DeclStmt>(line, false, BaseType::Int);
    VarDef vd;
    vd.name = kIv;
    vd.line = line;
    kd->defs.push_back(std::move(vd));
    newOuterBody->items.push_back(std::move(kd));
  }
  if (StmtPtr kZero = lixCloneZeroInitAsAssign(midBody->items[0].get())) {
    newOuterBody->items.push_back(std::move(kZero));
  } else {
    return false;
  }
  newOuterBody->items.push_back(std::move(kWhile));
  newOuterBody->items.push_back(lixCloneAssign(outerInc));

  auto newOuterWhile =
      make_unique<WhileStmt>(line, lixCloneExpr(outerW->cond.get()), nullptr);
  newOuterWhile->body = std::move(newOuterBody);

  if (head.ivFromDecl) {
    lixStripDeclZeroInit(items[k].get());
    items.erase(items.begin() + static_cast<ptrdiff_t>(k + 1),
                items.begin() + static_cast<ptrdiff_t>(k + 2));
    items.insert(items.begin() + static_cast<ptrdiff_t>(k + 1),
                 std::move(newOuterWhile));
  } else {
    items.erase(items.begin() + static_cast<ptrdiff_t>(k),
                items.begin() + static_cast<ptrdiff_t>(k + 2));
    items.insert(items.begin() + static_cast<ptrdiff_t>(k),
                 std::move(newOuterWhile));
  }
  return true;
#endif
}

static void lixProcessBlock(BlockStmt *blk) {
  if (!blk) {
    return;
  }
  for (auto &it : blk->items) {
    if (it->kind == StmtKind::Block) {
      lixProcessBlock(static_cast<BlockStmt *>(it.get()));
    } else if (it->kind == StmtKind::While) {
      lixProcessBlock(dynamic_cast<BlockStmt *>(static_cast<WhileStmt *>(it.get())->body.get()));
    } else if (it->kind == StmtKind::If) {
      auto *ifs = static_cast<IfStmt *>(it.get());
      if (ifs->thenStmt && ifs->thenStmt->kind == StmtKind::Block) {
        lixProcessBlock(static_cast<BlockStmt *>(ifs->thenStmt.get()));
      }
      if (ifs->elseStmt && ifs->elseStmt->kind == StmtKind::Block) {
        lixProcessBlock(static_cast<BlockStmt *>(ifs->elseStmt.get()));
      }
    }
  }
  for (size_t i = 0; i + 1 < blk->items.size();) {
    if (lixTryInterchangeGemmIjk(blk->items, i)) {
      ++i;
      continue;
    }
    if (lixTryInterchangeTransposePair(blk->items, i)) {
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
    lixProcessBlock(item.func->body.get());
  }
  loopTilingPass(program);
}
