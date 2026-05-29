#include "rv_passes.h"

#include "rv_asm.h"

#include "../common.h"

#include <unordered_map>

namespace rv {

namespace {

bool rvpeEnabled() {
  return !envFlagTruthy("SYSY_CC_NO_RV_PEEPHOLE");
}

bool rvpeAggressiveEnabled() {
  return envFlagTruthy("SYSY_CC_ENABLE_RV_AGGRESSIVE_PEEPHOLE");
}

std::string rvpeTrim(const std::string &s) {
  size_t b = s.find_first_not_of(" \t");
  if (b == std::string::npos) return "";
  size_t e = s.find_last_not_of(" \t");
  return s.substr(b, e - b + 1);
}

}  // namespace

void regPeephole(std::vector<std::string> &lines, PassStats *stats) {
  if (!rvpeEnabled()) return;

  std::vector<std::string> out;
  out.reserve(lines.size());

  const bool aggressive = rvpeAggressiveEnabled();

  // Only allocate stateful structures when aggressive mode is requested
  std::string lastLoadAddr, lastLoadReg;
  bool memDirty = false;
  std::unordered_map<std::string, std::string> immToReg;

  auto flushBlockState = [&]() {
    lastLoadAddr.clear();
    lastLoadReg.clear();
    memDirty = false;
    immToReg.clear();
  };

  for (const auto &line : lines) {
    if (!line.empty() && line[0] != '\t' && line[0] != ' ') {
      if (aggressive) flushBlockState();
      out.push_back(line);
      continue;
    }

    AsmInst inst;
    if (!parseAsmLine(line, inst)) {
      out.push_back(line);
      continue;
    }

    // Always-safe identity removal (mv x,x and addi x,0)
    if (inst.mnemonic == "mv" && inst.uses.size() == 1 && inst.defs.size() == 1 &&
        inst.defs[0] == inst.uses[0]) {
      if (stats) ++stats->removedMv;
      continue;
    }
    if (inst.mnemonic == "add" && inst.defs.size() == 1 && inst.uses.size() >= 2) {
      const std::string &dst = inst.defs[0];
      const std::string &rs = inst.uses[0];
      if (inst.uses.size() >= 2 &&
          (inst.uses[1] == "x0" || inst.uses[1] == "zero") && dst == rs) {
        if (stats) ++stats->removedMv;
        continue;
      }
    }
    if ((inst.mnemonic == "addi" || inst.mnemonic == "addiw") &&
        inst.raw.find(", 0") != std::string::npos) {
      size_t p = inst.raw.find('\t');
      size_t comma = inst.raw.find(',', p);
      if (comma != std::string::npos) {
        std::string dst = rvpeTrim(inst.raw.substr(p + inst.mnemonic.size() + 1,
                                               comma - (p + inst.mnemonic.size() + 1)));
        size_t comma2 = inst.raw.find(',', comma + 1);
        if (comma2 != std::string::npos) {
          std::string src = rvpeTrim(inst.raw.substr(comma + 1, comma2 - comma - 1));
          if (dst == src) {
            if (stats) ++stats->removedMv;
            continue;
          }
        }
      }
    }

    if (!aggressive) {
      out.push_back(line);
      continue;
    }

    // === Aggressive optimizations (opt-in via SYSY_CC_ENABLE_RV_AGGRESSIVE_PEEPHOLE) ===
    if (inst.isStore || inst.mnemonic == "call") {
      memDirty = true;
      lastLoadAddr.clear();
      lastLoadReg.clear();
      immToReg.clear();
    }

    if ((inst.mnemonic == "li" || inst.mnemonic == "la") && inst.defs.size() == 1) {
      size_t comma = line.find(',');
      if (comma != std::string::npos) {
        std::string key = rvpeTrim(line.substr(comma + 1));
        auto it = immToReg.find(key);
        if (it != immToReg.end() && it->second != inst.defs[0]) {
          out.push_back("\tmv\t" + inst.defs[0] + ", " + it->second);
          if (stats) ++stats->combined;
          continue;
        }
        immToReg[key] = inst.defs[0];
      }
    }

    if (inst.isLoad && !inst.memAddr.empty() && !memDirty) {
      if (!lastLoadAddr.empty() && inst.memAddr == lastLoadAddr &&
          !inst.defs.empty() && inst.defs[0] == lastLoadReg) {
        if (stats) ++stats->removedDupLoad;
        continue;
      }
      if (!inst.defs.empty()) {
        lastLoadAddr = inst.memAddr;
        lastLoadReg = inst.defs[0];
      }
    } else if (inst.isLoad) {
      lastLoadAddr = inst.memAddr;
      lastLoadReg = inst.defs.empty() ? "" : inst.defs[0];
      memDirty = false;
    }

    out.push_back(line);
  }

  lines.swap(out);
}

}  // namespace rv
