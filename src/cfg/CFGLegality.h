// compiler2026-x phase-C (header layout)
#ifndef CFG_LEGALITY_H
#define CFG_LEGALITY_H

// compiler2026-x phase-1 (header layout)
#include <string>
#include <vector>
#include "../dialect_hir/DhirOps.h"
#include "CFGOps.h"


namespace sys::cfg {

bool verifyHIRLegalSet(const dhir::Module &module, std::vector<std::string> &errors);
bool verifyCFGLegalSet(const Module &module, std::vector<std::string> &errors);
bool verifyHIRToCFGConversion(const dhir::Module &hirModule, const Module &cfgModule, std::vector<std::string> &errors);

}  // namespace sys::cfg

#endif
