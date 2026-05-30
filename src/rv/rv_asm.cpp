#include "rv_asm.h"

#include <algorithm>
#include <cctype>

namespace rv {

namespace {

std::string rvaTrim(const std::string &s) {
  size_t b = s.find_first_not_of(" \t");
  if (b == std::string::npos) return "";
  size_t e = s.find_last_not_of(" \t");
  return s.substr(b, e - b + 1);
}

std::vector<std::string> rvaSplitOperands(const std::string &ops) {
  std::vector<std::string> parts;
  std::string cur;
  for (char c : ops) {
    if (c == ',') {
      parts.push_back(rvaTrim(cur));
      cur.clear();
    } else {
      cur += c;
    }
  }
  if (!cur.empty()) parts.push_back(rvaTrim(cur));
  return parts;
}

bool rvaIsReg(const std::string &s) {
  if (s.empty()) return false;
  if (s[0] != 'x' && s[0] != 'f' && s[0] != 'a' && s[0] != 't' && s[0] != 's')
    return false;
  for (size_t i = 1; i < s.size(); ++i) {
    if (!std::isalnum(static_cast<unsigned char>(s[i]))) return false;
  }
  return true;
}

std::string rvaNormalizeMemAddr(const std::string &operand) {
  size_t lp = operand.find('(');
  if (lp == std::string::npos) return "";
  size_t rp = operand.find(')', lp);
  if (rp == std::string::npos) return "";
  return operand.substr(lp);
}

bool rvaParseInsnBody(const std::string &line, std::string &mnemonic,
                      std::string &ops) {
  size_t pos = line.find_first_not_of(" \t");
  if (pos == std::string::npos) return false;
  size_t mnEnd = line.find_first_of(" \t", pos);
  if (mnEnd == std::string::npos) {
    mnemonic = line.substr(pos);
    ops.clear();
    return true;
  }
  mnemonic = line.substr(pos, mnEnd - pos);
  size_t opsStart = line.find_first_not_of(" \t", mnEnd);
  ops = (opsStart == std::string::npos) ? "" : line.substr(opsStart);
  return true;
}

void rvaFillAsmInstOperands(AsmInst &out, const std::vector<std::string> &parts) {
  auto addUse = [&](const std::string &r) {
    if (rvaIsReg(r)) out.uses.push_back(r);
  };
  auto addDef = [&](const std::string &r) {
    if (rvaIsReg(r)) out.defs.push_back(r);
  };

  const std::string &m = out.mnemonic;
  if (m == "mv" || m == "fmv.s") {
    if (parts.size() >= 2) {
      addDef(parts[0]);
      addUse(parts[1]);
      out.dst = parts[0];
    }
  } else if (m == "li" || m == "lui" || m == "la") {
    if (!parts.empty()) addDef(parts[0]);
    if (!parts.empty()) out.dst = parts[0];
  } else if (m == "lw" || m == "ld" || m == "flw" || m == "fld") {
    out.isLoad = true;
    if (!parts.empty()) {
      addDef(parts[0]);
      out.dst = parts[0];
      out.memAddr = rvaNormalizeMemAddr(parts.size() > 1 ? parts[1] : parts[0]);
      if (parts.size() > 1) addUse(parts[1]);
    }
  } else if (m == "sw" || m == "sd" || m == "fsw" || m == "fsd") {
    out.isStore = true;
    if (parts.size() >= 2) {
      addUse(parts[0]);
      out.memAddr = rvaNormalizeMemAddr(parts[1]);
      addUse(parts[1]);
    }
  } else if (m == "addi" || m == "addiw" || m == "slli" || m == "srli" ||
             m == "srai" || m == "andi" || m == "ori" || m == "xori" ||
             m == "slti" || m == "sltiu") {
    if (parts.size() >= 2) {
      addDef(parts[0]);
      addUse(parts[1]);
      out.dst = parts[0];
    }
  } else if (m == "slt" || m == "sltu") {
    if (parts.size() >= 3) {
      addDef(parts[0]);
      addUse(parts[1]);
      addUse(parts[2]);
      out.dst = parts[0];
    }
  } else if (m == "add" || m == "addw" || m == "sub" || m == "subw" ||
             m == "mul" || m == "mulw" || m == "and" || m == "or" ||
             m == "xor" || m == "sll" || m == "srl" || m == "sra") {
    if (parts.size() >= 3) {
      addDef(parts[0]);
      addUse(parts[1]);
      addUse(parts[2]);
      out.dst = parts[0];
    }
  } else if (m == "div" || m == "divw" || m == "rem" || m == "remw") {
    if (parts.size() >= 3) {
      addDef(parts[0]);
      addUse(parts[1]);
      addUse(parts[2]);
      out.dst = parts[0];
    }
  } else {
    for (const auto &p : parts) addUse(p);
    if (!parts.empty() && rvaIsReg(parts[0])) {
      addDef(parts[0]);
      out.dst = parts[0];
    }
    out.pinned = true;
  }

  if (m == "call" || m == "ret" || m == "j" || m == "jr" ||
      m == "beqz" || m == "bnez" || m == "blt" || m == "bge" ||
      m == "ble" || m == "bgt" || m == "beq" || m == "bne" ||
      m.find("b.") == 0) {
    out.pinned = true;
  }
  if (m == "sd" || m == "sw" || m == "fsw" || m == "fsd") {
    out.pinned = true;
  }
}

}  // namespace

std::string asmLeadingWhitespace(const std::string &line) {
  size_t pos = line.find_first_not_of(" \t");
  if (pos == std::string::npos || pos == 0) return "";
  return line.substr(0, pos);
}

bool isBlockTerminatorLine(const std::string &line) {
  std::string m, ops;
  if (!rvaParseInsnBody(line, m, ops)) return false;
  if (m == "ret" || m == "j" || m == "jr") return true;
  if (!m.empty() && m[0] == 'b') return true;
  return false;
}

bool parseAsmLine(const std::string &line, AsmInst &out) {
  out = AsmInst{};
  out.raw = line;
  if (line.empty()) return false;

  if (line[0] != '\t' && line[0] != ' ') {
    if (line.back() == ':') {
      out.isLabel = true;
      out.pinned = true;
      out.mnemonic = "label";
      return true;
    }
    out.pinned = true;
    return true;
  }

  std::string mnemonic, ops;
  if (!rvaParseInsnBody(line, mnemonic, ops)) {
    out.pinned = true;
    return true;
  }
  if (mnemonic.empty()) {
    out.pinned = true;
    return true;
  }

  out.mnemonic = mnemonic;
  auto parts = rvaSplitOperands(ops);
  rvaFillAsmInstOperands(out, parts);

  out.latency = instLatency(out.mnemonic);
  return true;
}

void splitBasicBlocks(const std::vector<std::string> &lines,
                      std::vector<std::pair<size_t, size_t>> &blocks) {
  blocks.clear();
  size_t start = 0;
  for (size_t i = 0; i < lines.size(); ++i) {
    const std::string &l = lines[i];
    bool isLabel = !l.empty() && l[0] != '\t' && l[0] != ' ' && l.back() == ':';
    if (isLabel && i > start) {
      blocks.push_back({start, i});
      start = i;
    }
    if (isBlockTerminatorLine(l)) {
      blocks.push_back({start, i + 1});
      start = i + 1;
    }
  }
  if (start < lines.size()) blocks.push_back({start, lines.size()});
}

int instLatency(const std::string &mnemonic) {
  if (mnemonic == "lw" || mnemonic == "ld" || mnemonic == "flw" || mnemonic == "fld")
    return 3;
  if (mnemonic == "mul" || mnemonic == "mulw" || mnemonic == "mulh")
    return 3;
  if (mnemonic == "div" || mnemonic == "divw" || mnemonic == "rem" || mnemonic == "remw")
    return 12;
  return 1;
}

}  // namespace rv
