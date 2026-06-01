#include "MemoryOpt.h"

#include "Analysis.h"

#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <vector>

using namespace sys;

namespace {

bool acEnvEnabled(const char *name, bool defaultVal = false) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return defaultVal;
  return std::strcmp(v, "0") != 0 && std::strcmp(v, "false") != 0;
}

Op *acAllocaBase(Op *addr) {
  if (!addr)
    return nullptr;
  std::set<Op*> seen;
  Op *cur = addr;
  while (cur && !seen.count(cur)) {
    seen.insert(cur);
    if (isa<AllocaOp>(cur))
      return cur;
    if (cur->has<AliasAttr>() && !ALIAS(cur)->unknown) {
      for (auto [base, _] : ALIAS(cur)->location) {
        if (isa<AllocaOp>(base))
          return base;
      }
    }
    if (auto *add = dyn_cast<AddLOp>(cur)) {
      Op *a = add->DEF(0);
      Op *b = add->DEF(1);
      if (isa<AllocaOp>(a))
        return a;
      if (isa<AllocaOp>(b))
        return b;
      if (a->has<AliasAttr>())
        cur = a;
      else if (b->has<AliasAttr>())
        cur = b;
      else
        break;
    } else {
      break;
    }
  }
  return nullptr;
}

bool acTouchesAlloca(Op *memOp, Op *slot) {
  if (!memOp || !slot)
    return false;
  if (isa<LoadOp>(memOp))
    return acAllocaBase(memOp->DEF()) == slot;
  if (isa<StoreOp>(memOp))
    return acAllocaBase(memOp->DEF(1)) == slot;
  return false;
}

bool acEscapes(AllocaOp *slot, FuncOp *func) {
  for (auto *call : func->findAll<CallOp>()) {
    for (auto operand : call->getOperands()) {
      if (acAllocaBase(operand.defining) == slot)
        return true;
    }
  }
  for (auto *store : func->findAll<StoreOp>()) {
    if (store->DEF(0) == slot)
      return true;
  }
  return false;
}

BasicBlock *acFirstUseBlock(AllocaOp *slot, Region *region) {
  for (auto *bb : region->getBlocks()) {
    for (auto *op : bb->getOps()) {
      if (acTouchesAlloca(op, slot))
        return bb;
    }
  }
  return nullptr;
}

bool acOnlyUsedInBlock(AllocaOp *slot, BasicBlock *bb) {
  Region *region = bb->getParent();
  for (auto *ob : region->getBlocks()) {
    if (ob == bb)
      continue;
    for (auto *mem : ob->getOps()) {
      if (acTouchesAlloca(mem, slot))
        return false;
    }
  }
  return true;
}

}  // namespace

std::map<std::string, int> AllocaCoalesce::stats() {
  return {
    { "moved", moved },
    { "coalesced", coalesced },
  };
}

void AllocaCoalesce::runImpl(Region *region) {
  auto *func = cast<FuncOp>(region->getParent());
  Builder builder;

  for (auto *op : func->findAll<AllocaOp>()) {
    auto *slot = cast<AllocaOp>(op);
    if (SIZE(slot) != 4)
      continue;
    if (acEscapes(slot, func))
      continue;
    if (!acEnvEnabled("SYSY_CC_ALLOCA_DEFER", false))
      continue;
    if (auto *useBB = acFirstUseBlock(slot, region)) {
      if (useBB != slot->getParent()) {
        slot->moveBefore(useBB->getFirstOp());
        moved++;
      }
    }
  }

  for (auto *bb : region->getBlocks()) {
    std::vector<AllocaOp*> scalars;
    for (auto *op : bb->getOps()) {
      auto *slot = dyn_cast<AllocaOp>(op);
      if (!slot || SIZE(slot) != 4)
        continue;
      if (acEscapes(slot, func))
        continue;
      if (!acOnlyUsedInBlock(slot, bb))
        continue;
      scalars.push_back(slot);
    }
    if (scalars.size() < 2)
      continue;

    int total = static_cast<int>(scalars.size()) * 4;
    builder.setToBlockStart(bb);
    auto *slab = builder.create<AllocaOp>({ new SizeAttr(total) });

    for (size_t i = 0; i < scalars.size(); i++) {
      auto *slot = scalars[i];
      int off = static_cast<int>(i) * 4;
      Op *newAddr = slab;
      if (off != 0) {
        builder.setAfterOp(slab);
        auto *offOp = builder.create<IntOp>({ new IntAttr(off) });
        newAddr = builder.create<AddLOp>(std::vector<Value>{ slab, offOp });
      }

      std::vector<Op*> users(slot->getUses().begin(), slot->getUses().end());
      for (auto *use : users) {
        if (auto *load = dyn_cast<LoadOp>(use)) {
          if (load->DEF() == slot)
            load->setOperand(0, newAddr);
        } else if (auto *store = dyn_cast<StoreOp>(use)) {
          if (store->DEF(1) == slot)
            store->setOperand(1, newAddr);
        }
      }
      if (slot->getUses().empty())
        slot->erase();
    }
    coalesced += static_cast<int>(scalars.size()) - 1;
  }
}

void AllocaCoalesce::run() {
  for (auto *func : collectFuncs())
    runImpl(func->getRegion());
}
