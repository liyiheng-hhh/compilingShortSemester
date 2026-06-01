#include "GepChainFold.h"

#include <cstdlib>
#include <cstring>
#include <map>
#include <unordered_set>
#include <vector>

using namespace sys;

namespace {

bool envFlag(const char *name) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return false;
  return std::strcmp(v, "0") != 0 && std::strcmp(v, "false") != 0;
}

struct GcfMulKey {
  Op *iv = nullptr;
  int coeff = 0;

  bool operator<(const GcfMulKey &o) const {
    if (iv != o.iv)
      return iv < o.iv;
    return coeff < o.coeff;
  }
};

GetGlobalOp *getBaseGlobal(Op *addr) {
  std::unordered_set<Op*> visited;
  while (addr && visited.find(addr) == visited.end()) {
    visited.insert(addr);
    if (auto *g = dyn_cast<GetGlobalOp>(addr))
      return g;
    if (auto *add = dyn_cast<AddLOp>(addr)) {
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

bool gcfIsAddressMul(MulIOp *mul) {
  if (!mul || mul->getOperandCount() != 2)
    return false;
  if (!isa<IntOp>(mul->DEF(0)) && !isa<IntOp>(mul->DEF(1)))
    return false;
  if (mul->getUses().empty())
    return false;
  for (Op *user : mul->getUses()) {
    if (!isa<AddLOp>(user))
      return false;
  }
  return true;
}

GcfMulKey gcfMulKey(MulIOp *mul) {
  if (isa<IntOp>(mul->DEF(0)))
    return { mul->DEF(1), V(mul->DEF(0)) };
  return { mul->DEF(0), V(mul->DEF(1)) };
}

bool normalizeAddrOp(Op *op, bool debug) {
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

bool tryNormalizeAddressChain(const std::vector<Op*> &chain, bool debug) {
  if (chain.size() < 3)
    return false;

  bool changed = false;
  if (!chain.empty())
    changed |= normalizeAddrOp(chain[0], debug);

  for (auto *op : chain) {
    if (auto *phi = dyn_cast<PhiOp>(op)) {
      if (debug)
        std::cerr << "[gep-chain-fold] processing Phi with " << phi->getOperandCount()
                  << " incoming\n";
      for (int i = 0; i < phi->getOperandCount(); i++) {
        Op *inc = phi->DEF(i);
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

int gcfCseAddressMulsInBlock(BasicBlock *bb, bool debug) {
  std::map<GcfMulKey, std::vector<MulIOp*>> groups;
  for (auto *op : bb->getOps()) {
    auto *mul = dyn_cast<MulIOp>(op);
    if (!mul || !gcfIsAddressMul(mul))
      continue;
    groups[gcfMulKey(mul)].push_back(mul);
  }

  int removed = 0;
  for (auto &[key, muls] : groups) {
    if (muls.size() < 2)
      continue;
    MulIOp *keep = muls.front();
    for (size_t i = 1; i < muls.size(); i++) {
      MulIOp *dup = muls[i];
      if (dup == keep)
        continue;
      dup->replaceAllUsesWith(keep);
      dup->erase();
      removed++;
      if (debug) {
        std::cerr << "[gep-chain-fold] cse mul(iv," << key.coeff << ") in "
                  << bbmap[bb] << "\n";
      }
    }
  }
  return removed;
}

struct GcfRowKey {
  Op *base = nullptr;
  GcfMulKey mul;

  bool operator<(const GcfRowKey &o) const {
    if (base != o.base)
      return base < o.base;
    return mul < o.mul;
  }
};

bool gcfRowBaseKey(AddLOp *add, GcfRowKey &key) {
  auto *mul = dyn_cast<MulIOp>(add->DEF(0));
  if (!mul || !gcfIsAddressMul(mul))
    return false;
  Op *base = add->DEF(1);
  if (!getBaseGlobal(base))
    return false;
  key = { base, gcfMulKey(mul) };
  return true;
}

bool gcfAddOnlyUsedByAddL(AddLOp *add) {
  for (Op *user : add->getUses()) {
    if (user == add)
      continue;
    if (!isa<AddLOp>(user))
      return false;
  }
  return !add->getUses().empty();
}

int gcfCseRowBaseAddsInBlock(BasicBlock *bb, bool debug) {
  std::map<GcfRowKey, std::vector<AddLOp*>> groups;
  for (auto *op : bb->getOps()) {
    auto *add = dyn_cast<AddLOp>(op);
    if (!add)
      continue;
    GcfRowKey key;
    if (!gcfRowBaseKey(add, key))
      continue;
    if (!gcfAddOnlyUsedByAddL(add))
      continue;
    groups[key].push_back(add);
  }

  int removed = 0;
  for (auto &[key, adds] : groups) {
    if (adds.size() < 2)
      continue;
    AddLOp *keep = adds.front();
    for (size_t i = 1; i < adds.size(); i++) {
      AddLOp *dup = adds[i];
      if (dup == keep)
        continue;
      dup->replaceAllUsesWith(keep);
      dup->erase();
      removed++;
      if (debug) {
        std::cerr << "[gep-chain-fold] cse row base addl in " << bbmap[bb]
                  << " mul(iv," << key.mul.coeff << ")\n";
      }
    }
  }
  return removed;
}

}  // namespace

GepChainFold::GepChainFold(ModuleOp *module, GcfMode modeIn) : Pass(module), mode(modeIn) {
  debug = envFlag("SYSY_CC_GEP_CHAIN_DEBUG");
}

std::map<std::string, int> GepChainFold::stats() {
  return {
    { "folded", folded },
    { "cse", cse },
  };
}

void GepChainFold::runOnRegion(Region *region) {
  if (!region)
    return;

  for (auto *bb : region->getBlocks()) {
    int removed = gcfCseAddressMulsInBlock(bb, debug);
    removed += gcfCseRowBaseAddsInBlock(bb, debug);
    cse += removed;
    folded += removed;
  }

  if (mode == GcfMode::CseOnly)
    return;

  for (auto *bb : region->getBlocks()) {
    for (auto *op : bb->getOps()) {
      Op *addr = nullptr;
      if (auto *load = dyn_cast<LoadOp>(op))
        addr = load->DEF(0);
      else if (auto *store = dyn_cast<StoreOp>(op))
        addr = store->DEF(1);
      if (!addr)
        continue;

      std::vector<Op*> chain;
      if (tryFoldChain(addr, chain)) {
        folded++;
        if (debug) {
          std::cerr << "[gep-chain-fold] found chain of size " << chain.size();
          for (auto *chainOp : chain)
            std::cerr << " " << (chainOp ? chainOp->getName() : "null");
          std::cerr << "\n";
        }
        tryRewriteChain(op, chain);
      }
    }
  }
}

bool GepChainFold::tryFoldChain(Op *addr, std::vector<Op*> &chain) {
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
    if (auto *add = dyn_cast<AddLOp>(cur))
      cur = add->DEF(0);
    else if (auto *mul = dyn_cast<MulIOp>(cur))
      cur = mul->DEF(0);
    else if (auto *mul = dyn_cast<MulLOp>(cur))
      cur = mul->DEF(0);
    else if (auto *phi = dyn_cast<PhiOp>(cur)) {
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

  if (opCount >= 3)
    return true;

  chain.clear();
  return false;
}

void GepChainFold::tryRewriteChain(Op *memOp, const std::vector<Op*> &chain) {
  if (!memOp || chain.empty())
    return;
  if (tryNormalizeAddressChain(chain, debug)) {
    folded++;
    if (debug)
      std::cerr << "[gep-chain-fold] rewrite changed, folded now " << folded << "\n";
  }
}

void GepChainFold::run() {
  for (auto *func : collectFuncs())
    runOnRegion(func->getRegion());
}
