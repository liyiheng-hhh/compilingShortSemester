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
  bool syncStackSlots = false; // 有 LoadLocal/StoreLocal 时写回 vreg 栈槽
  uint32_t usedCalleeSavedInt = 0; // bit i => s(i+1) 需在序言保存
  std::vector<IRRegallocInfo> vreg;
};

bool irFunctionContainsCall(const IRFunction &fn);

// 在 irAssignSlots 之后调用。无体内 Call 用 t3–t6；含 Call 用 s1–s11（codegen 跟踪 Local↔vreg）
IRRegallocSummary irRegallocGraphColor(IRFunction &fn, bool optEnabled);

const char *irRegallocIntRegName(const IRRegallocSummary &sum, int physIdx,
                                 bool internalCallPool);

const char *irRegallocFloatRegName(const IRRegallocSummary &sum, int physIdx);
