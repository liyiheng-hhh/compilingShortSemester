// AST 层循环分块：矩形二重循环、以及「j 中 k 内」矩阵乘三重循环（tile=16）。
// 在 loopInterchangePass 之后运行；SYSY_CC_NO_LOOP_TILING=1 可关闭。

#include "loop_interchange.h"

#include "common.h"

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::unordered_set;
using std::vector;

namespace {

constexpr int kTile = 16;

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

static StmtPtr cloneStmt(const Stmt *s) {
  if (!s) {
    return nullptr;
  }
  if (auto *as = dynamic_cast<const AssignStmt *>(s)) {
    auto lhs = make_unique<LValExpr>(as->line, as->lhs->name);
    lhs->symbol = as->lhs->symbol;
    for (const auto &ix : as->lhs->indices) {
      lhs->indices.push_back(cloneExpr(ix.get()));
    }
    return make_unique<AssignStmt>(as->line, std::move(lhs), cloneExpr(as->rhs.get()));
  }
  if (auto *es = dynamic_cast<const ExprStmt *>(s)) {
    return make_unique<ExprStmt>(es->line, cloneExpr(es->expr.get()));
  }
  if (auto *d = dynamic_cast<const DeclStmt *>(s)) {
    auto nd = make_unique<DeclStmt>(d->line, d->isConst, d->base);
    for (const auto &def : d->defs) {
      VarDef vd;
      vd.name = def.name;
      vd.symbol = def.symbol;
      vd.line = def.line;
      for (const auto &dim : def.dims) {
        vd.dims.push_back(cloneExpr(dim.get()));
      }
      if (def.init) {
        vd.init = make_unique<InitVal>();
        vd.init->isList = def.init->isList;
        if (def.init->expr) {
          vd.init->expr = cloneExpr(def.init->expr.get());
        }
      }
      nd->defs.push_back(std::move(vd));
    }
    return nd;
  }
  return nullptr;
}

static unique_ptr<LValExpr> makeScalarLVal(int line, const string &name) {
  return make_unique<LValExpr>(line, name);
}

static unique_ptr<NumberExpr> makeInt(int line, int v) {
  return make_unique<NumberExpr>(line, v);
}

static unique_ptr<AssignStmt> makeAssign(int line, const string &lhs,
                                         ExprPtr rhs) {
  return make_unique<AssignStmt>(line, makeScalarLVal(line, lhs),
                                 std::move(rhs));
}

static unique_ptr<AssignStmt> makeZeroAssign(int line, const string &v) {
  return makeAssign(line, v, makeInt(line, 0));
}

static unique_ptr<AssignStmt> makeIncBy(int line, const string &v, int delta) {
  auto add = make_unique<BinaryExpr>(
      line, "+", makeScalarLVal(line, v), makeInt(line, delta));
  return make_unique<AssignStmt>(line, makeScalarLVal(line, v), std::move(add));
}

static unique_ptr<AssignStmt> makeIncByOne(int line, const string &v) {
  return makeIncBy(line, v, 1);
}

static ExprPtr makeLt(int line, const string &lhs, ExprPtr rhs) {
  return make_unique<BinaryExpr>(line, "<", makeScalarLVal(line, lhs),
                                 std::move(rhs));
}

static ExprPtr makeAnd(int line, ExprPtr a, ExprPtr b) {
  return make_unique<BinaryExpr>(line, "&&", std::move(a), std::move(b));
}

static ExprPtr makeTiledWhileCond(int line, const string &iv,
                                  const string &tileIv, const Expr *limit) {
  ExprPtr c0 = makeLt(line, iv, cloneExpr(limit));
  auto tileEnd = make_unique<BinaryExpr>(
      line, "+", makeScalarLVal(line, tileIv), makeInt(line, kTile));
  ExprPtr c1 = make_unique<BinaryExpr>(line, "<", makeScalarLVal(line, iv),
                                       std::move(tileEnd));
  return makeAnd(line, std::move(c0), std::move(c1));
}

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
    return exprUsesVarName(b->lhs.get(), name) || exprUsesVarName(b->rhs.get(), name);
  }
  default:
    return false;
  }
}

static bool extractLtBound(const Expr *cond, string *iv, ExprPtr *limit) {
  auto *b = dynamic_cast<const BinaryExpr *>(cond);
  if (!b || b->op != "<") {
    return false;
  }
  if (auto *lv = dynamic_cast<const LValExpr *>(b->lhs.get())) {
    if (!lv->indices.empty()) {
      return false;
    }
    *iv = lv->name;
    *limit = cloneExpr(b->rhs.get());
    return true;
  }
  if (auto *lv = dynamic_cast<const LValExpr *>(b->rhs.get())) {
    if (!lv->indices.empty()) {
      return false;
    }
    *iv = lv->name;
    *limit = cloneExpr(b->lhs.get());
    return true;
  }
  return false;
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
  return l && l->name == v && l->indices.empty() && r && !r->isFloat &&
         r->intVal == 1;
}

static bool isZeroScalarAssign(const AssignStmt *as, const string &v) {
  if (!as || !as->lhs || as->lhs->name != v || !as->lhs->indices.empty()) {
    return false;
  }
  auto *n = dynamic_cast<const NumberExpr *>(as->rhs.get());
  return n && !n->isFloat && n->intVal == 0;
}

static bool parseZeroInit(const Stmt *s, string *name) {
  if (auto *as = dynamic_cast<const AssignStmt *>(s)) {
    if (as->lhs && as->lhs->indices.empty() &&
        isZeroScalarAssign(as, as->lhs->name)) {
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

static bool blockHasUnsafeForTiling(const BlockStmt *blk) {
  if (!blk) {
    return true;
  }
  for (const auto &it : blk->items) {
    switch (it->kind) {
    case StmtKind::Break:
    case StmtKind::Continue:
    case StmtKind::Decl:
    case StmtKind::While:
    case StmtKind::If:
      return true;
    case StmtKind::Block:
      if (blockHasUnsafeForTiling(static_cast<const BlockStmt *>(it.get()))) {
        return true;
      }
      break;
    default:
      break;
    }
  }
  return false;
}

static int gTileNestId = 0;
static unordered_set<string> gHoistedLoopIvs;

static string tileVar(const string &iv, int nestId) {
  return string("_t") + iv + "_" + std::to_string(nestId);
}

static StmtPtr makeTileVarDecl(int line, int nestId, const string &outerIv,
                               const string &innerIv, bool declareInnerIv) {
  auto d = make_unique<DeclStmt>(line, false, BaseType::Int);
  VarDef vo;
  vo.name = tileVar(outerIv, nestId);
  vo.line = line;
  d->defs.push_back(std::move(vo));
  VarDef vt;
  vt.name = tileVar(innerIv, nestId);
  vt.line = line;
  d->defs.push_back(std::move(vt));
  if (declareInnerIv) {
    VarDef vi;
    vi.name = innerIv;
    vi.line = line;
    d->defs.push_back(std::move(vi));
  }
  return d;
}

// 常数上界过小则不分块，避免 I-cache 膨胀且收益甚微。
static bool boundsTooSmallForTiling(const Expr *outerLimit,
                                    const Expr *innerLimit) {
  auto small = [](const Expr *e) {
    auto *n = dynamic_cast<const NumberExpr *>(e);
    return n && !n->isFloat && n->intVal < kTile;
  };
  return small(outerLimit) && small(innerLimit);
}

static unique_ptr<WhileStmt>
buildTiled2D(int line, int nestId, const string &outerIv, const string &innerIv,
             const Expr *outerLimit, const Expr *innerLimit,
             vector<StmtPtr> core) {
  const string ii = tileVar(outerIv, nestId);
  const string jj = tileVar(innerIv, nestId);

  auto innerCore = make_unique<BlockStmt>(line);
  for (auto &st : core) {
    innerCore->items.push_back(std::move(st));
  }
  innerCore->items.push_back(makeIncByOne(line, innerIv));

  auto iBody = make_unique<BlockStmt>(line);
  iBody->items.push_back(makeAssign(line, innerIv, makeScalarLVal(line, jj)));
  auto iWhile = make_unique<WhileStmt>(
      line, makeTiledWhileCond(line, innerIv, jj, innerLimit), nullptr);
  iWhile->body = std::move(innerCore);
  iBody->items.push_back(std::move(iWhile));
  iBody->items.push_back(makeIncByOne(line, outerIv));

  auto jTileBody = make_unique<BlockStmt>(line);
  jTileBody->items.push_back(makeAssign(line, outerIv, makeScalarLVal(line, ii)));
  auto jTileWhile = make_unique<WhileStmt>(
      line, makeTiledWhileCond(line, outerIv, ii, outerLimit), nullptr);
  jTileWhile->body = std::move(iBody);
  jTileBody->items.push_back(std::move(jTileWhile));
  jTileBody->items.push_back(makeIncBy(line, jj, kTile));

  auto jjWhile =
      make_unique<WhileStmt>(line, makeLt(line, jj, cloneExpr(innerLimit)), nullptr);
  jjWhile->body = std::move(jTileBody);

  auto iiBody = make_unique<BlockStmt>(line);
  iiBody->items.push_back(makeZeroAssign(line, jj));
  iiBody->items.push_back(std::move(jjWhile));
  iiBody->items.push_back(makeIncBy(line, ii, kTile));

  auto iiWhile =
      make_unique<WhileStmt>(line, makeLt(line, ii, cloneExpr(outerLimit)), nullptr);
  iiWhile->body = std::move(iiBody);
  return iiWhile;
}

static bool tryTile2DNest(vector<StmtPtr> &items, size_t k) {
  if (k + 1 >= items.size()) {
    return false;
  }
  auto *outerInit = dynamic_cast<AssignStmt *>(items[k].get());
  auto *outerW = dynamic_cast<WhileStmt *>(items[k + 1].get());
  if (!outerInit || !outerW) {
    return false;
  }
  string outerIv;
  ExprPtr outerLimit;
  if (!extractLtBound(outerW->cond.get(), &outerIv, &outerLimit) ||
      !isZeroScalarAssign(outerInit, outerIv)) {
    return false;
  }
  auto *outerBody = dynamic_cast<BlockStmt *>(outerW->body.get());
  if (!outerBody || outerBody->items.size() < 3) {
    return false;
  }
  size_t pos = 0;
  string innerIv;
  if (!parseZeroInit(outerBody->items[pos].get(), &innerIv)) {
    return false;
  }
  ++pos;
  auto *innerW = dynamic_cast<WhileStmt *>(outerBody->items[pos].get());
  if (!innerW) {
    return false;
  }
  ++pos;
  auto *outerInc = dynamic_cast<AssignStmt *>(outerBody->items[pos].get());
  if (!isIncByOne(outerInc, outerIv) || pos + 1 != outerBody->items.size()) {
    return false;
  }
  string innerIv2;
  ExprPtr innerLimit;
  if (!extractLtBound(innerW->cond.get(), &innerIv2, &innerLimit) ||
      innerIv2 != innerIv || innerIv == outerIv) {
    return false;
  }
  auto *innerBody = dynamic_cast<BlockStmt *>(innerW->body.get());
  if (!innerBody || innerBody->items.size() < 2) {
    return false;
  }
  auto *innerInc = dynamic_cast<AssignStmt *>(innerBody->items.back().get());
  if (!isIncByOne(innerInc, innerIv) || blockHasUnsafeForTiling(innerBody)) {
    return false;
  }
  vector<StmtPtr> core;
  for (size_t i = 0; i + 1 < innerBody->items.size(); ++i) {
    if (StmtPtr c = cloneStmt(innerBody->items[i].get())) {
      core.push_back(std::move(c));
    }
  }
  if (core.empty()) {
    return false;
  }
  if (boundsTooSmallForTiling(outerLimit.get(), innerLimit.get())) {
    return false;
  }
  if (exprUsesVarName(innerLimit.get(), outerIv) ||
      exprUsesVarName(outerLimit.get(), innerIv)) {
    return false;
  }

  const bool innerWasDecl =
      dynamic_cast<const DeclStmt *>(outerBody->items[0].get()) != nullptr;
  bool declareInner = innerWasDecl;
  if (declareInner && !gHoistedLoopIvs.insert(innerIv).second) {
    declareInner = false;
  }
  const int nestId = gTileNestId++;
  int line = outerW->line;
  vector<StmtPtr> rep;
  rep.push_back(makeTileVarDecl(line, nestId, outerIv, innerIv, declareInner));
  rep.push_back(makeZeroAssign(line, tileVar(outerIv, nestId)));
  rep.push_back(buildTiled2D(line, nestId, outerIv, innerIv, outerLimit.get(),
                             innerLimit.get(), std::move(core)));
  items.erase(items.begin() + static_cast<ptrdiff_t>(k),
              items.begin() + static_cast<ptrdiff_t>(k + 2));
  items.insert(items.begin() + static_cast<ptrdiff_t>(k),
               std::make_move_iterator(rep.begin()),
               std::make_move_iterator(rep.end()));
  return true;
}

// 中层 j： [pre: k/sum 初始化…][while k {body}][post: 写回][j++]
static bool tryTile3DMatmul(vector<StmtPtr> &items, size_t k) {
  if (k + 1 >= items.size()) {
    return false;
  }
  auto *outerInit = dynamic_cast<AssignStmt *>(items[k].get());
  auto *outerW = dynamic_cast<WhileStmt *>(items[k + 1].get());
  if (!outerInit || !outerW) {
    return false;
  }
  string outerIv;
  ExprPtr limit;
  if (!extractLtBound(outerW->cond.get(), &outerIv, &limit) ||
      !isZeroScalarAssign(outerInit, outerIv)) {
    return false;
  }
  auto *outerBody = dynamic_cast<BlockStmt *>(outerW->body.get());
  if (!outerBody || outerBody->items.size() < 3) {
    return false;
  }
  size_t pos = 0;
  string midIv;
  if (!parseZeroInit(outerBody->items[pos].get(), &midIv)) {
    return false;
  }
  ++pos;
  auto *midW = dynamic_cast<WhileStmt *>(outerBody->items[pos].get());
  if (!midW) {
    return false;
  }
  ++pos;
  auto *outerInc = dynamic_cast<AssignStmt *>(outerBody->items[pos].get());
  if (!isIncByOne(outerInc, outerIv) || pos + 1 != outerBody->items.size()) {
    return false;
  }
  string midIv2;
  ExprPtr midLimit;
  if (!extractLtBound(midW->cond.get(), &midIv2, &midLimit) || midIv2 != midIv) {
    return false;
  }
  auto *midBody = dynamic_cast<BlockStmt *>(midW->body.get());
  if (!midBody || midBody->items.size() < 4) {
    return false;
  }
  size_t kWhileIdx = SIZE_MAX;
  for (size_t i = 0; i < midBody->items.size(); ++i) {
    if (midBody->items[i]->kind == StmtKind::While) {
      if (kWhileIdx != SIZE_MAX) {
        return false;
      }
      kWhileIdx = i;
    } else if (midBody->items[i]->kind == StmtKind::If ||
               midBody->items[i]->kind == StmtKind::Break ||
               midBody->items[i]->kind == StmtKind::Continue) {
      return false;
    }
  }
  if (kWhileIdx == SIZE_MAX) {
    return false;
  }
  auto *kW = dynamic_cast<WhileStmt *>(midBody->items[kWhileIdx].get());
  string kIv;
  ExprPtr kLimit;
  if (!extractLtBound(kW->cond.get(), &kIv, &kLimit)) {
    return false;
  }
  auto *kBody = dynamic_cast<BlockStmt *>(kW->body.get());
  if (!kBody || kBody->items.size() != 2 || kBody->items[0]->kind != StmtKind::Assign ||
      !isIncByOne(dynamic_cast<AssignStmt *>(kBody->items[1].get()), kIv)) {
    return false;
  }
  if (kWhileIdx + 2 >= midBody->items.size()) {
    return false;
  }
  auto *midInc = dynamic_cast<AssignStmt *>(midBody->items.back().get());
  if (!isIncByOne(midInc, midIv)) {
    return false;
  }

  vector<StmtPtr> midCore;
  for (size_t i = 0; i < kWhileIdx; ++i) {
    if (StmtPtr c = cloneStmt(midBody->items[i].get())) {
      midCore.push_back(std::move(c));
    }
  }
  if (StmtPtr kAssign = cloneStmt(kBody->items[0].get())) {
    auto kLoopBody = make_unique<BlockStmt>(midW->line);
    kLoopBody->items.push_back(std::move(kAssign));
    kLoopBody->items.push_back(makeIncByOne(midW->line, kIv));
    auto kLoop =
        make_unique<WhileStmt>(midW->line, cloneExpr(kW->cond.get()), nullptr);
    kLoop->body = std::move(kLoopBody);
    midCore.push_back(std::move(kLoop));
  } else {
    return false;
  }
  for (size_t i = kWhileIdx + 1; i + 1 < midBody->items.size(); ++i) {
    if (StmtPtr c = cloneStmt(midBody->items[i].get())) {
      midCore.push_back(std::move(c));
    }
  }
  midCore.push_back(makeIncByOne(midW->line, midIv));

  if (boundsTooSmallForTiling(limit.get(), midLimit.get())) {
    return false;
  }
  if (exprUsesVarName(midLimit.get(), outerIv) ||
      exprUsesVarName(kLimit.get(), outerIv) || exprUsesVarName(kLimit.get(), midIv) ||
      exprUsesVarName(limit.get(), midIv)) {
    return false;
  }

  const bool midWasDecl =
      dynamic_cast<const DeclStmt *>(outerBody->items[0].get()) != nullptr;
  bool declareMid = midWasDecl;
  if (declareMid && !gHoistedLoopIvs.insert(midIv).second) {
    declareMid = false;
  }
  const int nestId = gTileNestId++;
  int line = outerW->line;
  vector<StmtPtr> rep;
  rep.push_back(makeTileVarDecl(line, nestId, outerIv, midIv, declareMid));
  rep.push_back(makeZeroAssign(line, tileVar(outerIv, nestId)));
  rep.push_back(buildTiled2D(line, nestId, outerIv, midIv, limit.get(),
                             midLimit.get(), std::move(midCore)));
  items.erase(items.begin() + static_cast<ptrdiff_t>(k),
              items.begin() + static_cast<ptrdiff_t>(k + 2));
  items.insert(items.begin() + static_cast<ptrdiff_t>(k),
               std::make_move_iterator(rep.begin()),
               std::make_move_iterator(rep.end()));
  return true;
}

static void processBlockTiling(BlockStmt *blk) {
  if (!blk) {
    return;
  }
  for (size_t i = 0; i + 1 < blk->items.size();) {
    if (tryTile3DMatmul(blk->items, i)) {
      i += 3;
      continue;
    }
    if (tryTile2DNest(blk->items, i)) {
      i += 3;
      continue;
    }
    ++i;
  }
  for (auto &it : blk->items) {
    if (it->kind == StmtKind::Block) {
      processBlockTiling(static_cast<BlockStmt *>(it.get()));
    } else if (it->kind == StmtKind::While) {
      processBlockTiling(
          static_cast<BlockStmt *>(static_cast<WhileStmt *>(it.get())->body.get()));
    } else if (it->kind == StmtKind::If) {
      auto *ifs = static_cast<IfStmt *>(it.get());
      if (ifs->thenStmt && ifs->thenStmt->kind == StmtKind::Block) {
        processBlockTiling(static_cast<BlockStmt *>(ifs->thenStmt.get()));
      }
      if (ifs->elseStmt && ifs->elseStmt->kind == StmtKind::Block) {
        processBlockTiling(static_cast<BlockStmt *>(ifs->elseStmt.get()));
      }
    }
  }
}

} // namespace

void loopTilingPass(Program &program) {
  if (envFlagTruthy("SYSY_CC_NO_LOOP_TILING")) {
    return;
  }
  for (auto &item : program.items) {
    if (!item.func || !item.func->body) {
      continue;
    }
    gTileNestId = 0;
    gHoistedLoopIvs.clear();
    processBlockTiling(item.func->body.get());
  }
}
