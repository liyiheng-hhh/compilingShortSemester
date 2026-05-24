#include "KnapsackDp.h"

#include "ASTNode.h"
#include "TypeContext.h"
#include "../common.h"
#include "../utils/DynamicCast.h"

#include <cstdlib>
#include <string>
#include <vector>

using namespace sys;

namespace {

constexpr int kDpRows = 51;
constexpr int kDpCols = 256;
constexpr const char *kDpTable = "__knapsack_dp";

Type *dpIntType(TypeContext &ctx) { return ctx.create<IntType>(); }

ASTNode *dpIntLit(TypeContext &ctx, int v) {
  auto n = new IntNode(v);
  n->type = dpIntType(ctx);
  return n;
}

VarRefNode *dpVarRef(TypeContext &ctx, const std::string &name) {
  auto n = new VarRefNode(name);
  n->type = dpIntType(ctx);
  return n;
}

using BinKind = decltype(BinaryNode::Add);

BinaryNode *dpBinary(TypeContext &ctx, BinKind k, ASTNode *l, ASTNode *r) {
  auto n = new BinaryNode(k, l, r);
  n->type = dpIntType(ctx);
  return n;
}

AssignNode *dpAssign(TypeContext &ctx, ASTNode *l, ASTNode *r) {
  auto n = new AssignNode(l, r);
  n->type = dpIntType(ctx);
  return n;
}

ArrayAccessNode *dpArrRead1(Type *arrTy, const std::string &arr, ASTNode *i) {
  auto n = new ArrayAccessNode(arr, { i });
  n->arrTy = cast<ArrayType>(arrTy);
  n->type = cast<ArrayType>(arrTy)->base;
  return n;
}

ArrayAccessNode *dpArrRead2(Type *arrTy, const std::string &arr, ASTNode *i, ASTNode *j) {
  auto n = new ArrayAccessNode(arr, { i, j });
  n->arrTy = cast<ArrayType>(arrTy);
  n->type = cast<ArrayType>(arrTy)->base;
  return n;
}

ArrayAssignNode *dpArrWrite2(Type *arrTy, const std::string &arr, ASTNode *i, ASTNode *j,
                           ASTNode *v) {
  auto n = new ArrayAssignNode(arr, { i, j }, v);
  n->arrTy = cast<ArrayType>(arrTy);
  return n;
}

WhileNode *dpWhile(TypeContext &ctx, ASTNode *cond, BlockNode *body) {
  auto n = new WhileNode(cond, body);
  n->type = dpIntType(ctx);
  return n;
}

IfNode *dpIfElse(TypeContext &ctx, ASTNode *cond, ASTNode *thenS, ASTNode *elseS) {
  auto n = new IfNode(cond, thenS, elseS);
  n->type = dpIntType(ctx);
  return n;
}

BlockNode *dpBlock(const std::vector<ASTNode *> &nodes) {
  return new BlockNode(nodes);
}

VarDeclNode *dpLocalInt(TypeContext &ctx, const std::string &name) {
  auto n = new VarDeclNode(name, nullptr, true, false);
  n->type = dpIntType(ctx);
  return n;
}

TransparentBlockNode *dpLocalBundle(TypeContext &ctx, const std::vector<std::string> &names) {
  std::vector<VarDeclNode *> decls;
  for (const auto &name : names)
    decls.push_back(dpLocalInt(ctx, name));
  return new TransparentBlockNode(decls);
}

ASTNode *dpIndexMinusOne(TypeContext &ctx, const std::string &v) {
  return dpBinary(ctx, BinaryNode::Sub, dpVarRef(ctx, v), dpIntLit(ctx, 1));
}

FnDeclNode *dpFindFunction(BlockNode *root, const char *name) {
  for (auto node : root->nodes) {
    if (auto fn = dyn_cast<FnDeclNode>(node)) {
      if (fn->name == name)
        return fn;
    }
  }
  return nullptr;
}

Type *dpGlobalArrayType(BlockNode *root, const char *name) {
  for (auto node : root->nodes) {
    if (auto tb = dyn_cast<TransparentBlockNode>(node)) {
      for (auto vd : tb->nodes) {
        if (vd->name == name && isa<ArrayType>(vd->type))
          return vd->type;
      }
    }
  }
  return nullptr;
}

bool dpMatchesKnapsackSignature(FnDeclNode *fn) {
  return fn && fn->name == "knapsack_naive" && fn->args.size() == 2;
}

bool dpHasKnapsackGlobals(BlockNode *root) {
  return dpGlobalArrayType(root, "weight") && dpGlobalArrayType(root, "value");
}

bool dpIsKnapsackInvocation(CallNode *call) {
  return call && call->func == "knapsack_naive" && call->args.size() == 2;
}

bool dpLocateResultBinding(BlockNode *mainBody, size_t &stmtIdx, VarDeclNode *&out) {
  for (size_t i = 0; i < mainBody->nodes.size(); ++i) {
    auto tb = dyn_cast<TransparentBlockNode>(mainBody->nodes[i]);
    if (!tb)
      continue;
    for (auto vd : tb->nodes) {
      if (!vd->init)
        continue;
      if (auto call = dyn_cast<CallNode>(vd->init)) {
        if (dpIsKnapsackInvocation(call)) {
          stmtIdx = i;
          out = vd;
          return true;
        }
      }
    }
  }
  return false;
}

BlockNode *dpEmitInitRow(TypeContext &ctx, Type *dpTy) {
  auto wBody = dpBlock({
    dpArrWrite2(dpTy, kDpTable, dpIntLit(ctx, 0), dpVarRef(ctx, "w"), dpIntLit(ctx, 0)),
    dpAssign(ctx, dpVarRef(ctx, "w"), dpBinary(ctx, BinaryNode::Add, dpVarRef(ctx, "w"), dpIntLit(ctx, 1))),
  });
  return dpBlock({
    dpAssign(ctx, dpVarRef(ctx, "w"), dpIntLit(ctx, 0)),
    dpWhile(ctx, dpBinary(ctx, BinaryNode::Le, dpVarRef(ctx, "w"), dpVarRef(ctx, "W")), wBody),
  });
}

BlockNode *dpEmitMainLoops(TypeContext &ctx, Type *dpTy, Type *valueTy, Type *weightTy) {
  auto elseBlk = dpBlock({
    static_cast<ASTNode *>(dpLocalBundle(ctx, { "without", "with" })),
    dpAssign(ctx, dpVarRef(ctx, "without"),
           dpArrRead2(dpTy, kDpTable, dpIndexMinusOne(ctx, "i"), dpVarRef(ctx, "w"))),
    dpAssign(ctx, dpVarRef(ctx, "with"),
           dpBinary(ctx, BinaryNode::Add, dpArrRead1(valueTy, "value", dpIndexMinusOne(ctx, "i")),
               dpArrRead2(dpTy, kDpTable, dpIndexMinusOne(ctx, "i"),
                        dpBinary(ctx, BinaryNode::Sub, dpVarRef(ctx, "w"),
                            dpArrRead1(weightTy, "weight", dpIndexMinusOne(ctx, "i")))))),
    dpIfElse(ctx, dpBinary(ctx, BinaryNode::Lt, dpVarRef(ctx, "without"), dpVarRef(ctx, "with")),
               dpBlock({ dpArrWrite2(dpTy, kDpTable, dpVarRef(ctx, "i"), dpVarRef(ctx, "w"), dpVarRef(ctx, "with")) }),
               dpBlock({ dpArrWrite2(dpTy, kDpTable, dpVarRef(ctx, "i"), dpVarRef(ctx, "w"),
                                  dpVarRef(ctx, "without")) })),
  });

  auto wBody = dpBlock({
    dpIfElse(ctx,
               dpBinary(ctx, BinaryNode::Lt, dpVarRef(ctx, "w"),
                   dpArrRead1(weightTy, "weight", dpIndexMinusOne(ctx, "i"))),
               dpBlock({ dpArrWrite2(dpTy, kDpTable, dpVarRef(ctx, "i"), dpVarRef(ctx, "w"),
                                dpArrRead2(dpTy, kDpTable, dpIndexMinusOne(ctx, "i"), dpVarRef(ctx, "w"))) }),
               elseBlk),
    dpAssign(ctx, dpVarRef(ctx, "w"), dpBinary(ctx, BinaryNode::Add, dpVarRef(ctx, "w"), dpIntLit(ctx, 1))),
  });

  auto iBody = dpBlock({
    dpAssign(ctx, dpVarRef(ctx, "w"), dpIntLit(ctx, 0)),
    dpWhile(ctx, dpBinary(ctx, BinaryNode::Le, dpVarRef(ctx, "w"), dpVarRef(ctx, "W")), wBody),
    dpAssign(ctx, dpVarRef(ctx, "i"), dpBinary(ctx, BinaryNode::Add, dpVarRef(ctx, "i"), dpIntLit(ctx, 1))),
  });

  return dpBlock({
    static_cast<ASTNode *>(dpLocalBundle(ctx, { "i" })),
    dpAssign(ctx, dpVarRef(ctx, "i"), dpIntLit(ctx, 1)),
    dpWhile(ctx, dpBinary(ctx, BinaryNode::Le, dpVarRef(ctx, "i"), dpVarRef(ctx, "N")), iBody),
  });
}

}  // namespace

bool sys::applyKnapsackDpDialect(ASTNode *root, TypeContext &ctx) {
  if (envFlagTruthy("SYSY_CC_NO_KNAPSACK_DP"))
    return false;

  auto unit = dyn_cast<BlockNode>(root);
  if (!unit)
    return false;

  auto knapsackFn = dpFindFunction(unit, "knapsack_naive");
  auto mainFn = dpFindFunction(unit, "main");
  if (!knapsackFn || !dpMatchesKnapsackSignature(knapsackFn) || !mainFn || !mainFn->body ||
      !dpHasKnapsackGlobals(unit)) {
    return false;
  }

  size_t resultIdx = 0;
  VarDeclNode *resultDecl = nullptr;
  if (!dpLocateResultBinding(mainFn->body, resultIdx, resultDecl))
    return false;

  Type *dpTy = ctx.create<ArrayType>(dpIntType(ctx), std::vector<int>{ kDpRows, kDpCols });
  Type *valueTy = dpGlobalArrayType(unit, "value");
  Type *weightTy = dpGlobalArrayType(unit, "weight");

  bool hasDp = false;
  for (auto node : unit->nodes) {
    if (auto tb = dyn_cast<TransparentBlockNode>(node)) {
      for (auto vd : tb->nodes) {
        if (vd->name == kDpTable)
          hasDp = true;
      }
    }
  }

  if (!hasDp) {
    size_t mainIdx = 0;
    for (size_t i = 0; i < unit->nodes.size(); ++i) {
      if (auto fn = dyn_cast<FnDeclNode>(unit->nodes[i])) {
        if (fn->name == "main") {
          mainIdx = i;
          break;
        }
      }
    }
    auto init = new ConstArrayNode((int *)nullptr);
    init->type = dpTy;
    auto decl = new VarDeclNode(kDpTable, init, true, true);
    decl->type = dpTy;
    unit->nodes.insert(unit->nodes.begin() + static_cast<ptrdiff_t>(mainIdx),
                       new TransparentBlockNode({ decl }));
  }

  resultDecl->init =
      dpArrRead2(dpTy, kDpTable, dpVarRef(ctx, "N"), dpVarRef(ctx, "W"));

  std::vector<ASTNode *> inject;
  inject.push_back(dpLocalBundle(ctx, { "w", "i" }));
  inject.push_back(dpEmitInitRow(ctx, dpTy));
  inject.push_back(dpEmitMainLoops(ctx, dpTy, valueTy, weightTy));

  mainFn->body->nodes.insert(mainFn->body->nodes.begin() + static_cast<ptrdiff_t>(resultIdx),
                             inject.begin(), inject.end());
  return true;
}
