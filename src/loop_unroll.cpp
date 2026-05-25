#include "loop_unroll.h"

#include "common.h"

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {

constexpr int kMaxTrip = 16;
constexpr int kMaxBodyStmts = 48;
constexpr int kMaxUnrolledStmts = 800;

static int lurUnrollTripCap() {
  const char *v = getenv("SYSY_CC_LOOP_UNROLL_MAX");
  if (!v || !*v) {
    return kMaxTrip;
  }
  return std::max(0, std::min(kMaxTrip, static_cast<int>(std::strtol(v, nullptr, 10))));
}

static void lurCopyExprMeta(Expr *dst, const Expr *src) {
  if (!dst || !src) {
    return;
  }
  dst->type = src->type;
  dst->isConst = src->isConst;
  dst->constVal = src->constVal;
}

static ExprPtr lurCloneExpr(const Expr *e) {
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
  case ExprKind::LVal: {
    auto *lv = static_cast<const LValExpr *>(e);
    auto c = make_unique<LValExpr>(lv->line, lv->name);
    c->symbol = lv->symbol;
    for (const auto &ix : lv->indices) {
      c->indices.push_back(lurCloneExpr(ix.get()));
    }
    out = std::move(c);
    break;
  }
  case ExprKind::Unary: {
    auto *u = static_cast<const UnaryExpr *>(e);
    out = make_unique<UnaryExpr>(u->line, u->op, lurCloneExpr(u->expr.get()));
    break;
  }
  case ExprKind::Binary: {
    auto *b = static_cast<const BinaryExpr *>(e);
    out = make_unique<BinaryExpr>(b->line, b->op, lurCloneExpr(b->lhs.get()),
                                    lurCloneExpr(b->rhs.get()));
    break;
  }
  default:
    return nullptr;
  }
  lurCopyExprMeta(out.get(), e);
  return out;
}

static ExprPtr lurSubstIvExpr(const Expr *e, const string &iv, int val) {
  if (!e) {
    return nullptr;
  }
  if (e->kind == ExprKind::LVal) {
    auto *lv = static_cast<const LValExpr *>(e);
    if (lv->name == iv && lv->indices.empty()) {
      return make_unique<NumberExpr>(e->line, val);
    }
    auto c = make_unique<LValExpr>(lv->line, lv->name);
    c->symbol = lv->symbol;
    for (const auto &ix : lv->indices) {
      c->indices.push_back(lurSubstIvExpr(ix.get(), iv, val));
    }
    return c;
  }
  if (e->kind == ExprKind::Unary) {
    auto *u = static_cast<const UnaryExpr *>(e);
    return make_unique<UnaryExpr>(u->line, u->op, lurSubstIvExpr(u->expr.get(), iv, val));
  }
  if (e->kind == ExprKind::Binary) {
    auto *b = static_cast<const BinaryExpr *>(e);
    return make_unique<BinaryExpr>(
        b->line, b->op, lurSubstIvExpr(b->lhs.get(), iv, val),
        lurSubstIvExpr(b->rhs.get(), iv, val));
  }
  return lurCloneExpr(e);
}

static StmtPtr lurCloneStmt(const Stmt *s);
static StmtPtr lurSubstIvStmt(const Stmt *s, const string &iv, int val);

static StmtPtr lurSubstIvStmt(const Stmt *s, const string &iv, int val) {
  if (!s) {
    return nullptr;
  }
  if (auto *as = dynamic_cast<const AssignStmt *>(s)) {
    auto lhs = make_unique<LValExpr>(as->line, as->lhs->name);
    lhs->symbol = as->lhs->symbol;
    for (const auto &ix : as->lhs->indices) {
      lhs->indices.push_back(lurSubstIvExpr(ix.get(), iv, val));
    }
    return make_unique<AssignStmt>(as->line, std::move(lhs),
                                   lurSubstIvExpr(as->rhs.get(), iv, val));
  }
  if (auto *es = dynamic_cast<const ExprStmt *>(s)) {
    return make_unique<ExprStmt>(es->line, lurSubstIvExpr(es->expr.get(), iv, val));
  }
  if (auto *ifs = dynamic_cast<const IfStmt *>(s)) {
    StmtPtr thenS =
        ifs->thenStmt ? lurSubstIvStmt(ifs->thenStmt.get(), iv, val) : nullptr;
    StmtPtr elseS =
        ifs->elseStmt ? lurSubstIvStmt(ifs->elseStmt.get(), iv, val) : nullptr;
    return make_unique<IfStmt>(ifs->line, lurSubstIvExpr(ifs->cond.get(), iv, val),
                               std::move(thenS), std::move(elseS));
  }
  if (auto *blk = dynamic_cast<const BlockStmt *>(s)) {
    auto nb = make_unique<BlockStmt>(blk->line);
    for (const auto &it : blk->items) {
      if (StmtPtr c = lurSubstIvStmt(it.get(), iv, val)) {
        nb->items.push_back(std::move(c));
      }
    }
    return nb;
  }
  return lurCloneStmt(s);
}

static StmtPtr lurCloneStmt(const Stmt *s) {
  if (!s) {
    return nullptr;
  }
  if (auto *as = dynamic_cast<const AssignStmt *>(s)) {
    auto lhs = make_unique<LValExpr>(as->line, as->lhs->name);
    lhs->symbol = as->lhs->symbol;
    for (const auto &ix : as->lhs->indices) {
      lhs->indices.push_back(lurCloneExpr(ix.get()));
    }
    return make_unique<AssignStmt>(as->line, std::move(lhs), lurCloneExpr(as->rhs.get()));
  }
  if (auto *es = dynamic_cast<const ExprStmt *>(s)) {
    return make_unique<ExprStmt>(es->line, lurCloneExpr(es->expr.get()));
  }
  if (auto *ifs = dynamic_cast<const IfStmt *>(s)) {
    StmtPtr thenS = ifs->thenStmt ? lurCloneStmt(ifs->thenStmt.get()) : nullptr;
    StmtPtr elseS = ifs->elseStmt ? lurCloneStmt(ifs->elseStmt.get()) : nullptr;
    return make_unique<IfStmt>(ifs->line, lurCloneExpr(ifs->cond.get()), std::move(thenS),
                               std::move(elseS));
  }
  if (auto *blk = dynamic_cast<const BlockStmt *>(s)) {
    auto nb = make_unique<BlockStmt>(blk->line);
    for (const auto &it : blk->items) {
      if (StmtPtr c = lurCloneStmt(it.get())) {
        nb->items.push_back(std::move(c));
      }
    }
    return nb;
  }
  return nullptr;
}

static bool lurExtractLtBound(const Expr *cond, string *iv, int *limitConst) {
  auto *b = dynamic_cast<const BinaryExpr *>(cond);
  if (!b || b->op != "<") {
    return false;
  }
  auto *lv = dynamic_cast<const LValExpr *>(b->lhs.get());
  auto *rn = dynamic_cast<const NumberExpr *>(b->rhs.get());
  if (lv && lv->indices.empty() && rn && !rn->isFloat) {
    *iv = lv->name;
    *limitConst = rn->intVal;
    return true;
  }
  return false;
}

static bool lurIsZeroInit(const Stmt *s, const string &iv) {
  if (auto *as = dynamic_cast<const AssignStmt *>(s)) {
    if (!as->lhs || as->lhs->name != iv || as->lhs->indices.empty()) {
      return false;
    }
    auto *n = dynamic_cast<const NumberExpr *>(as->rhs.get());
    return n && !n->isFloat && n->intVal == 0;
  }
  if (auto *d = dynamic_cast<const DeclStmt *>(s)) {
    if (d->defs.size() != 1 || d->defs[0].name != iv) {
      return false;
    }
    if (d->defs[0].init && d->defs[0].init->expr) {
      auto *n = dynamic_cast<const NumberExpr *>(d->defs[0].init->expr.get());
      return n && !n->isFloat && n->intVal == 0;
    }
  }
  return false;
}

static bool lurExprIvUsedAsArrayIndex(const Expr *e, const string &iv) {
  if (!e) {
    return false;
  }
  if (e->kind == ExprKind::LVal) {
    auto *lv = static_cast<const LValExpr *>(e);
    for (const auto &ix : lv->indices) {
      if (auto *ixlv = dynamic_cast<const LValExpr *>(ix.get())) {
        if (ixlv->name == iv && ixlv->indices.empty()) {
          return true;
        }
      }
      if (lurExprIvUsedAsArrayIndex(ix.get(), iv)) {
        return true;
      }
    }
    return false;
  }
  if (e->kind == ExprKind::Unary) {
    return lurExprIvUsedAsArrayIndex(
        static_cast<const UnaryExpr *>(e)->expr.get(), iv);
  }
  if (e->kind == ExprKind::Binary) {
    auto *b = static_cast<const BinaryExpr *>(e);
    return lurExprIvUsedAsArrayIndex(b->lhs.get(), iv) ||
           lurExprIvUsedAsArrayIndex(b->rhs.get(), iv);
  }
  return false;
}

static bool lurStmtIvUsedAsArrayIndex(const Stmt *s, const string &iv) {
  if (!s) {
    return false;
  }
  switch (s->kind) {
  case StmtKind::Assign: {
    auto *as = static_cast<const AssignStmt *>(s);
    return lurExprIvUsedAsArrayIndex(as->lhs.get(), iv) ||
           lurExprIvUsedAsArrayIndex(as->rhs.get(), iv);
  }
  case StmtKind::Expr: {
    return lurExprIvUsedAsArrayIndex(static_cast<const ExprStmt *>(s)->expr.get(), iv);
  }
  case StmtKind::If: {
    auto *ifs = static_cast<const IfStmt *>(s);
    return lurExprIvUsedAsArrayIndex(ifs->cond.get(), iv) ||
           lurStmtIvUsedAsArrayIndex(ifs->thenStmt.get(), iv) ||
           lurStmtIvUsedAsArrayIndex(ifs->elseStmt.get(), iv);
  }
  case StmtKind::Block: {
    for (const auto &it : static_cast<const BlockStmt *>(s)->items) {
      if (lurStmtIvUsedAsArrayIndex(it.get(), iv)) {
        return true;
      }
    }
    return false;
  }
  default:
    return false;
  }
}

static bool lurIsIncByOne(const AssignStmt *as, const string &iv) {
  if (!as || !as->lhs || as->lhs->name != iv || !as->lhs->indices.empty()) {
    return false;
  }
  auto *bin = dynamic_cast<const BinaryExpr *>(as->rhs.get());
  if (!bin || bin->op != "+") {
    return false;
  }
  auto *l = dynamic_cast<const LValExpr *>(bin->lhs.get());
  auto *r = dynamic_cast<const NumberExpr *>(bin->rhs.get());
  return l && l->name == iv && l->indices.empty() && r && !r->isFloat &&
         r->intVal == 1;
}

static bool lurStmtHasBreakContinue(const Stmt *s) {
  if (!s) {
    return false;
  }
  switch (s->kind) {
  case StmtKind::Break:
  case StmtKind::Continue:
    return true;
  case StmtKind::Block: {
    for (const auto &it : static_cast<const BlockStmt *>(s)->items) {
      if (lurStmtHasBreakContinue(it.get())) {
        return true;
      }
    }
    return false;
  }
  case StmtKind::If: {
    auto *ifs = static_cast<const IfStmt *>(s);
    return lurStmtHasBreakContinue(ifs->thenStmt.get()) ||
           lurStmtHasBreakContinue(ifs->elseStmt.get());
  }
  case StmtKind::While:
    return true;
  default:
    return false;
  }
}

static bool lurCollectLoopBody(const WhileStmt *w, const string &iv,
                            vector<const Stmt *> &body, const AssignStmt **inc) {
  *inc = nullptr;
  body.clear();
  auto *blk = dynamic_cast<BlockStmt *>(w->body.get());
  if (!blk) {
    return false;
  }
  for (const auto &st : blk->items) {
    if (auto *as = dynamic_cast<const AssignStmt *>(st.get())) {
      if (lurIsIncByOne(as, iv)) {
        *inc = as;
        continue;
      }
    }
    if (lurStmtHasBreakContinue(st.get())) {
      return false;
    }
    body.push_back(st.get());
  }
  return *inc != nullptr && !body.empty() &&
         static_cast<int>(body.size()) <= kMaxBodyStmts;
}

static bool lurTryUnrollInitWhile(BlockStmt *parent, size_t initIdx) {
  if (initIdx + 1 >= parent->items.size()) {
    return false;
  }
  auto *init = parent->items[initIdx].get();
  auto *w = dynamic_cast<WhileStmt *>(parent->items[initIdx + 1].get());
  if (!w) {
    return false;
  }

  string iv;
  int limit = 0;
  if (!lurExtractLtBound(w->cond.get(), &iv, &limit) || !lurIsZeroInit(init, iv)) {
    return false;
  }

  const int cap = lurUnrollTripCap();
  if (limit <= 0 || limit > cap) {
    return false;
  }

  const AssignStmt *inc = nullptr;
  vector<const Stmt *> body;
  if (!lurCollectLoopBody(w, iv, body, &inc)) {
    return false;
  }
  for (const Stmt *st : body) {
    if (lurStmtIvUsedAsArrayIndex(st, iv)) {
      return false;
    }
  }

  vector<StmtPtr> unrolled;
  unrolled.reserve(static_cast<size_t>(limit) * body.size());
  for (int k = 0; k < limit; ++k) {
    for (const Stmt *st : body) {
      if (StmtPtr c = lurSubstIvStmt(st, iv, k)) {
        unrolled.push_back(std::move(c));
      }
    }
    if (static_cast<int>(unrolled.size()) > kMaxUnrolledStmts) {
      return false;
    }
  }

  vector<StmtPtr> newItems;
  newItems.reserve(parent->items.size() + unrolled.size() + 2);
  for (size_t i = 0; i < initIdx; ++i) {
    newItems.push_back(std::move(parent->items[i]));
  }
  // 保留 `int iv = 0` / `iv = 0`，供后续链式 while (iv < N2) 使用
  newItems.push_back(std::move(parent->items[initIdx]));
  for (auto &st : unrolled) {
    newItems.push_back(std::move(st));
  }
  {
    const int line = w->line;
    auto lhs = make_unique<LValExpr>(line, iv);
    newItems.push_back(make_unique<AssignStmt>(
        line, std::move(lhs), make_unique<NumberExpr>(line, limit)));
  }
  for (size_t i = initIdx + 2; i < parent->items.size(); ++i) {
    newItems.push_back(std::move(parent->items[i]));
  }
  parent->items = std::move(newItems);
  return true;
}

static bool lurTransformBlock(BlockStmt *blk) {
  bool changed = false;
  for (size_t i = 0; i < blk->items.size();) {
    if (lurTryUnrollInitWhile(blk, i)) {
      changed = true;
      continue;
    }
    if (auto *inner = dynamic_cast<BlockStmt *>(blk->items[i].get())) {
      changed |= lurTransformBlock(inner);
    } else if (auto *w = dynamic_cast<WhileStmt *>(blk->items[i].get())) {
      if (auto *wb = dynamic_cast<BlockStmt *>(w->body.get())) {
        changed |= lurTransformBlock(wb);
      }
    }
    ++i;
  }
  return changed;
}

static void lurTransformFunc(FuncDef *fn) {
  if (!fn || !fn->body) {
    return;
  }
  if (auto *blk = dynamic_cast<BlockStmt *>(fn->body.get())) {
    lurTransformBlock(blk);
  }
}

}  // namespace

void applySmallLoopUnrollPass(Program &program) {
  if (envFlagTruthy("SYSY_CC_NO_LOOP_UNROLL")) {
    return;
  }
  for (auto &item : program.items) {
    if (item.func) {
      lurTransformFunc(item.func.get());
    }
  }
}
