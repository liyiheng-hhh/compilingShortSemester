#pragma once

#include "codegen/Ops.h"

#include <memory>
#include <string>
#include <vector>

namespace sys {

// Parse SysY source with dialect frontend, HIR→CFG→ModuleOp, then minimal mid-end passes.
// Returns nullptr on failure; errors are human-readable strings.
std::unique_ptr<ModuleOp> buildDialectModuleFromSource(const std::string &source,
                                                       std::vector<std::string> &errors);

void appendDialectMidEndPasses(class PassManager &pm);

// Mid-end + mlir_rv backend; returns full assembly text for the translation unit.
std::string emitDialectModuleAsm(ModuleOp *module, bool enableSchedule = true);

bool dialectPipelineEnabled();

}  // namespace sys
