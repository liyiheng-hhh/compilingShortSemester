// 拆分 if/while 条件里的 &&，便于 conv2d 等走 IR（irFunctionEligible 拒绝含 && 的 cond）。

#include "land_lor_split.h"

#include "common.h"

#include <memory>
#include <utility>
#include <vector>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {

static void llsCopyExprMeta(Expr *dst, const Expr *src) {
  if (!dst || !src) {
    return;
  }
  dst->type = src->type;
  dst->isConst = src->isConst;
  dst->constVal = src->constVal;
}

static ExprPtr llsCloneExpr(const Expr *e) {
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
      c->indices.push_back(llsCloneExpr(ix.get()));
    }
    out = std::move(c);
    break;
  }
  case ExprKind::Unary: {
    auto *u = static_cast<const UnaryExpr *>(e);
    out = make_unique<UnaryExpr>(u->line, u->op, llsCloneExpr(u->expr.get()));
    break;
  }
  case ExprKind::Binary: {
    auto *b = static_cast<const BinaryExpr *>(e);
    out = make_unique<BinaryExpr>(b->line, b->op, llsCloneExpr(b->lhs.get()),
                                    llsCloneExpr(b->rhs.get()));
    break;
  }
  case ExprKind::Call: {
    auto *c = static_cast<const CallExpr *>(e);
    auto n = make_unique<CallExpr>(c->line, c->name);
    n->function = c->function;
    for (const auto &a : c->args) {
      n->args.push_back(llsCloneExpr(a.get()));
    }
    out = std::move(n);
    break;
  }
  default:
    return nullptr;
  }
  llsCopyExprMeta(out.get(), e);
  return out;
}

static StmtPtr llsCloneStmt(const Stmt *s);

static unique_ptr<InitVal> llsCloneInitVal(const InitVal *iv) {
  if (!iv) {
    return nullptr;
  }
  auto out = make_unique<InitVal>();
  out->isList = iv->isList;
  if (iv->isList) {
    for (const auto &c : iv->list) {
      out->list.push_back(llsCloneInitVal(c.get()));
    }
  } else {
    out->expr = llsCloneExpr(iv->expr.get());
  }
  return out;
}

static StmtPtr llsCloneStmt(const Stmt *s) {
  if (!s) {
    return nullptr;
  }
  switch (s->kind) {
  case StmtKind::Assign: {
    auto *as = static_cast<const AssignStmt *>(s);
    auto lhs = make_unique<LValExpr>(as->line, as->lhs->name);
    lhs->symbol = as->lhs->symbol;
    for (const auto &ix : as->lhs->indices) {
      lhs->indices.push_back(llsCloneExpr(ix.get()));
    }
    return make_unique<AssignStmt>(as->line, std::move(lhs), llsCloneExpr(as->rhs.get()));
  }
  case StmtKind::Expr:
    return make_unique<ExprStmt>(s->line, llsCloneExpr(static_cast<const ExprStmt *>(s)->expr.get()));
  case StmtKind::Decl: {
    auto *d = static_cast<const DeclStmt *>(s);
    auto nd = make_unique<DeclStmt>(d->line, d->isConst, d->base);
    for (const auto &def : d->defs) {
      VarDef vd;
      vd.name = def.name;
      vd.symbol = def.symbol;
      vd.line = def.line;
      for (const auto &dim : def.dims) {
        vd.dims.push_back(llsCloneExpr(dim.get()));
      }
      if (def.init) {
        vd.init = llsCloneInitVal(def.init.get());
      }
      nd->defs.push_back(std::move(vd));
    }
    return nd;
  }
  case StmtKind::Block: {
    auto *b = static_cast<const BlockStmt *>(s);
    auto nb = make_unique<BlockStmt>(b->line);
    for (const auto &it : b->items) {
      if (StmtPtr c = llsCloneStmt(it.get())) {
        nb->items.push_back(std::move(c));
      }
    }
    return nb;
  }
  case StmtKind::If: {
    auto *ifs = static_cast<const IfStmt *>(s);
    return make_unique<IfStmt>(ifs->line, llsCloneExpr(ifs->cond.get()),
                               llsCloneStmt(ifs->thenStmt.get()),
                               llsCloneStmt(ifs->elseStmt.get()));
  }
  case StmtKind::While: {
    auto *w = static_cast<const WhileStmt *>(s);
    return make_unique<WhileStmt>(w->line, llsCloneExpr(w->cond.get()),
                                    llsCloneStmt(w->body.get()));
  }
  case StmtKind::Break:
    return make_unique<BreakStmt>(s->line);
  case StmtKind::Continue:
    return make_unique<ContinueStmt>(s->line);
  case StmtKind::Return: {
    auto *r = static_cast<const ReturnStmt *>(s);
    return make_unique<ReturnStmt>(r->line, llsCloneExpr(r->expr.get()));
  }
  default:
    return nullptr;
  }
}

static void llsCollectAndFactors(Expr *e, vector<ExprPtr> &out) {
  if (!e) {
    return;
  }
  if (e->kind == ExprKind::Binary) {
    auto *b = static_cast<BinaryExpr *>(e);
    if (b->op == "&&") {
      llsCollectAndFactors(b->lhs.get(), out);
      llsCollectAndFactors(b->rhs.get(), out);
      return;
    }
  }
  if (ExprPtr c = llsCloneExpr(e)) {
    out.push_back(std::move(c));
  }
}

static bool llsExprIsAndChain(Expr *e) {
  if (!e || e->kind != ExprKind::Binary) {
    return false;
  }
  auto *b = static_cast<BinaryExpr *>(e);
  return b->op == "&&";
}

static bool llsExprIsOrChain(Expr *e) {
  if (!e || e->kind != ExprKind::Binary) {
    return false;
  }
  auto *b = static_cast<BinaryExpr *>(e);
  return b->op == "||";
}

static void llsCollectOrFactors(Expr *e, vector<ExprPtr> &out) {
  if (!e) {
    return;
  }
  if (e->kind == ExprKind::Binary) {
    auto *b = static_cast<BinaryExpr *>(e);
    if (b->op == "||") {
      llsCollectOrFactors(b->lhs.get(), out);
      llsCollectOrFactors(b->rhs.get(), out);
      return;
    }
  }
  if (ExprPtr c = llsCloneExpr(e)) {
    out.push_back(std::move(c));
  }
}

static StmtPtr llsNestIfFromOr(int line, vector<ExprPtr> &parts, StmtPtr thenStmt,
                            StmtPtr elseStmt) {
  StmtPtr cur = std::move(elseStmt);
  StmtPtr thenCopy = llsCloneStmt(thenStmt.get());
  for (int i = static_cast<int>(parts.size()) - 1; i >= 0; --i) {
    StmtPtr thenBranch = thenCopy ? llsCloneStmt(thenCopy.get()) : nullptr;
    auto ifs =
        make_unique<IfStmt>(line, std::move(parts[static_cast<size_t>(i)]), std::move(thenBranch),
                            std::move(cur));
    cur = std::move(ifs);
  }
  return cur;
}

static StmtPtr llsRewriteWhileOr(WhileStmt *w, vector<ExprPtr> &parts) {
  auto blk = make_unique<BlockStmt>(w->line);
  StmtPtr bodyCopy = llsCloneStmt(w->body.get());
  for (ExprPtr &p : parts) {
    auto inner = make_unique<BlockStmt>(w->line);
    if (bodyCopy) {
      inner->items.push_back(llsCloneStmt(bodyCopy.get()));
    }
    inner->items.push_back(make_unique<ContinueStmt>(w->line));
    blk->items.push_back(
        make_unique<IfStmt>(w->line, std::move(p), std::move(inner), nullptr));
  }
  blk->items.push_back(make_unique<BreakStmt>(w->line));
  auto one = make_unique<NumberExpr>(w->line, 1);
  return make_unique<WhileStmt>(w->line, std::move(one), std::move(blk));
}

static StmtPtr llsNestIfFromAnd(int line, vector<ExprPtr> &parts, StmtPtr thenStmt,
                             StmtPtr elseStmt) {
  StmtPtr cur = std::move(thenStmt);
  StmtPtr elseCopy = llsCloneStmt(elseStmt.get());
  for (int i = static_cast<int>(parts.size()) - 1; i >= 0; --i) {
    StmtPtr elseBranch = elseCopy ? llsCloneStmt(elseCopy.get()) : nullptr;
    auto ifs =
        make_unique<IfStmt>(line, std::move(parts[static_cast<size_t>(i)]), std::move(cur),
                              std::move(elseBranch));
    cur = std::move(ifs);
  }
  return cur;
}

static StmtPtr llsRewriteWhileAnd(WhileStmt *w, vector<ExprPtr> &parts) {
  auto blk = make_unique<BlockStmt>(w->line);
  for (ExprPtr &p : parts) {
    auto neg = make_unique<UnaryExpr>(w->line, "!", llsCloneExpr(p.get()));
    auto brk = make_unique<BreakStmt>(w->line);
    blk->items.push_back(
        make_unique<IfStmt>(w->line, std::move(neg), std::move(brk), nullptr));
  }
  if (StmtPtr body = llsCloneStmt(w->body.get())) {
    blk->items.push_back(std::move(body));
  }
  auto one = make_unique<NumberExpr>(w->line, 1);
  return make_unique<WhileStmt>(w->line, std::move(one), std::move(blk));
}

static StmtPtr llsRewriteStmtTree(Stmt *s);

static bool llsRewriteBlock(BlockStmt *blk) {
  if (!blk) {
    return false;
  }
  bool changed = false;
  for (auto &it : blk->items) {
    if (StmtPtr rep = llsRewriteStmtTree(it.get())) {
      it = std::move(rep);
      changed = true;
    }
  }
  return changed;
}

static StmtPtr llsRewriteStmtTree(Stmt *s) {
  if (!s) {
    return nullptr;
  }
  switch (s->kind) {
  case StmtKind::Block: {
    auto *b = static_cast<BlockStmt *>(s);
    llsRewriteBlock(b);
    return nullptr;
  }
  case StmtKind::If: {
    auto *ifs = static_cast<IfStmt *>(s);
    if (llsExprIsOrChain(ifs->cond.get())) {
      vector<ExprPtr> parts;
      llsCollectOrFactors(ifs->cond.get(), parts);
      if (parts.size() >= 2) {
        StmtPtr thenS = llsCloneStmt(ifs->thenStmt.get());
        StmtPtr elseS = llsCloneStmt(ifs->elseStmt.get());
        StmtPtr nested = llsNestIfFromOr(ifs->line, parts, std::move(thenS), std::move(elseS));
        if (nested) {
          return nested;
        }
      }
    }
    if (llsExprIsAndChain(ifs->cond.get())) {
      vector<ExprPtr> parts;
      llsCollectAndFactors(ifs->cond.get(), parts);
      if (parts.size() >= 2) {
        StmtPtr thenS = llsCloneStmt(ifs->thenStmt.get());
        StmtPtr elseS = llsCloneStmt(ifs->elseStmt.get());
        StmtPtr nested =
            llsNestIfFromAnd(ifs->line, parts, std::move(thenS), std::move(elseS));
        if (nested) {
          return nested;
        }
      }
    }
    if (StmtPtr t = llsRewriteStmtTree(ifs->thenStmt.get())) {
      ifs->thenStmt = std::move(t);
    }
    if (ifs->elseStmt) {
      if (StmtPtr e = llsRewriteStmtTree(ifs->elseStmt.get())) {
        ifs->elseStmt = std::move(e);
      }
    }
    return nullptr;
  }
  case StmtKind::While: {
    auto *w = static_cast<WhileStmt *>(s);
    if (llsExprIsOrChain(w->cond.get())) {
      vector<ExprPtr> parts;
      llsCollectOrFactors(w->cond.get(), parts);
      if (parts.size() >= 2) {
        if (StmtPtr nw = llsRewriteWhileOr(w, parts)) {
          return nw;
        }
      }
    }
    if (llsExprIsAndChain(w->cond.get())) {
      vector<ExprPtr> parts;
      llsCollectAndFactors(w->cond.get(), parts);
      if (parts.size() >= 2) {
        if (StmtPtr nw = llsRewriteWhileAnd(w, parts)) {
          return nw;
        }
      }
    }
    if (StmtPtr b = llsRewriteStmtTree(w->body.get())) {
      w->body = std::move(b);
    }
    return nullptr;
  }
  default:
    return nullptr;
  }
}

} // namespace

void splitLogicalAndPass(Program &program) {
  if (envFlagTruthy("SYSY_CC_NO_LAND_LOR_SPLIT")) {
    return;
  }
  for (auto &item : program.items) {
    if (!item.func || !item.func->body) {
      continue;
    }
    for (int pass = 0; pass < 32; ++pass) {
      if (!llsRewriteBlock(item.func->body.get())) {
        break;
      }
    }
  }
}
