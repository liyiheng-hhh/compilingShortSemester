// AST 层循环分块：矩形二重循环（可含首部 continue-skip if）、
// i-j-k 与 k-i-j 三重循环（默认 tile=32，可用 SYSY_TILE_SIZE 覆盖）。
// 在 loopInterchangePass 之后运行；SYSY_CC_NO_LOOP_TILING=1 可关闭。

#include "loop_interchange.h"

#include "common.h"

#include <cstddef>
#include <cstdlib>
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

static int loopTileSize() {
  static const int k = [] {
    const char *v = std::getenv("SYSY_TILE_SIZE");
    if (!v || v[0] == '\0') {
      return 32;
    }
    char *end = nullptr;
    const long n = std::strtol(v, &end, 10);
    if (end == v || n < 4 || n > 128) {
      return 32;
    }
    const int z = static_cast<int>(n);
    return (z & (z - 1)) == 0 ? z : 32;
  }();
  return k;
}

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
  if (auto *ifs = dynamic_cast<const IfStmt *>(s)) {
    StmtPtr thenS = ifs->thenStmt ? cloneStmt(ifs->thenStmt.get()) : nullptr;
    StmtPtr elseS = ifs->elseStmt ? cloneStmt(ifs->elseStmt.get()) : nullptr;
    return make_unique<IfStmt>(ifs->line, cloneExpr(ifs->cond.get()), std::move(thenS),
                               std::move(elseS));
  }
  if (auto *blk = dynamic_cast<const BlockStmt *>(s)) {
    auto nb = make_unique<BlockStmt>(blk->line);
    for (const auto &it : blk->items) {
      if (StmtPtr c = cloneStmt(it.get())) {
        nb->items.push_back(std::move(c));
      }
    }
    return nb;
  }
  if (auto *cont = dynamic_cast<const ContinueStmt *>(s)) {
    return make_unique<ContinueStmt>(cont->line);
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

static StmtPtr makeBreakUnless(int line, ExprPtr cond) {
  auto neg = make_unique<UnaryExpr>(line, "!", std::move(cond));
  return make_unique<IfStmt>(line, std::move(neg), make_unique<BreakStmt>(line),
                             nullptr);
}

// 分块内层 while：用 while(1)+break 代替 iv<lim && iv<tileEnd，避免 IR 与行指针失效。
static unique_ptr<WhileStmt> makeTiledWhileShell(int line, const string &iv,
                                                 const string &tileIv,
                                                 const Expr *limit,
                                                 unique_ptr<BlockStmt> body) {
  auto shell = make_unique<BlockStmt>(line);
  shell->items.push_back(
      makeBreakUnless(line, makeLt(line, iv, cloneExpr(limit))));
  auto tileEnd = make_unique<BinaryExpr>(
      line, "+", makeScalarLVal(line, tileIv), makeInt(line, loopTileSize()));
  shell->items.push_back(makeBreakUnless(
      line, make_unique<BinaryExpr>(line, "<", makeScalarLVal(line, iv),
                                    std::move(tileEnd))));
  for (auto &st : body->items) {
    shell->items.push_back(std::move(st));
  }
  return make_unique<WhileStmt>(line, makeInt(line, 1), std::move(shell));
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

struct OuterLoopHead {
  string iv;
  ExprPtr limit;
  bool ivFromDecl = false;
};

static bool matchOuterLoopHead(const Stmt *initStmt, const WhileStmt *outerW,
                               OuterLoopHead *out) {
  if (!initStmt || !outerW || !out) {
    return false;
  }
  if (!extractLtBound(outerW->cond.get(), &out->iv, &out->limit)) {
    return false;
  }
  out->ivFromDecl = false;
  if (auto *as = dynamic_cast<const AssignStmt *>(initStmt)) {
    return isZeroScalarAssign(as, out->iv);
  }
  if (auto *d = dynamic_cast<const DeclStmt *>(initStmt)) {
    if (d->defs.size() != 1 || d->base != BaseType::Int) {
      return false;
    }
    string name;
    if (!parseZeroInit(d, &name) || name != out->iv) {
      return false;
    }
    out->ivFromDecl = true;
    return true;
  }
  return false;
}

static void stripDeclZeroInit(Stmt *s) {
  if (auto *d = dynamic_cast<DeclStmt *>(s)) {
    if (d->defs.size() == 1) {
      d->defs[0].init.reset();
    }
  }
}

static void insertTiledLoopReplace(vector<StmtPtr> &items, size_t k,
                                   bool keepOuterDecl, vector<StmtPtr> rep) {
  if (keepOuterDecl) {
    stripDeclZeroInit(items[k].get());
    items.erase(items.begin() + static_cast<ptrdiff_t>(k + 1),
                items.begin() + static_cast<ptrdiff_t>(k + 2));
    items.insert(items.begin() + static_cast<ptrdiff_t>(k + 1),
                 std::make_move_iterator(rep.begin()),
                 std::make_move_iterator(rep.end()));
  } else {
    items.erase(items.begin() + static_cast<ptrdiff_t>(k),
                items.begin() + static_cast<ptrdiff_t>(k + 2));
    items.insert(items.begin() + static_cast<ptrdiff_t>(k),
                 std::make_move_iterator(rep.begin()),
                 std::make_move_iterator(rep.end()));
  }
}

static bool stmtIsContinue(const Stmt *s) {
  return s && s->kind == StmtKind::Continue;
}

// then 分支为 iv++ 后 continue（或仅 continue，由外层再统一 iv++）。
static bool thenBranchIsSkipContinue(const Stmt *thenStmt, const string &iv) {
  if (!thenStmt) {
    return false;
  }
  if (stmtIsContinue(thenStmt)) {
    return true;
  }
  auto *blk = dynamic_cast<const BlockStmt *>(thenStmt);
  if (!blk) {
    return isIncByOne(dynamic_cast<const AssignStmt *>(thenStmt), iv);
  }
  if (blk->items.size() == 1) {
    return stmtIsContinue(blk->items[0].get());
  }
  if (blk->items.size() == 2) {
    return isIncByOne(dynamic_cast<const AssignStmt *>(blk->items[0].get()), iv) &&
           stmtIsContinue(blk->items[1].get());
  }
  return false;
}

static bool isContinueSkipIf(const Stmt *s, const string &iv) {
  auto *ifs = dynamic_cast<const IfStmt *>(s);
  return ifs && !ifs->elseStmt && thenBranchIsSkipContinue(ifs->thenStmt.get(), iv);
}

static bool isSimpleInnerLoopBody(const BlockStmt *innerBody, const string &innerIv) {
  if (!innerBody || innerBody->items.size() != 2) {
    return false;
  }
  return innerBody->items[0]->kind == StmtKind::Assign &&
         isIncByOne(dynamic_cast<const AssignStmt *>(innerBody->items[1].get()), innerIv);
}

// 内层体：可选首部 continue-skip if，若干 decl/assign，末尾 iv++。
static bool collectInnerLoopCore(const BlockStmt *innerBody, const string &innerIv,
                                 vector<StmtPtr> *out) {
  if (!innerBody || innerBody->items.size() < 2) {
    return false;
  }
  if (!isIncByOne(dynamic_cast<const AssignStmt *>(innerBody->items.back().get()), innerIv)) {
    return false;
  }
  out->clear();
  size_t pos = 0;
  if (pos + 1 < innerBody->items.size() &&
      isContinueSkipIf(innerBody->items[pos].get(), innerIv)) {
    if (StmtPtr c = cloneStmt(innerBody->items[pos].get())) {
      out->push_back(std::move(c));
    } else {
      return false;
    }
    ++pos;
  }
  for (; pos + 1 < innerBody->items.size(); ++pos) {
    const Stmt *st = innerBody->items[pos].get();
    switch (st->kind) {
    case StmtKind::Assign:
    case StmtKind::Decl:
    case StmtKind::Expr:
      if (StmtPtr c = cloneStmt(st)) {
        out->push_back(std::move(c));
      } else {
        return false;
      }
      break;
    default:
      return false;
    }
  }
  return !out->empty();
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
    return n && !n->isFloat && n->intVal < loopTileSize();
  };
  return small(outerLimit) && small(innerLimit);
}

static unique_ptr<WhileStmt>
buildTiled2D(int line, int nestId, const string &outerIv, const string &innerIv,
             const Expr *outerLimit, const Expr *innerLimit, vector<StmtPtr> core,
             vector<StmtPtr> iBodyPrefix = {}) {
  const string ii = tileVar(outerIv, nestId);
  const string jj = tileVar(innerIv, nestId);

  auto innerCore = make_unique<BlockStmt>(line);
  for (auto &st : core) {
    innerCore->items.push_back(std::move(st));
  }
  innerCore->items.push_back(makeIncByOne(line, innerIv));

  auto iBody = make_unique<BlockStmt>(line);
  for (auto &st : iBodyPrefix) {
    iBody->items.push_back(std::move(st));
  }
  iBody->items.push_back(makeAssign(line, innerIv, makeScalarLVal(line, jj)));
  iBody->items.push_back(
      makeTiledWhileShell(line, innerIv, jj, innerLimit, std::move(innerCore)));
  iBody->items.push_back(makeIncByOne(line, outerIv));

  auto jTileBody = make_unique<BlockStmt>(line);
  jTileBody->items.push_back(makeAssign(line, outerIv, makeScalarLVal(line, ii)));
  jTileBody->items.push_back(
      makeTiledWhileShell(line, outerIv, ii, outerLimit, std::move(iBody)));
  jTileBody->items.push_back(makeIncBy(line, jj, loopTileSize()));

  auto jjWhile =
      make_unique<WhileStmt>(line, makeLt(line, jj, cloneExpr(innerLimit)), nullptr);
  jjWhile->body = std::move(jTileBody);

  auto iiBody = make_unique<BlockStmt>(line);
  iiBody->items.push_back(makeZeroAssign(line, jj));
  iiBody->items.push_back(std::move(jjWhile));
  iiBody->items.push_back(makeIncBy(line, ii, loopTileSize()));

  auto iiWhile =
      make_unique<WhileStmt>(line, makeLt(line, ii, cloneExpr(outerLimit)), nullptr);
  iiWhile->body = std::move(iiBody);
  return iiWhile;
}

static bool tryTile2DNest(vector<StmtPtr> &items, size_t k) {
  if (k + 1 >= items.size()) {
    return false;
  }
  auto *outerW = dynamic_cast<WhileStmt *>(items[k + 1].get());
  OuterLoopHead head;
  if (!matchOuterLoopHead(items[k].get(), outerW, &head)) {
    return false;
  }
  const string &outerIv = head.iv;
  const ExprPtr &outerLimit = head.limit;
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
  vector<StmtPtr> core;
  if (!collectInnerLoopCore(innerBody, innerIv, &core)) {
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
  insertTiledLoopReplace(items, k, head.ivFromDecl, std::move(rep));
  return true;
}

// k 外层：while(k){ i=0; while(i){ [skip?] j=0; while(j){…} i++ } k++ }（01_mm2 风格）
static bool tryTile3DKOuter(vector<StmtPtr> &items, size_t k) {
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
  if (!midBody || midBody->items.size() < 3) {
    return false;
  }
  pos = 0;
  if (pos < midBody->items.size() && isContinueSkipIf(midBody->items[pos].get(), midIv)) {
    ++pos;
  }
  string innerIv;
  if (!parseZeroInit(midBody->items[pos].get(), &innerIv)) {
    return false;
  }
  ++pos;
  auto *innerW = dynamic_cast<WhileStmt *>(midBody->items[pos].get());
  if (!innerW) {
    return false;
  }
  ++pos;
  auto *midInc = dynamic_cast<AssignStmt *>(midBody->items[pos].get());
  if (!isIncByOne(midInc, midIv) || pos + 1 != midBody->items.size()) {
    return false;
  }
  string innerIv2;
  ExprPtr innerLimit;
  if (!extractLtBound(innerW->cond.get(), &innerIv2, &innerLimit) || innerIv2 != innerIv) {
    return false;
  }
  auto *innerBody = dynamic_cast<BlockStmt *>(innerW->body.get());
  if (!isSimpleInnerLoopBody(innerBody, innerIv)) {
    return false;
  }

  if (boundsTooSmallForTiling(outerLimit.get(), midLimit.get())) {
    return false;
  }
  if (exprUsesVarName(midLimit.get(), outerIv) ||
      exprUsesVarName(innerLimit.get(), outerIv) ||
      exprUsesVarName(innerLimit.get(), midIv) ||
      exprUsesVarName(outerLimit.get(), midIv)) {
    return false;
  }

  vector<StmtPtr> midCore;
  vector<StmtPtr> iBodyPrefix;
  pos = 0;
  if (pos < midBody->items.size() && isContinueSkipIf(midBody->items[pos].get(), midIv)) {
    if (StmtPtr c = cloneStmt(midBody->items[pos].get())) {
      iBodyPrefix.push_back(std::move(c));
    } else {
      return false;
    }
    ++pos;
  }
  // 跳过 j=0 初始化：buildTiled2D 在每个 tile 内会执行 j=jj。
  // 只保留内层循环体赋值；勿再包一层 while(j)（buildTiled2D 已生成 j 循环并追加 j++）。
  ++pos;
  if (StmtPtr jAssign = cloneStmt(innerBody->items[0].get())) {
    midCore.push_back(std::move(jAssign));
  } else {
    return false;
  }
  // midIv++ 由 buildTiled2D 统一追加，勿重复

  const bool midWasDecl =
      dynamic_cast<const DeclStmt *>(outerBody->items[0].get()) != nullptr;
  bool declareMid = midWasDecl;
  if (declareMid && !gHoistedLoopIvs.insert(midIv).second) {
    declareMid = false;
  }
  const int nestId = gTileNestId++;
  int line = outerW->line;
  // 只分块中层 i × 内层 j；k 外层保持原序，末尾保留 k++（勿用 buildTiled2D(k,i) 以免 k 在内层分块里多次自增）。
  auto tiledIJ = buildTiled2D(line, nestId, midIv, innerIv, midLimit.get(),
                              innerLimit.get(), std::move(midCore),
                              std::move(iBodyPrefix));
  auto kBody = make_unique<BlockStmt>(line);
  // 每个 k 迭代须从 ii=0 重扫 i×j 分块（否则第二次 k 起 ii 仍为 n）。
  kBody->items.push_back(makeZeroAssign(line, tileVar(midIv, nestId)));
  kBody->items.push_back(std::move(tiledIJ));
  if (StmtPtr kInc = cloneStmt(outerInc)) {
    kBody->items.push_back(std::move(kInc));
  } else {
    return false;
  }
  auto kWhile =
      make_unique<WhileStmt>(line, cloneExpr(outerW->cond.get()), nullptr);
  kWhile->body = std::move(kBody);

  vector<StmtPtr> rep;
  if (StmtPtr kInit = cloneStmt(outerInit)) {
    rep.push_back(std::move(kInit));
  } else {
    return false;
  }
  rep.push_back(makeTileVarDecl(line, nestId, midIv, innerIv, declareMid));
  rep.push_back(makeZeroAssign(line, tileVar(midIv, nestId)));
  rep.push_back(std::move(kWhile));
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
  auto *outerW = dynamic_cast<WhileStmt *>(items[k + 1].get());
  OuterLoopHead head;
  if (!matchOuterLoopHead(items[k].get(), outerW, &head)) {
    return false;
  }
  const string &outerIv = head.iv;
  const ExprPtr &limit = head.limit;
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
  // innerIv(j)++ 由 buildTiled2D 统一追加，勿重复

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
  insertTiledLoopReplace(items, k, head.ivFromDecl, std::move(rep));
  return true;
}

static void processBlockTiling(BlockStmt *blk) {
  if (!blk) {
    return;
  }
  for (size_t i = 0; i + 1 < blk->items.size();) {
    if (tryTile3DKOuter(blk->items, i)) {
      i += 4;
      continue;
    }
    if (tryTile3DMatmul(blk->items, i)) {
      const bool outerDecl =
          dynamic_cast<const DeclStmt *>(blk->items[i].get()) != nullptr;
      i += outerDecl ? 4 : 3;
      continue;
    }
    if (tryTile2DNest(blk->items, i)) {
      const bool outerDecl =
          dynamic_cast<const DeclStmt *>(blk->items[i].get()) != nullptr;
      i += outerDecl ? 4 : 3;
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
