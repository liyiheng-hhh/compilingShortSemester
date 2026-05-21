#include "ir_loop_opt.h"

#include "common.h"

#include <unordered_map>
#include <vector>

using namespace std;

// 基本块内标量 Local：StoreLocal 后 LoadLocal 改为 Copy（简易 mem2reg，通用、保守）
static void irPromoteScalarLocalsInRange(IRFunction &fn, size_t begin, size_t end) {
  if (end <= begin) {
    return;
  }
  vector<IRInst> &inst = fn.insts;
  unordered_map<Symbol *, int> slot;

  for (size_t i = begin; i < end; ++i) {
    IRInst &in = inst[i];
    if (in.op == IROp::StoreLocal && in.sym && !in.sym->isArray && in.u >= 0) {
      slot[in.sym] = in.u;
      continue;
    }
    if (in.op == IROp::LoadLocal && in.sym && !in.sym->isArray && in.dst >= 0) {
      auto it = slot.find(in.sym);
      if (it != slot.end() && it->second >= 0) {
        in.op = IROp::Copy;
        in.u = it->second;
        in.sym = nullptr;
        continue;
      }
    }
    if (in.op == IROp::Label || in.op == IROp::Call || in.op == IROp::Ret) {
      slot.clear();
    }
  }
}

static void irPromoteScalarLocalsByBlocks(IRFunction &fn) {
  irRefreshCFG(fn);
  if (fn.blocks.empty()) {
    irPromoteScalarLocalsInRange(fn, 0, fn.insts.size());
    return;
  }
  for (const IRBlock &blk : fn.blocks) {
    if (blk.end > blk.begin) {
      irPromoteScalarLocalsInRange(fn, blk.begin, blk.end);
    }
  }
}

void irOptimizeLoopsAndScalars(IRFunction &fn, const O1Profile &prof) {
  (void)prof;
  if (envFlagTruthy("SYSY_CC_NO_IR_LOOP_OPT")) {
    return;
  }
  irPromoteScalarLocalsByBlocks(fn);
}
