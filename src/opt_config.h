#pragma once

#include "common.h"

#include <cctype>
#include <cstdlib>

// =============================================================================
// 分层 -O1（平台默认 **B**；A/C/D 见 EVAL_BUGLOG.md「升档计划」）
//
// | 档 | 内容 |
// |----|------|
// | A  | Codegen：除常 magic、2^k 除模、常量分支、下标 stride、leaf 参数缓存等 |
// | B  | + IR 发射；中端仅 **常量折叠 + DCE**（无 LICM、无 store→load、无算术 CSE） |
// | C  | + 单块 While LICM + 算术 CSE |
// | D  | + CFG LICM + store→load 前瞻 + AST 转置交换 |
//
// 默认（`SYSY_O1_FULL=0`、未设 `SYSY_O1_TIER`）：**B**（`SYSY_O1_DEFAULT_TIER`）。
// 本地回退 A：`CXXFLAGS_EXTRA=-DSYSY_O1_DEFAULT_TIER=1 make`。
// 全开 D：`CXXFLAGS_EXTRA=-DSYSY_O1_FULL=1` 或 `SYSY_O1_TIER=D`。
// =============================================================================

#ifndef SYSY_O1_FULL
#define SYSY_O1_FULL 0
#endif

#ifndef SYSY_O1_DEFAULT_TIER
#define SYSY_O1_DEFAULT_TIER 2
#endif
#if SYSY_O1_DEFAULT_TIER < 1 || SYSY_O1_DEFAULT_TIER > 4
#error SYSY_O1_DEFAULT_TIER must be 1..4 (A..D)
#endif

struct O1Profile {
  bool codegen = false;
  bool irBackend = false;
  bool irMidend = false;
  bool irSimpleLicm = false;
  bool irCfgLicm = false;
  bool irStoreLoadForward = false;
  bool irArithmeticCse = false;
  bool astLoopInterchange = false;
};

inline int parseO1TierFromEnv() {
  const char *v = getenv("SYSY_O1_TIER");
  if (!v || v[0] == '\0') {
    return -1;
  }
  if (v[1] == '\0' || v[1] == ' ' || v[1] == '\t') {
    switch (v[0]) {
    case 'A':
    case 'a':
      return 1;
    case 'B':
    case 'b':
      return 2;
    case 'C':
    case 'c':
      return 3;
    case 'D':
    case 'd':
      return 4;
    default:
      break;
    }
  }
  char *end = nullptr;
  long n = std::strtol(v, &end, 10);
  if (end != v && n >= 1 && n <= 4) {
    return static_cast<int>(n);
  }
  return -1;
}

inline void fillO1ProfileFromTier(int tier, O1Profile &p) {
  if (tier <= 0) {
    return;
  }
  p.codegen = true;
  if (tier >= 2) {
    p.irBackend = true;
    p.irMidend = true;
  }
  if (tier >= 3) {
    p.irSimpleLicm = true;
    p.irArithmeticCse = true;
  }
  if (tier >= 4) {
    p.irCfgLicm = true;
    p.irStoreLoadForward = true;
    p.astLoopInterchange = true;
  }
}

inline O1Profile resolveO1Profile(bool cliPassedO1) {
  O1Profile p;
  if (!cliPassedO1) {
    return p;
  }

  int tier = SYSY_O1_DEFAULT_TIER;
  const int envTier = parseO1TierFromEnv();
  if (envTier >= 1) {
    tier = envTier;
  }
#if SYSY_O1_FULL
  tier = 4;
#endif
  if (envFlagTruthy("SYSY_CC_FORCE_AGGRESSIVE_O1")) {
    tier = 4;
  }
  if (envFlagTruthy("SYSY_CC_DISABLE_ALL_OPTIMIZATIONS")) {
    tier = 0;
  }

  fillO1ProfileFromTier(tier, p);

  if (envFlagTruthy("SYSY_CC_NO_IR_OPT")) {
    p.irBackend = false;
    p.irMidend = false;
    p.irSimpleLicm = false;
    p.irCfgLicm = false;
    p.irStoreLoadForward = false;
    p.irArithmeticCse = false;
  }
  if (envFlagTruthy("SYSY_CC_NO_SIMPLE_WHILE_LICM")) {
    p.irSimpleLicm = false;
  }
  if (envFlagTruthy("SYSY_CC_NO_CFG_LICM")) {
    p.irCfgLicm = false;
    p.irStoreLoadForward = false;
  }
  if (envFlagTruthy("SYSY_CC_NO_AST_LOOP_INTERCHANGE")) {
    p.astLoopInterchange = false;
  }

  return p;
}

inline bool o1AstLoopInterchangeEffective(const O1Profile &p) {
  return p.astLoopInterchange;
}
