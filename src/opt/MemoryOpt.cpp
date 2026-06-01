#include "MemoryOpt.h"

#include "Analysis.h"
#include "../pre-opt/PreAttrs.h"

#include <map>
#include <set>
#include <vector>

using namespace sys;

namespace {

Op *memAddr(Op *memOp) {
  if (isa<LoadOp>(memOp))
    return memOp->DEF();
  if (isa<StoreOp>(memOp))
    return memOp->DEF(1);
  return nullptr;
}

struct MemLocKey {
  Op *base = nullptr;
  std::vector<int> sub;
  Op *addr = nullptr;

  bool operator<(const MemLocKey &o) const {
    if (base != o.base)
      return base < o.base;
    if (sub != o.sub)
      return sub < o.sub;
    return addr < o.addr;
  }
};

MemLocKey memLocKey(Op *addr) {
  MemLocKey key;
  if (addr && addr->has<BaseAttr>() && addr->has<SubscriptAttr>()) {
    key.base = BASE(addr);
    key.sub = SUBSCRIPT(addr);
    return key;
  }
  key.addr = addr;
  return key;
}

bool memSameLocation(Op *a, Op *b) {
  if (!a || !b)
    return false;
  if (a == b)
    return true;
  if (mustAlias(a, b))
    return true;
  if (a->has<BaseAttr>() && b->has<BaseAttr>() &&
      a->has<SubscriptAttr>() && b->has<SubscriptAttr>() &&
      BASE(a) == BASE(b) && SUBSCRIPT(a) == SUBSCRIPT(b))
    return true;
  return false;
}

bool memSameLocationKey(const MemLocKey &a, const MemLocKey &b) {
  if (a.base && b.base)
    return a.base == b.base && a.sub == b.sub;
  if (a.addr && b.addr)
    return memSameLocation(a.addr, b.addr);
  return false;
}

bool memSameValue(Op *a, Op *b) {
  return a && b && a == b;
}

bool memInvalidatesStoreMap(Op *call) {
  if (!isa<CallOp>(call))
    return false;
  if (call->has<ImpureAttr>())
    return true;
  for (auto operand : call->getOperands()) {
    auto *def = operand.defining;
    if (!def || !def->has<AliasAttr>())
      continue;
    if (ALIAS(def)->unknown)
      return true;
  }
  return false;
}

void memPruneStoreMap(std::map<MemLocKey, Op*> &map, Op *storeAddr) {
  MemLocKey storeKey = memLocKey(storeAddr);
  for (auto it = map.begin(); it != map.end();) {
    if (memSameLocationKey(it->first, storeKey))
      it = map.erase(it);
    else if (it->first.addr && storeAddr && mayAlias(it->first.addr, storeAddr))
      it = map.erase(it);
    else
      ++it;
  }
}

}  // namespace

std::map<std::string, int> RemoveRedundantStore::stats() {
  return { { "removed", removed } };
}

void RemoveRedundantStore::runImpl(Region *region) {
  for (auto *bb : region->getBlocks()) {
    std::map<MemLocKey, Op*> lastLoadVal;
    std::vector<StoreOp*> eraseStores;

    std::vector<Op*> ops(bb->getOps().begin(), bb->getOps().end());
    for (auto *op : ops) {
      if (isa<CallOp>(op)) {
        if (memInvalidatesStoreMap(op))
          lastLoadVal.clear();
        continue;
      }

      if (auto *load = dyn_cast<LoadOp>(op)) {
        lastLoadVal[memLocKey(memAddr(load))] = load;
        continue;
      }

      if (auto *store = dyn_cast<StoreOp>(op)) {
        Op *addr = memAddr(store);
        Op *val = store->DEF(0);
        MemLocKey key = memLocKey(addr);

        auto it = lastLoadVal.find(key);
        if (it != lastLoadVal.end()) {
          auto *prevLoad = dyn_cast<LoadOp>(it->second);
          if (prevLoad && memSameValue(val, prevLoad)) {
            eraseStores.push_back(store);
            continue;
          }
        }

        for (auto lit = lastLoadVal.begin(); lit != lastLoadVal.end();) {
          if (memSameLocationKey(lit->first, key))
            lit = lastLoadVal.erase(lit);
          else if (lit->first.addr && addr && mayAlias(lit->first.addr, addr))
            lit = lastLoadVal.erase(lit);
          else
            ++lit;
        }
      }
    }

    for (auto *store : eraseStores) {
      if (!store->getParent())
        continue;
      store->erase();
      removed++;
    }
  }

  // store %v, %a  followed by store %w, %a' (same cell) with no intervening load/call
  for (auto *bb : region->getBlocks()) {
    std::vector<Op*> ops(bb->getOps().begin(), bb->getOps().end());
    for (size_t i = 0; i < ops.size(); i++) {
      auto *store = dyn_cast<StoreOp>(ops[i]);
      if (!store || !store->getParent())
        continue;
      Op *addr = memAddr(store);
      for (size_t j = i + 1; j < ops.size(); j++) {
        if (isa<LoadOp>(ops[j]) || isa<CallOp>(ops[j]))
          break;
        if (auto *next = dyn_cast<StoreOp>(ops[j])) {
          if (memSameLocation(addr, memAddr(next))) {
            store->erase();
            removed++;
          }
          break;
        }
      }
    }
  }
}

void RemoveRedundantStore::run() {
  Alias(module).run();
  for (auto *func : collectFuncs())
    runImpl(func->getRegion());
}

std::map<std::string, int> ArrayStoreLoadForward::stats() {
  return { { "forwarded", forwarded } };
}

void ArrayStoreLoadForward::runImpl(Region *region) {
  for (auto *bb : region->getBlocks()) {
    std::map<MemLocKey, Op*> recentStore;
    std::vector<std::pair<LoadOp*, Op*>> forward;

    std::vector<Op*> ops(bb->getOps().begin(), bb->getOps().end());
    for (auto *op : ops) {
      if (isa<CallOp>(op)) {
        if (memInvalidatesStoreMap(op)) {
          recentStore.clear();
          continue;
        }
        std::set<Op*> touched;
        for (auto operand : op->getOperands()) {
          auto *def = operand.defining;
          if (!def || !def->has<AliasAttr>())
            continue;
          for (auto [base, _] : ALIAS(def)->location)
            touched.insert(base);
        }
        for (auto it = recentStore.begin(); it != recentStore.end();) {
          if (it->first.base && touched.count(it->first.base))
            it = recentStore.erase(it);
          else
            ++it;
        }
        continue;
      }

      if (auto *store = dyn_cast<StoreOp>(op)) {
        Op *addr = memAddr(store);
        memPruneStoreMap(recentStore, addr);
        recentStore[memLocKey(addr)] = store->DEF(0);
        continue;
      }

      if (auto *load = dyn_cast<LoadOp>(op)) {
        MemLocKey key = memLocKey(memAddr(load));
        for (auto &[skey, val] : recentStore) {
          if (!memSameLocationKey(skey, key))
            continue;
          forward.emplace_back(load, val);
          break;
        }
      }
    }

    for (auto [load, val] : forward) {
      if (!load->getParent())
        continue;
      load->replaceAllUsesWith(val);
      load->erase();
      forwarded++;
    }
  }
}

void ArrayStoreLoadForward::run() {
  Alias(module).run();
  for (auto *func : collectFuncs())
    runImpl(func->getRegion());
}

namespace {

Op *rowaAllocaBase(Op *addr) {
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

bool rowaEscapes(AllocaOp *slot, FuncOp *func) {
  for (auto *call : func->findAll<CallOp>()) {
    for (auto operand : call->getOperands()) {
      if (rowaAllocaBase(operand.defining) == slot)
        return true;
    }
  }
  for (auto *store : func->findAll<StoreOp>()) {
    if (store->DEF(0) == slot)
      return true;
  }
  return false;
}

}  // namespace

std::map<std::string, int> RemoveOnlyWriteArray::stats() {
  return {
    { "removed-arrays", removedArrays },
    { "removed-stores", removedStores },
  };
}

void RemoveOnlyWriteArray::runImpl(Region *region) {
  auto *func = cast<FuncOp>(region->getParent());
  std::vector<AllocaOp*> dead;

  for (auto *slot : func->findAll<AllocaOp>()) {
    auto *alloca = cast<AllocaOp>(slot);
    if (rowaEscapes(alloca, func))
      continue;

    bool loaded = false;
    for (auto *load : func->findAll<LoadOp>()) {
      if (rowaAllocaBase(load->DEF()) == alloca) {
        loaded = true;
        break;
      }
    }
    if (loaded)
      continue;

    std::vector<StoreOp*> stores;
    for (auto *storeOp : func->findAll<StoreOp>()) {
      auto *store = cast<StoreOp>(storeOp);
      if (rowaAllocaBase(store->DEF(1)) == alloca)
        stores.push_back(store);
    }
    if (stores.empty())
      continue;

    for (auto *store : stores) {
      store->erase();
      removedStores++;
    }
    dead.push_back(alloca);
  }

  for (auto *slot : dead) {
    if (slot->getUses().empty()) {
      slot->erase();
      removedArrays++;
    }
  }
}

void RemoveOnlyWriteArray::run() {
  Alias(module).run();
  for (auto *func : collectFuncs())
    runImpl(func->getRegion());
}
