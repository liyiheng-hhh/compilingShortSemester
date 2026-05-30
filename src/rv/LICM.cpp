#include "LICM.h"

#include "../common.h"

#include <algorithm>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace rv {

namespace {

bool envDisabled() {
  return envFlagTruthy("SYSY_CC_NO_RV_LICM");
}

std::string trim(const std::string &s) {
  size_t b = s.find_first_not_of(" \t");
  if (b == std::string::npos) return "";
  size_t e = s.find_last_not_of(" \t");
  return s.substr(b, e - b + 1);
}

std::string lineLabel(const std::string &line) {
  if (line.empty() || line[0] == '\t' || line[0] == ' ')
    return "";
  if (line.back() != ':')
    return "";
  return line.substr(0, line.size() - 1);
}

std::optional<std::string> branchTarget(const std::string &line) {
  AsmInst inst;
  if (!parseAsmLine(line, inst)) return std::nullopt;
  const std::string &m = inst.mnemonic;
  if (m == "j" || m == "jr") {
    size_t pos = line.find_first_not_of(" \t");
    if (pos == std::string::npos) return std::nullopt;
    size_t sp = line.find_first_of(" \t", pos);
    if (sp == std::string::npos) return std::nullopt;
    std::string t = trim(line.substr(sp));
    if (!t.empty() && t[0] == '.') return t;
    return std::nullopt;
  }
  if (m.empty() || m[0] != 'b') return std::nullopt;
  size_t lastComma = line.rfind(',');
  if (lastComma == std::string::npos) return std::nullopt;
  std::string t = trim(line.substr(lastComma + 1));
  if (t.empty() || t[0] != '.') return std::nullopt;
  return t;
}

bool isGlobalLabelOperand(const std::string &line, const AsmInst &inst) {
  if (inst.mnemonic != "la") return false;
  size_t comma = line.find(',');
  if (comma == std::string::npos) return false;
  std::string sym = trim(line.substr(comma + 1));
  return !sym.empty() && sym[0] != '.';
}

std::string laSymbol(const std::string &line) {
  size_t comma = line.find(',');
  if (comma == std::string::npos) return "";
  return trim(line.substr(comma + 1));
}

bool regDefinedLaterInBlock(const std::vector<std::string> &lines, size_t blockStart,
                            size_t blockEnd, size_t fromIdx, const std::string &reg) {
  for (size_t i = fromIdx + 1; i < blockEnd; ++i) {
    AsmInst inst;
    if (!parseAsmLine(lines[i], inst)) continue;
    for (const auto &d : inst.defs) {
      if (d == reg) return true;
    }
  }
  return false;
}

bool regDefinedElsewhereInLoop(const std::vector<std::string> &lines,
                               const std::vector<std::pair<size_t, size_t>> &blocks,
                               size_t headerBlock, size_t latchBlock, size_t skipIdx,
                               const std::string &reg) {
  for (size_t bi = headerBlock; bi <= latchBlock; ++bi) {
    auto [start, end] = blocks[bi];
    for (size_t i = start; i < end; ++i) {
      if (i == skipIdx) continue;
      AsmInst inst;
      if (!parseAsmLine(lines[i], inst)) continue;
      for (const auto &d : inst.defs) {
        if (d == reg) return true;
      }
    }
  }
  return false;
}

void replaceRegInLoop(std::vector<std::string> &lines,
                      const std::vector<std::pair<size_t, size_t>> &blocks,
                      size_t headerBlock, size_t latchBlock, size_t afterIdx,
                      const std::string &from, const std::string &to) {
  if (from.empty() || from == to) return;
  for (size_t bi = headerBlock; bi <= latchBlock; ++bi) {
    auto [start, end] = blocks[bi];
    for (size_t i = start; i < end; ++i) {
      if (i <= afterIdx) continue;
      AsmInst inst;
      if (!parseAsmLine(lines[i], inst)) continue;
      bool changed = false;
      for (auto &u : inst.uses) {
        if (u == from) {
          u = to;
          changed = true;
        }
      }
      if (!changed) continue;
      std::string indent = asmLeadingWhitespace(lines[i]);
      if (indent.empty()) indent = "  ";
      if (inst.mnemonic == "mv" && inst.defs.size() == 1 && inst.uses.size() == 1)
        lines[i] = indent + "mv " + inst.defs[0] + ", " + inst.uses[0];
      else if (inst.mnemonic == "addi" && inst.defs.size() == 1 && inst.uses.size() >= 1) {
        size_t c2 = lines[i].rfind(',');
        if (c2 != std::string::npos)
          lines[i] = indent + "addi " + inst.defs[0] + ", " + inst.uses[0] +
                     ", " + trim(lines[i].substr(c2 + 1));
      } else if (inst.defs.size() >= 1 && inst.uses.size() >= 2) {
        lines[i] = indent + inst.mnemonic + " " + inst.defs[0] + ", " +
                   inst.uses[0] + ", " + inst.uses[1];
      }
    }
  }
}

size_t preheaderInsertPos(const std::vector<std::string> &lines,
                          const std::vector<std::pair<size_t, size_t>> &blocks,
                          size_t preheaderBlock) {
  auto [start, end] = blocks[preheaderBlock];
  size_t pos = end;
  for (size_t i = start; i < end; ++i) {
    if (isBlockTerminatorLine(lines[i]))
      pos = i;
  }
  return pos;
}

struct LoopRegion {
  size_t headerBlock = 0;
  size_t latchBlock = 0;
};

std::vector<LoopRegion> findNaturalLoops(
    const std::vector<std::string> &lines,
    const std::vector<std::pair<size_t, size_t>> &blocks) {
  std::unordered_map<std::string, size_t> labelBlock;
  for (size_t bi = 0; bi < blocks.size(); ++bi) {
    auto [start, end] = blocks[bi];
    for (size_t i = start; i < end; ++i) {
      std::string lab = lineLabel(lines[i]);
      if (!lab.empty())
        labelBlock[lab] = bi;
    }
  }

  std::vector<LoopRegion> loops;
  for (size_t bi = 0; bi < blocks.size(); ++bi) {
    auto [start, end] = blocks[bi];
    for (size_t i = start; i < end; ++i) {
      auto target = branchTarget(lines[i]);
      if (!target) continue;
      auto it = labelBlock.find(*target);
      if (it == labelBlock.end()) continue;
      if (it->second > bi) continue;
      loops.push_back({it->second, bi});
    }
  }

  std::sort(loops.begin(), loops.end(), [](const LoopRegion &a, const LoopRegion &b) {
    size_t spanA = a.latchBlock - a.headerBlock;
    size_t spanB = b.latchBlock - b.headerBlock;
    if (spanA != spanB) return spanA < spanB;
    return a.headerBlock > b.headerBlock;
  });
  return loops;
}

void eraseLineIndices(std::vector<std::string> &lines, std::vector<size_t> &idxs) {
  std::sort(idxs.begin(), idxs.end());
  idxs.erase(std::unique(idxs.begin(), idxs.end()), idxs.end());
  for (auto it = idxs.rbegin(); it != idxs.rend(); ++it) {
    if (*it < lines.size())
      lines.erase(lines.begin() + static_cast<long>(*it));
  }
}

}  // namespace

RvLICM::Stats RvLICM::run(std::vector<std::string> &lines) {
  Stats stats;
  if (envDisabled()) return stats;

  std::vector<std::pair<size_t, size_t>> blocks;
  splitBasicBlocks(lines, blocks);
  if (blocks.size() < 2) return stats;

  const auto loops = findNaturalLoops(lines, blocks);

  for (const auto &loop : loops) {
    splitBasicBlocks(lines, blocks);
    if (loop.headerBlock == 0 || loop.headerBlock >= blocks.size()) continue;
    const size_t preheader = loop.headerBlock - 1;

    std::vector<size_t> eraseIdx;
    std::vector<std::string> insertLines;
    std::unordered_map<std::string, std::string> laCanonReg;

    auto [phStart, phEnd] = blocks[preheader];
    for (size_t pi = phStart; pi < phEnd; ++pi) {
      AsmInst pInst;
      if (!parseAsmLine(lines[pi], pInst) || pInst.mnemonic != "la") continue;
      if (!isGlobalLabelOperand(lines[pi], pInst) || pInst.defs.empty()) continue;
      laCanonReg[laSymbol(lines[pi])] = pInst.defs[0];
    }

    for (size_t bi = loop.headerBlock; bi <= loop.latchBlock && bi < blocks.size(); ++bi) {
      auto [start, end] = blocks[bi];
      const bool isHeader = (bi == loop.headerBlock);

      for (size_t idx = start; idx < end; ++idx) {
        if (!lineLabel(lines[idx]).empty()) continue;

        AsmInst inst;
        if (!parseAsmLine(lines[idx], inst)) continue;
        if (inst.mnemonic != "la" && inst.mnemonic != "li") continue;
        if (inst.defs.empty()) continue;
        if (isHeader && inst.mnemonic == "li") continue;

        const std::string &rd = inst.defs[0];
        if (regDefinedLaterInBlock(lines, start, end, idx, rd)) continue;
        if (regDefinedElsewhereInLoop(lines, blocks, loop.headerBlock, loop.latchBlock, idx, rd))
          continue;

        if (inst.mnemonic == "la") {
          if (!isGlobalLabelOperand(lines[idx], inst)) continue;
          const std::string sym = laSymbol(lines[idx]);
          auto it = laCanonReg.find(sym);
          if (it != laCanonReg.end()) {
            replaceRegInLoop(lines, blocks, loop.headerBlock, loop.latchBlock, idx, rd, it->second);
            eraseIdx.push_back(idx);
            stats.laMerged++;
            continue;
          }
          laCanonReg[sym] = rd;
        }

        insertLines.push_back(lines[idx]);
        eraseIdx.push_back(idx);
        stats.hoisted++;
      }
    }

    if (insertLines.empty() && eraseIdx.empty()) continue;

    eraseLineIndices(lines, eraseIdx);
    splitBasicBlocks(lines, blocks);
    const size_t at = preheaderInsertPos(lines, blocks, preheader);
    lines.insert(lines.begin() + static_cast<long>(at), insertLines.begin(), insertLines.end());
  }

  return stats;
}

}  // namespace rv
