#pragma once

#include "rv_asm.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace rv {

// Block-local CSE on RISC-V asm lines (li/la materialize + pure arithmetic).
// Mirrors reference project's BlockLocalCSE but adapted to AsmInst stream.
class BlockLocalCSE {
public:
  struct Stats {
    int liCSE = 0;
    int laCSE = 0;
    int arithCSE = 0;
  };

  static Stats run(std::vector<std::string> &lines, bool laOnly = false);

private:
  struct ExprKey {
    std::string mnemonic;
    std::string op1;
    std::string op2;
    int64_t imm = 0;
    bool hasImm = false;
    bool usesSp = false;

    bool operator==(const ExprKey &o) const;
  };

  struct ExprKeyHash {
    size_t operator()(const ExprKey &k) const;
  };

  struct AvailEntry {
    size_t defIdx = 0;
    std::string defReg;
  };

  struct MaterialKey {
    std::string mnemonic;  // "li" or "la"
    int64_t imm = 0;
    std::string label;
    bool hasImm = false;

    bool operator==(const MaterialKey &o) const;
  };

  struct MaterialKeyHash {
    size_t operator()(const MaterialKey &k) const;
  };

  static bool isCSEable(const std::string &mnemonic);
  static bool isMaterialize(const std::string &mnemonic);
  static std::optional<MaterialKey> buildMaterialKey(const std::string &line, const AsmInst &inst);
  static std::optional<ExprKey> buildExprKey(const AsmInst &inst);
  static bool isSpReg(const std::string &reg);
  static void invalidateForSp(std::unordered_map<ExprKey, AvailEntry, ExprKeyHash> &avail, int &spVer);
  static std::string buildMvLine(const std::string &origLine, const std::string &newRd, const std::string &canonRd);
};

}  // namespace rv
