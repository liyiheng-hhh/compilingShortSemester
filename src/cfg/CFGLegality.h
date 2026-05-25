#ifndef CFG_LEGALITY_H
#define CFG_LEGALITY_H

// compiler2026-x phase-1 (header layout)
#include "CFGOps.h"


#include "../dialect_hir/DhirOps.h"

#include <string>
#include <vector>

namespace sys::cfg {

bool verifyHIRLegalSet(const dhir::Module &module, std::vector<std::string> &errors);
bool verifyCFGLegalSet(const Module &module, std::vector<std::string> &errors);
bool verifyHIRToCFGConversion(const dhir::Module &hirModule, const Module &cfgModule, std::vector<std::string> &errors);

}  // namespace sys::cfg

#endif
