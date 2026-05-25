// compiler2026-x phase-C (header layout)
#ifndef HIR_TO_CFG_H
#define HIR_TO_CFG_H

#include <string>
#include <vector>
#include "CFGOps.h"


namespace sys::cfg {

Module lowerFromHIR(const dhir::Module &hirModule, std::vector<std::string> &errors);

}  // namespace sys::cfg

#endif
