#pragma once

#include "common.h"

// =============================================================================
// 「-O1」总开关：默认 **AC 优先**（隐藏 crc/排序 等不可测时）
//
// 编译器命令行仍接收 `-O1`，但 **默认不会** 打开任何激进路径，效果与 **不加 -O1**
// 基本一致（IR 中端、Codegen O1 特技、AST 循环交换 均不启用）。
//
// 恢复历史「性能向 -O1」任选其一：
//   • 编译编译器时：`CXXFLAGS_EXTRA=-DSYSY_O1_FULL=1 make`
//   • 运行时环境：`SYSY_CC_FORCE_AGGRESSIVE_O1=1`（本地/自管机；官方希冀未设则保持安全默认）
//
// 在「已开启激进」的前提下，仍可用 `SYSY_CC_NO_*` 细粒度关掉某一类（二分用）。
// =============================================================================

#ifndef SYSY_O1_FULL
#define SYSY_O1_FULL 0
#endif

// 为 true 时才启用：IR 后端 + irOptimizeBlock（LICM/CSE/DCE…）+ CodeGen 内所有原 `optO1_` 分支 + AST 交换
inline bool compilerUsesAggressiveO1(bool cliPassedO1) {
  if (!cliPassedO1) {
    return false;
  }
#if SYSY_O1_FULL
  return !envFlagTruthy("SYSY_CC_DISABLE_ALL_OPTIMIZATIONS");
#else
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
