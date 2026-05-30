#include "rv_passes.h"

#include "BlockLocalCSE.h"
#include "../common.h"

namespace rv {

void runRvAsmPasses(std::vector<std::string> &lines, PassStats *stats) {
  if (envFlagTruthy("SYSY_CC_NO_RV_ASM_PASSES")) return;

  PassStats local;
  PassStats *s = stats ? stats : &local;

  strengthReductAsm(lines);
  branchPeephole(lines, s);
  regPeephole(lines, s);
  instCombine(lines, s);

  // BlockLocalCSE: li/la materialize + pure arithmetic (reference: BlockLocalCSE)
  // Run after InstCombine so that identity rules have already fired.
  if (!envFlagTruthy("SYSY_CC_NO_RV_BLOCK_CSE")) {
    auto cseStats = BlockLocalCSE::run(lines);
    if (s) {
      s->blockCSE_li += cseStats.liCSE;
      s->blockCSE_la += cseStats.laCSE;
      s->blockCSE_arith += cseStats.arithCSE;
    }
  }

  if (envFlagTruthy("SYSY_CC_ENABLE_RV_SCHEDULE")) rvSchedule(lines, s);
}

}  // namespace rv
