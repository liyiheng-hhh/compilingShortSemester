// Stage 3: Strength reduction helpers.
// Primary signed div/mul-by-constant lowering lives in src/codegen.cpp (IR emission).
// This file documents the split; future RV-op-level rewrites can be added here.

#include "rv_passes.h"

#include "../common.h"

namespace rv {

// Reserved for additional asm-level strength reductions (e.g. rem -> sub+mul).
// Currently a no-op placeholder so the Stage 3 pipeline layout matches Sisyphus.
void strengthReductAsm(std::vector<std::string> & /*lines*/) {
  if (envFlagTruthy("SYSY_CC_NO_RV_STRENGTH_REDUCT")) return;
}

}  // namespace rv
