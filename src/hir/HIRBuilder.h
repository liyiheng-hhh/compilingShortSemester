#pragma once

#include "HIROps.h"
#include "../ast.h"

// Minimal HIRBuilder: converts SysY AST into HIR (preserves While/For/If structure)
// This is the foundation for advanced loop optimizations (Interchange, Tiling, Unroll)

namespace sys::hir {

std::unique_ptr<Module> buildHIR(const Program &program);

} // namespace sys::hir