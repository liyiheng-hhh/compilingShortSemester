#ifndef HIR_LOWERING_H
#define HIR_LOWERING_H

#include "HIROps.h"

#include <memory>
#include <vector>

class IRFunction;

namespace sys::hir {

// Real HIR → legacy IR lowering (Stage 2).
// For every Func in the HIR Module, we create a corresponding IRFunction,
// emit real IRInst (including Label/J/Beqz for control flow), and
// populate the provided vector. After this, the functions are ready for
// Mem2Reg / GVN / LICM etc.
//
// If outFns is empty on return, lowering produced nothing (caller can
// fall back to the original AST path).
void lowerToLegacyIR(const Module &module, std::vector<IRFunction> &outFns);

}  // namespace sys::hir

#endif
