// compiler2026-x phase-C (header layout)
#ifndef SMT_PASSES_H
#define SMT_PASSES_H

// compiler2026-x phase-1 (header layout)
#include "Pass.h"
#include "../utils/smt/SMT.h"
#include "../codegen/CodeGen.h"
#include "../codegen/Attrs.h"


namespace sys {

// Use SMT solver to guess a formula for constant arrys.
class SynthConstArray : public Pass {
  smt::BvExprContext ctx;

  std::vector<smt::BvExpr*> candidates;
  Builder builder;

  Op *reconstruct(smt::BvExpr *expr, Op *subscript, int c0, int c1);
public:
  SynthConstArray(ModuleOp *module);

  std::string name() override { return "synth-const-array"; };
  std::map<std::string, int> stats() override { return {}; }
  void run() override;
};

}

#endif