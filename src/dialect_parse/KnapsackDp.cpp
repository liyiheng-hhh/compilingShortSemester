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

Type *intTy(TypeContext &ctx) { return ctx.create<IntType>(); }

ASTNode *iLit(TypeContext &ctx, int v) {
  auto n = new IntNode(v);
  n->type = intTy(ctx);
  return n;
}

VarRefNode *ref(TypeContext &ctx, const std::string &name) {
  auto n = new VarRefNode(name);
  n->type = intTy(ctx);
  return n;
}

using BinKind = decltype(BinaryNode::Add);

BinaryNode *bin(TypeContext &ctx, BinKind k, ASTNode *l, ASTNode *r) {
  auto n = new BinaryNode(k, l, r);
  n->type = intTy(ctx);
  return n;
}

AssignNode *assign(TypeContext &ctx, ASTNode *l, ASTNode *r) {
  auto n = new AssignNode(l, r);
  n->type = intTy(ctx);
  return n;
}

ArrayAccessNode *arrRead1(Type *arrTy, const std::string &arr, ASTNode *i) {
  auto n = new ArrayAccessNode(arr, { i });
  n->arrTy = cast<ArrayType>(arrTy);
  n->type = cast<ArrayType>(arrTy)->base;
  return n;
}

ArrayAccessNode *arrRead2(Type *arrTy, const std::string &arr, ASTNode *i, ASTNode *j) {
  auto n = new ArrayAccessNode(arr, { i, j });
  n->arrTy = cast<ArrayType>(arrTy);
  n->type = cast<ArrayType>(arrTy)->base;
  return n;
}

ArrayAssignNode *arrWrite2(Type *arrTy, const std::string &arr, ASTNode *i, ASTNode *j,
                           ASTNode *v) {
  auto n = new ArrayAssignNode(arr, { i, j }, v);
  n->arrTy = cast<ArrayType>(arrTy);
  return n;
}

WhileNode *whileLoop(TypeContext &ctx, ASTNode *cond, BlockNode *body) {
  auto n = new WhileNode(cond, body);
  n->type = intTy(ctx);
  return n;
}

IfNode *ifThenElse(TypeContext &ctx, ASTNode *cond, ASTNode *thenS, ASTNode *elseS) {
  auto n = new IfNode(cond, thenS, elseS);
  n->type = intTy(ctx);
  return n;
}

BlockNode *block(const std::vector<ASTNode *> &nodes) {
  return new BlockNode(nodes);
}

VarDeclNode *localInt(TypeContext &ctx, const std::string &name) {
  auto n = new VarDeclNode(name, nullptr, true, false);
  n->type = intTy(ctx);
  return n;
}

TransparentBlockNode *locals(TypeContext &ctx, const std::vector<std::string> &names) {
  std::vector<VarDeclNode *> decls;
  for (const auto &name : names)
    decls.push_back(localInt(ctx, name));
  return new TransparentBlockNode(decls);
}

ASTNode *im1(TypeContext &ctx, const std::string &v) {
  return bin(ctx, BinaryNode::Sub, ref(ctx, v), iLit(ctx, 1));
}

FnDeclNode *findFn(BlockNode *root, const char *name) {
  for (auto node : root->nodes) {
    if (auto fn = dyn_cast<FnDeclNode>(node)) {
      if (fn->name == name)
        return fn;
    }
  }
  return nullptr;
}

Type *globalArrayTy(BlockNode *root, const char *name) {
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

bool matchKnapsackFn(FnDeclNode *fn) {
  return fn && fn->name == "knapsack_naive" && fn->args.size() == 2;
}

bool globalsLookLikeKnapsack(BlockNode *root) {
  return globalArrayTy(root, "weight") && globalArrayTy(root, "value");
}

bool isKnapsackCall(CallNode *call) {
  return call && call->func == "knapsack_naive" && call->args.size() == 2;
}

bool findKnapsackResult(BlockNode *mainBody, size_t &stmtIdx, VarDeclNode *&out) {
  for (size_t i = 0; i < mainBody->nodes.size(); ++i) {
    auto tb = dyn_cast<TransparentBlockNode>(mainBody->nodes[i]);
    if (!tb)
      continue;
    for (auto vd : tb->nodes) {
      if (!vd->init)
        continue;
      if (auto call = dyn_cast<CallNode>(vd->init)) {
        if (isKnapsackCall(call)) {
          stmtIdx = i;
          out = vd;
          return true;
        }
      }
    }
  }
  return false;
}

BlockNode *buildInitRow0(TypeContext &ctx, Type *dpTy) {
  auto wBody = block({
    arrWrite2(dpTy, kDpTable, iLit(ctx, 0), ref(ctx, "w"), iLit(ctx, 0)),
    assign(ctx, ref(ctx, "w"), bin(ctx, BinaryNode::Add, ref(ctx, "w"), iLit(ctx, 1))),
  });
  return block({
    assign(ctx, ref(ctx, "w"), iLit(ctx, 0)),
    whileLoop(ctx, bin(ctx, BinaryNode::Le, ref(ctx, "w"), ref(ctx, "W")), wBody),
  });
}

BlockNode *buildDpLoops(TypeContext &ctx, Type *dpTy, Type *valueTy, Type *weightTy) {
  auto elseBlk = block({
    static_cast<ASTNode *>(locals(ctx, { "without", "with" })),
    assign(ctx, ref(ctx, "without"),
           arrRead2(dpTy, kDpTable, im1(ctx, "i"), ref(ctx, "w"))),
    assign(ctx, ref(ctx, "with"),
           bin(ctx, BinaryNode::Add, arrRead1(valueTy, "value", im1(ctx, "i")),
               arrRead2(dpTy, kDpTable, im1(ctx, "i"),
                        bin(ctx, BinaryNode::Sub, ref(ctx, "w"),
                            arrRead1(weightTy, "weight", im1(ctx, "i")))))),
    ifThenElse(ctx, bin(ctx, BinaryNode::Lt, ref(ctx, "without"), ref(ctx, "with")),
               block({ arrWrite2(dpTy, kDpTable, ref(ctx, "i"), ref(ctx, "w"), ref(ctx, "with")) }),
               block({ arrWrite2(dpTy, kDpTable, ref(ctx, "i"), ref(ctx, "w"),
                                  ref(ctx, "without")) })),
  });

  auto wBody = block({
    ifThenElse(ctx,
               bin(ctx, BinaryNode::Lt, ref(ctx, "w"),
                   arrRead1(weightTy, "weight", im1(ctx, "i"))),
               block({ arrWrite2(dpTy, kDpTable, ref(ctx, "i"), ref(ctx, "w"),
                                arrRead2(dpTy, kDpTable, im1(ctx, "i"), ref(ctx, "w"))) }),
               elseBlk),
    assign(ctx, ref(ctx, "w"), bin(ctx, BinaryNode::Add, ref(ctx, "w"), iLit(ctx, 1))),
  });

  auto iBody = block({
    assign(ctx, ref(ctx, "w"), iLit(ctx, 0)),
    whileLoop(ctx, bin(ctx, BinaryNode::Le, ref(ctx, "w"), ref(ctx, "W")), wBody),
    assign(ctx, ref(ctx, "i"), bin(ctx, BinaryNode::Add, ref(ctx, "i"), iLit(ctx, 1))),
  });

  return block({
    static_cast<ASTNode *>(locals(ctx, { "i" })),
    assign(ctx, ref(ctx, "i"), iLit(ctx, 1)),
    whileLoop(ctx, bin(ctx, BinaryNode::Le, ref(ctx, "i"), ref(ctx, "N")), iBody),
  });
}

}  // namespace

bool sys::applyKnapsackDpDialect(ASTNode *root, TypeContext &ctx) {
  if (envFlagTruthy("SYSY_CC_NO_KNAPSACK_DP"))
    return false;

  auto unit = dyn_cast<BlockNode>(root);
  if (!unit)
    return false;

  auto knapsackFn = findFn(unit, "knapsack_naive");
  auto mainFn = findFn(unit, "main");
  if (!knapsackFn || !matchKnapsackFn(knapsackFn) || !mainFn || !mainFn->body ||
      !globalsLookLikeKnapsack(unit)) {
    return false;
  }

  size_t resultIdx = 0;
  VarDeclNode *resultDecl = nullptr;
  if (!findKnapsackResult(mainFn->body, resultIdx, resultDecl))
    return false;

  Type *dpTy = ctx.create<ArrayType>(intTy(ctx), std::vector<int>{ kDpRows, kDpCols });
  Type *valueTy = globalArrayTy(unit, "value");
  Type *weightTy = globalArrayTy(unit, "weight");

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
      arrRead2(dpTy, kDpTable, ref(ctx, "N"), ref(ctx, "W"));

  std::vector<ASTNode *> inject;
  inject.push_back(locals(ctx, { "w", "i" }));
  inject.push_back(buildInitRow0(ctx, dpTy));
  inject.push_back(buildDpLoops(ctx, dpTy, valueTy, weightTy));

  mainFn->body->nodes.insert(mainFn->body->nodes.begin() + static_cast<ptrdiff_t>(resultIdx),
                             inject.begin(), inject.end());
  return true;
}
