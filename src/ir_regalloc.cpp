#include "ir_regalloc.h"

#include "common.h"
#include "opt_config.h"

#include <algorithm>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

bool irFunctionContainsCall(const IRFunction &fn) {
  for (const IRInst &in : fn.insts) {
    if (in.op == IROp::Call) {
      return true;
    }
  }
  return false;
}

static void irraMarkVregTypes(const IRFunction &fn, vector<char> &isFloat) {
  const int nv = fn.nextVreg;
  isFloat.assign(static_cast<size_t>(max(nv, 0)), 0);
  for (const IRInst &in : fn.insts) {
    if (in.dst < 0 || in.dst >= nv) {
      continue;
    }
    switch (in.op) {
    case IROp::ConstF:
    case IROp::FAdd:
    case IROp::FSub:
    case IROp::FMul:
    case IROp::FDiv:
    case IROp::FNeg:
    case IROp::FCmp:
    case IROp::FCvtI:
      isFloat[static_cast<size_t>(in.dst)] = 1;
      break;
    case IROp::ICvtF:
    case IROp::ConstI:
    case IROp::Add:
    case IROp::Sub:
    case IROp::Mul:
    case IROp::Sll:
    case IROp::Sra:
    case IROp::Div:
    case IROp::Rem:
    case IROp::Neg:
    case IROp::Slt:
    case IROp::Seqz:
    case IROp::Snez:
      isFloat[static_cast<size_t>(in.dst)] = 0;
      break;
    case IROp::Copy:
      if (in.u >= 0 && in.u < nv) {
        isFloat[static_cast<size_t>(in.dst)] = isFloat[static_cast<size_t>(in.u)];
      }
      break;
    case IROp::LoadMem:
    case IROp::LoadGlobal:
    case IROp::LoadLocal:
      isFloat[static_cast<size_t>(in.dst)] = in.isFloat ? 1 : 0;
      break;
    default:
      break;
    }
  }
}

static void irraComputeLiveIn(IRFunction &fn, vector<vector<char>> &liveIn) {
  const int nv = fn.nextVreg;
  const size_t n = fn.insts.size();
  liveIn.assign(n + 1, vector<char>(static_cast<size_t>(max(nv, 0)), 0));
  if (nv <= 0 || n == 0) {
    return;
  }

  irRefreshCFG(fn);
  const int nb = static_cast<int>(fn.blocks.size());
  constexpr size_t kBudget = 24u * 1024u * 1024u;
  if (static_cast<size_t>(nb) * static_cast<size_t>(nv) > kBudget) {
    return;
  }

  vector<vector<char>> useB(static_cast<size_t>(nb)), defB(static_cast<size_t>(nb));
  for (int b = 0; b < nb; ++b) {
    useB[static_cast<size_t>(b)].assign(static_cast<size_t>(nv), 0);
    defB[static_cast<size_t>(b)].assign(static_cast<size_t>(nv), 0);
    const IRBlock &blk = fn.blocks[static_cast<size_t>(b)];
    for (size_t i = blk.begin; i < blk.end; ++i) {
      const IRInst &in = fn.insts[i];
      if (in.dst >= 0 && in.dst < nv) {
        defB[static_cast<size_t>(b)][static_cast<size_t>(in.dst)] = 1;
      }
    }
    for (size_t i = blk.begin; i < blk.end; ++i) {
      const IRInst &in = fn.insts[i];
      auto uu = [&](int r) {
        if (r >= 0 && r < nv) {
          useB[static_cast<size_t>(b)][static_cast<size_t>(r)] = 1;
        }
      };
      uu(in.u);
      uu(in.v);
      for (int a : in.args) {
        uu(a);
      }
    }
  }

  vector<vector<char>> inn(static_cast<size_t>(nb)), out(static_cast<size_t>(nb));
  for (int b = 0; b < nb; ++b) {
    inn[static_cast<size_t>(b)].assign(static_cast<size_t>(nv), 0);
    out[static_cast<size_t>(b)].assign(static_cast<size_t>(nv), 0);
  }

  for (int it = 0; it < nb + 64; ++it) {
    bool chg = false;
    for (int b = nb - 1; b >= 0; --b) {
      vector<char> nOut(static_cast<size_t>(nv), 0);
      for (int s : fn.blocks[static_cast<size_t>(b)].succ) {
        if (s >= 0 && s < nb) {
          for (int v = 0; v < nv; ++v) {
            if (inn[static_cast<size_t>(s)][static_cast<size_t>(v)]) {
              nOut[static_cast<size_t>(v)] = 1;
            }
          }
        }
      }
      if (out[static_cast<size_t>(b)] != nOut) {
        chg = true;
      }
      out[static_cast<size_t>(b)] = nOut;
    }
    for (int b = 0; b < nb; ++b) {
      vector<char> nIn(static_cast<size_t>(nv), 0);
      for (int v = 0; v < nv; ++v) {
        if (useB[static_cast<size_t>(b)][static_cast<size_t>(v)] ||
            (out[static_cast<size_t>(b)][static_cast<size_t>(v)] &&
             !defB[static_cast<size_t>(b)][static_cast<size_t>(v)])) {
          nIn[static_cast<size_t>(v)] = 1;
        }
      }
      if (inn[static_cast<size_t>(b)] != nIn) {
        chg = true;
      }
      inn[static_cast<size_t>(b)] = nIn;
    }
    if (!chg) {
      break;
    }
  }

  vector<unsigned char> active(static_cast<size_t>(nv), 0);
  vector<int> list;
  list.reserve(256);

  auto resetActive = [&](const vector<char> &live) {
    fill(active.begin(), active.end(), 0);
    list.clear();
    for (int v = 0; v < nv; ++v) {
      if (live[static_cast<size_t>(v)]) {
        active[static_cast<size_t>(v)] = 1;
        list.push_back(v);
      }
    }
  };

  for (int b = 0; b < nb; ++b) {
    const IRBlock &blk = fn.blocks[static_cast<size_t>(b)];
    resetActive(inn[static_cast<size_t>(b)]);
    for (size_t i = blk.begin; i < blk.end; ++i) {
      liveIn[i] = vector<char>(active.begin(), active.end());
      const IRInst &in = fn.insts[i];
      auto bump = [&](int r) {
        if (r >= 0 && r < nv && !active[static_cast<size_t>(r)]) {
          active[static_cast<size_t>(r)] = 1;
          list.push_back(r);
        }
      };
      bump(in.u);
      bump(in.v);
      for (int a : in.args) {
        bump(a);
      }
      if (in.dst >= 0 && in.dst < nv) {
        active[static_cast<size_t>(in.dst)] = 0;
        for (size_t k = 0; k < list.size(); ++k) {
          if (list[k] == in.dst) {
            list[k] = list.back();
            list.pop_back();
            break;
          }
        }
        list.push_back(in.dst);
        active[static_cast<size_t>(in.dst)] = 1;
      }
    }
  }
}

// CFG 活跃分析超预算时：按基本块局部反向扫描（偏保守，但足以驱动着色）
static void irraComputeLiveInPerBlock(const IRFunction &fn, vector<vector<char>> &liveIn) {
  const int nv = fn.nextVreg;
  const size_t n = fn.insts.size();
  liveIn.assign(n + 1, vector<char>(static_cast<size_t>(max(nv, 0)), 0));
  if (nv <= 0 || n == 0 || fn.blocks.empty()) {
    return;
  }

  vector<unsigned char> active(static_cast<size_t>(nv), 0);
  vector<int> list;
  list.reserve(256);

  for (const IRBlock &blk : fn.blocks) {
    fill(active.begin(), active.end(), 0);
    list.clear();
    for (size_t i = blk.begin; i < blk.end; ++i) {
      liveIn[i] = vector<char>(active.begin(), active.end());
      const IRInst &in = fn.insts[i];
      auto bump = [&](int r) {
        if (r >= 0 && r < nv && !active[static_cast<size_t>(r)]) {
          active[static_cast<size_t>(r)] = 1;
          list.push_back(r);
        }
      };
      bump(in.u);
      bump(in.v);
      for (int a : in.args) {
        bump(a);
      }
      if (in.dst >= 0 && in.dst < nv) {
        active[static_cast<size_t>(in.dst)] = 0;
        for (size_t k = 0; k < list.size(); ++k) {
          if (list[k] == in.dst) {
            list[k] = list.back();
            list.pop_back();
            break;
          }
        }
        list.push_back(in.dst);
        active[static_cast<size_t>(in.dst)] = 1;
      }
    }
  }
}

static bool irraLiveInAnyActive(const vector<vector<char>> &liveIn) {
  for (const vector<char> &li : liveIn) {
    for (char c : li) {
      if (c) {
        return true;
      }
    }
  }
  return false;
}

static vector<int> irraBuildIntPool(bool internalCall) {
  vector<int> pool;
  if (!internalCall) {
    // 无体内 Call：callee-saved s1–s6（prologue 保存）；t0–t3 为 codegen 临时
    for (int s = 1; s <= 6; ++s) {
      pool.push_back(100 + s);
    }
    return pool;
  }
  for (int s = 1; s <= 11; ++s) {
    pool.push_back(100 + s);
  }
  return pool;
}

static vector<int> irraBuildFloatPool() {
  vector<int> pool;
  for (int f = 0; f <= 11; ++f) {
    pool.push_back(f);
  }
  return pool;
}

static string irraPoolEntryName(int code) {
  if (code >= 100) {
    return "s" + to_string(code - 100);
  }
  if (code >= 0 && code <= 31) {
    return "t" + to_string(code);
  }
  if (code >= 32 && code < 100) {
    return "fs" + to_string(code - 32);
  }
  return "";
}

const char *irRegallocIntRegName(const IRRegallocSummary &sum, int physIdx,
                                 bool internalCallPool) {
  (void)sum;
  static thread_local string buf;
  static vector<int> noCall = irraBuildIntPool(false);
  static vector<int> withCall = irraBuildIntPool(true);
  const vector<int> &pool = internalCallPool ? withCall : noCall;
  if (physIdx < 0 || physIdx >= static_cast<int>(pool.size())) {
    return "";
  }
  buf = irraPoolEntryName(pool[static_cast<size_t>(physIdx)]);
  return buf.c_str();
}

const char *irRegallocFloatRegName(const IRRegallocSummary &sum, int physIdx) {
  (void)sum;
  static thread_local string buf;
  if (physIdx < 0 || physIdx > 11) {
    return "";
  }
  buf = "fs" + to_string(physIdx);
  return buf.c_str();
}

static void irraAddEdge(vector<unordered_set<int>> &adj, int a, int b) {
  if (a == b || a < 0 || b < 0) {
    return;
  }
  adj[static_cast<size_t>(a)].insert(b);
  adj[static_cast<size_t>(b)].insert(a);
}

static vector<int> irraColorGraph(int nv, const vector<char> &inBank,
                              const vector<unordered_set<int>> &adj,
                              const vector<int> &pool) {
  vector<int> color(static_cast<size_t>(nv), -1);
  vector<int> order;
  order.reserve(static_cast<size_t>(nv));
  for (int v = 0; v < nv; ++v) {
    if (inBank[static_cast<size_t>(v)]) {
      order.push_back(v);
    }
  }
  sort(order.begin(), order.end(), [&](int a, int b) {
    return adj[static_cast<size_t>(a)].size() > adj[static_cast<size_t>(b)].size();
  });

  const int k = static_cast<int>(pool.size());
  for (int v : order) {
    vector<char> used(static_cast<size_t>(k), 0);
    for (int nb : adj[static_cast<size_t>(v)]) {
      int c = color[static_cast<size_t>(nb)];
      if (c >= 0 && c < k) {
        used[static_cast<size_t>(c)] = 1;
      }
    }
    int pick = -1;
    for (int c = 0; c < k; ++c) {
      if (!used[static_cast<size_t>(c)]) {
        pick = c;
        break;
      }
    }
    color[static_cast<size_t>(v)] = pick;
  }
  return color;
}

IRRegallocSummary irRegallocGraphColor(IRFunction &fn, bool optEnabled) {
  IRRegallocSummary sum;
  const int nv = fn.nextVreg;
  constexpr int kMaxVreg = 4096;
  if (!optEnabled || nv <= 0 || nv > kMaxVreg ||
      envFlagTruthy("SYSY_CC_NO_IR_REGALLOC")) {
    sum.vreg.assign(static_cast<size_t>(max(nv, 0)), IRRegallocInfo{});
    return sum;
  }

  sum.hasCall = irFunctionContainsCall(fn);

  vector<char> isFloat;
  irraMarkVregTypes(fn, isFloat);

  vector<vector<char>> liveIn;
  irraComputeLiveIn(fn, liveIn);
  if (liveIn.empty() || liveIn[0].empty()) {
    sum.vreg.assign(static_cast<size_t>(nv), IRRegallocInfo{});
    return sum;
  }
  if (!irraLiveInAnyActive(liveIn)) {
    irraComputeLiveInPerBlock(fn, liveIn);
  }
  if (!irraLiveInAnyActive(liveIn)) {
    sum.vreg.assign(static_cast<size_t>(nv), IRRegallocInfo{});
    return sum;
  }

  vector<unordered_set<int>> adjInt(static_cast<size_t>(nv)), adjFloat(static_cast<size_t>(nv));
  vector<char> inInt(static_cast<size_t>(nv), 0), inFloat(static_cast<size_t>(nv), 0);

  auto addInterferenceAt = [&](const vector<char> &atOp) {
    vector<int> ints, floats;
    for (int v = 0; v < nv; ++v) {
      if (!atOp[static_cast<size_t>(v)]) {
        continue;
      }
      if (isFloat[static_cast<size_t>(v)]) {
        inFloat[static_cast<size_t>(v)] = 1;
        floats.push_back(v);
      } else {
        inInt[static_cast<size_t>(v)] = 1;
        ints.push_back(v);
      }
    }
    for (size_t a = 0; a < ints.size(); ++a) {
      for (size_t b = a + 1; b < ints.size(); ++b) {
        irraAddEdge(adjInt, ints[a], ints[b]);
      }
    }
    for (size_t a = 0; a < floats.size(); ++a) {
      for (size_t b = a + 1; b < floats.size(); ++b) {
        irraAddEdge(adjFloat, floats[a], floats[b]);
      }
    }
  };

  for (size_t i = 0; i < fn.insts.size(); ++i) {
    if (i >= liveIn.size()) {
      break;
    }
    const IRInst &in = fn.insts[i];
    vector<char> atOp = liveIn[i];
    auto markLive = [&](int r) {
      if (r >= 0 && r < nv) {
        atOp[static_cast<size_t>(r)] = 1;
      }
    };
    markLive(in.u);
    markLive(in.v);
    for (int a : in.args) {
      markLive(a);
    }
    // liveIn[i] 是指令前活跃集；三地址运算时 dst 与 u/v 同时需要物理寄存器
    const bool copySame =
        in.op == IROp::Copy && in.dst >= 0 && in.dst == in.u;
    if (in.dst >= 0 && !copySame) {
      markLive(in.dst);
    }
    addInterferenceAt(atOp);
  }

  vector<int> intPool = irraBuildIntPool(sum.hasCall);
  vector<int> floatPool = irraBuildFloatPool();
  vector<int> icol = irraColorGraph(nv, inInt, adjInt, intPool);
  vector<int> fcol = irraColorGraph(nv, inFloat, adjFloat, floatPool);

  auto coloringValid = [&](const vector<int> &col,
                             const vector<unordered_set<int>> &adj) -> bool {
    for (int v = 0; v < nv; ++v) {
      int cv = col[static_cast<size_t>(v)];
      if (cv < 0) {
        continue;
      }
      for (int nb : adj[static_cast<size_t>(v)]) {
        if (col[static_cast<size_t>(nb)] == cv) {
          return false;
        }
      }
    }
    return true;
  };
  if (!coloringValid(icol, adjInt) || !coloringValid(fcol, adjFloat)) {
    sum.vreg.assign(static_cast<size_t>(nv), IRRegallocInfo{});
    return sum;
  }

  sum.vreg.resize(static_cast<size_t>(nv));
  bool anyColored = false;
  for (int v = 0; v < nv; ++v) {
    IRRegallocInfo info;
    if (icol[static_cast<size_t>(v)] >= 0) {
      info.intPhys = icol[static_cast<size_t>(v)];
      int code = intPool[static_cast<size_t>(info.intPhys)];
      if (code >= 100) {
        sum.usedCalleeSavedInt |= 1u << static_cast<unsigned>(code - 100 - 1);
      }
      anyColored = true;
    }
    if (fcol[static_cast<size_t>(v)] >= 0) {
      info.floatPhys = fcol[static_cast<size_t>(v)];
      anyColored = true;
    }
    sum.vreg[static_cast<size_t>(v)] = info;
  }
  sum.enabled = anyColored;
  sum.syncStackSlots = sum.enabled;
  return sum;
}
