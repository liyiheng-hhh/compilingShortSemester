#pragma once

#include "ir.h"

#include <cstdint>
#include <string>
#include <vector>

// 图着色寄存器分配结果（与 vregSlots 栈槽并存：未着色者仅走栈）
struct IRRegallocInfo {
  int intPhys = -1;   // 池内下标，-1 表示仅栈
  int floatPhys = -1;
};

struct IRRegallocSummary {
  bool enabled = false;
  bool hasCall = false;
  uint32_t usedCalleeSavedInt = 0; // bit i => s(i+1) 需在序言保存
  std::vector<IRRegallocInfo> vreg;
};

bool irFunctionContainsCall(const IRFunction &fn);

// 在 irAssignSlots 之后调用；为 vreg 分配物理寄存器（图着色，仅无 Call 的 leaf）
IRRegallocSummary irRegallocGraphColor(IRFunction &fn, bool optEnabled);

const char *irRegallocIntRegName(const IRRegallocSummary &sum, int physIdx, bool leafPool);

const char *irRegallocFloatRegName(const IRRegallocSummary &sum, int physIdx);
