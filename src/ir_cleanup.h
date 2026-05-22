#pragma once

#include "ir.h"

// IR 中端清理：CFG 简化、常量分支、块内冗余 store、除常数强度削弱。
// 关闭：SYSY_CC_NO_IR_CLEANUP=1

bool irCleanupCFG(IRFunction &fn);
bool irCleanupPerBlock(IRFunction &fn);
