// compiler2026-x phase-C (header layout)
#ifndef CFG_VERIFIER_H
#define CFG_VERIFIER_H

// compiler2026-x phase-1 (header layout)
#include <string>
#include <vector>
#include "CFGOps.h"


namespace sys::cfg {

bool verify(const Module &module, std::vector<std::string> &errors);

}  // namespace sys::cfg

#endif
