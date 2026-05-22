#pragma once

#include "ast.h"

// Row-scratch matmul optimization (inspired by Sisyphus RowScratchMatmul)
// Detects naive 3-nested matmul loops and applies A[i][k] hoist or
// rewrites to a more cache-friendly form.
// Enable/disable via SYSY_CC_ENABLE_ROW_SCRATCH_MATMUL (default on for O1)

void applyRowScratchMatmulPass(Program &program);