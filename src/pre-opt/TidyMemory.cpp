#include "PrePasses.h"
#include "PreAnalysis.h"

// compiler2026-x phase-D (trivial opt dedup)

using namespace sys;

namespace {

bool tmIsScalarAlloca(Op *base) {
  return isa<AllocaOp>(base) && SIZE(base) == 4;
}

} // namespace

std::map<std::string, int> TidyMemory::stats() {
  return { { "tidied-ops", tidied } };
}

void TidyMemory::runImpl(Region *region) {
  std::unordered_map<Op*, Op*> addrCache;
  for (auto bb : region->getBlocks()) {
    auto ops = bb->getOps();
    for (auto op : ops) {
      if (op->getRegionCount()) {
        addrCache.clear();
        for (auto r : op->getRegions())
          runImpl(r);
        continue;
      }
      if (isa<CallOp>(op) && op->has<ImpureAttr>()) {
        addrCache.clear();
        continue;
      }
      if (isa<StoreOp>(op)) {
        auto addr = op->DEF(1);
        auto val = op->DEF(0);
        if (isa<LoadOp>(val) || isa<CallOp>(val) || isa<GetArgOp>(val)) {
          addrCache.erase(addr);
          continue;
        }
        if (!addr->has<BaseAttr>()) {
          addrCache.clear();
          continue;
        }
        if (!tmIsScalarAlloca(BASE(addr)))
          continue;
        addrCache[addr] = val;
        continue;
      }
      if (isa<LoadOp>(op) && addrCache.count(op->DEF())) {
        op->replaceAllUsesWith(addrCache[op->DEF()]);
        op->erase();
        tidied++;
      }
    }
  }
}

void TidyMemory::run() {
  Base(module).run();
  for (auto func : collectFuncs())
    runImpl(func->getRegion());
}
