#pragma once

#include "codegen/Ops.h"

#include <string>
#include <vector>

namespace sys {

bool rvMlirBackendEnabled();

// Run reference-style RV passes; return asm for onlyFunc (empty = whole module).
std::string runRvMlirPipeline(ModuleOp *module, bool enableSchedule = true,
                              const std::string &onlyFunc = "");

}  // namespace sys
