// compiler2026-x phase-E (codegen expressions)
// compiler2026-x phase-E (codegen)

#include "CodeGen.h"
#include "../common.h"
#include "../utils/DynamicCast.h"
#include "Attrs.h"
#include "OpBase.h"
#include "Ops.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace sys;

namespace {

bool cgBitStubFoldEnabled() {
  if (envFlagTruthy("SYSY_CC_NO_BIT_STUB_FOLD"))
    return false;
  const char *v = std::getenv("SYSY_CC_ENABLE_BIT_STUB_FOLD");
  if (!v || !v[0])
    return true;
  return envFlagTruthy("SYSY_CC_ENABLE_BIT_STUB_FOLD");
}

Op *cgEmitShiftAmount(Builder &builder, int bits) {
  return builder.create<IntOp>({ new IntAttr(bits) });
}

bool cgAstIsFloat(Type *ty) {
  return ty && isa<FloatType>(ty);
}

bool cgValueIsFloat(Value v) {
  if (!v.defining)
    return false;
  auto ty = v.defining->getResultType();
  return ty == Value::f32 || ty == Value::f128;
}

bool cgPreferFloat(Type *astTy, Value v) {
  return cgAstIsFloat(astTy) || cgValueIsFloat(v);
}

}  // namespace

Value CodeGen::cgcEmitBinary(BinaryNode *node) {
  // TODO: for float, we need to test zero in a different way.
  if (node->kind == BinaryNode::And) {
    auto alloca = builder.create<AllocaOp>({ new SizeAttr(4) });
    //   l && r
    // becomes
    //   if (l)
    //     %1 = not_zero r
    //     store %1, %alloca
    //   else
    //     store 0, %alloca
    //   load %alloca
    auto l = cgcEmitExpr(node->l);
    auto branch = builder.create<IfOp>({ l });
    {
      auto ifso = branch->appendRegion();
      auto block = ifso->appendBlock();
      Builder::Guard guard(builder);

      builder.setToBlockStart(block);
      auto r = cgcEmitExpr(node->r);
      auto snez = builder.create<SetNotZeroOp>({ r });
      builder.create<StoreOp>({ snez, alloca }, { new SizeAttr(4) });
    }
    {
      auto ifnot = branch->appendRegion();
      auto block = ifnot->appendBlock();
      Builder::Guard guard(builder);

      builder.setToBlockStart(block);
      auto zero = builder.create<IntOp>({ new IntAttr(0) });
      builder.create<StoreOp>({ zero, alloca }, { new SizeAttr(4) });
    }
    return builder.create<LoadOp>(Value::i32, { alloca }, { new SizeAttr(4) });
  }

  if (node->kind == BinaryNode::Or) {
    auto alloca = builder.create<AllocaOp>({ new SizeAttr(4) });
    //   l || r
    // becomes
    //   if (l)
    //     store 1, %alloca
    //   else
    //     %1 = not_zero r
    //     store %1, %alloca
    //   load %alloca
    auto l = cgcEmitExpr(node->l);
    auto branch = builder.create<IfOp>({ l });
    {
      auto ifso = branch->appendRegion();
      auto block = ifso->appendBlock();
      Builder::Guard guard(builder);

      builder.setToBlockStart(block);
      auto one = builder.create<IntOp>({ new IntAttr(1) });
      builder.create<StoreOp>({ one, alloca }, { new SizeAttr(4) });
    }
    {
      auto ifnot = branch->appendRegion();
      auto block = ifnot->appendBlock();
      Builder::Guard guard(builder);

      builder.setToBlockStart(block);
      auto r = cgcEmitExpr(node->r);
      auto sez = builder.create<SetNotZeroOp>({ r });
      builder.create<StoreOp>({ sez, alloca }, { new SizeAttr(4) });
    }
    return builder.create<LoadOp>(Value::i32, { alloca }, { new SizeAttr(4) });
  }

  auto l = cgcEmitExpr(node->l);
  auto r = cgcEmitExpr(node->r);
  bool lhsFloat = cgPreferFloat(node->l ? node->l->type : nullptr, l);
  bool rhsFloat = cgPreferFloat(node->r ? node->r->type : nullptr, r);
  if (!lhsFloat && !rhsFloat) {
    switch (node->kind) {
    case BinaryNode::Add:
      return builder.create<AddIOp>({ l, r });
    case BinaryNode::Sub:
      return builder.create<SubIOp>({ l, r });
    case BinaryNode::Mul:
      return builder.create<MulIOp>({ l, r });
    case BinaryNode::Div:
      return builder.create<DivIOp>({ l, r });
    case BinaryNode::Mod:
      return builder.create<ModIOp>({ l, r });
    case BinaryNode::Eq:
      return builder.create<EqOp>({ l, r });
    case BinaryNode::Ne:
      return builder.create<NeOp>({ l, r });
    case BinaryNode::Lt:
      return builder.create<LtOp>({ l, r });
    case BinaryNode::Le:
      return builder.create<LeOp>({ l, r });
    default:
      assert(false);
      std::abort();
    }
  } else {
    switch (node->kind) {
    case BinaryNode::Add:
      return builder.create<AddFOp>({ l, r });
    case BinaryNode::Sub:
      return builder.create<SubFOp>({ l, r });
    case BinaryNode::Mul:
      return builder.create<MulFOp>({ l, r });
    case BinaryNode::Div:
      return builder.create<DivFOp>({ l, r });
    case BinaryNode::Mod:
      return builder.create<ModFOp>({ l, r });
    case BinaryNode::Eq:
      return builder.create<EqFOp>({ l, r });
    case BinaryNode::Ne:
      return builder.create<NeFOp>({ l, r });
    case BinaryNode::Lt:
      return builder.create<LtFOp>({ l, r });
    case BinaryNode::Le:
      return builder.create<LeFOp>({ l, r });
    default:
      std::cerr << "unsupported float binary " << node->kind << "\n";
      assert(false);
      std::abort();
    }
  }
}

Value CodeGen::cgcEmitUnary(UnaryNode *node) {
  auto value = cgcEmitExpr(node->node);
  switch (node->kind) {
  case UnaryNode::Float2Int:
    return builder.create<F2IOp>({ value });
  case UnaryNode::Int2Float:
    return builder.create<I2FOp>({ value });
  case UnaryNode::Not:
    return builder.create<NotOp>({ value });
  case UnaryNode::Minus:
    if (cgPreferFloat(node->type, value))
      return builder.create<MinusFOp>({ value });
    else
      return builder.create<MinusOp>({ value });
  }
  assert(false);
  std::abort();
}

Value CodeGen::cgcEmitExpr(ASTNode *node) {
  if (auto binary = dyn_cast<BinaryNode>(node))
    return cgcEmitBinary(binary);
  
  if (auto unary = dyn_cast<UnaryNode>(node))
    return cgcEmitUnary(unary);

  if (auto lint = dyn_cast<IntNode>(node))
    return builder.create<IntOp>({ new IntAttr(lint->value) });

  if (auto lfloat = dyn_cast<FloatNode>(node))
    return builder.create<FloatOp>({ new FloatAttr(lfloat->value) });

  if (auto ref = dyn_cast<VarRefNode>(node)) {
    bool isFloat = cgAstIsFloat(node->type) || cgAstIsFloat(ref->type);
    Value::Type resultTy = isFloat ? Value::f32 : Value::i32;

    if (!symbols.count(ref->name)) {
      if (globals.count(ref->name)) {
        auto addr = builder.create<GetGlobalOp>({
          new NameAttr(ref->name)
        });
        // No extra indirection for global arrays.
        if (isa<ArrayType>(ref->type) || isa<PointerType>(ref->type))
          return addr;

        auto load = builder.create<LoadOp>(resultTy, { addr }, {
          new SizeAttr(getSize(ref->type))
        });
        return load;
      }

      std::cerr << "cannot find symbol " << ref->name << "\n";
      assert(false);
    }
    auto from = symbols[ref->name];
    auto load = builder.create<LoadOp>(resultTy, { from }, {
      new SizeAttr(getSize(ref->type))
    });
    return load;
  }
  
  if (auto call = dyn_cast<CallNode>(node)) {
    std::vector<Value> args;
    for (auto arg : call->args)
      args.push_back(cgcEmitExpr(arg));

    // Note that "starttime" and "stoptime" are actually "_sysy_{start,stop}time".
    auto name = call->func;
    if (name == "starttime")
      name = "_sysy_starttime";
    if (name == "stoptime")
      name = "_sysy_stoptime";

    bool isFP = cgAstIsFloat(call->type);
    if (cgBitStubFoldEnabled() && !isFP) {
      if (args.size() == 2 && cgcIsBitwiseStubCallee(name)) {
        if (name == "_and")
          return builder.create<AndIOp>(args);
        if (name == "_xor")
          return builder.create<XorIOp>(args);
        if (name == "_or")
          return builder.create<OrIOp>(args);
      }
      if (name == "rotr8" && args.size() == 1)
        return builder.create<RShiftOp>({ args[0], cgEmitShiftAmount(builder, 8) });
      if (args.size() == 2) {
        if (auto *kNode = dyn_cast<IntNode>(call->args[1])) {
          int k = kNode->value;
          if (k >= 1 && k <= 30) {
            auto amt = cgEmitShiftAmount(builder, k);
            if (name == "rotrN")
              return builder.create<RShiftOp>({ args[0], amt });
            if (name == "rotlN")
              return builder.create<LShiftOp>({ args[0], amt });
          }
        }
      }
    }

    auto callOp = builder.create<CallOp>(isFP ? Value::f32 : Value::i32, args, {
      new NameAttr(name),
    });
    return callOp;
  }

  if (auto access = dyn_cast<ArrayAccessNode>(node)) {
    auto arrTy = cast<ArrayType>(access->arrTy);

    // Calculate a series of stride.
    std::vector<int> sizes;
    auto size = getSize(arrTy->base) * arrTy->getSize();
    for (int i = 0; i < arrTy->dims.size(); i++)
      sizes.push_back(size /= arrTy->dims[i]);
    
    Value addr;
    if (symbols.count(access->array))
      addr = builder.create<LoadOp>(Value::i64, {
        symbols[access->array]
      }, { new SizeAttr(8) });
    else if (globals.count(access->array))
      addr = builder.create<GetGlobalOp>({
        new NameAttr(access->array)
      });
    else {
      std::cerr << "unknown array: " << access->array << "\n";
      assert(false);
    }
    for (int i = 0; i < access->indices.size(); i++) {
      auto index = cgcEmitExpr(access->indices[i]);
      auto strideVal = builder.create<IntOp>({ new IntAttr(sizes[i]) });
      auto stride = builder.create<MulIOp>({ index, strideVal });
      addr = builder.create<AddLOp>({ addr, stride });
    }
    // This is not a value, but just an address.
    // Directly return the address (for, e.g. function arguments)
    if (arrTy->dims.size() > access->indices.size())
      return addr;

    // Store the value in addr.
    bool isFP = isa<FloatType>(arrTy->base);
    return builder.create<LoadOp>(isFP ? Value::f32 : Value::i32, { addr }, {
      new SizeAttr(getSize(arrTy->base))
    });
  }

  std::cerr << "cannot codegen node type " << node->getID() << "\n";
  assert(false);
  std::abort();
}
