// compiler2026-x phase-C (header layout)
#ifndef RV_DUP_PASSES_H
#define RV_DUP_PASSES_H

#include "../codegen/Attrs.h"
#include "../codegen/CodeGen.h"
#include "../codegen/Ops.h"
#include "../opt/Pass.h"
#include "RvAttrs.h"
#include "RvOps.h"


namespace sys::rv {

// The only difference with opt/DCE is that `isImpure` behaves differently.
class RvDCE : public Pass {
  std::vector<Op*> removeable;
  int elim = 0;

  bool isImpure(Op *op);
  void markImpure(Region *region);
  void runOnRegion(Region *region);

public:
  RvDCE(ModuleOp *module): Pass(module) {}
    
  std::string name() override { return "rv-dce"; };
  std::map<std::string, int> stats() override;
  void run() override;
};

}

#endif
