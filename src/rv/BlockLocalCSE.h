#pragma once

#include "rv_asm.h"

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace rv {

// Block-local CSE on RISC-V asm (li/la materialize + pure arithmetic).
// Reference: compiler2026-main BlockLocalCSE, adapted to textual AsmInst stream.
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
    int spVersion = 0;
  };

  struct MaterialKey {
    std::string mnemonic;
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
  static bool isSpReg(const std::string &reg);
  static bool isPinned(const AsmInst &inst);

  static std::optional<MaterialKey> buildMaterialKey(const std::string &line, const AsmInst &inst);
  static std::optional<ExprKey> buildExprKey(const std::vector<std::string> &lines,
                                             size_t idx, const AsmInst &inst);

  static std::string operandKey(const std::vector<std::string> &lines, size_t idx,
                                const std::string &reg);
  static bool isRegDefinedSince(const std::vector<std::string> &lines,
                                size_t blockStart, size_t blockEnd,
                                size_t fromIdx, size_t toIdx, const std::string &reg);
  static bool isAvailEntryLive(const std::vector<std::string> &lines,
                               size_t blockStart, size_t blockEnd,
                               size_t useIdx, const AvailEntry &entry,
                               const AsmInst &defInst);
  static bool allUsesReplaceable(const std::vector<std::string> &lines,
                                 size_t blockStart, size_t blockEnd,
                                 size_t dupIdx, size_t canonDefIdx,
                                 const std::string &dupRd, const std::string &canonRd);
  static void replaceUsesWithCanon(std::vector<std::string> &lines,
                                   size_t blockStart, size_t blockEnd,
                                   size_t dupIdx, size_t canonDefIdx,
                                   const std::string &dupRd, const std::string &canonRd);
  static std::string replaceRegInLine(const std::string &line,
                                      const std::string &from, const std::string &to);

  static void invalidateForSp(std::unordered_map<ExprKey, AvailEntry, ExprKeyHash> &avail,
                              int &spVer);
  static void invalidateDefsOfInst(std::unordered_map<ExprKey, AvailEntry, ExprKeyHash> &avail,
                                 const std::vector<std::string> &lines, size_t producerIdx);
  static void invalidateMaterialAvail(
      std::unordered_map<MaterialKey, AvailEntry, MaterialKeyHash> &matAvail,
      const std::vector<std::string> &lines, size_t producerIdx);
  static void decrementDefIdxAfter(std::unordered_map<ExprKey, AvailEntry, ExprKeyHash> &avail,
                                   size_t erasedIdx);
  static void decrementMaterialDefIdxAfter(
      std::unordered_map<MaterialKey, AvailEntry, MaterialKeyHash> &matAvail,
      size_t erasedIdx);

  static bool blockHasBackwardBranch(const std::vector<std::string> &lines,
                                     size_t start, size_t end);
  static bool optimizeBlock(std::vector<std::string> &lines, size_t start, size_t end,
                            bool laOnly, Stats &stats);
  static void compactElided(std::vector<std::string> &lines);

  static constexpr const char *kElidedMarker = "\t# rv-cse-elided";
};

}  // namespace rv
