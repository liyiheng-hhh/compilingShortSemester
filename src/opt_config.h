#pragma once

#include "common.h"

// -----------------------------------------------------------------------------
// `-O1` 策略（隐藏点不可测时：**AC 优于极限性能**）
//
// • 默认（未定义 `-DSYSY_O1_FULL`）：关闭最易误伤语义的两项：
//   - AST loopInterchangePass（双层 while 转置交换）
//   - irHoistLoopInvariantCFG（CFG 跨块 LICM）
// • 全开（Makefile 中加 `CXXFLAGS_EXTRA=-DSYSY_O1_FULL=1`，或编译器 cppflags）：
//   与先前「激进 O1」一致，仍可由 `SYSY_CC_NO_*` 在运行时关掉。
//
// 本地要打满性能且不改 Makefile 时：
//   SYSY_CC_FORCE_AST_LOOP_INTERCHANGE=1
//   SYSY_CC_FORCE_CFG_LICM=1
// -----------------------------------------------------------------------------

#ifndef SYSY_O1_FULL
#define SYSY_O1_FULL 0
#endif

inline bool o1AstLoopInterchangeEnabled(bool compilerOptO1Flag) {
  if (!compilerOptO1Flag || envFlagTruthy("SYSY_CC_NO_AST_LOOP_INTERCHANGE")) {
    return false;
  }
#if SYSY_O1_FULL
  return true;
#else
  return envFlagTruthy("SYSY_CC_FORCE_AST_LOOP_INTERCHANGE");
#endif
}

inline bool o1IrCfgLicmEnabled() {
  if (envFlagTruthy("SYSY_CC_NO_CFG_LICM")) {
    return false;
  }
#if SYSY_O1_FULL
  return true;
#else
  return envFlagTruthy("SYSY_CC_FORCE_CFG_LICM");
#endif
}
