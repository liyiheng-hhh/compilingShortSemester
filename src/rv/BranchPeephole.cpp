#include "rv_passes.h"

#include "rv_asm.h"

#include "../common.h"

#include <optional>
#include <string>
#include <vector>

namespace rv {

namespace {

bool rvbpEnabled() {
  return !envFlagTruthy("SYSY_CC_NO_RV_BRANCH_PEEPHOLE");
}

bool rvbpIsBlockBoundary(const std::string &line) {
  return !line.empty() && line[0] != '\t' && line[0] != ' ';
}

std::optional<std::string> rvbpTrimImm(const std::string &raw) {
  size_t lastComma = raw.rfind(',');
  if (lastComma == std::string::npos)
    return std::nullopt;
  size_t b = raw.find_first_not_of(" \t", lastComma + 1);
  if (b == std::string::npos)
    return std::nullopt;
  size_t e = raw.find_last_not_of(" \t");
  return raw.substr(b, e - b + 1);
}

// slt/slti (+ optional xori 1) + beqz/bnez rd,x0,L  ->  direct blt/bge/...
bool rvbpTryFoldCompareBranch(const std::vector<std::string> &lines, size_t i,
                              std::string &replacement, size_t &skip) {
  if (i >= lines.size())
    return false;

  AsmInst cmp;
  if (!parseAsmLine(lines[i], cmp))
    return false;

  bool isSlt = cmp.mnemonic == "slt";
  bool isSlti = cmp.mnemonic == "slti";
  if (!isSlt && !isSlti)
    return false;
  if (cmp.defs.size() != 1 || cmp.uses.size() < 1)
    return false;

  const std::string &cmpRd = cmp.defs[0];
  const std::string &lhs = cmp.uses[0];
  std::string rhs;
  if (isSlt) {
    if (cmp.uses.size() < 2)
      return false;
    rhs = cmp.uses[1];
  } else {
    auto imm = rvbpTrimImm(lines[i]);
    if (!imm)
      return false;
    rhs = *imm;
  }

  size_t j = i + 1;
  bool inverted = false;
  if (j < lines.size()) {
    AsmInst maybeXor;
    if (parseAsmLine(lines[j], maybeXor) && maybeXor.mnemonic == "xori" &&
        maybeXor.defs.size() == 1 && maybeXor.uses.size() >= 1 &&
        maybeXor.defs[0] == cmpRd && maybeXor.uses[0] == cmpRd) {
      auto ximm = rvbpTrimImm(lines[j]);
      if (ximm && *ximm == "1") {
        inverted = true;
        ++j;
      }
    }
  }

  if (j >= lines.size())
    return false;
  AsmInst br;
  if (!parseAsmLine(lines[j], br))
    return false;

  std::string brMnem = br.mnemonic;
  if (brMnem != "beqz" && brMnem != "bnez")
    return false;
  if (br.uses.empty() || br.uses[0] != cmpRd)
    return false;

  size_t labelComma = lines[j].rfind(',');
  if (labelComma == std::string::npos)
    return false;
  std::string label = lines[j].substr(labelComma + 1);
  size_t lb = label.find_first_not_of(" \t");
  if (lb != std::string::npos)
    label = label.substr(lb);
  size_t le = label.find_last_not_of(" \t");
  if (le != std::string::npos)
    label = label.substr(0, le + 1);

  bool branchOnTrue = (brMnem == "bnez") ^ inverted;
  std::string direct;
  if (isSlt) {
    direct = branchOnTrue ? "blt" : "bge";
    replacement = "\t" + direct + "\t" + lhs + ", " + rhs + ", " + label;
  } else {
    // slti rd, rs, imm  — only fold the slti 0 / slti 1 shapes codegen emits.
    if (rhs == "0") {
      direct = branchOnTrue ? "blt" : "bge";
      replacement = "\t" + direct + "\t" + lhs + ", x0, " + label;
    } else if (rhs == "1") {
      direct = branchOnTrue ? "bne" : "beq";
      replacement = "\t" + direct + "\t" + lhs + ", x0, " + label;
    } else {
      return false;
    }
  }

  skip = j - i + 1;
  return true;
}

}  // namespace

void branchPeephole(std::vector<std::string> &lines, PassStats *stats) {
  if (!rvbpEnabled())
    return;

  std::vector<std::string> out;
  out.reserve(lines.size());

  for (size_t i = 0; i < lines.size(); ++i) {
    if (rvbpIsBlockBoundary(lines[i])) {
      out.push_back(lines[i]);
      continue;
    }

    std::string fused;
    size_t skip = 0;
    if (rvbpTryFoldCompareBranch(lines, i, fused, skip)) {
      out.push_back(fused);
      if (stats)
        ++stats->combined;
      i += skip - 1;
      continue;
    }

    out.push_back(lines[i]);
  }

  lines.swap(out);
}

}  // namespace rv
