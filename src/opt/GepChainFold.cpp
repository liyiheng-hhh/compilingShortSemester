#include "GepChainFold.h"

#include <cstdlib>
#include <cstring>
#include <unordered_set>

using namespace sys;

namespace {

bool envFlag(const char *name) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return false;
  return std::strcmp(v, "0") != 0 && std::strcmp(v, "false") != 0;
}

// Check if op is a constant IntOp with specific value.
static bool isConstInt(Op *op, int val) {
  if (auto *intOp = dyn_cast<IntOp>(op))
    return V(intOp) == val;
  return false;
}

// Check if op is a Phi (likely induction variable).
static bool isPhi(Op *op) {
  return isa<PhiOp>(op);
}

// Extract the base GetGlobalOp from an address chain (if any).
// Phase 2.3 improvement: also walk through Phi (for matmul2 hotspot patterns).
static GetGlobalOp *getBaseGlobal(Op *addr) {
  std::unordered_set<Op*> visited;
  while (addr && visited.find(addr) == visited.end()) {
    visited.insert(addr);
    if (auto *g = dyn_cast<GetGlobalOp>(addr))
      return g;
    if (auto *add = dyn_cast<AddLOp>(addr)) {
      // Prefer the operand that is a global or has alias attr.
      for (int i = 0; i < add->getOperandCount(); i++) {
        if (auto *g = dyn_cast<GetGlobalOp>(add->DEF(i)))
          return g;
      }
      addr = add->DEF(0);
      continue;
    }
    if (auto *mul = dyn_cast<MulIOp>(addr)) {
      addr = mul->DEF(0);
      continue;
    }
    if (auto *phi = dyn_cast<PhiOp>(addr)) {
      // Walk through phi (common in loop-carried address updates).
      // Prefer the operand that leads to a global.
      for (int i = 0; i < phi->getOperandCount(); i++) {
        if (auto *g = getBaseGlobal(phi->DEF(i)))
          return g;
      }
      addr = phi->DEF(0);
      continue;
    }
    break;
  }
  return nullptr;
}

}  // namespace

GepChainFold::GepChainFold(ModuleOp *module) : Pass(module) {
  debug = envFlag("SYSY_CC_GEP_CHAIN_DEBUG");
}

std::map<std::string, int> GepChainFold::stats() {
  return {
    { "folded", folded }
  };
}

void GepChainFold::runOnRegion(Region *region) {
  if (!region)
    return;
  for (auto *bb : region->getBlocks()) {
    for (auto *op : bb->getOps()) {
      Op *addr = nullptr;
      if (auto *load = dyn_cast<LoadOp>(op)) {
        addr = load->DEF(0);
      } else if (auto *store = dyn_cast<StoreOp>(op)) {
        addr = store->DEF(1);
      }
      if (!addr)
        continue;

      std::vector<Op*> chain;
      if (tryFoldChain(addr, chain)) {
        // Phase 2.2: placeholder rewrite — just count for now.
        // Real rewrite (Builder + replace uses) will be added after validation.
        folded++;
        if (debug) {
          std::cerr << "[gep-chain-fold] found chain of size " << chain.size() << "\n";
        }
      }
    }
  }
}

bool GepChainFold::tryFoldChain(Op *addr, std::vector<Op*> &chain) {
  // Phase 2.3 improved detection:
  // 1. Must have a GetGlobalOp base (via getBaseGlobal)
  // 2. The address is computed via a chain of AddL / MulI / MulL / Phi
  // 3. Require at least 3 address ops (base + at least two AddL/Mul) for a "chain"
  if (!addr)
    return false;

  GetGlobalOp *base = getBaseGlobal(addr);
  if (!base)
    return false;

  int depth = 0;
  int opCount = 0;
  Op *cur = addr;
  std::unordered_set<Op*> visited;
  while (cur && visited.find(cur) == visited.end() && depth < 6) {
    visited.insert(cur);
    chain.push_back(cur);
    opCount++;
    if (auto *add = dyn_cast<AddLOp>(cur)) {
      cur = add->DEF(0);
    } else if (auto *mul = dyn_cast<MulIOp>(cur)) {
      cur = mul->DEF(0);
    } else if (auto *mul = dyn_cast<MulLOp>(cur)) {
      cur = mul->DEF(0);
    } else if (auto *phi = dyn_cast<PhiOp>(cur)) {
      // For phi-carried addresses, take the first non-phi operand to continue.
      cur = nullptr;
      for (int i = 0; i < phi->getOperandCount(); i++) {
        if (!isa<PhiOp>(phi->DEF(i))) {
          cur = phi->DEF(i);
          break;
        }
      }
    } else {
      break;
    }
    depth++;
  }

  // Require a meaningful chain: base + at least two address ops.
  if (opCount >= 3)
    return true;

  chain.clear();
  return false;
}

void GepChainFold::run() {
  if (!envFlag("SYSY_CC_ENABLE_GEP_CHAIN"))
    return;
  auto funcs = collectFuncs();
  for (auto *func : funcs) {
    runOnRegion(func->getRegion());
  }
}