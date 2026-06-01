#ifndef GEP_CHAIN_FOLD_H
#define GEP_CHAIN_FOLD_H

#include "Pass.h"
#include "../codegen/CodeGen.h"
#include "../codegen/Attrs.h"

namespace sys {

enum class GcfMode {
  Full,
  CseOnly,
};

/// Fold repeated GEP-like address calculations (base + iv1*stride1 + iv2*stride2)
/// into a single AddL/MulI chain. Runs before GVN so that GVN can CSE the new chain.
/// On by default in O1; disable with SYSY_CC_NO_GEP_CHAIN=1.
class GepChainFold : public Pass {
  int folded = 0;
  int cse = 0;
  bool debug = false;
  GcfMode mode = GcfMode::Full;

  void runOnRegion(Region *region);
  bool tryFoldChain(Op *addr, std::vector<Op*> &chain);
  void tryRewriteChain(Op *memOp, const std::vector<Op*> &chain);

public:
  GepChainFold(ModuleOp *module, GcfMode mode = GcfMode::Full);

  std::string name() override { return "gep-chain-fold"; }
  std::map<std::string, int> stats() override;
  void run() override;
};

}  // namespace sys

#endif  // GEP_CHAIN_FOLD_H