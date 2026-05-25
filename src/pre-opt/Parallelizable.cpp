#include "PreAnalysis.h"

// compiler2026-x phase-D (trivial opt dedup)

using namespace sys;

namespace {

inline bool parAbort(bool cond) { return cond; }

int parLoopIndexAtDepth(Op *addr, int depth) {
  const auto &sub = SUBSCRIPT(addr);
  auto stride = sub[depth];
  return stride ? sub.back() / (stride / 4) : -1;
}

bool parCallIsAllowed(Op *call, const std::map<std::string, FuncOp*> &fnByName) {
  const auto &name = NAME(call);
  if (!call->has<ImpureAttr>())
    return true;
  if (isExtern(name))
    return false;
  return fnByName.at(name)->has<NoStoreAttr>();
}

} // namespace

void Parallelizable::runImpl(Op *loop, int depth) {
  NoStore(module).run();
  auto fnByName = getFunctionMap();

  for (auto op : loop->getRegion()->getFirstBlock()->getOps()) {
    if (isa<ForOp>(op))
      runImpl(op, depth + 1);
  }

  for (auto call : loop->findAll<CallOp>()) {
    if (parAbort(!parCallIsAllowed(call, fnByName)))
      return;
  }
  if (loop->findAll<ReturnOp>().size())
    return;

  std::unordered_map<Op*, std::vector<std::pair<Op*, bool>>> memAccess, memOps;
  for (auto store : loop->findAll<StoreOp>()) {
    auto addr = store->DEF(1);
    if (parAbort(!addr->has<BaseAttr>()))
      return;
    memAccess[BASE(addr)].emplace_back(addr, true);
    memOps[BASE(addr)].emplace_back(store, true);
  }
  for (auto load : loop->findAll<LoadOp>()) {
    auto addr = load->DEF();
    if (parAbort(!addr->has<BaseAttr>()))
      return;
    memAccess[BASE(addr)].emplace_back(addr, false);
    memOps[BASE(addr)].emplace_back(load, false);
  }

  std::set<Op*> topLevelStoreBases;
  for (auto op : loop->getRegion()->getFirstBlock()->getOps()) {
    if (isa<StoreOp>(op))
      topLevelStoreBases.insert(BASE(op->DEF(1)));
  }

  for (const auto &[memBase, entries] : memAccess) {
    (void)memBase;
    assert(entries.size());
    auto [addr, isStore] = entries[0];
    if (!addr->has<SubscriptAttr>()) {
      if (!topLevelStoreBases.count(addr))
        continue;
      for (auto [_, storesHere] : entries) {
        if (parAbort(storesHere))
          return;
      }
      continue;
    }

    bool anyStore = false;
    for (auto [_, storesHere] : entries) {
      if (storesHere) { anyStore = true; break; }
    }
    if (!anyStore)
      continue;

    auto refStride = SUBSCRIPT(addr)[depth];
    auto refIndex = parLoopIndexAtDepth(addr, depth);
    for (auto [peerAddr, _] : entries) {
      if (parAbort(!peerAddr->has<SubscriptAttr>()))
        return;
      auto peerStride = SUBSCRIPT(peerAddr)[depth];
      auto peerIndex = parLoopIndexAtDepth(peerAddr, depth);
      if (parAbort(peerStride != refStride || peerIndex != refIndex))
        return;
    }
  }

  for (const auto &[memBase, trace] : memOps) {
    (void)memBase;
    Op *firstLoad = nullptr;
    for (auto [op, isStore] : trace) {
      if (!isStore) { firstLoad = op; break; }
    }
    if (!firstLoad || firstLoad == trace[0].first)
      continue;

    auto [firstStore, _] = trace[0];
    std::vector<Op*> storeAncestors;
    for (auto runner = firstStore; runner != loop; runner = runner->getParentOp())
      storeAncestors.push_back(runner);

    for (auto runner = firstLoad; runner != loop; runner = runner->getParentOp()) {
      bool decided = false, storeBeforeLoad = false;
      for (auto parent : storeAncestors) {
        if (runner->getParent() != parent->getParent())
          continue;
        decided = true;
        for (auto w = parent; !w->atBack(); w = w->nextOp()) {
          if (w == runner) { storeBeforeLoad = true; break; }
        }
        if (parent->getParent()->getLastOp() == runner) {
          storeBeforeLoad = true;
          break;
        }
      }
      if (decided && !storeBeforeLoad)
        return;
      if (decided)
        break;
    }
  }

  loop->add<ParallelizableAttr>();
}

void Parallelizable::run() {
  ArrayAccess(module).run();
  Base(module).run();
  for (auto func : collectFuncs()) {
    auto region = func->getRegion();
    for (auto bb : region->getBlocks())
      for (auto op : bb->getOps())
        op->remove<ParallelizableAttr>();
    for (auto bb : region->getBlocks())
      for (auto op : bb->getOps())
        if (isa<ForOp>(op))
          runImpl(op, 0);
  }
}
