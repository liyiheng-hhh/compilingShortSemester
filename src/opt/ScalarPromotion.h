#ifndef SCALAR_PROMOTION_H
#define SCALAR_PROMOTION_H

#include "Pass.h"

namespace sys {

/// Promote scalar allocas (and scalar globals) inside loops to SSA phi values.
/// This reduces memory traffic for loop-carried scalars like `temp` in matmul2.
/// On by default in O1; disable with SYSY_CC_NO_SCALAR_PROMOTION=1.
class ScalarPromotion : public Pass {
  int promoted = 0;
  bool debug = false;

  void runOnFunc(FuncOp *func);

public:
  ScalarPromotion(ModuleOp *module);

  std::string name() override { return "scalar-promotion"; }
  std::map<std::string, int> stats() override;
  void run() override;
};

}  // namespace sys

#endif  // SCALAR_PROMOTION_H
