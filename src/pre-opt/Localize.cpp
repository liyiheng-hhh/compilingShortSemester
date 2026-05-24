#include "PrePasses.h"

using namespace sys;

namespace {

bool loadEscapesToCall(Op *load) {
  for (auto use : load->getUses()) {
    if (isa<CallOp>(use))
      return true;
  }
  return false;
}

bool globalEscapesToCall(FuncOp *user, const std::string &name) {
  for (auto get : user->findAll<GetGlobalOp>()) {
    if (NAME(get) != name)
      continue;
    for (auto use : get->getUses()) {
      if (isa<CallOp>(use))
        return true;
      if (isa<LoadOp>(use) && loadEscapesToCall(use))
        return true;
    }
  }
  return false;
}

bool globalHasStores(FuncOp *user, const std::string &name) {
  for (auto get : user->findAll<GetGlobalOp>()) {
    if (NAME(get) != name)
      continue;
    for (auto use : get->getUses()) {
      if (isa<StoreOp>(use))
        return true;
    }
  }
  return false;
}

}

void Localize::run() {
  auto funcs = collectFuncs();
  auto fnMap = getFunctionMap();

  auto getglobs = module->findAll<GetGlobalOp>();
  auto gMap = getGlobalMap();
  std::map<GlobalOp*, std::set<FuncOp*>> accessed;

  Builder builder;

  for (auto get : getglobs) {
    const auto &name = NAME(get);
    accessed[gMap[name]].insert(get->getParentOp<FuncOp>());
  }

  for (auto [name, k] : gMap) {
    // We don't want to localize an array. In fact, we hope to globalize them.
    if (SIZE(k) != 4)
      continue;

    if (!accessed.count(k)) {
      // The global variable is never accessed. Remove it.
      k->erase();
      continue;
    }

    auto v = accessed[k];
    if (v.size() > 1)
      continue;

    if (!(*v.begin())->has<AtMostOnceAttr>())
      continue;

    auto user = *v.begin();
  // Read-only scalars are better handled by EarlyConstFold than alloca lowering.
    if (!globalHasStores(user, name))
      continue;

    if (globalEscapesToCall(user, name))
      continue;

    // Now we can replace the global with a local variable.
    auto region = user->getRegion();

    auto entry = region->getFirstBlock();
    Op *addr;
    if (beforeFlatten) {
      builder.setToBlockEnd(entry);
      addr = builder.create<AllocaOp>({ new SizeAttr(4) });
    } else {
      builder.setBeforeOp(entry->getLastOp());
      addr = builder.create<AllocaOp>({ new SizeAttr(4) });
    }

    auto bb = region->insertAfter(entry);
    // We must make sure the whole entry block contains only alloca.
    // That's why we inserted a new block here.
    // This is also for further transformations that append allocas to the first block.
    builder.setToBlockStart(bb);
    Value init;
    if (auto intArr = k->find<IntArrayAttr>()) {
      init = builder.create<IntOp>({
        new IntAttr(intArr->vi[0])
      });
    } else {
      init = builder.create<FloatOp>({
        new FloatAttr(k->get<FloatArrayAttr>()->vf[0])
      });
    }
    builder.create<StoreOp>({ init, addr }, { new SizeAttr(4) });

    if (!beforeFlatten) {
      // Remember to supply terminators for after FlattenCFG.
      entry->getLastOp()->moveToEnd(bb);

      builder.setToBlockEnd(entry);
      builder.create<GotoOp>({ new TargetAttr(bb) });
    }

    // Replace all "getglobal" to use the addr instead.
    auto gets = user->findAll<GetGlobalOp>();
    for (auto get : gets) {
      if (NAME(get) == name) {
        get->replaceAllUsesWith(addr);
        get->erase();
      }
    }
  }
}
