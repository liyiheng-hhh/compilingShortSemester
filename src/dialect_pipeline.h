#pragma once

#include "codegen/Ops.h"
#include "opt/PassManager.h"

#include <memory>
#include <string>
#include <vector>

namespace sys {

// Parse SysY source with dialect frontend (CodeGen structured IR), run mid-end passes, emit RV asm.
// Returns nullptr on failure; errors are human-readable strings.
std::unique_ptr<ModuleOp> buildDialectModuleFromSource(const std::string &source,
                                                       std::vector<std::string> &errors);

void appendDialectMidEndPasses(PassManager &pm);

// Mid-end + mlir_rv backend; returns full assembly text for the translation unit.
// Debug hooks via PassDebugOptions or env: SYSY_CC_COMPARE_WITH, SYSY_CC_SIMULATE_INPUT, ...
std::string emitDialectModuleAsm(ModuleOp *module, bool enableSchedule = true,
                                 const PassDebugOptions &debug = {});

bool dialectPipelineEnabled();

}  // namespace sys
