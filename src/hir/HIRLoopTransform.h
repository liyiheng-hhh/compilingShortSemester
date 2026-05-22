#pragma once

#include "HIROps.h"

namespace sys::hir {

// HIR-level loop transformations (Stage 2)
// These operate on the structured While/For nodes instead of flat IR or AST.

bool tryLoopInterchangeOnHIR(Module &module);
bool tryLoopTilingOnHIR(Module &module, int tileSize = 32);
bool tryLoopUnrollOnHIR(Module &module, int factor = 4);

} // namespace sys::hir