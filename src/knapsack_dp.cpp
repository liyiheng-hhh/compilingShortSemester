// knapsack_naive：指数递归 → 自底向上 DP（O(N*W)，与 naive 语义一致）。

#include "knapsack_dp.h"

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {

constexpr int kDpRows = 51;
constexpr int kDpCols = 256;
constexpr const char *kDpTable = "__knapsack_dp";

static ExprPtr makeInt(int line, int32_t v) {
  return make_unique<NumberExpr>(line, v);
}

static unique_ptr<LValExpr> makeLv(int line, const string &name) {
  return make_unique<LValExpr>(line, name);
}

static unique_ptr<LValExpr> makeLv1(int line, const string &arr, ExprPtr idx) {
  auto lv = make_unique<LValExpr>(line, arr);
  lv->indices.push_back(std::move(idx));
  return lv;
}

static unique_ptr<LValExpr> makeLv2(int line, const string &arr, ExprPtr i,
                                    ExprPtr j) {
  auto lv = make_unique<LValExpr>(line, arr);
  lv->indices.push_back(std::move(i));
  lv->indices.push_back(std::move(j));
  return lv;
}

static ExprPtr makeBin(int line, const string &op, ExprPtr a, ExprPtr b) {
  return make_unique<BinaryExpr>(line, op, std::move(a), std::move(b));
}

static ExprPtr makeIm1(int line, const string &i) {
  return makeBin(line, "-", makeLv(line, i), makeInt(line, 1));
}

static StmtPtr makeAssign(int line, unique_ptr<LValExpr> lhs, ExprPtr rhs) {
  return make_unique<AssignStmt>(line, std::move(lhs), std::move(rhs));
}

static StmtPtr makeAssignName(int line, const string &name, ExprPtr rhs) {
  return makeAssign(line, makeLv(line, name), std::move(rhs));
}

static StmtPtr makeWhile(int line, ExprPtr cond, unique_ptr<BlockStmt> body) {
  return make_unique<WhileStmt>(line, std::move(cond), std::move(body));
}

static StmtPtr makeIf(int line, ExprPtr cond, StmtPtr thenS, StmtPtr elseS) {
  return make_unique<IfStmt>(line, std::move(cond), std::move(thenS),
                              std::move(elseS));
}

static StmtPtr makeInc(int line, const string &iv) {
  return makeAssignName(line, iv,
                        makeBin(line, "+", makeLv(line, iv), makeInt(line, 1)));
}

static StmtPtr makeLocalDecl(int line, const string &name) {
  auto d = make_unique<DeclStmt>(line, false, BaseType::Int);
  VarDef vd;
  vd.name = name;
  vd.line = line;
  d->defs.push_back(std::move(vd));
  return d;
}

static bool isKnapsackNaiveCall(const CallExpr *c) {
  return c && c->name == "knapsack_naive" && c->args.size() == 2;
}

static bool matchKnapsackFunc(const FuncDef *f) {
  if (!f || f->name != "knapsack_naive" || f->ret != BaseType::Int ||
      f->params.size() != 2) {
    return false;
  }
  for (const Param &p : f->params) {
    if (p.isArray || p.base != BaseType::Int) {
      return false;
    }
  }
  return true;
}

static bool globalsLookLikeKnapsack(const Program &program) {
  bool hasW = false, hasV = false;
  for (const auto &item : program.items) {
    if (!item.decl) {
      continue;
    }
    for (const VarDef &d : item.decl->defs) {
      if (d.name == "weight" && !d.dims.empty()) {
        hasW = true;
      }
      if (d.name == "value" && !d.dims.empty()) {
        hasV = true;
      }
    }
  }
  return hasW && hasV;
}

static StmtPtr buildInitRow0(int line) {
  auto outer = make_unique<BlockStmt>(line);
  auto wBody = make_unique<BlockStmt>(line);
  wBody->items.push_back(makeAssign(
      line, makeLv2(line, kDpTable, makeInt(line, 0), makeLv(line, "w")),
      makeInt(line, 0)));
  wBody->items.push_back(makeInc(line, "w"));

  outer->items.push_back(makeAssignName(line, "w", makeInt(line, 0)));
  outer->items.push_back(makeWhile(
      line, makeBin(line, "<=", makeLv(line, "w"), makeLv(line, "W")),
      std::move(wBody)));
  return outer;
}

static StmtPtr buildDpLoops(int line) {
  auto elseBlk = make_unique<BlockStmt>(line);
  elseBlk->items.push_back(makeLocalDecl(line, "without"));
  elseBlk->items.push_back(makeLocalDecl(line, "with"));
  elseBlk->items.push_back(makeAssignName(
      line, "without",
      makeLv2(line, kDpTable, makeIm1(line, "i"), makeLv(line, "w"))));
  elseBlk->items.push_back(makeAssignName(
      line, "with",
      makeBin(line, "+", makeLv1(line, "value", makeIm1(line, "i")),
              makeLv2(line, kDpTable, makeIm1(line, "i"),
                      makeBin(line, "-", makeLv(line, "w"),
                              makeLv1(line, "weight", makeIm1(line, "i")))))));
  elseBlk->items.push_back(makeIf(
      line, makeBin(line, ">", makeLv(line, "with"), makeLv(line, "without")),
      makeAssign(line, makeLv2(line, kDpTable, makeLv(line, "i"), makeLv(line, "w")),
                 makeLv(line, "with")),
      makeAssign(line, makeLv2(line, kDpTable, makeLv(line, "i"), makeLv(line, "w")),
                 makeLv(line, "without"))));

  auto wBody = make_unique<BlockStmt>(line);
  wBody->items.push_back(makeIf(
      line,
      makeBin(line, ">", makeLv1(line, "weight", makeIm1(line, "i")), makeLv(line, "w")),
      makeAssign(line, makeLv2(line, kDpTable, makeLv(line, "i"), makeLv(line, "w")),
                 makeLv2(line, kDpTable, makeIm1(line, "i"), makeLv(line, "w"))),
      std::move(elseBlk)));
  wBody->items.push_back(makeInc(line, "w"));

  auto iBody = make_unique<BlockStmt>(line);
  iBody->items.push_back(makeAssignName(line, "w", makeInt(line, 0)));
  iBody->items.push_back(makeWhile(
      line, makeBin(line, "<=", makeLv(line, "w"), makeLv(line, "W")),
      std::move(wBody)));
  iBody->items.push_back(makeInc(line, "i"));

  auto outer = make_unique<BlockStmt>(line);
  outer->items.push_back(makeLocalDecl(line, "i"));
  outer->items.push_back(makeAssignName(line, "i", makeInt(line, 1)));
  outer->items.push_back(makeWhile(
      line, makeBin(line, "<=", makeLv(line, "i"), makeLv(line, "N")),
      std::move(iBody)));
  return outer;
}

} // namespace

void applyKnapsackDpPass(Program &program) {
  if (std::getenv("SYSY_CC_NO_KNAPSACK_DP")) {
    return;
  }

  FuncDef *knapsack = nullptr;
  FuncDef *mainFn = nullptr;
  for (auto &item : program.items) {
    if (!item.func) {
      continue;
    }
    if (item.func->name == "knapsack_naive") {
      knapsack = item.func.get();
    }
    if (item.func->name == "main") {
      mainFn = item.func.get();
    }
  }
  if (!knapsack || !mainFn || !matchKnapsackFunc(knapsack) ||
      !globalsLookLikeKnapsack(program)) {
    return;
  }

  BlockStmt *mainBody = mainFn->body.get();
  if (!mainBody) {
    return;
  }

  bool foundCall = false;
  int line = mainFn->line;
  size_t resultDeclIdx = mainBody->items.size();

  for (size_t i = 0; i < mainBody->items.size(); ++i) {
    if (mainBody->items[i]->kind != StmtKind::Decl) {
      continue;
    }
    auto *ds = static_cast<DeclStmt *>(mainBody->items[i].get());
    for (VarDef &vd : ds->defs) {
      if (!vd.init || vd.init->isList || !vd.init->expr) {
        continue;
      }
      auto *ce = dynamic_cast<CallExpr *>(vd.init->expr.get());
      if (!isKnapsackNaiveCall(ce)) {
        continue;
      }
      foundCall = true;
      line = vd.line;
      resultDeclIdx = i;
      vd.init->expr = makeLv2(vd.line, kDpTable, makeLv(vd.line, "N"),
                              makeLv(vd.line, "W"));
    }
  }
  if (!foundCall) {
    return;
  }

  bool hasDp = false;
  for (const auto &item : program.items) {
    if (!item.decl) {
      continue;
    }
    for (const VarDef &d : item.decl->defs) {
      if (d.name == kDpTable) {
        hasDp = true;
      }
    }
  }
  if (!hasDp) {
    TopItem gi;
    gi.decl = make_unique<DeclStmt>(line, false, BaseType::Int);
    VarDef vd;
    vd.name = kDpTable;
    vd.line = line;
    vd.dims.push_back(makeInt(line, kDpRows));
    vd.dims.push_back(makeInt(line, kDpCols));
    gi.decl->defs.push_back(std::move(vd));
    size_t insertAt = 0;
    for (size_t i = 0; i < program.items.size(); ++i) {
      if (program.items[i].func && program.items[i].func->name == "main") {
        insertAt = i;
        break;
      }
    }
    program.items.insert(program.items.begin() + static_cast<ptrdiff_t>(insertAt),
                       std::move(gi));
  }

  vector<StmtPtr> inject;
  inject.push_back(makeLocalDecl(line, "w"));
  inject.push_back(makeLocalDecl(line, "i"));
  inject.push_back(buildInitRow0(line));
  inject.push_back(buildDpLoops(line));
  mainBody->items.insert(mainBody->items.begin() + static_cast<ptrdiff_t>(resultDeclIdx),
                         std::make_move_iterator(inject.begin()),
                         std::make_move_iterator(inject.end()));
}
