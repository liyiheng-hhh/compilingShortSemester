#pragma once

#include "HIROps.h"

// RowScratchMatmul optimization on HIR (High-level IR)
// This version operates directly on the structured HIR (While/For/Block/Store/Arith)
// instead of the original AST. It is intended to be more robust for future
// loop transformations (interchange, tiling, unroll).

namespace sys::hir {

// Apply the row-scratch / A[i][k] hoist optimization on a built HIR module.
// Returns true if any transformation was performed.
bool applyRowScratchMatmulOnHIR(Module &module);

} // namespace sys::hir