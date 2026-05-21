#pragma once

#include "ir.h"

// 完整 SSA Mem2Reg：带 Phi 节点的变量提升
// 实现 Cytron et al. 1991 算法
//
// 使用示例：
//   IRFunction fn;
//   irBuildFunction(def, semantic, fn);
//   irMem2Reg(fn);  // 执行 Mem2Reg
//   irRefreshCFG(fn);

// 主入口函数
bool irMem2Reg(IRFunction &fn);

// 判断变量是否可提升（标量局部变量）
bool isPromotableForMem2Reg(Symbol *sym);
