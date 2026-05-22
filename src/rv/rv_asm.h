#pragma once

#include <string>
#include <vector>

namespace rv {

// One parsed RISC-V instruction line (tab-indented codegen output).
struct AsmInst {
  std::string raw;
  std::string mnemonic;
  std::string dst;           // first operand if it is a def (mv/addi/lw/...)
  std::vector<std::string> uses;
  std::vector<std::string> defs;
  std::string memAddr;       // normalized "(offset)base" for ld/st
  int latency = 1;
  bool pinned = false;       // must not move (branch/call/ret/label/stack)
  bool isLabel = false;
  bool isStore = false;
  bool isLoad = false;
};

bool parseAsmLine(const std::string &line, AsmInst &out);

// Split asm into basic blocks; each block is [start, end) indices into lines.
void splitBasicBlocks(const std::vector<std::string> &lines,
                      std::vector<std::pair<size_t, size_t>> &blocks);

int instLatency(const std::string &mnemonic);

}  // namespace rv
