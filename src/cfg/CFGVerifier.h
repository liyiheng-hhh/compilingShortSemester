#ifndef CFG_VERIFIER_H
#define CFG_VERIFIER_H

// compiler2026-x phase-1 (header layout)
#include "CFGOps.h"


#include <string>
#include <vector>

namespace sys::cfg {

bool verify(const Module &module, std::vector<std::string> &errors);

}  // namespace sys::cfg

#endif
