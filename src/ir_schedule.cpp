#include "ir_schedule.h"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// 延迟模型实现
int DelayModel::getLatency(IROp op, bool isFloat) {
  if (isFloat) {
    switch (op) {
    case IROp::FDiv:
    case IROp::FCvtI:
      return FLOAT_LATENCY + 2;
    default:
      return FLOAT_LATENCY;
    }
  }

  switch (op) {
  case IROp::LoadLocal:
  case IROp::LoadGlobal:
  case IROp::LoadMem:
    return LOAD_LATENCY;

  case IROp::Mul:
  case IROp::Sll:
  case IROp::Sra:
  case IROp::ICvtF:
    return MUL_LATENCY;

  case IROp::Div:
  case IROp::Rem:
    return DIV_LATENCY;

  default:
    return 1;  // 默认单周期
  }
}

// 获取指令延迟
int InstScheduler::getInstLatency(const IRInst &inst) {
  return DelayModel::getLatency(inst.op, inst.isFloat);
}

// 判断数据依赖：指令 b 是否依赖指令 a 的结果
bool InstScheduler::hasDataDependency(const IRInst &a, const IRInst &b) {
  // a 定义的值被 b 使用
  if (a.dst < 0) return false;

  // b 的源操作数是否使用 a 的目标
  if (b.u == a.dst || b.v == a.dst) return true;

  // 检查 args 中的依赖
  for (int arg : b.args) {
    if (arg == a.dst) return true;
  }

  return false;
}

// 判断内存依赖
bool InstScheduler::hasMemoryDependency(const IRInst &a, const IRInst &b) {
  // 保守策略：Store 和 Load/Store 之间可能存在依赖
  bool aIsStore = (a.op == IROp::StoreLocal || a.op == IROp::StoreGlobal ||
                   a.op == IROp::StoreMem);
  bool bIsStore = (b.op == IROp::StoreLocal || b.op == IROp::StoreGlobal ||
                   b.op == IROp::StoreMem);
  bool aIsLoad = (a.op == IROp::LoadLocal || a.op == IROp::LoadGlobal ||
                  a.op == IROp::LoadMem || a.op == IROp::LoadParamAddr);
  bool bIsLoad = (b.op == IROp::LoadLocal || b.op == IROp::LoadGlobal ||
                  b.op == IROp::LoadMem || b.op == IROp::LoadParamAddr);

  // Store 后 Load：可能存在依赖（如果访问同一地址）
  if (aIsStore && bIsLoad) {
    // 如果访问相同符号，则一定依赖
    if (a.sym && b.sym && a.sym == b.sym) return true;
    // 保守起见，认为可能依赖
    // return true;  // 过于保守，暂时禁用
  }

  // Store 后 Store：WAW 依赖
  if (aIsStore && bIsStore) {
    if (a.sym && b.sym && a.sym == b.sym) return true;
  }

  // Load 后 Store：RAW 依赖（如果访问同一地址）
  if (aIsLoad && bIsStore) {
    if (a.sym && b.sym && a.sym == b.sym) return true;
  }

  return false;
}

// 判断控制依赖
bool InstScheduler::hasControlDependency(const IRInst &a, const IRInst &b) {
  // 分支指令后续指令依赖
  if (a.op == IROp::Beqz || a.op == IROp::J) {
    // 分支后的指令依赖分支
    return true;
  }
  return false;
}

// 构建依赖图
void InstScheduler::buildDependencyGraph(int blockStart, int blockEnd) {
  int n = blockEnd - blockStart;
  nodes.assign(n, {});

  // 初始化节点
  for (int i = 0; i < n; i++) {
    nodes[i].instIdx = blockStart + i;
    nodes[i].latency = getInstLatency(fn->insts[blockStart + i]);
    nodes[i].numPreds = 0;
    nodes[i].earliestCycle = 0;
  }

  // 构建依赖边
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      const auto &a = fn->insts[blockStart + i];
      const auto &b = fn->insts[blockStart + j];

      if (hasDataDependency(a, b) ||
          hasMemoryDependency(a, b) ||
          hasControlDependency(a, b)) {
        // i -> j 有依赖
        nodes[i].succs.push_back(j);
        nodes[j].preds.push_back(i);
        nodes[j].numPreds++;

        // 更新最早执行周期
        int readyCycle = nodes[i].earliestCycle + nodes[i].latency;
        nodes[j].earliestCycle = max(nodes[j].earliestCycle, readyCycle);
      }
    }
  }
}

// 列表调度一个基本块
void InstScheduler::scheduleBlock(int blockStart, int blockEnd) {
  buildDependencyGraph(blockStart, blockEnd);

  int n = blockEnd - blockStart;
  if (n <= 1) return;  // 单指令无需调度

  // 就绪列表（无未满足依赖的指令）
  readyList.clear();
  for (int i = 0; i < n; i++) {
    if (nodes[i].numPreds == 0) {
      readyList.push_back(i);
    }
  }

  // 调度结果
  vector<int> schedule;
  schedule.reserve(n);

  // 当前周期
  int cycle = 0;

  // 执行中指令（结束周期 -> 指令索引）
  priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> executing;

  // 记录哪些指令已调度
  vector<bool> scheduled(n, false);

  while (schedule.size() < static_cast<size_t>(n)) {
    // 完成本周期结束的指令
    while (!executing.empty() && executing.top().first <= cycle) {
      int finishedIdx = executing.top().second;
      executing.pop();

      // 释放后继
      for (int succ : nodes[finishedIdx].succs) {
        nodes[succ].numPreds--;
        if (nodes[succ].numPreds == 0 && !scheduled[succ]) {
          readyList.push_back(succ);
        }
      }
    }

    // 从就绪列表选择最优指令
    if (!readyList.empty()) {
      // 策略：优先选择高延迟指令（加载、乘法、除法）尽早发射
      // 这样可以隐藏延迟
      auto bestIt = readyList.begin();
      int bestLatency = nodes[*bestIt].latency;
      int bestIdx = *bestIt;

      for (auto it = readyList.begin(); it != readyList.end(); ++it) {
        int lat = nodes[*it].latency;
        // 优先高延迟指令，其次按最早就绪周期
        if (lat > bestLatency ||
            (lat == bestLatency && nodes[*it].earliestCycle < nodes[bestIdx].earliestCycle)) {
          bestLatency = lat;
          bestIdx = *it;
          bestIt = it;
        }
      }

      // 调度该指令
      scheduled[bestIdx] = true;
      schedule.push_back(nodes[bestIdx].instIdx);
      readyList.erase(bestIt);

      // 加入执行队列
      int finishCycle = cycle + nodes[bestIdx].latency;
      executing.push({finishCycle, bestIdx});
    }

    cycle++;

    // 防死锁
    if (cycle > n * 10) break;
  }

  // 如果有未调度的指令，按原始顺序追加
  for (int i = 0; i < n; i++) {
    if (!scheduled[i]) {
      schedule.push_back(nodes[i].instIdx);
    }
  }

  // 重排指令（如果调度结果不同）
  if (schedule.size() == static_cast<size_t>(n)) {
    vector<IRInst> newInsts;
    newInsts.reserve(n);
    for (int idx : schedule) {
      newInsts.push_back(fn->insts[idx]);
    }

    // 替换原基本块的指令
    for (int i = 0; i < n; i++) {
      fn->insts[blockStart + i] = newInsts[i];
    }
  }
}

// 主入口
bool InstScheduler::run() {
  if (fn->blocks.empty()) {
    irRefreshCFG(*fn);
  }

  bool changed = false;

  // 对每个基本块进行调度
  for (const auto &blk : fn->blocks) {
    int start = blk.begin;
    int end = blk.end;

    if (end - start > 2) {  // 至少3条指令才值得调度
      scheduleBlock(start, end);
      changed = true;
    }
  }

  return changed;
}

// 加载提前优化：将加载指令尽可能提前到使用处之前
bool irHoistLoadsEarly(IRFunction &fn) {
  if (fn.blocks.empty()) {
    irRefreshCFG(fn);
  }

  bool changed = false;

  for (const auto &blk : fn.blocks) {
    for (int i = blk.begin + 1; i < blk.end; i++) {
      auto &inst = fn.insts[i];

      // 找到加载指令
      if (inst.op != IROp::LoadLocal &&
          inst.op != IROp::LoadGlobal &&
          inst.op != IROp::LoadMem) {
        continue;
      }

      if (inst.dst < 0) continue;

      // 查找该加载值的第一次使用
      int firstUse = -1;
      for (int j = i + 1; j < blk.end; j++) {
        const auto &useInst = fn.insts[j];
        if (useInst.u == inst.dst || useInst.v == inst.dst) {
          firstUse = j;
          break;
        }
        // 检查 args
        for (int arg : useInst.args) {
          if (arg == inst.dst) {
            firstUse = j;
            break;
          }
        }
        if (firstUse >= 0) break;
      }

      // 如果有使用且距离较远，尝试提前
      if (firstUse > i + 2) {
        // 检查是否可以安全提前（无依赖）
        bool canHoist = true;
        for (int k = i - 1; k >= blk.begin && k >= i - 5; k--) {
          // 保守：不跨越 Store 或 Call
          auto &prev = fn.insts[k];
          if (prev.op == IROp::StoreLocal || prev.op == IROp::StoreGlobal ||
              prev.op == IROp::StoreMem || prev.op == IROp::Call) {
            canHoist = false;
            break;
          }
        }

        if (canHoist && i > blk.begin) {
          // 与前一条指令交换（逐步提前）
          swap(fn.insts[i], fn.insts[i - 1]);
          changed = true;
        }
      }
    }
  }

  return changed;
}

// 外部接口
bool irScheduleInstructions(IRFunction &fn) {
  InstScheduler scheduler(&fn);
  return scheduler.run();
}
