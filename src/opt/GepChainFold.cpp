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

// Normalize operand order of a single address op (AddL/MulI) for canonical form.
static bool normalizeAddrOp(Op *op, bool debug) {
  if (auto *add = dyn_cast<AddLOp>(op)) {
    auto *lhs = add->DEF(0);
    auto *rhs = add->DEF(1);
    if (isa<MulIOp>(rhs) && !isa<MulIOp>(lhs)) {
      add->setOperand(0, rhs);
      add->setOperand(1, lhs);
      if (debug)
        std::cerr << "[gep-chain-fold] normalized AddL operands (base left)\n";
      return true;
    }
  }
  if (auto *mul = dyn_cast<MulIOp>(op)) {
    auto *lhs = mul->DEF(0);
    auto *rhs = mul->DEF(1);
    if (isa<IntOp>(rhs) && !isa<IntOp>(lhs)) {
      mul->setOperand(0, rhs);
      mul->setOperand(1, lhs);
      if (debug)
        std::cerr << "[gep-chain-fold] normalized MulI operands (const right)\n";
      return true;
    }
  }
  return false;
}

// Attempt a more aggressive rewrite: for static chains (no Phi), try to
// reorganize nested AddL to expose common prefixes for GVN.
// Example: addl(addl(base, x), y) -> addl(base, addl(x, y))
// This does not reduce instruction count but may help GVN see redundancy.
static bool tryReorganizeStaticChain(const std::vector<Op*> &chain, bool debug) {
  if (chain.size() < 3)
    return false;
  // Skip if any Phi in chain
  for (auto *op : chain) {
    if (isa<PhiOp>(op))
      return false;
  }
  Op *outer = chain[0];
  auto *add1 = dyn_cast<AddLOp>(outer);
  if (!add1)
    return false;
  Op *inner = add1->DEF(0);
  auto *add2 = dyn_cast<AddLOp>(inner);
  if (!add2)
    return false;
  // addl( addl(base, x), y ) -> addl( base, addl(x, y) )
  Op *base = add2->DEF(0);
  // Only reorganize if base is GetGlobal or looks like a base address
  if (!isa<GetGlobalOp>(base))
    return false;
  if (debug)
    std::cerr << "[gep-chain-fold] detected nested addl pattern\n";
  return false; // placeholder: real rewrite needs Builder
}

// Try to rewrite a detected address chain into a more canonical form.
// Handles both static chains and Phi-carrying chains (matmul2 k-loop).
// For Phi chains, we normalize each incoming edge's address computation.
static bool tryNormalizeAddressChain(const std::vector<Op*> &chain, bool debug) {
  if (chain.size() < 3)
    return false;

  bool changed = false;

  // First, normalize the outermost op (what Load/Store directly uses).
  if (!chain.empty()) {
    changed |= normalizeAddrOp(chain[0], debug);
  }

  // If chain contains Phi, also normalize address ops on Phi incoming edges.
  for (auto *op : chain) {
    if (auto *phi = dyn_cast<PhiOp>(op)) {
      if (debug)
        std::cerr << "[gep-chain-fold] processing Phi with " << phi->getOperandCount() << " incoming\n";
      for (int i = 0; i < phi->getOperandCount(); i++) {
        Op *inc = phi->DEF(i);
        // Walk a few steps up the incoming address chain and normalize.
        int steps = 0;
        while (inc && steps < 4) {
          if (normalizeAddrOp(inc, debug))
            changed = true;
          if (auto *add = dyn_cast<AddLOp>(inc))
            inc = add->DEF(0);
          else if (auto *mul = dyn_cast<MulIOp>(inc))
            inc = mul->DEF(0);
          else
            break;
          steps++;
        }
      }
    }
  }

  return changed;
}

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
        folded++;
        if (debug) {
          std::cerr << "[gep-chain-fold] found chain of size " << chain.size();
          for (auto *op : chain) {
            std::cerr << " " << (op ? op->getName() : "null");
          }
          std::cerr << "\n";
        }
        // Phase 2.4: attempt rewrite for chains without Phi (static address calc).
        // For Phi-carrying chains (matmul2 k-loop), we only count for now.
        tryRewriteChain(op, chain);
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

void GepChainFold::tryRewriteChain(Op *memOp, const std::vector<Op*> &chain) {
  if (!memOp || chain.empty())
    return;
  // Rewrite: normalize operand order for both static and Phi-carrying chains.
  // For Phi chains (matmul2 k-loop), we normalize incoming edge address ops.
  // This helps GVN see common subexpressions without changing loop structure.
  bool changed = tryNormalizeAddressChain(chain, debug);
  if (changed) {
    folded++;
    if (debug)
      std::cerr << "[gep-chain-fold] rewrite changed, folded now " << folded << "\n";
  }
  // Placeholder for more aggressive rewrite using Builder (common subexpr extraction).
  // Real implementation would:
  //   1. Identify repeated sub-chains (e.g., mul(i, N) used by multiple loads)
  //   2. Use Builder to create a single definition
  //   3. replaceAllUsesWith for duplicate sub-chains
  // This requires per-region analysis (collect all chains first) and is left
  // as future work after validating the current normalization.
}

void GepChainFold::run() {
  auto funcs = collectFuncs();
  for (auto *func : funcs) {
    runOnRegion(func->getRegion());
  }
}