#pragma once

#include "../../codegen/OpBase.h"

#include <functional>
#include <unordered_map>

namespace sys::backend::shared {

// Coarse block weights for spill heuristics: loop headers (back edges) and
// call-heavy blocks are treated as hotter.
template <typename IsHotOp>
inline std::unordered_map<BasicBlock *, int>
computeBlockHotness(Region *region, IsHotOp isHotOp) {
  region->updateDoms();
  std::unordered_map<BasicBlock *, int> weight;
  for (auto *bb : region->getBlocks())
    weight[bb] = 1;

  for (auto *bb : region->getBlocks()) {
    for (auto *succ : bb->succs) {
      if (succ->dominates(bb))
        weight[succ] += 8;
    }
    for (auto *op : bb->getOps()) {
      if (isHotOp(op)) {
        weight[bb] += 4;
        break;
      }
    }
  }
  return weight;
}

} // namespace sys::backend::shared
