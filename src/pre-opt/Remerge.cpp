#include "PrePasses.h"

// compiler2026-x phase-D (trivial opt dedup)

using namespace sys;

namespace {

void rmgFlattenToEntry(Region *region) {
  auto entry = region->getFirstBlock();
  const auto &bbs = region->getBlocks();
  for (auto bb : bbs) {
    if (bb != entry)
      bb->inlineToEnd(entry);
  }
  for (auto it = --bbs.end(); it != bbs.begin();) {
    auto prev = it;
    --prev;
    (*it)->erase();
    it = prev;
  }
}

} // namespace

void Remerge::runImpl(Region *region) {
  rmgFlattenToEntry(region);
  for (auto op : region->getFirstBlock()->getOps()) {
    if (!op->getRegionCount())
      continue;
    for (auto nested : op->getRegions())
      runImpl(nested);
  }
}

void Remerge::run() {
  for (auto func : collectFuncs())
    runImpl(func->getRegion());
  MoveAlloca(module).run();
}
