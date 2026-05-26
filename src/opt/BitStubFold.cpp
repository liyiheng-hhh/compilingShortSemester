#include "Passes.h"

#include <cstdlib>
#include <cstring>

using namespace sys;

namespace {

bool bitStubEnvOn(const char *name, bool fallback = true) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return fallback;
  if (std::strcmp(v, "0") == 0 || std::strcmp(v, "false") == 0)
    return false;
  return true;
}

// SysY 评测里常见的「逐位模拟」：32 次 while，内部 %2 与 /2。
bool looksLikeBitwiseSimulator(FuncOp *fn) {
  if (!fn || !fn->getRegion())
    return false;
  int modOps = 0;
  int divOps = 0;
  bool hasWhile = false;
  for (auto *bb : fn->getRegion()->getBlocks()) {
    for (auto *op : bb->getOps()) {
      if (isa<WhileOp>(op))
        hasWhile = true;
      if (isa<ModIOp>(op))
        modOps++;
      if (isa<DivIOp>(op))
        divOps++;
    }
  }
  return hasWhile && modOps >= 2 && divOps >= 2;
}

bool isUnaryDivBy(FuncOp *fn, int divisor) {
  if (!fn->get<ArgCountAttr>() || fn->get<ArgCountAttr>()->count != 1)
    return false;
  auto rets = fn->findAll<ReturnOp>();
  if (rets.size() != 1 || rets[0]->getOperandCount() != 1)
    return false;
  auto *val = rets[0]->DEF(0);
  if (!isa<DivIOp>(val) || val->getOperandCount() != 2)
    return false;
  auto *den = val->DEF(1);
  return isa<IntOp>(den) && V(den) == divisor;
}

enum class BitStubKind { None, And, Xor, Or };

BitStubKind classifyBitStub(const std::string &name, FuncOp *fn) {
  BitStubKind kind = BitStubKind::None;
  if (name == "_and")
    kind = BitStubKind::And;
  else if (name == "_xor")
    kind = BitStubKind::Xor;
  else if (name == "_or")
    kind = BitStubKind::Or;
  else
    return BitStubKind::None;

  if (!fn->get<ArgCountAttr>() || fn->get<ArgCountAttr>()->count != 2)
    return BitStubKind::None;
  if (!looksLikeBitwiseSimulator(fn))
    return BitStubKind::None;
  return kind;
}

Op *shiftAmount(Builder &builder, Op *before, int bits) {
  builder.setBeforeOp(before);
  return builder.create<IntOp>({ new IntAttr(bits) });
}

}  // namespace

std::map<std::string, int> BitStubFold::stats() {
  return {
    { "bitwise-stubs", bitwise },
    { "shift-stubs", shifts },
  };
}

void BitStubFold::run() {
  if (!bitStubEnvOn("SYSY_CC_ENABLE_BIT_STUB_FOLD", true))
    return;

  auto fnMap = getFunctionMap();
  std::map<std::string, BitStubKind> stubs;
  for (auto &[name, fn] : fnMap) {
    if (isExtern(name) || fn->has<ImpureAttr>())
      continue;
    auto kind = classifyBitStub(name, fn);
    if (kind != BitStubKind::None)
      stubs[name] = kind;
  }

  Builder builder;

  runRewriter([&](CallOp *call) {
    if (call->has<ImpureAttr>() || call->getOperandCount() == 0)
      return false;
    const auto &callee = NAME(call);
    auto it = stubs.find(callee);
    if (it != stubs.end() && call->getOperandCount() == 2) {
      builder.setBeforeOp(call);
      auto a = call->getOperand(0);
      auto b = call->getOperand(1);
      switch (it->second) {
      case BitStubKind::And:
        builder.replace<AndIOp>(call, { a, b });
        break;
      case BitStubKind::Xor:
        builder.replace<XorIOp>(call, { a, b });
        break;
      case BitStubKind::Or:
        builder.replace<OrIOp>(call, { a, b });
        break;
      default:
        return false;
      }
      bitwise++;
      return true;
    }

    if (call->getOperandCount() == 1 && isUnaryDivBy(fnMap[callee], 256)) {
      builder.setBeforeOp(call);
      auto amt = shiftAmount(builder, call, 8);
      builder.replace<RShiftOp>(call, { call->getOperand(0), amt });
      shifts++;
      return true;
    }

    if (call->getOperandCount() == 2) {
      auto *nDef = call->DEF(1);
      if (!isa<IntOp>(nDef))
        return false;
      int k = V(nDef);
      if (k < 1 || k > 30)
        return false;

      if (callee == "rotrN" || callee == "rotr8") {
        builder.setBeforeOp(call);
        auto amt = shiftAmount(builder, call, k);
        builder.replace<RShiftOp>(call, { call->getOperand(0), amt });
        shifts++;
        return true;
      }
      if (callee == "rotlN") {
        builder.setBeforeOp(call);
        auto amt = shiftAmount(builder, call, k);
        builder.replace<LShiftOp>(call, { call->getOperand(0), amt });
        shifts++;
        return true;
      }
    }

    return false;
  });
}
