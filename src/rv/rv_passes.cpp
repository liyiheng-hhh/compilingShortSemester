#include "rv_passes.h"

#include "../common.h"

namespace rv {

void runRvAsmPasses(std::vector<std::string> &lines, PassStats *stats) {
  if (envFlagTruthy("SYSY_CC_NO_RV_ASM_PASSES")) return;

  PassStats local;
  PassStats *s = stats ? stats : &local;

  strengthReductAsm(lines);
  branchPeephole(lines, s);       // slt/slti + beqz/bnez -> blt/bge (reference: FoldCompareBranch)
  regPeephole(lines, s);          // safe identity removal by default; aggressive CSE requires SYSY_CC_ENABLE_RV_AGGRESSIVE_PEEPHOLE
  instCombine(lines, s);
  if (envFlagTruthy("SYSY_CC_ENABLE_RV_SCHEDULE")) rvSchedule(lines, s);
}

}  // namespace rv
