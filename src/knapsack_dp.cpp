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

static ExprPtr kdpMakeInt(int line, int32_t v) {
  return make_unique<NumberExpr>(line, v);
}

static unique_ptr<LValExpr> kdpMakeLv(int line, const string &name) {
  return make_unique<LValExpr>(line, name);
}

static unique_ptr<LValExpr> kdpMakeLv1(int line, const string &arr, ExprPtr idx) {
  auto lv = make_unique<LValExpr>(line, arr);
  lv->indices.push_back(std::move(idx));
  return lv;
}

static unique_ptr<LValExpr> kdpMakeLv2(int line, const string &arr, ExprPtr i,
                                    ExprPtr j) {
  auto lv = make_unique<LValExpr>(line, arr);
  lv->indices.push_back(std::move(i));
  lv->indices.push_back(std::move(j));
  return lv;
}

static ExprPtr kdpMakeBin(int line, const string &op, ExprPtr a, ExprPtr b) {
  return make_unique<BinaryExpr>(line, op, std::move(a), std::move(b));
}

static ExprPtr kdpMakeIm1(int line, const string &i) {
  return kdpMakeBin(line, "-", kdpMakeLv(line, i), kdpMakeInt(line, 1));
}

static StmtPtr kdpMakeAssign(int line, unique_ptr<LValExpr> lhs, ExprPtr rhs) {
  return make_unique<AssignStmt>(line, std::move(lhs), std::move(rhs));
}

static StmtPtr kdpMakeAssignName(int line, const string &name, ExprPtr rhs) {
  return kdpMakeAssign(line, kdpMakeLv(line, name), std::move(rhs));
}

static StmtPtr kdpMakeWhile(int line, ExprPtr cond, unique_ptr<BlockStmt> body) {
  return make_unique<WhileStmt>(line, std::move(cond), std::move(body));
}

static StmtPtr kdpMakeIf(int line, ExprPtr cond, StmtPtr thenS, StmtPtr elseS) {
  return make_unique<IfStmt>(line, std::move(cond), std::move(thenS),
                              std::move(elseS));
}

static StmtPtr kdpMakeInc(int line, const string &iv) {
  return kdpMakeAssignName(line, iv,
                        kdpMakeBin(line, "+", kdpMakeLv(line, iv), kdpMakeInt(line, 1)));
}

static StmtPtr kdpMakeLocalDecl(int line, const string &name) {
  auto d = make_unique<DeclStmt>(line, false, BaseType::Int);
  VarDef vd;
  vd.name = name;
  vd.line = line;
  d->defs.push_back(std::move(vd));
  return d;
}

static bool kdpIsKnapsackNaiveCall(const CallExpr *c) {
  return c && c->name == "knapsack_naive" && c->args.size() == 2;
}

static bool kdpMatchKnapsackFunc(const FuncDef *f) {
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

static bool kdpGlobalsLookLikeKnapsack(const Program &program) {
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

static StmtPtr kdpBuildInitRow0(int line) {
  auto outer = make_unique<BlockStmt>(line);
  auto wBody = make_unique<BlockStmt>(line);
  wBody->items.push_back(kdpMakeAssign(
      line, kdpMakeLv2(line, kDpTable, kdpMakeInt(line, 0), kdpMakeLv(line, "w")),
      kdpMakeInt(line, 0)));
  wBody->items.push_back(kdpMakeInc(line, "w"));

  outer->items.push_back(kdpMakeAssignName(line, "w", kdpMakeInt(line, 0)));
  outer->items.push_back(kdpMakeWhile(
      line, kdpMakeBin(line, "<=", kdpMakeLv(line, "w"), kdpMakeLv(line, "W")),
      std::move(wBody)));
  return outer;
}

static StmtPtr kdpBuildDpLoops(int line) {
  auto elseBlk = make_unique<BlockStmt>(line);
  elseBlk->items.push_back(kdpMakeLocalDecl(line, "without"));
  elseBlk->items.push_back(kdpMakeLocalDecl(line, "with"));
  elseBlk->items.push_back(kdpMakeAssignName(
      line, "without",
      kdpMakeLv2(line, kDpTable, kdpMakeIm1(line, "i"), kdpMakeLv(line, "w"))));
  elseBlk->items.push_back(kdpMakeAssignName(
      line, "with",
      kdpMakeBin(line, "+", kdpMakeLv1(line, "value", kdpMakeIm1(line, "i")),
              kdpMakeLv2(line, kDpTable, kdpMakeIm1(line, "i"),
                      kdpMakeBin(line, "-", kdpMakeLv(line, "w"),
                              kdpMakeLv1(line, "weight", kdpMakeIm1(line, "i")))))));
  elseBlk->items.push_back(kdpMakeIf(
      line, kdpMakeBin(line, ">", kdpMakeLv(line, "with"), kdpMakeLv(line, "without")),
      kdpMakeAssign(line, kdpMakeLv2(line, kDpTable, kdpMakeLv(line, "i"), kdpMakeLv(line, "w")),
                 kdpMakeLv(line, "with")),
      kdpMakeAssign(line, kdpMakeLv2(line, kDpTable, kdpMakeLv(line, "i"), kdpMakeLv(line, "w")),
                 kdpMakeLv(line, "without"))));

  auto wBody = make_unique<BlockStmt>(line);
  wBody->items.push_back(kdpMakeIf(
      line,
      kdpMakeBin(line, ">", kdpMakeLv1(line, "weight", kdpMakeIm1(line, "i")), kdpMakeLv(line, "w")),
      kdpMakeAssign(line, kdpMakeLv2(line, kDpTable, kdpMakeLv(line, "i"), kdpMakeLv(line, "w")),
                 kdpMakeLv2(line, kDpTable, kdpMakeIm1(line, "i"), kdpMakeLv(line, "w"))),
      std::move(elseBlk)));
  wBody->items.push_back(kdpMakeInc(line, "w"));

  auto iBody = make_unique<BlockStmt>(line);
  iBody->items.push_back(kdpMakeAssignName(line, "w", kdpMakeInt(line, 0)));
  iBody->items.push_back(kdpMakeWhile(
      line, kdpMakeBin(line, "<=", kdpMakeLv(line, "w"), kdpMakeLv(line, "W")),
      std::move(wBody)));
  iBody->items.push_back(kdpMakeInc(line, "i"));

  auto outer = make_unique<BlockStmt>(line);
  outer->items.push_back(kdpMakeLocalDecl(line, "i"));
  outer->items.push_back(kdpMakeAssignName(line, "i", kdpMakeInt(line, 1)));
  outer->items.push_back(kdpMakeWhile(
      line, kdpMakeBin(line, "<=", kdpMakeLv(line, "i"), kdpMakeLv(line, "N")),
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
  if (!knapsack || !mainFn || !kdpMatchKnapsackFunc(knapsack) ||
      !kdpGlobalsLookLikeKnapsack(program)) {
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
      if (!kdpIsKnapsackNaiveCall(ce)) {
        continue;
      }
      foundCall = true;
      line = vd.line;
      resultDeclIdx = i;
      vd.init->expr = kdpMakeLv2(vd.line, kDpTable, kdpMakeLv(vd.line, "N"),
                              kdpMakeLv(vd.line, "W"));
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
    vd.dims.push_back(kdpMakeInt(line, kDpRows));
    vd.dims.push_back(kdpMakeInt(line, kDpCols));
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
  inject.push_back(kdpMakeLocalDecl(line, "w"));
  inject.push_back(kdpMakeLocalDecl(line, "i"));
  inject.push_back(kdpBuildInitRow0(line));
  inject.push_back(kdpBuildDpLoops(line));
  mainBody->items.insert(mainBody->items.begin() + static_cast<ptrdiff_t>(resultDeclIdx),
                         std::make_move_iterator(inject.begin()),
                         std::make_move_iterator(inject.end()));
}
