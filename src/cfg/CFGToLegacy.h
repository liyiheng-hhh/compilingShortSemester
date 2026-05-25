// compiler2026-x phase-C (header layout)
#ifndef CFG_TO_LEGACY_H
#define CFG_TO_LEGACY_H

// compiler2026-x phase-1 (header layout)
#include <memory>
#include <string>
#include <vector>
#include "../codegen/Ops.h"
#include "CFGOps.h"


namespace sys::cfg {

bool verifyCFGToLegacyLegality(const Module &cfgModule, ModuleOp *legacyModule, std::vector<std::string> &errors);
std::unique_ptr<ModuleOp> lowerToLegacyIR(const Module &cfgModule, std::vector<std::string> &errors);

}  // namespace sys::cfg

#endif
