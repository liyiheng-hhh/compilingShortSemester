#ifndef GEP_CHAIN_FOLD_H
#define GEP_CHAIN_FOLD_H

#include "Pass.h"
#include "../codegen/CodeGen.h"
#include "../codegen/Attrs.h"

namespace sys {

/// Fold repeated GEP-like address calculations (base + iv1*stride1 + iv2*stride2)
/// into a single AddL/MulI chain. Runs before GVN so that GVN can CSE the new chain.
/// Enabled by SYSY_CC_ENABLE_GEP_CHAIN=1 (default off, experimental).
class GepChainFold : public Pass {
  int folded = 0;
  bool debug = false;

  void runOnRegion(Region *region);
  bool tryFoldChain(Op *addr, std::vector<Op*> &chain);

public:
  GepChainFold(ModuleOp *module);

  std::string name() override { return "gep-chain-fold"; }
  std::map<std::string, int> stats() override;
  void run() override;
};

}  // namespace sys

#endif  // GEP_CHAIN_FOLD_H