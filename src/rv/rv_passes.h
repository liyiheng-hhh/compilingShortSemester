#pragma once

#include <string>
#include <vector>

// Stage 3: RISC-V assembly post-processing passes on textual asm.
// Operates on textual asm emitted by CodeGen.

namespace rv {

struct PassStats {
  int removedMv = 0;
  int removedDupLoad = 0;
  int combined = 0;
  int scheduled = 0;
  int blockCSE_li = 0;
  int blockCSE_la = 0;
  int blockCSE_arith = 0;
  int licm_hoisted = 0;
  int licm_laMerged = 0;
};

// Run all enabled RV passes in order: RegPeephole -> InstCombine -> Schedule.
void runRvAsmPasses(std::vector<std::string> &lines, PassStats *stats = nullptr);

void regPeephole(std::vector<std::string> &lines, PassStats *stats = nullptr);
void branchPeephole(std::vector<std::string> &lines, PassStats *stats = nullptr);
void instCombine(std::vector<std::string> &lines, PassStats *stats = nullptr);
void rvSchedule(std::vector<std::string> &lines, PassStats *stats = nullptr);
void strengthReductAsm(std::vector<std::string> &lines);

}  // namespace rv
