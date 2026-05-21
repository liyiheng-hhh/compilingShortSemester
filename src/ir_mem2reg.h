#pragma once

#include "ir.h"

// 简化的 Mem2Reg：基本块内 Store->Load 转发
// 不做跨块的 Phi 插入，避免栈槽分配复杂性

bool irMem2Reg(IRFunction &fn);
bool isPromotable(Symbol *sym);
