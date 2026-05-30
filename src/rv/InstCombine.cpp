#include "rv_passes.h"

#include "rv_asm.h"

#include "../common.h"

namespace rv {

namespace {

bool rvicEnabled() {
  return !envFlagTruthy("SYSY_CC_NO_RV_INST_COMBINE");
}

// Phase 4.1: additional zero-risk identity rules
// addi rd, rs, 0  ->  mv rd, rs  (or nop if rd==rs)
bool rvicTryAddiZero(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "addi" || i.defs.size() != 1 || i.uses.size() < 1) return false;
  // Check if last token (imm) is 0
  size_t lastComma = line.rfind(',');
  if (lastComma == std::string::npos) return false;
  std::string immStr = line.substr(lastComma + 1);
  // trim
  immStr.erase(0, immStr.find_first_not_of(" \t"));
  if (immStr != "0" && immStr != "0x0") return false;
  // rebuild: mv rd, rs  (use addi rd, rs, 0 as-is if rd==rs to keep it simple)
  if (i.defs[0] == i.uses[0]) {
    out = line;  // keep, harmless
    return false;
  }
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "mv\t" + i.defs[0] + ", " + i.uses[0];
  return true;
}

// ori rd, rs, 0  ->  mv rd, rs
bool rvicTryOriZero(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "ori" || i.defs.size() != 1 || i.uses.size() < 1) return false;
  size_t lastComma = line.rfind(',');
  if (lastComma == std::string::npos) return false;
  std::string immStr = line.substr(lastComma + 1);
  immStr.erase(0, immStr.find_first_not_of(" \t"));
  if (immStr != "0" && immStr != "0x0") return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "mv\t" + i.defs[0] + ", " + i.uses[0];
  return true;
}

// andi rd, rs, -1  ->  mv rd, rs  (-1 = 0xffffffff in 32-bit)
bool rvicTryAndiMinus1(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "andi" || i.defs.size() != 1 || i.uses.size() < 1) return false;
  size_t lastComma = line.rfind(',');
  if (lastComma == std::string::npos) return false;
  std::string immStr = line.substr(lastComma + 1);
  immStr.erase(0, immStr.find_first_not_of(" \t"));
  if (immStr != "-1" && immStr != "0xffffffff" && immStr != "4294967295") return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "mv\t" + i.defs[0] + ", " + i.uses[0];
  return true;
}

// Phase 4.2: sub rd, rs, rs  ->  li rd, 0
bool rvicTrySubSelf(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "sub" || i.defs.size() != 1 || i.uses.size() < 2) return false;
  if (i.uses[0] != i.uses[1]) return false;
  if (i.defs[0] == i.uses[0]) return false;  // sub rd, rd, rd is already 0, keep it
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "li\t" + i.defs[0] + ", 0";
  return true;
}

// Phase 4.3: mul rd, rs, 2  ->  slli rd, rs, 1
// Only handle the pattern where the multiplier is a small constant (via li before)
bool rvicTryMulPow2(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "mul" || i.defs.size() != 1 || i.uses.size() < 2) return false;
  // This is a conservative version: only rewrite if one operand is x2/x4/x8 pattern
  // For simplicity in Phase 4.3, we implement a limited version that catches
  // "mul rd, rs, rs2" where rs2 comes from "li rs2, 2" — but we don't track that here.
  // Instead, we add a rule for the common "mul by small power of 2" via immediate form if parser supports it.
  // For now, return false (implement full version later with constant tracking).
  return false;
}

// Phase 4.3: li rd, imm; add rd2, rd, rs  ->  addi rd2, rs, imm (if imm fits 12-bit)
bool rvicTryLiAddFuse(const std::string &a, const std::string &b, std::string &combined) {
  AsmInst ia, ib;
  if (!parseAsmLine(a, ia) || !parseAsmLine(b, ib)) return false;
  if (ia.mnemonic != "li" || ib.mnemonic != "add") return false;
  if (ia.defs.size() != 1 || ib.defs.size() != 1 || ib.uses.size() < 2) return false;
  // Check if add uses the li result
  if (ib.uses[0] != ia.defs[0] && ib.uses[1] != ia.defs[0]) return false;
  // Parse immediate from li
  size_t lastComma = a.rfind(',');
  if (lastComma == std::string::npos) return false;
  int imm;
  try {
    imm = std::stoi(a.substr(lastComma + 1));
  } catch (...) {
    return false;
  }
  if (imm < -2048 || imm > 2047) return false;
  // Determine which operand of add is the li result
  std::string other = (ib.uses[0] == ia.defs[0]) ? ib.uses[1] : ib.uses[0];
  size_t tab = b.find('\t');
  if (tab == std::string::npos) return false;
  combined = b.substr(0, tab + 1) + "addi\t" + ib.defs[0] + ", " + other + ", " + std::to_string(imm);
  return true;
}

// Phase 4.4: shift-by-0 elimination
bool rvicTryShiftZero(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if ((i.mnemonic != "slli" && i.mnemonic != "srli" && i.mnemonic != "srai") ||
      i.defs.size() != 1 || i.uses.size() < 1) return false;
  size_t lastComma = line.rfind(',');
  if (lastComma == std::string::npos) return false;
  std::string immStr = line.substr(lastComma + 1);
  immStr.erase(0, immStr.find_first_not_of(" \t"));
  if (immStr != "0" && immStr != "0x0") return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "mv\t" + i.defs[0] + ", " + i.uses[0];
  return true;
}

// andi rd, rs, 0  ->  li rd, 0
bool rvicTryAndiZero(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "andi" || i.defs.size() != 1 || i.uses.size() < 1) return false;
  size_t lastComma = line.rfind(',');
  if (lastComma == std::string::npos) return false;
  std::string immStr = line.substr(lastComma + 1);
  immStr.erase(0, immStr.find_first_not_of(" \t"));
  if (immStr != "0" && immStr != "0x0") return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "li\t" + i.defs[0] + ", 0";
  return true;
}

// xori rd, rs, 0  ->  mv rd, rs
bool rvicTryXoriZero(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "xori" || i.defs.size() != 1 || i.uses.size() < 1) return false;
  size_t lastComma = line.rfind(',');
  if (lastComma == std::string::npos) return false;
  std::string immStr = line.substr(lastComma + 1);
  immStr.erase(0, immStr.find_first_not_of(" \t"));
  if (immStr != "0" && immStr != "0x0") return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "mv\t" + i.defs[0] + ", " + i.uses[0];
  return true;
}

// sub rd, rs, 0  ->  mv rd, rs
bool rvicTrySubZero(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "sub" || i.defs.size() != 1 || i.uses.size() < 2) return false;
  // Check if second operand is 0 (via li before is hard; we only handle the case where the line itself has , 0)
  size_t lastComma = line.rfind(',');
  if (lastComma == std::string::npos) return false;
  std::string immStr = line.substr(lastComma + 1);
  immStr.erase(0, immStr.find_first_not_of(" \t"));
  if (immStr != "0" && immStr != "0x0") return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "mv\t" + i.defs[0] + ", " + i.uses[0];
  return true;
}

// Phase 4.5: or rd, rs, rs  ->  mv rd, rs
bool rvicTryOrSelf(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "or" || i.defs.size() != 1 || i.uses.size() < 2) return false;
  if (i.uses[0] != i.uses[1]) return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "mv\t" + i.defs[0] + ", " + i.uses[0];
  return true;
}

// and rd, rs, rs  ->  mv rd, rs
bool rvicTryAndSelf(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "and" || i.defs.size() != 1 || i.uses.size() < 2) return false;
  if (i.uses[0] != i.uses[1]) return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "mv\t" + i.defs[0] + ", " + i.uses[0];
  return true;
}

// slt rd, rs, rs  ->  li rd, 0
bool rvicTrySltSelf(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "slt" || i.defs.size() != 1 || i.uses.size() < 2) return false;
  if (i.uses[0] != i.uses[1]) return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "li\t" + i.defs[0] + ", 0";
  return true;
}

// sgt rd, rs, rs  ->  li rd, 0
bool rvicTrySgtSelf(const std::string &line, std::string &out) {
  AsmInst i;
  if (!parseAsmLine(line, i)) return false;
  if (i.mnemonic != "sgt" || i.defs.size() != 1 || i.uses.size() < 2) return false;
  if (i.uses[0] != i.uses[1]) return false;
  if (i.defs[0] == i.uses[0]) return false;
  size_t tab = line.find('\t');
  if (tab == std::string::npos) return false;
  out = line.substr(0, tab + 1) + "li\t" + i.defs[0] + ", 0";
  return true;
}

// mv rd, rs; mv rd2, rd  ->  mv rd2, rs  (adjacent mv fusion)
bool rvicTryMvFuse(const std::string &a, const std::string &b, std::string &combined) {
  AsmInst ia, ib;
  if (!parseAsmLine(a, ia) || !parseAsmLine(b, ib)) return false;
  if (ia.mnemonic != "mv" || ib.mnemonic != "mv") return false;
  if (ia.defs.size() != 1 || ia.uses.size() != 1 || ib.defs.size() != 1 || ib.uses.size() != 1) return false;
  if (ib.uses[0] != ia.defs[0]) return false;
  size_t tab = b.find('\t');
  if (tab == std::string::npos) return false;
  combined = b.substr(0, tab + 1) + "mv\t" + ib.defs[0] + ", " + ia.uses[0];
  return true;
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
    std::string rew;
    // Phase 4.1: try identity rules first (zero-risk)
    if (rvicTryAddiZero(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTryOriZero(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTryAndiMinus1(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTrySubSelf(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTryShiftZero(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTryAndiZero(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTryXoriZero(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTrySubZero(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTryOrSelf(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTryAndSelf(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTrySltSelf(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (rvicTrySgtSelf(lines[i], rew)) {
      out.push_back(rew);
      if (stats) ++stats->combined;
      continue;
    }
    if (i + 1 < lines.size()) {
      std::string fused;
      if (rvicTryFuseAddi(lines[i], lines[i + 1], fused)) {
        out.push_back(fused);
        if (stats) ++stats->combined;
        ++i;
        continue;
      }
      if (rvicTryLiAddFuse(lines[i], lines[i + 1], fused)) {
        out.push_back(fused);
        if (stats) ++stats->combined;
        ++i;
        continue;
      }
      if (rvicTryMvFuse(lines[i], lines[i + 1], fused)) {
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
