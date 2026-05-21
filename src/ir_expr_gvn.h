#pragma once

#include "ir.h"

// 表达式 GVN (Global Value Numbering)
// 跨基本块识别并消除冗余表达式计算

// 跨块 GVN（保守版本）
bool irExprGvnAcrossBlocks(IRFunction &fn);

// 简化版 GVN（块内）
bool irExprGvn(IRFunction &fn);
