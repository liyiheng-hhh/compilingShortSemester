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

static int ltileLoopTileSize() {
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

static void ltileCopyExprMeta(Expr *dst, const Expr *src) {
  if (!dst || !src) {
    return;
  }
  dst->type = src->type;
  dst->isConst = src->isConst;
  dst->constVal = src->constVal;
}

static ExprPtr ltileCloneExpr(const Expr *e) {
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
      c->indices.push_back(ltileCloneExpr(ix.get()));
    }
    out = std::move(c);
    break;
  }
  case ExprKind::Unary: {
    auto *u = static_cast<const UnaryExpr *>(e);
    out = make_unique<UnaryExpr>(u->line, u->op, ltileCloneExpr(u->expr.get()));
    break;
  }
  case ExprKind::Binary: {
    auto *b = static_cast<const BinaryExpr *>(e);
    out = make_unique<BinaryExpr>(b->line, b->op, ltileCloneExpr(b->lhs.get()),
                                    ltileCloneExpr(b->rhs.get()));
    break;
  }
  case ExprKind::Call: {
    auto *c = static_cast<const CallExpr *>(e);
    auto n = make_unique<CallExpr>(c->line, c->name);
    n->function = c->function;
    for (const auto &a : c->args) {
      n->args.push_back(ltileCloneExpr(a.get()));
    }
    out = std::move(n);
    break;
  }
  default:
    return nullptr;
  }
  ltileCopyExprMeta(out.get(), e);
  return out;
}

static StmtPtr ltileCloneStmt(const Stmt *s) {
  if (!s) {
    return nullptr;
  }
  if (auto *as = dynamic_cast<const AssignStmt *>(s)) {
    auto lhs = make_unique<LValExpr>(as->line, as->lhs->name);
    lhs->symbol = as->lhs->symbol;
    for (const auto &ix : as->lhs->indices) {
      lhs->indices.push_back(ltileCloneExpr(ix.get()));
    }
    return make_unique<AssignStmt>(as->line, std::move(lhs), ltileCloneExpr(as->rhs.get()));
  }
  if (auto *es = dynamic_cast<const ExprStmt *>(s)) {
    return make_unique<ExprStmt>(es->line, ltileCloneExpr(es->expr.get()));
  }
  if (auto *d = dynamic_cast<const DeclStmt *>(s)) {
    auto nd = make_unique<DeclStmt>(d->line, d->isConst, d->base);
    for (const auto &def : d->defs) {
      VarDef vd;
      vd.name = def.name;
      vd.symbol = def.symbol;
      vd.line = def.line;
      for (const auto &dim : def.dims) {
        vd.dims.push_back(ltileCloneExpr(dim.get()));
      }
      if (def.init) {
        vd.init = make_unique<InitVal>();
        vd.init->isList = def.init->isList;
        if (def.init->expr) {
          vd.init->expr = ltileCloneExpr(def.init->expr.get());
        }
      }
      nd->defs.push_back(std::move(vd));
    }
    return nd;
  }
  if (auto *ifs = dynamic_cast<const IfStmt *>(s)) {
    StmtPtr thenS = ifs->thenStmt ? ltileCloneStmt(ifs->thenStmt.get()) : nullptr;
    StmtPtr elseS = ifs->elseStmt ? ltileCloneStmt(ifs->elseStmt.get()) : nullptr;
    return make_unique<IfStmt>(ifs->line, ltileCloneExpr(ifs->cond.get()), std::move(thenS),
                               std::move(elseS));
  }
  if (auto *blk = dynamic_cast<const BlockStmt *>(s)) {
    auto nb = make_unique<BlockStmt>(blk->line);
    for (const auto &it : blk->items) {
      if (StmtPtr c = ltileCloneStmt(it.get())) {
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

static unique_ptr<LValExpr> ltileMakeScalarLVal(int line, const string &name) {
  return make_unique<LValExpr>(line, name);
}

static unique_ptr<NumberExpr> ltileMakeInt(int line, int v) {
  return make_unique<NumberExpr>(line, v);
}

static unique_ptr<AssignStmt> ltileMakeAssign(int line, const string &lhs,
                                         ExprPtr rhs) {
  return make_unique<AssignStmt>(line, ltileMakeScalarLVal(line, lhs),
                                 std::move(rhs));
}

static unique_ptr<AssignStmt> ltileMakeZeroAssign(int line, const string &v) {
  return ltileMakeAssign(line, v, ltileMakeInt(line, 0));
}

static unique_ptr<AssignStmt> ltileMakeIncBy(int line, const string &v, int delta) {
  auto add = make_unique<BinaryExpr>(
      line, "+", ltileMakeScalarLVal(line, v), ltileMakeInt(line, delta));
  return make_unique<AssignStmt>(line, ltileMakeScalarLVal(line, v), std::move(add));
}

static unique_ptr<AssignStmt> ltileMakeIncByOne(int line, const string &v) {
  return ltileMakeIncBy(line, v, 1);
}

static ExprPtr ltileMakeLt(int line, const string &lhs, ExprPtr rhs) {
  return make_unique<BinaryExpr>(line, "<", ltileMakeScalarLVal(line, lhs),
                                 std::move(rhs));
}

static StmtPtr ltileMakeBreakUnless(int line, ExprPtr cond) {
  auto neg = make_unique<UnaryExpr>(line, "!", std::move(cond));
  return make_unique<IfStmt>(line, std::move(neg), make_unique<BreakStmt>(line),
                             nullptr);
}

// 分块内层 while：用 while(1)+break 代替 iv<lim && iv<tileEnd，避免 IR 与行指针失效。
static unique_ptr<WhileStmt> ltileMakeTiledWhileShell(int line, const string &iv,
                                                 const string &tileIv,
                                                 const Expr *limit,
                                                 unique_ptr<BlockStmt> body) {
  auto shell = make_unique<BlockStmt>(line);
  shell->items.push_back(
      ltileMakeBreakUnless(line, ltileMakeLt(line, iv, ltileCloneExpr(limit))));
  auto tileEnd = make_unique<BinaryExpr>(
      line, "+", ltileMakeScalarLVal(line, tileIv), ltileMakeInt(line, ltileLoopTileSize()));
  shell->items.push_back(ltileMakeBreakUnless(
      line, make_unique<BinaryExpr>(line, "<", ltileMakeScalarLVal(line, iv),
                                    std::move(tileEnd))));
  for (auto &st : body->items) {
    shell->items.push_back(std::move(st));
  }
  return make_unique<WhileStmt>(line, ltileMakeInt(line, 1), std::move(shell));
}

static bool ltileExprUsesVarName(const Expr *e, const string &name) {
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
      if (ltileExprUsesVarName(ix.get(), name)) {
        return true;
      }
    }
    return false;
  }
  case ExprKind::Unary:
    return ltileExprUsesVarName(static_cast<const UnaryExpr *>(e)->expr.get(), name);
  case ExprKind::Binary: {
    auto *b = static_cast<const BinaryExpr *>(e);
    return ltileExprUsesVarName(b->lhs.get(), name) || ltileExprUsesVarName(b->rhs.get(), name);
  }
  default:
    return false;
  }
}

static bool ltileExtractLtBound(const Expr *cond, string *iv, ExprPtr *limit) {
  auto *b = dynamic_cast<const BinaryExpr *>(cond);
  if (!b || b->op != "<") {
    return false;
  }
  if (auto *lv = dynamic_cast<const LValExpr *>(b->lhs.get())) {
    if (!lv->indices.empty()) {
      return false;
    }
    *iv = lv->name;
    *limit = ltileCloneExpr(b->rhs.get());
    return true;
  }
  if (auto *lv = dynamic_cast<const LValExpr *>(b->rhs.get())) {
    if (!lv->indices.empty()) {
      return false;
    }
    *iv = lv->name;
    *limit = ltileCloneExpr(b->lhs.get());
    return true;
  }
  return false;
}

static bool ltileIsIncByOne(const AssignStmt *as, const string &v) {
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

static bool ltileIsZeroScalarAssign(const AssignStmt *as, const string &v) {
  if (!as || !as->lhs || as->lhs->name != v || !as->lhs->indices.empty()) {
    return false;
  }
  auto *n = dynamic_cast<const NumberExpr *>(as->rhs.get());
  return n && !n->isFloat && n->intVal == 0;
}

static bool ltileParseZeroInit(const Stmt *s, string *name) {
  if (auto *as = dynamic_cast<const AssignStmt *>(s)) {
    if (as->lhs && as->lhs->indices.empty() &&
        ltileIsZeroScalarAssign(as, as->lhs->name)) {
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

struct LtileOuterLoopHead {
  string iv;
  ExprPtr limit;
  bool ivFromDecl = false;
};

static bool ltileMatchOuterLoopHead(const Stmt *initStmt, const WhileStmt *outerW,
                               LtileOuterLoopHead *out) {
  if (!initStmt || !outerW || !out) {
    return false;
  }
  if (!ltileExtractLtBound(outerW->cond.get(), &out->iv, &out->limit)) {
    return false;
  }
  out->ivFromDecl = false;
  if (auto *as = dynamic_cast<const AssignStmt *>(initStmt)) {
    return ltileIsZeroScalarAssign(as, out->iv);
  }
  if (auto *d = dynamic_cast<const DeclStmt *>(initStmt)) {
    if (d->defs.size() != 1 || d->base != BaseType::Int) {
      return false;
    }
    string name;
    if (!ltileParseZeroInit(d, &name) || name != out->iv) {
      return false;
    }
    out->ivFromDecl = true;
    return true;
  }
  return false;
}

static void ltileStripDeclZeroInit(Stmt *s) {
  if (auto *d = dynamic_cast<DeclStmt *>(s)) {
    if (d->defs.size() == 1) {
      d->defs[0].init.reset();
    }
  }
}

static void ltileInsertTiledLoopReplace(vector<StmtPtr> &items, size_t k,
                                   bool keepOuterDecl, vector<StmtPtr> rep) {
  if (keepOuterDecl) {
    ltileStripDeclZeroInit(items[k].get());
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

static bool ltileStmtIsContinue(const Stmt *s) {
  return s && s->kind == StmtKind::Continue;
}

// then 分支为 iv++ 后 continue（或仅 continue，由外层再统一 iv++）。
static bool ltileThenBranchIsSkipContinue(const Stmt *thenStmt, const string &iv) {
  if (!thenStmt) {
    return false;
  }
  if (ltileStmtIsContinue(thenStmt)) {
    return true;
  }
  auto *blk = dynamic_cast<const BlockStmt *>(thenStmt);
  if (!blk) {
    return ltileIsIncByOne(dynamic_cast<const AssignStmt *>(thenStmt), iv);
  }
  if (blk->items.size() == 1) {
    return ltileStmtIsContinue(blk->items[0].get());
  }
  if (blk->items.size() == 2) {
    return ltileIsIncByOne(dynamic_cast<const AssignStmt *>(blk->items[0].get()), iv) &&
           ltileStmtIsContinue(blk->items[1].get());
  }
  return false;
}

static bool ltileIsContinueSkipIf(const Stmt *s, const string &iv) {
  auto *ifs = dynamic_cast<const IfStmt *>(s);
  return ifs && !ifs->elseStmt && ltileThenBranchIsSkipContinue(ifs->thenStmt.get(), iv);
}

static bool ltileIsSimpleInnerLoopBody(const BlockStmt *innerBody, const string &innerIv) {
  if (!innerBody || innerBody->items.size() != 2) {
    return false;
  }
  return innerBody->items[0]->kind == StmtKind::Assign &&
         ltileIsIncByOne(dynamic_cast<const AssignStmt *>(innerBody->items[1].get()), innerIv);
}

static bool ltileLvalIndexIsIv(const Expr *e, const string &iv) {
  auto *lv = dynamic_cast<const LValExpr *>(e);
  return lv && lv->name == iv && lv->indices.empty();
}

// B[i][j]=C[i][j] 等同索引拷贝：k-i-j 分块曾导致 h-10 类用例 WA，勿 tile。
static bool ltileIsSameIndices2DArrayCopy(const AssignStmt *as, const string &rowIv,
                                   const string &colIv) {
  if (!as) {
    return false;
  }
  auto *lhs = dynamic_cast<const LValExpr *>(as->lhs.get());
  auto *rhs = dynamic_cast<const LValExpr *>(as->rhs.get());
  if (!lhs || !rhs || lhs->indices.size() != 2 || rhs->indices.size() != 2) {
    return false;
  }
  if (lhs->name == rhs->name) {
    return false;
  }
  return ltileLvalIndexIsIv(lhs->indices[0].get(), rowIv) &&
         ltileLvalIndexIsIv(lhs->indices[1].get(), colIv) &&
         ltileLvalIndexIsIv(rhs->indices[0].get(), rowIv) &&
         ltileLvalIndexIsIv(rhs->indices[1].get(), colIv);
}

// acc = acc + A[i][j]（或反向加）：分块改变浮点累加顺序 → h-10 等 WA。
static bool ltileIsScalar2DReductionAccum(const AssignStmt *as, const string &rowIv,
                                   const string &colIv) {
  if (!as || !as->lhs || !as->lhs->indices.empty()) {
    return false;
  }
  auto *add = dynamic_cast<const BinaryExpr *>(as->rhs.get());
  if (!add || add->op != "+") {
    return false;
  }
  const string &acc = as->lhs->name;
  auto matchAcc = [&](const Expr *e) {
    auto *lv = dynamic_cast<const LValExpr *>(e);
    return lv && lv->name == acc && lv->indices.empty();
  };
  auto matchArr = [&](const Expr *e) {
    auto *lv = dynamic_cast<const LValExpr *>(e);
    return lv && lv->indices.size() == 2 &&
           ltileLvalIndexIsIv(lv->indices[0].get(), rowIv) &&
           ltileLvalIndexIsIv(lv->indices[1].get(), colIv);
  };
  return (matchAcc(add->lhs.get()) && matchArr(add->rhs.get())) ||
         (matchArr(add->lhs.get()) && matchAcc(add->rhs.get()));
}

static bool ltileMatchLv2Indices(const Expr *e, const string &iIv, const string &jIv,
                            string *sym) {
  auto *lv = dynamic_cast<const LValExpr *>(e);
  if (!lv || lv->indices.size() != 2) {
    return false;
  }
  if (!ltileLvalIndexIsIv(lv->indices[0].get(), iIv) ||
      !ltileLvalIndexIsIv(lv->indices[1].get(), jIv)) {
    return false;
  }
  *sym = lv->name;
  return true;
}

// 01_mm：C[i][j]=C[i][j]*A[i][k]+B[k][j]（k 外层）— 分块后每 j 仍重复算 A[i][k]，勿 tile。
static bool ltileMatch01MmRank1Assign(const AssignStmt *as, const string &iIv,
                                 const string &jIv, const string &kIv,
                                 string *cSym, string *aSym, string *bSym) {
  if (!as) {
    return false;
  }
  if (!ltileMatchLv2Indices(as->lhs.get(), iIv, jIv, cSym)) {
    return false;
  }
  auto *add = dynamic_cast<const BinaryExpr *>(as->rhs.get());
  if (!add || add->op != "+") {
    return false;
  }
  auto *mul = dynamic_cast<const BinaryExpr *>(add->lhs.get());
  if (!mul || mul->op != "*") {
    return false;
  }
  string c2, a2;
  if (!ltileMatchLv2Indices(mul->lhs.get(), iIv, jIv, &c2) || c2 != *cSym) {
    return false;
  }
  if (!ltileMatchLv2Indices(mul->rhs.get(), iIv, kIv, &a2)) {
    return false;
  }
  if (!ltileMatchLv2Indices(add->rhs.get(), kIv, jIv, bSym)) {
    return false;
  }
  *aSym = a2;
  return true;
}

// 内层体：可选首部 continue-skip if，若干 decl/assign，末尾 iv++。
static bool ltileCollectInnerLoopCore(const BlockStmt *innerBody, const string &innerIv,
                                 vector<StmtPtr> *out) {
  if (!innerBody || innerBody->items.size() < 2) {
    return false;
  }
  if (!ltileIsIncByOne(dynamic_cast<const AssignStmt *>(innerBody->items.back().get()), innerIv)) {
    return false;
  }
  out->clear();
  size_t pos = 0;
  if (pos + 1 < innerBody->items.size() &&
      ltileIsContinueSkipIf(innerBody->items[pos].get(), innerIv)) {
    if (StmtPtr c = ltileCloneStmt(innerBody->items[pos].get())) {
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
      if (StmtPtr c = ltileCloneStmt(st)) {
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

static int ltileNestId = 0;
static unordered_set<string> ltileHoistedLoopIvs;

static string ltileTileVar(const string &iv, int nestId) {
  return string("_t") + iv + "_" + std::to_string(nestId);
}

static StmtPtr ltileMakeTileVarDecl(int line, int nestId, const string &outerIv,
                               const string &innerIv, bool declareInnerIv) {
  auto d = make_unique<DeclStmt>(line, false, BaseType::Int);
  VarDef vo;
  vo.name = ltileTileVar(outerIv, nestId);
  vo.line = line;
  d->defs.push_back(std::move(vo));
  VarDef vt;
  vt.name = ltileTileVar(innerIv, nestId);
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
static bool ltileBoundsTooSmallForTiling(const Expr *outerLimit,
                                    const Expr *innerLimit) {
  auto small = [](const Expr *e) {
    auto *n = dynamic_cast<const NumberExpr *>(e);
    return n && !n->isFloat && n->intVal < ltileLoopTileSize();
  };
  return small(outerLimit) && small(innerLimit);
}

static unique_ptr<WhileStmt>
ltileBuildTiled2D(int line, int nestId, const string &outerIv, const string &innerIv,
             const Expr *outerLimit, const Expr *innerLimit, vector<StmtPtr> core,
             vector<StmtPtr> iBodyPrefix = {}) {
  const string ii = ltileTileVar(outerIv, nestId);
  const string jj = ltileTileVar(innerIv, nestId);

  auto innerCore = make_unique<BlockStmt>(line);
  for (auto &st : core) {
    innerCore->items.push_back(std::move(st));
  }
  innerCore->items.push_back(ltileMakeIncByOne(line, innerIv));

  auto iBody = make_unique<BlockStmt>(line);
  for (auto &st : iBodyPrefix) {
    iBody->items.push_back(std::move(st));
  }
  iBody->items.push_back(ltileMakeAssign(line, innerIv, ltileMakeScalarLVal(line, jj)));
  iBody->items.push_back(
      ltileMakeTiledWhileShell(line, innerIv, jj, innerLimit, std::move(innerCore)));
  iBody->items.push_back(ltileMakeIncByOne(line, outerIv));

  auto jTileBody = make_unique<BlockStmt>(line);
  jTileBody->items.push_back(ltileMakeAssign(line, outerIv, ltileMakeScalarLVal(line, ii)));
  jTileBody->items.push_back(
      ltileMakeTiledWhileShell(line, outerIv, ii, outerLimit, std::move(iBody)));
  jTileBody->items.push_back(ltileMakeIncBy(line, jj, ltileLoopTileSize()));

  auto jjWhile =
      make_unique<WhileStmt>(line, ltileMakeLt(line, jj, ltileCloneExpr(innerLimit)), nullptr);
  jjWhile->body = std::move(jTileBody);

  auto iiBody = make_unique<BlockStmt>(line);
  iiBody->items.push_back(ltileMakeZeroAssign(line, jj));
  iiBody->items.push_back(std::move(jjWhile));
  iiBody->items.push_back(ltileMakeIncBy(line, ii, ltileLoopTileSize()));

  auto iiWhile =
      make_unique<WhileStmt>(line, ltileMakeLt(line, ii, ltileCloneExpr(outerLimit)), nullptr);
  iiWhile->body = std::move(iiBody);
  return iiWhile;
}

static bool ltileTryTile2DNest(vector<StmtPtr> &items, size_t k) {
  if (k + 1 >= items.size()) {
    return false;
  }
  auto *outerW = dynamic_cast<WhileStmt *>(items[k + 1].get());
  LtileOuterLoopHead head;
  if (!ltileMatchOuterLoopHead(items[k].get(), outerW, &head)) {
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
  if (!ltileParseZeroInit(outerBody->items[pos].get(), &innerIv)) {
    return false;
  }
  ++pos;
  auto *innerW = dynamic_cast<WhileStmt *>(outerBody->items[pos].get());
  if (!innerW) {
    return false;
  }
  ++pos;
  auto *outerInc = dynamic_cast<AssignStmt *>(outerBody->items[pos].get());
  if (!ltileIsIncByOne(outerInc, outerIv) || pos + 1 != outerBody->items.size()) {
    return false;
  }
  string innerIv2;
  ExprPtr innerLimit;
  if (!ltileExtractLtBound(innerW->cond.get(), &innerIv2, &innerLimit) ||
      innerIv2 != innerIv || innerIv == outerIv) {
    return false;
  }
  auto *innerBody = dynamic_cast<BlockStmt *>(innerW->body.get());
  vector<StmtPtr> core;
  if (!ltileCollectInnerLoopCore(innerBody, innerIv, &core)) {
    return false;
  }
  if (auto *coreAs = dynamic_cast<const AssignStmt *>(core[0].get());
      ltileIsSameIndices2DArrayCopy(coreAs, outerIv, innerIv) ||
      ltileIsScalar2DReductionAccum(coreAs, outerIv, innerIv)) {
    return false;
  }
  if (ltileBoundsTooSmallForTiling(outerLimit.get(), innerLimit.get())) {
    return false;
  }
  if (ltileExprUsesVarName(innerLimit.get(), outerIv) ||
      ltileExprUsesVarName(outerLimit.get(), innerIv)) {
    return false;
  }

  const bool innerWasDecl =
      dynamic_cast<const DeclStmt *>(outerBody->items[0].get()) != nullptr;
  bool declareInner = innerWasDecl;
  if (declareInner && !ltileHoistedLoopIvs.insert(innerIv).second) {
    declareInner = false;
  }
  const int nestId = ltileNestId++;
  int line = outerW->line;
  vector<StmtPtr> rep;
  rep.push_back(ltileMakeTileVarDecl(line, nestId, outerIv, innerIv, declareInner));
  rep.push_back(ltileMakeZeroAssign(line, ltileTileVar(outerIv, nestId)));
  rep.push_back(ltileBuildTiled2D(line, nestId, outerIv, innerIv, outerLimit.get(),
                             innerLimit.get(), std::move(core)));
  ltileInsertTiledLoopReplace(items, k, head.ivFromDecl, std::move(rep));
  return true;
}

// k 外层：while(k){ i=0; while(i){ [skip?] j=0; while(j){…} i++ } k++ }（01_mm2 风格）
static bool ltileTryTile3DKOuter(vector<StmtPtr> &items, size_t k) {
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
  if (!ltileExtractLtBound(outerW->cond.get(), &outerIv, &outerLimit) ||
      !ltileIsZeroScalarAssign(outerInit, outerIv)) {
    return false;
  }
  auto *outerBody = dynamic_cast<BlockStmt *>(outerW->body.get());
  if (!outerBody || outerBody->items.size() < 3) {
    return false;
  }
  size_t pos = 0;
  string midIv;
  if (!ltileParseZeroInit(outerBody->items[pos].get(), &midIv)) {
    return false;
  }
  ++pos;
  auto *midW = dynamic_cast<WhileStmt *>(outerBody->items[pos].get());
  if (!midW) {
    return false;
  }
  ++pos;
  auto *outerInc = dynamic_cast<AssignStmt *>(outerBody->items[pos].get());
  if (!ltileIsIncByOne(outerInc, outerIv) || pos + 1 != outerBody->items.size()) {
    return false;
  }
  string midIv2;
  ExprPtr midLimit;
  if (!ltileExtractLtBound(midW->cond.get(), &midIv2, &midLimit) || midIv2 != midIv) {
    return false;
  }
  auto *midBody = dynamic_cast<BlockStmt *>(midW->body.get());
  if (!midBody || midBody->items.size() < 3) {
    return false;
  }
  pos = 0;
  if (pos < midBody->items.size() && ltileIsContinueSkipIf(midBody->items[pos].get(), midIv)) {
    ++pos;
  }
  string innerIv;
  if (!ltileParseZeroInit(midBody->items[pos].get(), &innerIv)) {
    return false;
  }
  ++pos;
  auto *innerW = dynamic_cast<WhileStmt *>(midBody->items[pos].get());
  if (!innerW) {
    return false;
  }
  ++pos;
  auto *midInc = dynamic_cast<AssignStmt *>(midBody->items[pos].get());
  if (!ltileIsIncByOne(midInc, midIv) || pos + 1 != midBody->items.size()) {
    return false;
  }
  string innerIv2;
  ExprPtr innerLimit;
  if (!ltileExtractLtBound(innerW->cond.get(), &innerIv2, &innerLimit) || innerIv2 != innerIv) {
    return false;
  }
  auto *innerBody = dynamic_cast<BlockStmt *>(innerW->body.get());
  if (!ltileIsSimpleInnerLoopBody(innerBody, innerIv)) {
    return false;
  }
  if (auto *copyAs = dynamic_cast<const AssignStmt *>(innerBody->items[0].get());
      ltileIsSameIndices2DArrayCopy(copyAs, midIv, innerIv)) {
    return false;
  }
  if (auto *mmAs = dynamic_cast<const AssignStmt *>(innerBody->items[0].get())) {
    string cSym, aSym, bSym;
    if (ltileMatch01MmRank1Assign(mmAs, midIv, innerIv, outerIv, &cSym, &aSym, &bSym)) {
      return false;
    }
  }

  if (ltileBoundsTooSmallForTiling(outerLimit.get(), midLimit.get())) {
    return false;
  }
  if (ltileExprUsesVarName(midLimit.get(), outerIv) ||
      ltileExprUsesVarName(innerLimit.get(), outerIv) ||
      ltileExprUsesVarName(innerLimit.get(), midIv) ||
      ltileExprUsesVarName(outerLimit.get(), midIv)) {
    return false;
  }

  vector<StmtPtr> midCore;
  vector<StmtPtr> iBodyPrefix;
  pos = 0;
  if (pos < midBody->items.size() && ltileIsContinueSkipIf(midBody->items[pos].get(), midIv)) {
    if (StmtPtr c = ltileCloneStmt(midBody->items[pos].get())) {
      iBodyPrefix.push_back(std::move(c));
    } else {
      return false;
    }
    ++pos;
  }
  // 跳过 j=0 初始化：ltileBuildTiled2D 在每个 tile 内会执行 j=jj。
  // 只保留内层循环体赋值；勿再包一层 while(j)（ltileBuildTiled2D 已生成 j 循环并追加 j++）。
  ++pos;
  if (StmtPtr jAssign = ltileCloneStmt(innerBody->items[0].get())) {
    midCore.push_back(std::move(jAssign));
  } else {
    return false;
  }
  // midIv++ 由 ltileBuildTiled2D 统一追加，勿重复

  const bool midWasDecl =
      dynamic_cast<const DeclStmt *>(outerBody->items[0].get()) != nullptr;
  bool declareMid = midWasDecl;
  if (declareMid && !ltileHoistedLoopIvs.insert(midIv).second) {
    declareMid = false;
  }
  const int nestId = ltileNestId++;
  int line = outerW->line;
  // 只分块中层 i × 内层 j；k 外层保持原序，末尾保留 k++（勿用 ltileBuildTiled2D(k,i) 以免 k 在内层分块里多次自增）。
  auto tiledIJ = ltileBuildTiled2D(line, nestId, midIv, innerIv, midLimit.get(),
                              innerLimit.get(), std::move(midCore),
                              std::move(iBodyPrefix));
  auto kBody = make_unique<BlockStmt>(line);
  // 每个 k 迭代须从 ii=0 重扫 i×j 分块（否则第二次 k 起 ii 仍为 n）。
  kBody->items.push_back(ltileMakeZeroAssign(line, ltileTileVar(midIv, nestId)));
  kBody->items.push_back(std::move(tiledIJ));
  if (StmtPtr kInc = ltileCloneStmt(outerInc)) {
    kBody->items.push_back(std::move(kInc));
  } else {
    return false;
  }
  auto kWhile =
      make_unique<WhileStmt>(line, ltileCloneExpr(outerW->cond.get()), nullptr);
  kWhile->body = std::move(kBody);

  vector<StmtPtr> rep;
  if (StmtPtr kInit = ltileCloneStmt(outerInit)) {
    rep.push_back(std::move(kInit));
  } else {
    return false;
  }
  rep.push_back(ltileMakeTileVarDecl(line, nestId, midIv, innerIv, declareMid));
  rep.push_back(ltileMakeZeroAssign(line, ltileTileVar(midIv, nestId)));
  rep.push_back(std::move(kWhile));
  items.erase(items.begin() + static_cast<ptrdiff_t>(k),
              items.begin() + static_cast<ptrdiff_t>(k + 2));
  items.insert(items.begin() + static_cast<ptrdiff_t>(k),
               std::make_move_iterator(rep.begin()),
               std::make_move_iterator(rep.end()));
  return true;
}

// 中层 j： [pre: k/sum 初始化…][while k {body}][post: 写回][j++]
static bool ltileTryTile3DMatmul(vector<StmtPtr> &items, size_t k) {
  if (k + 1 >= items.size()) {
    return false;
  }
  auto *outerW = dynamic_cast<WhileStmt *>(items[k + 1].get());
  LtileOuterLoopHead head;
  if (!ltileMatchOuterLoopHead(items[k].get(), outerW, &head)) {
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
  if (!ltileParseZeroInit(outerBody->items[pos].get(), &midIv)) {
    return false;
  }
  ++pos;
  auto *midW = dynamic_cast<WhileStmt *>(outerBody->items[pos].get());
  if (!midW) {
    return false;
  }
  ++pos;
  auto *outerInc = dynamic_cast<AssignStmt *>(outerBody->items[pos].get());
  if (!ltileIsIncByOne(outerInc, outerIv) || pos + 1 != outerBody->items.size()) {
    return false;
  }
  string midIv2;
  ExprPtr midLimit;
  if (!ltileExtractLtBound(midW->cond.get(), &midIv2, &midLimit) || midIv2 != midIv) {
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
  if (!ltileExtractLtBound(kW->cond.get(), &kIv, &kLimit)) {
    return false;
  }
  auto *kBody = dynamic_cast<BlockStmt *>(kW->body.get());
  if (!kBody || kBody->items.size() != 2 || kBody->items[0]->kind != StmtKind::Assign ||
      !ltileIsIncByOne(dynamic_cast<AssignStmt *>(kBody->items[1].get()), kIv)) {
    return false;
  }
  // many_mat_cal 点积：sum=sum+C[i][k]*A[k][j] 由 tryInterchangeGemmIjk 改为 i-k-j，勿再 i-j 分块
  {
    auto *kas = dynamic_cast<const AssignStmt *>(kBody->items[0].get());
    if (kas && kas->lhs && kas->lhs->indices.empty()) {
      auto *add = dynamic_cast<const BinaryExpr *>(kas->rhs.get());
      if (add && add->op == "+") {
        auto *sl = dynamic_cast<const LValExpr *>(add->lhs.get());
        if (sl && sl->name == kas->lhs->name && sl->indices.empty()) {
          return false;
        }
      }
    }
  }
  if (kWhileIdx + 2 >= midBody->items.size()) {
    return false;
  }
  auto *midInc = dynamic_cast<AssignStmt *>(midBody->items.back().get());
  if (!ltileIsIncByOne(midInc, midIv)) {
    return false;
  }

  vector<StmtPtr> midCore;
  for (size_t i = 0; i < kWhileIdx; ++i) {
    if (StmtPtr c = ltileCloneStmt(midBody->items[i].get())) {
      midCore.push_back(std::move(c));
    }
  }
  if (StmtPtr kAssign = ltileCloneStmt(kBody->items[0].get())) {
    auto kLoopBody = make_unique<BlockStmt>(midW->line);
    kLoopBody->items.push_back(std::move(kAssign));
    kLoopBody->items.push_back(ltileMakeIncByOne(midW->line, kIv));
    auto kLoop =
        make_unique<WhileStmt>(midW->line, ltileCloneExpr(kW->cond.get()), nullptr);
    kLoop->body = std::move(kLoopBody);
    midCore.push_back(std::move(kLoop));
  } else {
    return false;
  }
  for (size_t i = kWhileIdx + 1; i + 1 < midBody->items.size(); ++i) {
    if (StmtPtr c = ltileCloneStmt(midBody->items[i].get())) {
      midCore.push_back(std::move(c));
    }
  }
  // innerIv(j)++ 由 ltileBuildTiled2D 统一追加，勿重复

  if (ltileBoundsTooSmallForTiling(limit.get(), midLimit.get())) {
    return false;
  }
  if (ltileExprUsesVarName(midLimit.get(), outerIv) ||
      ltileExprUsesVarName(kLimit.get(), outerIv) || ltileExprUsesVarName(kLimit.get(), midIv) ||
      ltileExprUsesVarName(limit.get(), midIv)) {
    return false;
  }

  const bool midWasDecl =
      dynamic_cast<const DeclStmt *>(outerBody->items[0].get()) != nullptr;
  bool declareMid = midWasDecl;
  if (declareMid && !ltileHoistedLoopIvs.insert(midIv).second) {
    declareMid = false;
  }
  const int nestId = ltileNestId++;
  int line = outerW->line;
  vector<StmtPtr> rep;
  rep.push_back(ltileMakeTileVarDecl(line, nestId, outerIv, midIv, declareMid));
  rep.push_back(ltileMakeZeroAssign(line, ltileTileVar(outerIv, nestId)));
  rep.push_back(ltileBuildTiled2D(line, nestId, outerIv, midIv, limit.get(),
                             midLimit.get(), std::move(midCore)));
  ltileInsertTiledLoopReplace(items, k, head.ivFromDecl, std::move(rep));
  return true;
}

static void ltileProcessBlockTiling(BlockStmt *blk) {
  if (!blk) {
    return;
  }
  for (size_t i = 0; i + 1 < blk->items.size();) {
    if (ltileTryTile3DKOuter(blk->items, i)) {
      i += 4;
      continue;
    }
    if (ltileTryTile3DMatmul(blk->items, i)) {
      const bool outerDecl =
          dynamic_cast<const DeclStmt *>(blk->items[i].get()) != nullptr;
      i += outerDecl ? 4 : 3;
      continue;
    }
    if (ltileTryTile2DNest(blk->items, i)) {
      const bool outerDecl =
          dynamic_cast<const DeclStmt *>(blk->items[i].get()) != nullptr;
      i += outerDecl ? 4 : 3;
      continue;
    }
    ++i;
  }
  for (auto &it : blk->items) {
    if (it->kind == StmtKind::Block) {
      ltileProcessBlockTiling(static_cast<BlockStmt *>(it.get()));
    } else if (it->kind == StmtKind::While) {
      ltileProcessBlockTiling(
          static_cast<BlockStmt *>(static_cast<WhileStmt *>(it.get())->body.get()));
    } else if (it->kind == StmtKind::If) {
      auto *ifs = static_cast<IfStmt *>(it.get());
      if (ifs->thenStmt && ifs->thenStmt->kind == StmtKind::Block) {
        ltileProcessBlockTiling(static_cast<BlockStmt *>(ifs->thenStmt.get()));
      }
      if (ifs->elseStmt && ifs->elseStmt->kind == StmtKind::Block) {
        ltileProcessBlockTiling(static_cast<BlockStmt *>(ifs->elseStmt.get()));
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
    ltileNestId = 0;
    ltileHoistedLoopIvs.clear();
    ltileProcessBlockTiling(item.func->body.get());
  }
}
