#pragma once

#include "ir.h"

#include <vector>

// Cooper 2001 idom + 支配树子节点（Mem2Reg / GVN / Reassociate 共用）
struct IrDomTree {
  int n = 0;
  std::vector<int> idom;
  std::vector<std::vector<int>> children;

  void build(IRFunction &fn);
};
