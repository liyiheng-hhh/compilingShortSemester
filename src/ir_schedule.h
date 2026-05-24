#pragma once

#include "ir.h"

// 后端指令调度优化
// 基于列表调度（List Scheduling）算法，减少流水线停顿
// 简单的延迟模型 + 依赖分析

// 延迟模型（简化 RISC-V 延迟）
struct DelayModel {
  // 加载延迟：Load 指令结果需要多少周期后才能使用
  static constexpr int LOAD_LATENCY = 2;  // L1 命中延迟

  // 乘法延迟
  static constexpr int MUL_LATENCY = 2;

  // 除法/取模延迟（高延迟）
  static constexpr int DIV_LATENCY = 10;

  // 浮点运算延迟
  static constexpr int FLOAT_LATENCY = 3;

  // 获取指令延迟
  static int getLatency(IROp op, bool isFloat = false);
};

// 指令调度器
struct InstScheduler {
  IRFunction *fn = nullptr;

  explicit InstScheduler(IRFunction *f) : fn(f) {}

  // 执行调度
  bool run();

private:
  // 构建依赖图
  struct DepNode {
    int instIdx = -1;          // 指令索引
    int latency = 1;         // 指令延迟
    std::vector<int> preds;  // 前驱（依赖源）
    std::vector<int> succs;  // 后继
    int numPreds = 0;        // 未满足的前驱数
    int earliestCycle = 0;   // 最早可调度周期
  };

  std::vector<DepNode> nodes;
  std::vector<int> readyList;  // 就绪指令列表

  // 延迟模型
  int getInstLatency(const IRInst &inst);

  // 依赖分析
  void buildDependencyGraph(int blockStart, int blockEnd);

  // 列表调度
  void scheduleBlock(int blockStart, int blockEnd);

  // 判断数据依赖
  bool hasDataDependency(const IRInst &a, const IRInst &b);

  // 判断内存依赖
  bool hasMemoryDependency(const IRInst &a, const IRInst &b);

  // 判断控制依赖
  bool hasControlDependency(const IRInst &a, const IRInst &b);
};

// 外部接口
bool irScheduleInstructions(IRFunction &fn);

// 加载延迟优化：将加载指令尽可能提前
bool irHoistLoadsEarly(IRFunction &fn);
