#include "BlockLocalCSE.h"

#include "rv_asm.h"
#include "../common.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace rv {

namespace {

bool envFlag(const char *name) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return false;
  return std::strcmp(v, "0") != 0 && std::strcmp(v, "false") != 0;
}

}  // namespace

bool BlockLocalCSE::isSpReg(const std::string &r) {
  return r == "sp" || r == "x2";
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
      "sra", "srai", "sraw", "sraiw",
      "mv", "li", "la"
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
  if (inst.mnemonic == "li") {
    // Parse immediate from raw line: li rd, imm
    size_t comma = line.find(',');
    if (comma != std::string::npos) {
      std::string immStr = line.substr(comma + 1);
      immStr.erase(0, immStr.find_first_not_of(" \t"));
      char *end = nullptr;
      long long v = std::strtoll(immStr.c_str(), &end, 0);
      if (end && *end == '\0') {
        key.imm = v;
        key.hasImm = true;
        return key;
      }
    }
  } else if (inst.mnemonic == "la") {
    if (!inst.uses.empty()) {
      key.label = inst.uses[0];
      return key;
    }
  }
  return std::nullopt;
}

std::optional<BlockLocalCSE::ExprKey> BlockLocalCSE::buildExprKey(const AsmInst &inst) {
  if (!isCSEable(inst.mnemonic) || isMaterialize(inst.mnemonic)) return std::nullopt;
  ExprKey key;
  key.mnemonic = inst.mnemonic;
  if (!inst.defs.empty()) key.op1 = inst.defs[0];
  if (!inst.uses.empty()) key.op2 = inst.uses[0];
  if (inst.uses.size() >= 2) {
    const std::string &s = inst.uses[1];
    char *end = nullptr;
    long long v = std::strtoll(s.c_str(), &end, 0);
    if (end && *end == '\0') {
      key.imm = v;
      key.hasImm = true;
    } else {
      key.op2 = s;
    }
  }
  key.usesSp = isSpReg(key.op1) || isSpReg(key.op2);
  return key;
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

std::string BlockLocalCSE::buildMvLine(const std::string &origLine, const std::string &newRd, const std::string &canonRd) {
  // Replace the entire instruction with mv newRd, canonRd, preserving indentation
  size_t tab = origLine.find('\t');
  std::string indent = (tab != std::string::npos) ? origLine.substr(0, tab + 1) : "\t";
  return indent + "mv\t" + newRd + ", " + canonRd;
}

BlockLocalCSE::Stats BlockLocalCSE::run(std::vector<std::string> &lines, bool laOnly) {
  Stats stats;
  if (envFlag("SYSY_CC_NO_RV_BLOCK_CSE")) return stats;

  std::vector<std::pair<size_t, size_t>> blocks;
  splitBasicBlocks(lines, blocks);

  for (auto [start, end] : blocks) {
    std::unordered_map<MaterialKey, AvailEntry, MaterialKeyHash> matAvail;
    std::unordered_map<ExprKey, AvailEntry, ExprKeyHash> exprAvail;
    int spVersion = 0;
    int blockBranchCount = 0;  // heuristic for loop header detection

    for (size_t i = start; i < end; ++i) {
      const std::string &line = lines[i];
      if (line.empty()) continue;
      if (line[0] == '.' || line.find(':') != std::string::npos) {
        if (line.find("beq") != std::string::npos || line.find("bne") != std::string::npos ||
            line.find("blt") != std::string::npos || line.find("bge") != std::string::npos)
          blockBranchCount++;
        continue;
      }

      AsmInst inst;
      if (!parseAsmLine(line, inst)) continue;

      // Track sp changes
      if (inst.mnemonic == "addi" && isSpReg(inst.dst)) {
        invalidateForSp(exprAvail, spVersion);
        continue;
      }

      // Materialize CSE (li/la)
      if (isMaterialize(inst.mnemonic)) {
        if (laOnly && inst.mnemonic != "la") continue;
        // Simple loop header heuristic: skip li CSE if block has many branches
        if (inst.mnemonic == "li" && blockBranchCount >= 2) {
          // still record for later blocks, but don't CSE
        } else {
          auto keyOpt = buildMaterialKey(line, inst);
          if (keyOpt) {
            auto key = *keyOpt;
            auto it = matAvail.find(key);
            if (it != matAvail.end() && !inst.defs.empty()) {
              // Replace with mv
              std::string newLine = buildMvLine(line, inst.defs[0], it->second.defReg);
              lines[i] = newLine;
              if (inst.mnemonic == "li") stats.liCSE++;
              else stats.laCSE++;
            } else {
              if (!inst.defs.empty())
                matAvail[key] = {i, inst.defs[0]};
            }
          }
        }
        continue;
      }

      // Arithmetic CSE
      if (!laOnly && isCSEable(inst.mnemonic)) {
        auto keyOpt = buildExprKey(inst);
        if (keyOpt) {
          auto key = *keyOpt;
          auto it = exprAvail.find(key);
          if (it != exprAvail.end() && !inst.defs.empty() && !it->second.defReg.empty()) {
            // Check if source operands are still live (simple heuristic: assume ok if same mnemonic)
            std::string newLine = buildMvLine(line, inst.defs[0], it->second.defReg);
            lines[i] = newLine;
            stats.arithCSE++;
          } else {
            if (!inst.defs.empty())
              exprAvail[key] = {i, inst.defs[0]};
          }
        }
      }
    }
  }

  return stats;
}

}  // namespace rv
