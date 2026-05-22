#include "ir_reassociate.h"

#include "ir_dom.h"

#include <map>
#include <vector>

using namespace std;

static bool isTerminator(IROp op) {
  return op == IROp::J || op == IROp::Ret || op == IROp::Beqz;
}

static int useCountVreg(int vreg, const vector<IRInst> &inst, int maxV) {
  if (vreg < 0 || vreg >= maxV) return 0;
  int c = 0;
  for (const auto &in : inst) {
    if (in.u == vreg || in.v == vreg) ++c;
    for (int a : in.args) {
      if (a == vreg) ++c;
    }
  }
  return c;
}

struct AddNode {
  bool ref = false;
  vector<int> leaves;
};

static void collectAdd(int dst, const vector<IRInst> &inst, const vector<int> &defOf,
                       map<int, AddNode> &nodes, int maxV) {
  if (dst < 0 || dst >= maxV) return;
  if (nodes.count(dst)) {
    nodes[dst].ref = true;
    return;
  }
  const int di = defOf[static_cast<size_t>(dst)];
  if (di < 0) {
    nodes[dst] = {false, {dst}};
    return;
  }
  const IRInst &d = inst[static_cast<size_t>(di)];
  if (d.op != IROp::Add || d.dst != dst) {
    nodes[dst] = {false, {dst}};
    return;
  }
  vector<int> mem;
  collectAdd(d.u, inst, defOf, nodes, maxV);
  nodes[d.u].ref = true;
  mem.insert(mem.end(), nodes[d.u].leaves.begin(), nodes[d.u].leaves.end());
  collectAdd(d.v, inst, defOf, nodes, maxV);
  nodes[d.v].ref = true;
  mem.insert(mem.end(), nodes[d.v].leaves.begin(), nodes[d.v].leaves.end());
  nodes[dst] = {false, mem};
}

static bool reassocBlock(IRFunction &fn, int blockIdx) {
  const IRBlock &blk = fn.blocks[static_cast<size_t>(blockIdx)];
  const int maxV = fn.nextVreg;
  vector<int> defOf(static_cast<size_t>(maxV), -1);
  for (int i = static_cast<int>(blk.begin); i < static_cast<int>(blk.end); ++i) {
    const IRInst &in = fn.insts[static_cast<size_t>(i)];
    if (in.dst >= 0 && in.dst < maxV) {
      defOf[static_cast<size_t>(in.dst)] = i;
    }
  }

  map<int, AddNode> nodes;
  for (int i = static_cast<int>(blk.begin); i < static_cast<int>(blk.end); ++i) {
    const IRInst &in = fn.insts[static_cast<size_t>(i)];
    if (in.op == IROp::Add && in.dst >= 0) {
      collectAdd(in.dst, fn.insts, defOf, nodes, maxV);
    }
  }

  bool changed = false;
  size_t insertAt = blk.end;
  while (insertAt > blk.begin && isTerminator(fn.insts[insertAt - 1].op)) {
    --insertAt;
  }

  for (auto &[root, info] : nodes) {
    if (info.ref || info.leaves.size() <= 2) continue;
    bool good = true;
    for (int leaf : info.leaves) {
      if (useCountVreg(leaf, fn.insts, maxV) > 1) {
        good = false;
        break;
      }
    }
    if (!good) continue;

    vector<int> work = info.leaves;
    while (work.size() > 1) {
      vector<int> next;
      for (size_t j = 0; j + 1 < work.size(); j += 2) {
        IRInst add;
        add.op = IROp::Add;
        add.dst = fn.allocVreg();
        add.u = work[j];
        add.v = work[j + 1];
        fn.insts.insert(fn.insts.begin() + static_cast<ptrdiff_t>(insertAt), add);
        next.push_back(add.dst);
        ++insertAt;
        changed = true;
      }
      if (work.size() & 1u) {
        next.push_back(work.back());
      }
      work = std::move(next);
    }

    const int di = defOf[static_cast<size_t>(root)];
    if (di >= 0 && !work.empty()) {
      IRInst &rootInst = fn.insts[static_cast<size_t>(di)];
      if (rootInst.op == IROp::Add && rootInst.dst == root) {
        rootInst.op = IROp::Copy;
        rootInst.u = work[0];
        rootInst.v = -1;
        changed = true;
      }
    }
  }

  return changed;
}

bool irReassociate(IRFunction &fn) {
  IrDomTree dt;
  dt.build(fn);
  if (dt.n <= 0) return false;
  bool changed = false;
  for (int b = 0; b < dt.n; ++b) {
    changed |= reassocBlock(fn, b);
  }
  if (changed) {
    irRefreshCFG(fn);
  }
  return changed;
}
