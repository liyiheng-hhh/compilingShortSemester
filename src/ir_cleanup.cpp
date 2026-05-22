#include "ir_cleanup.h"

#include "common.h"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <optional>
#include <unordered_map>
#include <vector>

using namespace std;

static bool cleanupDisabled() {
  return envFlagTruthy("SYSY_CC_NO_IR_CLEANUP");
}

static int findCopySrc(int r, const vector<IRInst> &inst, vector<int> &parent) {
  if (r < 0 || r >= static_cast<int>(parent.size())) {
    return r;
  }
  while (parent[r] != r) {
    parent[r] = parent[parent[r]];
    r = parent[r];
  }
  return r;
}

static optional<int32_t> resolveConstInt(int r, const vector<IRInst> &inst,
                                         const vector<int> &defIdx,
                                         vector<int> &copyParent) {
  r = findCopySrc(r, inst, copyParent);
  if (r < 0 || r >= static_cast<int>(defIdx.size())) {
    return nullopt;
  }
  const int di = defIdx[r];
  if (di < 0 || di >= static_cast<int>(inst.size())) {
    return nullopt;
  }
  const IRInst &d = inst[di];
  if (d.op == IROp::ConstI) {
    return d.immI;
  }
  return nullopt;
}

static bool blockEndsWithJ(const IRFunction &fn, int bi, int *tgtBlock) {
  const IRBlock &blk = fn.blocks[static_cast<size_t>(bi)];
  if (blk.begin >= blk.end) {
    return false;
  }
  size_t t = blk.end;
  while (t > blk.begin && fn.insts[t - 1].op == IROp::Nop) {
    --t;
  }
  if (t <= blk.begin) {
    return false;
  }
  const IRInst &term = fn.insts[t - 1];
  if (term.op != IROp::J || term.ext.empty()) {
    return false;
  }
  for (int i = 0; i < static_cast<int>(fn.blocks.size()); ++i) {
    const IRBlock &b = fn.blocks[static_cast<size_t>(i)];
    if (b.begin < b.end && fn.insts[b.begin].op == IROp::Label &&
        fn.insts[b.begin].ext == term.ext) {
      *tgtBlock = i;
      return true;
    }
  }
  return false;
}

// 唯一前驱 + 无条件跳转 → 合并后继块
bool irCleanupCFG(IRFunction &fn) {
  if (cleanupDisabled()) {
    return false;
  }
  irRefreshCFG(fn);
  const int nb = static_cast<int>(fn.blocks.size());
  // 线性块合并易与跨块 Beqz 标签引用冲突，默认关闭（可用 SYSY_CC_IR_CLEANUP_MERGE=1 开启）
  bool anyMerge = false;
  if (nb > 1 && envFlagTruthy("SYSY_CC_IR_CLEANUP_MERGE")) {
    vector<vector<int>> preds(static_cast<size_t>(nb));
    for (int b = 0; b < nb; ++b) {
      for (int s : fn.blocks[static_cast<size_t>(b)].succ) {
        if (s >= 0 && s < nb) {
          preds[static_cast<size_t>(s)].push_back(b);
        }
      }
    }

    vector<char> absorbed(static_cast<size_t>(nb), 0);
    vector<vector<int>> mergeChain(static_cast<size_t>(nb));

    for (int b = 0; b < nb; ++b) {
      int cur = b;
      while (true) {
        int c = -1;
        if (!blockEndsWithJ(fn, cur, &c) || c < 0 || c >= nb || c == cur) {
          break;
        }
        const auto &ps = preds[static_cast<size_t>(c)];
        if (ps.size() != 1 || ps[0] != cur) {
          break;
        }
        mergeChain[static_cast<size_t>(b)].push_back(c);
        absorbed[static_cast<size_t>(c)] = 1;
        cur = c;
      }
    }

    for (int b = 0; b < nb; ++b) {
      if (!mergeChain[static_cast<size_t>(b)].empty()) {
        anyMerge = true;
        break;
      }
    }

    if (anyMerge) {
      vector<IRInst> out;
      out.reserve(fn.insts.size());

      vector<int> blockOrder(nb);
      iota(blockOrder.begin(), blockOrder.end(), 0);
      sort(blockOrder.begin(), blockOrder.end(), [&](int a, int b) {
        return fn.blocks[static_cast<size_t>(a)].begin <
               fn.blocks[static_cast<size_t>(b)].begin;
      });

      auto appendRange = [&](size_t from, size_t to) {
        for (size_t i = from; i < to; ++i) {
          out.push_back(fn.insts[i]);
        }
      };

      auto appendBlockBody = [&](int bi, bool stripTrailingJ) {
        const IRBlock &blk = fn.blocks[static_cast<size_t>(bi)];
        size_t lo = blk.begin;
        size_t hi = blk.end;
        if (stripTrailingJ && hi > lo) {
          size_t t = hi;
          while (t > lo && fn.insts[t - 1].op == IROp::Nop) {
            --t;
          }
          if (t > lo && fn.insts[t - 1].op == IROp::J) {
            hi = t - 1;
          }
        }
        if (lo < hi && fn.insts[lo].op == IROp::Label &&
            absorbed[static_cast<size_t>(bi)]) {
          ++lo;
        }
        appendRange(lo, hi);
      };

      for (int bi : blockOrder) {
        if (absorbed[static_cast<size_t>(bi)]) {
          continue;
        }
        const bool hasChain = !mergeChain[static_cast<size_t>(bi)].empty();
        appendBlockBody(bi, hasChain);
        for (int c : mergeChain[static_cast<size_t>(bi)]) {
          appendBlockBody(c, true);
        }
      }

      fn.insts.swap(out);
      irRefreshCFG(fn);
    }
  }

  // Beqz 条件为编译期常量 → 改为 J 或删除
  const int nv = fn.nextVreg;
  vector<int> defIdx(max(nv, 1), -1);
  vector<int> copyParent(max(nv, 1));
  iota(copyParent.begin(), copyParent.end(), 0);

  // 常量 Beqz 折叠在部分用例上误判，默认关闭
  if (!envFlagTruthy("SYSY_CC_IR_CLEANUP_BEZ_FOLD")) {
    return anyMerge;
  }

  bool brChanged = false;
  for (size_t i = 0; i < fn.insts.size(); ++i) {
    IRInst &in = fn.insts[i];
    if (in.dst >= 0 && in.dst < nv) {
      defIdx[in.dst] = static_cast<int>(i);
    }
    if (in.op == IROp::Copy && in.dst >= 0 && in.u >= 0 && in.dst < nv && in.u < nv) {
      copyParent[in.dst] = findCopySrc(in.u, fn.insts, copyParent);
    }
    if (in.op != IROp::Beqz || in.u < 0) {
      continue;
    }
    auto cv = resolveConstInt(in.u, fn.insts, defIdx, copyParent);
    if (!cv) {
      continue;
    }
    if (*cv == 0) {
      in.op = IROp::J;
      brChanged = true;
    } else {
      in.op = IROp::Nop;
      in.ext.clear();
      brChanged = true;
    }
  }

  if (brChanged) {
    irRefreshCFG(fn);
  }
  return anyMerge || brChanged;
}

bool irCleanupPerBlock(IRFunction &fn) {
  if (cleanupDisabled() || fn.blocks.empty()) {
    return false;
  }

  const int nv = fn.nextVreg;
  vector<int> defIdx(max(nv, 1), -1);
  vector<int> copyParent(max(nv, 1));
  iota(copyParent.begin(), copyParent.end(), 0);

  bool changed = false;

  for (const IRBlock &blk : fn.blocks) {
    unordered_map<Symbol *, int> lastStoreLocal;
    unordered_map<int, int> lastStoreMem;
    unordered_map<int, char> memLoaded;

    auto invalidateMem = [&]() {
      memLoaded.clear();
    };

    for (size_t i = blk.begin; i < blk.end; ++i) {
      IRInst &in = fn.insts[i];
      if (in.dst >= 0 && in.dst < nv) {
        defIdx[in.dst] = static_cast<int>(i);
      }
      if (in.op == IROp::Copy && in.dst >= 0 && in.u >= 0 && in.dst < nv && in.u < nv) {
        copyParent[in.dst] = findCopySrc(in.u, fn.insts, copyParent);
      }

      if (in.op == IROp::Call) {
        lastStoreLocal.clear();
        invalidateMem();
        continue;
      }

      if (in.op == IROp::StoreGlobal) {
        lastStoreLocal.clear();
        invalidateMem();
        continue;
      }

      if (in.op == IROp::LoadMem) {
        int addr = in.u >= 0 ? findCopySrc(in.u, fn.insts, copyParent) : in.u;
        memLoaded[addr] = 1;
        continue;
      }

      if (in.op == IROp::StoreMem) {
        int addr = in.u >= 0 ? findCopySrc(in.u, fn.insts, copyParent) : in.u;
        auto it = lastStoreMem.find(addr);
        if (it != lastStoreMem.end() && !memLoaded.count(addr)) {
          fn.insts[static_cast<size_t>(it->second)].op = IROp::Nop;
          changed = true;
        }
        lastStoreMem[addr] = static_cast<int>(i);
        memLoaded.erase(addr);
        continue;
      }

      if (in.op == IROp::LoadLocal) {
        if (in.sym) {
          lastStoreLocal.erase(in.sym);
        }
        continue;
      }

      if (in.op == IROp::StoreLocal && in.sym) {
        auto it = lastStoreLocal.find(in.sym);
        if (it != lastStoreLocal.end()) {
          fn.insts[static_cast<size_t>(it->second)].op = IROp::Nop;
          changed = true;
        }
        lastStoreLocal[in.sym] = static_cast<int>(i);
        continue;
      }

    }
  }

  if (changed) {
    irRefreshCFG(fn);
  }
  return changed;
}
