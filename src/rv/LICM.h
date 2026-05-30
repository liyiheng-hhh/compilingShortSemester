#pragma once

#include "rv_asm.h"

#include <string>
#include <vector>

namespace rv {

// Hoist loop-invariant la/li from natural loops in textual asm.
// Reference: compiler2026-main backend/LICM.cpp (adapted to line-based blocks).
class RvLICM {
public:
  struct Stats {
    int hoisted = 0;
    int laMerged = 0;
  };

  static Stats run(std::vector<std::string> &lines);
};

}  // namespace rv
