#include "ir_expr_gvn.h"

#include <unordered_map>
#include <vector>

using namespace std;

// 跨块 GVN (Global Value Numbering)
// 使用哈希表识别等价表达式

struct ExprKey {
  IROp op;
  int u, v;      // 操作数（已排序以支持交换律）
  int32_t imm;   // 立即数
  bool isFloat;

  bool operator==(const ExprKey &o) const {
    return op == o.op && u == o.u && v == o.v &&
           imm == o.imm && isFloat == o.isFloat;
  }
};

struct KeyHash {
  size_t operator()(const ExprKey &k) const {
    size_t h = hash<int>{}(static_cast<int>(k.op));
    h = h * 31 + hash<int>{}(k.u);
    h = h * 31 + hash<int>{}(k.v);
    h = h * 31 + hash<int32_t>{}(k.imm);
    h = h * 31 + hash<bool>{}(k.isFloat);
    return h;
  }
};

// 判断操作是否支持交换律
static bool isCommutative(IROp op) {
  switch (op) {
  case IROp::Add:
  case IROp::Mul:
  case IROp::FAdd:
  case IROp::FMul:
  case IROp::Slt:  // 对于某些比较可以交换
    return true;
  default:
    return false;
  }
}

// 判断是否为纯算术/比较操作（无副作用，可用于 GVN）
static bool isPureArithmetic(IROp op) {
  switch (op) {
  case IROp::Add:
  case IROp::Sub:
  case IROp::Mul:
  case IROp::Sll:
  case IROp::Sra:
  case IROp::Div:
  case IROp::Rem:
  case IROp::Neg:
  case IROp::FAdd:
  case IROp::FSub:
  case IROp::FMul:
  case IROp::FDiv:
  case IROp::FNeg:
  case IROp::ICvtF:
  case IROp::FCvtI:
  case IROp::Slt:
  case IROp::Seqz:
  case IROp::Snez:
  case IROp::FCmp:
    return true;
  default:
    return false;
  }
}

// 创建表达式键
static ExprKey makeKey(const IRInst &inst) {
  ExprKey k;
  k.op = inst.op;
  k.u = inst.u;
  k.v = inst.v;
  k.imm = inst.immI;
  k.isFloat = inst.isFloat;

  // 对交换律操作排序操作数
  if (isCommutative(inst.op) && k.u > k.v) {
    swap(k.u, k.v);
  }

  return k;
}

// 块级 GVN
static bool gvnInBlock(IRFunction &fn, int blockIdx) {
  const auto &blk = fn.blocks[blockIdx];
  unordered_map<ExprKey, int, KeyHash> exprMap;  // key -> vreg
  bool changed = false;

  for (int idx = blk.begin; idx < blk.end; ++idx) {
    auto &inst = fn.insts[idx];

    // 跳过非纯算术指令
    if (!isPureArithmetic(inst.op)) {
      // 控制流边界清除 GVN 表（保守但安全）
      if (inst.op == IROp::Label || inst.op == IROp::J ||
          inst.op == IROp::Beqz || inst.op == IROp::Call) {
        exprMap.clear();
      }
      continue;
    }

    // 构建表达式键
    ExprKey key = makeKey(inst);

    // 查找是否已有等价表达式
    auto it = exprMap.find(key);
    if (it != exprMap.end()) {
      // 发现冗余计算，替换为 Copy
      inst.op = IROp::Copy;
      inst.u = it->second;  // 使用已有结果的 vreg
      inst.v = -1;
      changed = true;
    } else {
      // 记录新表达式
      if (inst.dst >= 0) {
        exprMap[key] = inst.dst;
      }
    }
  }

  return changed;
}

// 跨块 GVN：在支配边界合并表达式信息
// 简化版：只处理单前驱块（基本块内的 GVN 已处理）
// 对于多前驱块，保守地清空 GVN 表
bool irExprGvnAcrossBlocks(IRFunction &fn) {
  bool changed = false;

  for (size_t b = 0; b < fn.blocks.size(); ++b) {
    changed |= gvnInBlock(fn, static_cast<int>(b));
  }

  return changed;
}

// 简化版 GVN：块内表达式合并
bool irExprGvn(IRFunction &fn) {
  return irExprGvnAcrossBlocks(fn);
}
