#include "BlockLocalCSE.h"

#include "../common.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <sstream>

namespace rv {

namespace {

bool envFlag(const char *name) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return false;
  return std::strcmp(v, "0") != 0 && std::strcmp(v, "false") != 0;
}

std::string trim(const std::string &s) {
  size_t b = s.find_first_not_of(" \t");
  if (b == std::string::npos) return "";
  size_t e = s.find_last_not_of(" \t");
  return s.substr(b, e - b + 1);
}

bool parseImmToken(const std::string &tok, int64_t &out) {
  char *end = nullptr;
  long long v = std::strtoll(tok.c_str(), &end, 0);
  if (!end || *end != '\0')
    return false;
  out = v;
  return true;
}

// Return label if line is "Lfoo:" at column 0.
std::string lineLabel(const std::string &line) {
  if (line.empty() || line[0] == '\t' || line[0] == ' ')
    return "";
  if (line.back() != ':')
    return "";
  return line.substr(0, line.size() - 1);
}

}  // namespace

bool BlockLocalCSE::isSpReg(const std::string &r) {
  return r == "sp" || r == "x2";
}

bool BlockLocalCSE::isPinned(const AsmInst &inst) {
  return inst.pinned || inst.isLoad || inst.isStore;
}

bool BlockLocalCSE::ExprKey::operator==(const ExprKey &o) const {
  return mnemonic == o.mnemonic && op1 == o.op1 && op2 == o.op2 &&
         hasImm == o.hasImm && (!hasImm || imm == o.imm) && usesSp == o.usesSp;
}

size_t BlockLocalCSE::ExprKeyHash::operator()(const ExprKey &k) const {
  size_t h = std::hash<std::string>()(k.mnemonic);
  h ^= std::hash<std::string>()(k.op1) + 0x9e3779b9 + (h << 6) + (h >> 2);
  h ^= std::hash<std::string>()(k.op2) + 0x9e3779b9 + (h << 6) + (h >> 2);
  if (k.hasImm)
    h ^= std::hash<int64_t>()(k.imm) + 0x9e3779b9 + (h << 6) + (h >> 2);
  h ^= std::hash<bool>()(k.usesSp);
  return h;
}

bool BlockLocalCSE::MaterialKey::operator==(const MaterialKey &o) const {
  if (mnemonic != o.mnemonic) return false;
  if (mnemonic == "li")
    return hasImm == o.hasImm && (!hasImm || imm == o.imm);
  if (mnemonic == "la")
    return label == o.label;
  return false;
}

size_t BlockLocalCSE::MaterialKeyHash::operator()(const MaterialKey &k) const {
  size_t h = std::hash<std::string>()(k.mnemonic);
  if (k.hasImm)
    h ^= std::hash<int64_t>()(k.imm) + 0x9e3779b9 + (h << 6) + (h >> 2);
  h ^= std::hash<std::string>()(k.label) + 0x9e3779b9 + (h << 6) + (h >> 2);
  return h;
}

bool BlockLocalCSE::isCSEable(const std::string &mnemonic) {
  static const char *arith[] = {
      "add", "addi", "addw", "addiw", "sub", "subw",
      "and", "andi", "or", "ori", "xor", "xori",
      "sll", "slli", "sllw", "slliw", "srl", "srli", "srlw", "srliw",
      "sra", "srai", "sraw", "sraiw", "mv"
  };
  for (auto *m : arith) {
    if (mnemonic == m) return true;
  }
  return false;
}

bool BlockLocalCSE::isMaterialize(const std::string &mnemonic) {
  return mnemonic == "li" || mnemonic == "la";
}

std::optional<BlockLocalCSE::MaterialKey> BlockLocalCSE::buildMaterialKey(
    const std::string &line, const AsmInst &inst) {
  if (!isMaterialize(inst.mnemonic)) return std::nullopt;
  MaterialKey key;
  key.mnemonic = inst.mnemonic;
  size_t comma = line.find(',');
  if (inst.mnemonic == "li") {
    if (comma == std::string::npos) return std::nullopt;
    std::string immStr = trim(line.substr(comma + 1));
    int64_t v = 0;
    if (!parseImmToken(immStr, v)) return std::nullopt;
    key.imm = v;
    key.hasImm = true;
    return key;
  }
  if (inst.mnemonic == "la") {
    if (comma == std::string::npos) return std::nullopt;
    key.label = trim(line.substr(comma + 1));
    if (key.label.empty()) return std::nullopt;
    return key;
  }
  return std::nullopt;
}

std::string BlockLocalCSE::operandKey(const std::vector<std::string> &lines,
                                      size_t idx, const std::string &reg) {
  if (reg.empty()) return "";
  if (idx > 0) {
    AsmInst prev;
    if (parseAsmLine(lines[idx - 1], prev) && prev.mnemonic == "mv" &&
        prev.defs.size() == 1 && prev.uses.size() == 1 &&
        prev.defs[0] == reg) {
      const std::string &src = prev.uses[0];
      if (!isSpReg(src) && src != "x0" && src != "zero")
        return operandKey(lines, idx - 1, src);
    }
  }
  return reg;
}

std::optional<BlockLocalCSE::ExprKey> BlockLocalCSE::buildExprKey(
    const std::vector<std::string> &lines, size_t idx, const AsmInst &inst) {
  if (!isCSEable(inst.mnemonic) || isMaterialize(inst.mnemonic)) return std::nullopt;
  if (inst.defs.empty()) return std::nullopt;

  ExprKey key;
  key.mnemonic = inst.mnemonic;

  const std::string &m = inst.mnemonic;
  if (m == "mv") {
    if (inst.uses.size() < 1) return std::nullopt;
    key.op1 = operandKey(lines, idx, inst.uses[0]);
    return key;
  }

  if (m == "add" || m == "addw" || m == "sub" || m == "subw" ||
      m == "and" || m == "or" || m == "xor" ||
      m == "sll" || m == "srl" || m == "sra" ||
      m == "sllw" || m == "srlw" || m == "sraw") {
    if (inst.uses.size() < 2) return std::nullopt;
    key.op1 = operandKey(lines, idx, inst.uses[0]);
    key.op2 = operandKey(lines, idx, inst.uses[1]);
    key.usesSp = isSpReg(inst.uses[0]) || isSpReg(inst.uses[1]);
    return key;
  }

  if (m == "addi" || m == "addiw" || m == "andi" || m == "ori" || m == "xori" ||
      m == "slli" || m == "srli" || m == "srai" || m == "slliw" || m == "srliw" ||
      m == "sraiw" || m == "slti" || m == "sltiu") {
    if (inst.uses.empty()) return std::nullopt;
    key.op1 = operandKey(lines, idx, inst.uses[0]);
    key.usesSp = isSpReg(inst.uses[0]);
    const std::string &line = lines[idx];
    size_t comma = line.find(',');
    if (comma != std::string::npos) {
      size_t comma2 = line.find(',', comma + 1);
      std::string immStr = trim(line.substr(comma2 != std::string::npos ? comma2 + 1 : comma + 1));
      int64_t v = 0;
      if (parseImmToken(immStr, v)) {
        key.hasImm = true;
        key.imm = v;
      }
    }
    return key;
  }

  return std::nullopt;
}

bool BlockLocalCSE::isRegDefinedSince(const std::vector<std::string> &lines,
                                      size_t blockStart, size_t blockEnd,
                                      size_t fromIdx, size_t toIdx,
                                      const std::string &reg) {
  if (reg.empty() || fromIdx >= toIdx) return false;
  for (size_t i = fromIdx + 1; i < toIdx && i < blockEnd; ++i) {
    if (lines[i] == kElidedMarker) continue;
    AsmInst inst;
    if (!parseAsmLine(lines[i], inst)) continue;
    for (const auto &d : inst.defs) {
      if (d == reg) return true;
    }
  }
  return false;
}

bool BlockLocalCSE::isAvailEntryLive(const std::vector<std::string> &lines,
                                     size_t blockStart, size_t blockEnd,
                                     size_t useIdx, const AvailEntry &entry,
                                     const AsmInst &defInst) {
  if (entry.defReg.empty()) return false;
  if (isRegDefinedSince(lines, blockStart, blockEnd, entry.defIdx, useIdx, entry.defReg))
    return false;
  for (const auto &u : defInst.uses) {
    if (u.empty() || isSpReg(u)) continue;
    if (isRegDefinedSince(lines, blockStart, blockEnd, entry.defIdx, useIdx, u))
      return false;
  }
  return true;
}

bool BlockLocalCSE::allUsesReplaceable(const std::vector<std::string> &lines,
                                       size_t blockStart, size_t blockEnd,
                                       size_t dupIdx, size_t canonDefIdx,
                                       const std::string &dupRd,
                                       const std::string &canonRd) {
  if (dupRd.empty() || canonRd.empty() || dupRd == canonRd) return false;
  for (size_t j = dupIdx + 1; j < blockEnd; ++j) {
    if (lines[j] == kElidedMarker) continue;
    AsmInst user;
    if (!parseAsmLine(lines[j], user)) continue;
    for (const auto &u : user.uses) {
      if (u != dupRd) continue;
      if (isRegDefinedSince(lines, blockStart, blockEnd, canonDefIdx, j, canonRd))
        return false;
    }
  }
  return true;
}

std::string BlockLocalCSE::replaceRegInLine(const std::string &line,
                                            const std::string &from,
                                            const std::string &to) {
  if (from.empty() || from == to) return line;
  AsmInst inst;
  if (!parseAsmLine(line, inst)) return line;

  bool changed = false;
  for (auto &u : inst.uses) {
    if (u == from) {
      u = to;
      changed = true;
    }
  }
  if (!changed) return line;

  const std::string indent = asmLeadingWhitespace(line);
  if (indent.empty() && line[0] != '\t' && line[0] != ' ') return line;
  std::string prefix = indent.empty() ? "\t" : indent;
  std::ostringstream os;
  os << prefix << inst.mnemonic;
  if (inst.mnemonic == "mv" || inst.mnemonic == "fmv.s") {
    if (inst.defs.size() >= 1 && inst.uses.size() >= 1)
      os << '\t' << inst.defs[0] << ", " << inst.uses[0];
  } else if (isMaterialize(inst.mnemonic)) {
    size_t comma = line.find(',');
    if (!inst.defs.empty() && comma != std::string::npos)
      os << '\t' << inst.defs[0] << ", " << trim(line.substr(comma + 1));
  } else if (inst.mnemonic == "addi" || inst.mnemonic == "addiw" ||
             inst.mnemonic == "andi" || inst.mnemonic == "ori" ||
             inst.mnemonic == "xori" || inst.mnemonic == "slli" ||
             inst.mnemonic == "srli" || inst.mnemonic == "srai") {
    size_t c1 = line.find(',');
    size_t c2 = (c1 != std::string::npos) ? line.find(',', c1 + 1) : std::string::npos;
    if (!inst.defs.empty() && c1 != std::string::npos) {
      os << '\t' << inst.defs[0] << ", " << (inst.uses.empty() ? from : inst.uses[0]);
      if (c2 != std::string::npos)
        os << ", " << trim(line.substr(c2 + 1));
    }
  } else if (inst.defs.size() >= 1 && inst.uses.size() >= 2) {
    os << '\t' << inst.defs[0] << ", " << inst.uses[0] << ", " << inst.uses[1];
  } else {
    return line;
  }
  return os.str();
}

void BlockLocalCSE::replaceUsesWithCanon(std::vector<std::string> &lines,
                                         size_t blockStart, size_t blockEnd,
                                         size_t dupIdx, size_t canonDefIdx,
                                         const std::string &dupRd,
                                         const std::string &canonRd) {
  for (size_t j = dupIdx + 1; j < blockEnd; ++j) {
    if (lines[j] == kElidedMarker) continue;
    if (isRegDefinedSince(lines, blockStart, blockEnd, canonDefIdx, j, canonRd))
      continue;
    lines[j] = replaceRegInLine(lines[j], dupRd, canonRd);
  }
}

void BlockLocalCSE::invalidateForSp(
    std::unordered_map<ExprKey, AvailEntry, ExprKeyHash> &avail, int &spVer) {
  spVer++;
  for (auto it = avail.begin(); it != avail.end();) {
    if (it->first.usesSp)
      it = avail.erase(it);
    else
      ++it;
  }
}

void BlockLocalCSE::invalidateDefsOfInst(
    std::unordered_map<ExprKey, AvailEntry, ExprKeyHash> &avail,
    const std::vector<std::string> &lines, size_t producerIdx) {
  AsmInst producer;
  if (!parseAsmLine(lines[producerIdx], producer)) return;
  for (const auto &reg : producer.defs) {
    if (reg.empty()) continue;
    for (auto it = avail.begin(); it != avail.end();) {
      const ExprKey &k = it->first;
      if (k.op1 == reg || k.op2 == reg) {
        it = avail.erase(it);
        continue;
      }
      if (it->second.defReg == reg) {
        if (it->second.defIdx == producerIdx) {
          ++it;
          continue;
        }
        it = avail.erase(it);
        continue;
      }
      ++it;
    }
  }
}

void BlockLocalCSE::invalidateMaterialAvail(
    std::unordered_map<MaterialKey, AvailEntry, MaterialKeyHash> &matAvail,
    const std::vector<std::string> &lines, size_t producerIdx) {
  AsmInst producer;
  if (!parseAsmLine(lines[producerIdx], producer)) return;
  for (const auto &reg : producer.defs) {
    if (reg.empty()) continue;
    for (auto it = matAvail.begin(); it != matAvail.end();) {
      if (it->second.defReg == reg) {
        if (it->second.defIdx == producerIdx) {
          ++it;
          continue;
        }
        it = matAvail.erase(it);
      } else {
        ++it;
      }
    }
  }
}

void BlockLocalCSE::decrementDefIdxAfter(
    std::unordered_map<ExprKey, AvailEntry, ExprKeyHash> &avail, size_t erasedIdx) {
  for (auto &entry : avail) {
    if (entry.second.defIdx > erasedIdx)
      --entry.second.defIdx;
  }
}

void BlockLocalCSE::decrementMaterialDefIdxAfter(
    std::unordered_map<MaterialKey, AvailEntry, MaterialKeyHash> &matAvail,
    size_t erasedIdx) {
  for (auto &entry : matAvail) {
    if (entry.second.defIdx > erasedIdx)
      --entry.second.defIdx;
  }
}

bool BlockLocalCSE::blockHasBackwardBranch(const std::vector<std::string> &lines,
                                           size_t start, size_t end) {
  std::unordered_map<std::string, size_t> labelPos;
  for (size_t i = start; i < end; ++i) {
    std::string lab = lineLabel(lines[i]);
    if (!lab.empty())
      labelPos[lab] = i;
  }
  for (size_t i = start; i < end; ++i) {
    const std::string &line = lines[i];
    AsmInst inst;
    if (!parseAsmLine(line, inst)) continue;
    if (inst.isLabel || inst.pinned) {
      if (inst.mnemonic == "call" || inst.mnemonic == "ret") continue;
    }
    if (inst.mnemonic == "call" || inst.mnemonic == "ret") continue;
    if (inst.mnemonic.empty() || inst.mnemonic == "label") continue;
    if (inst.mnemonic[0] != 'b' && inst.mnemonic != "j") continue;
    size_t lastSp = line.rfind(' ');
    if (lastSp == std::string::npos) continue;
    std::string target = trim(line.substr(lastSp + 1));
    if (target.empty() || target[0] == 'x' || target[0] == 'a' || target[0] == 't')
      continue;
    auto it = labelPos.find(target);
    if (it != labelPos.end() && it->second <= i)
      return true;
  }
  return false;
}

bool BlockLocalCSE::optimizeBlock(std::vector<std::string> &lines, size_t start,
                                  size_t end, bool laOnly, Stats &stats) {
  bool changed = false;
  std::unordered_map<ExprKey, AvailEntry, ExprKeyHash> avail;
  std::unordered_map<MaterialKey, AvailEntry, MaterialKeyHash> matAvail;
  int spVersion = 0;

  for (size_t idx = start; idx < end; ++idx) {
    if (lines[idx] == kElidedMarker) continue;

    if (idx > start)
      invalidateDefsOfInst(avail, lines, idx - 1);
    if (idx > start)
      invalidateMaterialAvail(matAvail, lines, idx - 1);

    AsmInst inst;
    if (!parseAsmLine(lines[idx], inst)) continue;

    if (inst.mnemonic == "addi" && !inst.defs.empty() && isSpReg(inst.defs[0])) {
      invalidateForSp(avail, spVersion);
    }
    if (inst.mnemonic == "call") {
      avail.clear();
      matAvail.clear();
    }

    const bool allowMat = laOnly ? (inst.mnemonic == "la") : true;
    if (allowMat && isMaterialize(inst.mnemonic)) {
      auto keyOpt = buildMaterialKey(lines[idx], inst);
      if (keyOpt && !inst.defs.empty()) {
        const std::string &dupRd = inst.defs[0];
        bool handled = false;
        auto it = matAvail.find(*keyOpt);
        if (it != matAvail.end()) {
          AsmInst canonInst;
          parseAsmLine(lines[it->second.defIdx], canonInst);
          if (isAvailEntryLive(lines, start, end, idx, it->second, canonInst) &&
              allUsesReplaceable(lines, start, end, idx, it->second.defIdx,
                                 dupRd, it->second.defReg)) {
            replaceUsesWithCanon(lines, start, end, idx, it->second.defIdx,
                                 dupRd, it->second.defReg);
            lines[idx] = kElidedMarker;
            if (!laOnly) decrementDefIdxAfter(avail, idx);
            decrementMaterialDefIdxAfter(matAvail, idx);
            if (inst.mnemonic == "li") stats.liCSE++;
            else stats.laCSE++;
            changed = true;
            handled = true;
            --idx;
          } else {
            matAvail.erase(it);
          }
        }
        if (!handled) {
          matAvail[*keyOpt] = {idx, dupRd, spVersion};
        }
      }
      continue;
    }

    if (laOnly || isPinned(inst)) continue;

    auto keyOpt = buildExprKey(lines, idx, inst);
    if (!keyOpt || inst.defs.empty()) continue;

    const std::string &dupRd = inst.defs[0];
    bool handled = false;
    auto it = avail.find(*keyOpt);
    if (it != avail.end()) {
      AsmInst canonInst;
      parseAsmLine(lines[it->second.defIdx], canonInst);
      if (isAvailEntryLive(lines, start, end, idx, it->second, canonInst) &&
          (!keyOpt->usesSp || it->second.spVersion == spVersion) &&
          allUsesReplaceable(lines, start, end, idx, it->second.defIdx,
                             dupRd, it->second.defReg)) {
        replaceUsesWithCanon(lines, start, end, idx, it->second.defIdx,
                             dupRd, it->second.defReg);
        lines[idx] = kElidedMarker;
        decrementDefIdxAfter(avail, idx);
        decrementMaterialDefIdxAfter(matAvail, idx);
        stats.arithCSE++;
        changed = true;
        handled = true;
        --idx;
      } else {
        avail.erase(it);
      }
    }
    if (!handled)
      avail[*keyOpt] = {idx, dupRd, spVersion};
  }

  return changed;
}

void BlockLocalCSE::compactElided(std::vector<std::string> &lines) {
  lines.erase(std::remove(lines.begin(), lines.end(), std::string(kElidedMarker)),
              lines.end());
}

BlockLocalCSE::Stats BlockLocalCSE::run(std::vector<std::string> &lines, bool laOnly) {
  Stats stats;
  if (envFlag("SYSY_CC_NO_RV_BLOCK_CSE")) return stats;

  bool any = false;
  do {
    any = false;
    std::vector<std::pair<size_t, size_t>> blocks;
    splitBasicBlocks(lines, blocks);
    for (auto [start, end] : blocks) {
      if (optimizeBlock(lines, start, end, laOnly, stats))
        any = true;
    }
  } while (any);

  compactElided(lines);
  return stats;
}

}  // namespace rv
