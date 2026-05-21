#include "ir_mem2reg.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// 简化的 Mem2Reg：只做基本块内的 Store->Load 转发
// 不进行跨块的 Phi 插入，避免复杂的栈槽分配问题
// 这是 Sisyphus 项目实际采用的安全策略

bool irMem2Reg(IRFunction &fn) {
  if (fn.blocks.empty()) {
    irRefreshCFG(fn);
  }

  bool changed = false;
  unordered_map<Symbol*, int> slot;  // 变量 -> 当前 vreg

  for (const auto &blk : fn.blocks) {
    slot.clear();

    for (size_t i = blk.begin; i < blk.end; ++i) {
      auto &inst = fn.insts[i];

      if (inst.op == IROp::StoreLocal) {
        // 记录此变量的最新 vreg
        if (inst.dst >= 0 && isPromotable(inst.sym)) {
          slot[inst.sym] = inst.dst;
          changed = true;
        }
      }
      else if (inst.op == IROp::LoadLocal) {
        // 如果前面有 Store，替换为 Copy
        auto it = slot.find(inst.sym);
        if (it != slot.end() && isPromotable(inst.sym)) {
          inst.op = IROp::Copy;
          inst.u = it->second;
          inst.sym = nullptr;
          changed = true;
        }
      }
      else if (inst.op == IROp::Call || inst.op == IROp::Ret ||
               inst.op == IROp::Label) {
        // 控制流边界：清空状态（保守但安全）
        slot.clear();
      }
    }
  }

  return changed;
}

bool isPromotable(Symbol *sym) {
  if (!sym) return false;
  // 只提升标量局部变量（非数组，非参数，非全局）
  if (sym->isArray) return false;
  if (sym->isParam) return false;
  if (sym->isGlobal) return false;
  return true;
}
