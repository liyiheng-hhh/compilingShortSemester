#include "Analysis.h"

using namespace sys;

namespace {

bool pureGlobalHasStores(Op *getGlobal) {
  for (auto use : getGlobal->getUses()) {
    if (isa<StoreOp>(use))
      return true;
    if (isa<AddIOp>(use) || isa<AddLOp>(use)) {
      if (pureGlobalHasStores(use))
        return true;
      continue;
    }
    if (isa<LoadOp>(use))
      continue;
    return false;
  }
  return false;
}

}  // namespace

void Pureness::run() {
  auto funcs = collectFuncs();

  // Construct a call graph.
  auto fnMap = getFunctionMap();
  auto calls = module->findAll<CallOp>();
  for (auto call : calls) {
    auto func = call->getParentOp<FuncOp>();
    auto calledName = NAME(call);
    if (!isExtern(calledName))
      callGraph[func].insert(fnMap[calledName]);
    else if (!func->has<ImpureAttr>())
      // External functions are impure.
      func->add<ImpureAttr>();
  }

  // Every function that accesses globals is impure.
  for (auto func : funcs) {
    if (!func->has<ImpureAttr>() && !func->findAll<GetGlobalOp>().empty())
      func->add<ImpureAttr>();
  }

  // Propagate impureness across functions:
  // if a functions calls any impure function then it becomes impure.
  bool changed;
  do {
    changed = false;
    for (auto func : funcs) {
      bool impure = false;
      for (auto v : callGraph[func]) {
        if (v->has<ImpureAttr>()) {
          impure = true;
          break;
        }
      }
      if (!func->has<ImpureAttr>() && impure) {
        changed = true;
        func->add<ImpureAttr>();
      }
    }
  } while (changed);

  // GVN/LICM/GCM consult ImpureAttr on CallOp, not FuncOp. Mark call sites now
  // so the first post-flatten GVN cannot CSE impure calls like get_random().
  for (auto call : calls) {
    if (call->has<ImpureAttr>())
      continue;
    const auto &calledName = NAME(call);
    if (isExtern(calledName)) {
      call->add<ImpureAttr>();
      continue;
    }
    auto it = fnMap.find(calledName);
    if (it == fnMap.end() || !it->second || it->second->has<ImpureAttr>())
      call->add<ImpureAttr>();
  }

  // Mutable globals (e.g. crypto `state`) must survive DSE::removeUnread even
  // after impure callees are inlined and loads are optimized away.
  auto gMap = getGlobalMap();
  for (auto get : module->findAll<GetGlobalOp>()) {
    if (pureGlobalHasStores(get))
      gMap[NAME(get)]->add<ImpureAttr>();
  }
}
