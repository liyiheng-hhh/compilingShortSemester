#pragma once

#include "common.h"

// =============================================================================
// 「-O1」总开关：默认 **性能向**（希冀用 clang++ 递归编编译器时**不会**带 Makefile 的 EXTRA，故默认须在此打开）
//
// 传 `-S -O1` 时：`IR 后端 + irOptimizeBlock（LICM/CSE/DCE）+ CodeGen 原 O1 特技 + AST 交换` 全部按子开关生效。
//
// 若需临时 **AC 优先、与不加 -O1 同路径**：二选一
//   • 编编译器：`clang++ ... -DSYSY_O1_FULL=0 ...`
//   • 或运行评测时（若平台允许）：`SYSY_CC_DISABLE_ALL_OPTIMIZATIONS=1`
//
// 在未 `SYSY_O1_FULL` 的构建上强行开优化（极少用）：`SYSY_CC_FORCE_AGGRESSIVE_O1=1`
//
// 在「已开启激进」的前提下，仍可用 `SYSY_CC_NO_*` 细粒度关掉某一类（二分用）。
// =============================================================================

#ifndef SYSY_O1_FULL
#define SYSY_O1_FULL 1
#endif

// 为 true 时才启用：IR 后端 + irOptimizeBlock（LICM/CSE/DCE…）+ CodeGen 内所有原 `optO1_` 分支 + AST 交换
inline bool compilerUsesAggressiveO1(bool cliPassedO1) {
  if (!cliPassedO1) {
    return false;
  }
#if SYSY_O1_FULL
  return !envFlagTruthy("SYSY_CC_DISABLE_ALL_OPTIMIZATIONS");
#else
  // 仅在用 -DSYSY_O1_FULL=0 编成「安全版」编译器时，可用环境变量再打回全开
  return envFlagTruthy("SYSY_CC_FORCE_AGGRESSIVE_O1");
#endif
}

inline bool o1AstLoopInterchangeEffective(bool backendO1) {
  return backendO1 && !envFlagTruthy("SYSY_CC_NO_AST_LOOP_INTERCHANGE");
}

inline bool o1IrSimpleWhileLicmEffective(bool backendO1) {
  return backendO1 && !envFlagTruthy("SYSY_CC_NO_SIMPLE_WHILE_LICM");
}

inline bool o1IrCfgLicmEffective(bool backendO1) {
  return backendO1 && !envFlagTruthy("SYSY_CC_NO_CFG_LICM");
}

// false 时整张 irOptimizeBlock（除 refresh）跳过
inline bool o1IrMidendPipelineEffective(bool backendO1) {
  return backendO1 && !envFlagTruthy("SYSY_CC_NO_IR_OPT");
}
