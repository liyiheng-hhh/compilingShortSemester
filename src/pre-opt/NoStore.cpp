#include "PreAnalysis.h"

// compiler2026-x phase-D (trivial opt dedup)

using namespace sys;

namespace {

bool nsTouchesGlobalStore(FuncOp *func) {
  for (auto store : func->findAll<StoreOp>()) {
    auto addr = store->DEF(1);
    if (!addr->has<BaseAttr>())
      return true;
    if (isa<GetGlobalOp>(BASE(addr)))
      return true;
  }
  return false;
}

} // namespace

void NoStore::runImpl(Op *func) {
  if (nsTouchesGlobalStore(cast<FuncOp>(func)))
    return;
  if (!func->has<NoStoreAttr>())
    func->add<NoStoreAttr>();
}

void NoStore::run() {
  Base(module).run();
  for (auto func : collectFuncs())
    runImpl(func);
}
