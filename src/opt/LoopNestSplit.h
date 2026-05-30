#ifndef LOOP_NEST_SPLIT_H
#define LOOP_NEST_SPLIT_H

#include "Pass.h"
#include "../codegen/CodeGen.h"
#include "../codegen/Attrs.h"

namespace sys {

/// Experimental pass (Phase 3): split guarded matmul-like nests into
/// interior (no guard, eligible for aggressive RowScratch) and border.
/// On by default in O1; disable with SYSY_CC_NO_NEST_SPLIT=1.
/// Lightweight version: reuses LoopAnalysis + ParallelizableAttr.
class LoopNestSplit : public Pass {
  int split = 0;
  bool debug = false;

  void runOnFunc(FuncOp *func);

public:
  LoopNestSplit(ModuleOp *module);

  std::string name() override { return "loop-nest-split"; }
  std::map<std::string, int> stats() override;
  void run() override;
};

}  // namespace sys

#endif  // LOOP_NEST_SPLIT_H