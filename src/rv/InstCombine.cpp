#include "rv_passes.h"

#include "rv_asm.h"

#include "../common.h"

namespace rv {

namespace {

bool rvicEnabled() {
  return !envFlagTruthy("SYSY_CC_NO_RV_INST_COMBINE");
}

// addi rd, rs, imm  +  addi rd2, rd, imm2  ->  addi rd2, rs, imm+imm2
// when rd is not used between (single-use chain) — conservative: only adjacent
bool rvicTryFuseAddi(const std::string &a, const std::string &b, std::string &combined) {
  AsmInst ia, ib;
  if (!parseAsmLine(a, ia) || !parseAsmLine(b, ib)) return false;
  if (ia.mnemonic != "addi" || ib.mnemonic != "addi") return false;
  if (ia.defs.size() != 1 || ib.defs.size() != 1 || ia.uses.size() < 1 || ib.uses.size() < 1)
    return false;
  if (ib.uses[0] != ia.defs[0]) return false;

  // Parse immediates from raw lines
  auto lastImm = [](const std::string &raw) -> int {
    size_t lastComma = raw.rfind(',');
    if (lastComma == std::string::npos) return 0;
    try {
      return std::stoi(raw.substr(lastComma + 1));
    } catch (...) {
      return 0;
    }
  };

  int imm1 = lastImm(a);
  int imm2 = lastImm(b);
  int sum = imm1 + imm2;
  if (sum < -2048 || sum > 2047) return false;

  // Rebuild: addi rd2, rs, sum
  size_t tab = b.find('\t');
  if (tab == std::string::npos) return false;
  combined = b.substr(0, tab + 1) + "addi\t" + ib.defs[0] + ", " + ia.uses[0] +
               ", " + std::to_string(sum);
  return true;
}

}  // namespace

void instCombine(std::vector<std::string> &lines, PassStats *stats) {
  if (!rvicEnabled()) return;

  std::vector<std::string> out;
  out.reserve(lines.size());

  for (size_t i = 0; i < lines.size(); ++i) {
    if (i + 1 < lines.size()) {
      std::string fused;
      if (rvicTryFuseAddi(lines[i], lines[i + 1], fused)) {
        out.push_back(fused);
        if (stats) ++stats->combined;
        ++i;
        continue;
      }
    }
    out.push_back(lines[i]);
  }

  lines.swap(out);
}

}  // namespace rv
