#include "ir.h"

#include "common.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

using namespace std;

static bool sideEffecting(const IRInst &in) {
  switch (in.op) {
  case IROp::StoreMem:
  case IROp::StoreLocal:
  case IROp::StoreGlobal:
  case IROp::Call:
    return true;
  default:
    return false;
  }
}

// k > 1 且为 2 的幂时返回 log2(k)，否则 -1（用于 Mul → Sll 强度削弱）
static int intLog2PositivePow2_32(int32_t k) {
  if (k <= 1 || (k & (k - 1)) != 0) {
    return -1;
  }
  int r = 0;
  int32_t x = k;
  while (x > 1) {
    x >>= 1;
    ++r;
  }
  return r;
}

static int findRoot(vector<int> &uf, int x) {
  if (x < 0) {
    return x;
  }
  while (static_cast<size_t>(x) < uf.size() && uf[x] != x) {
    uf[x] = uf[uf[x]];
    x = uf[x];
  }
  return x;
}

void irRefreshCFG(IRFunction &fn) {
  fn.blocks.clear();
  fn.entryBlockIndex = 0;
  auto &inst = fn.insts;
  const size_t n = inst.size();
  if (n == 0) {
    return;
  }

  vector<size_t> leaders;
  leaders.reserve(32);
  leaders.push_back(0);
  for (size_t i = 1; i < n; ++i) {
    if (inst[i].op == IROp::Label) {
      leaders.push_back(i);
    }
    const IROp p = inst[i - 1].op;
    if (p == IROp::J || p == IROp::Ret || p == IROp::Beqz) {
      leaders.push_back(i);
    }
  }
  sort(leaders.begin(), leaders.end());
  leaders.erase(unique(leaders.begin(), leaders.end()), leaders.end());

  unordered_map<string, int> labelToBlock;
  for (size_t k = 0; k < leaders.size(); ++k) {
    const size_t b = leaders[k];
    const size_t e = (k + 1 < leaders.size()) ? leaders[k + 1] : n;
    IRBlock blk;
    blk.begin = b;
    blk.end = e;
    fn.blocks.push_back(blk);
    if (b < e && inst[b].op == IROp::Label) {
      labelToBlock[inst[b].ext] = static_cast<int>(k);
    }
  }

  auto findBlk = [&](const string &lab) -> int {
    auto it = labelToBlock.find(lab);
    return it == labelToBlock.end() ? -1 : it->second;
  };

  const int nb = static_cast<int>(fn.blocks.size());
  for (int bi = 0; bi < nb; ++bi) {
    IRBlock &blk = fn.blocks[static_cast<size_t>(bi)];
    blk.succ.clear();
    if (blk.begin >= blk.end) {
      if (bi + 1 < nb) {
        blk.succ.push_back(bi + 1);
      }
      continue;
    }
    size_t t = blk.end;
    while (t > blk.begin && inst[t - 1].op == IROp::Nop) {
      --t;
    }
    if (t == blk.begin) {
      if (bi + 1 < nb) {
        blk.succ.push_back(bi + 1);
      }
      continue;
    }
    const IRInst &term = inst[t - 1];
    if (term.op == IROp::J) {
      const int tg = findBlk(term.ext);
      if (tg >= 0) {
        blk.succ.push_back(tg);
      }
    } else if (term.op == IROp::Beqz) {
      if (bi + 1 < nb) {
        blk.succ.push_back(bi + 1);
      }
      const int tg = findBlk(term.ext);
      if (tg >= 0) {
        blk.succ.push_back(tg);
      }
    } else if (term.op == IROp::Ret) {
      // no successors
    } else {
      if (bi + 1 < nb) {
        blk.succ.push_back(bi + 1);
      }
    }
  }
}

// Single-block while: Label L; body (no inner Label); J L. Hoist LoadGlobal(s)
// with no StoreGlobal to the same symbol in the body and no Call in the body.
static void irHoistInvariantLoadGlobalSimpleWhile(IRFunction &fn) {
  vector<IRInst> &inst = fn.insts;
  const size_t n = inst.size();
  for (size_t i = 0; i + 2 < n; ++i) {
    if (inst[i].op != IROp::Label) {
      continue;
    }
    const string &lab = inst[i].ext;
    size_t j = i + 1;
    while (j < n && inst[j].op != IROp::Label) {
      ++j;
    }
    if (j <= i + 1 || inst[j - 1].op != IROp::J || inst[j - 1].ext != lab) {
      continue;
    }
    const size_t bodyStart = i + 1;
    const size_t bodyEnd = j - 2;
    bool hasCall = false;
    for (size_t k = bodyStart; k <= bodyEnd; ++k) {
      if (inst[k].op == IROp::Call) {
        hasCall = true;
        break;
      }
    }
    if (hasCall) {
      continue;
    }
    vector<pair<Symbol *, IRInst>> prelude;
    unordered_map<Symbol *, int> symToLoadVreg;
    unordered_map<Symbol *, int> symToLeaVreg;
    for (size_t k = bodyStart; k <= bodyEnd; ++k) {
      if (inst[k].op != IROp::LoadGlobal && inst[k].op != IROp::LeaGlobal) {
        continue;
      }
      Symbol *sym = inst[k].sym;
      if (!sym) {
        continue;
      }
      bool stored = false;
      for (size_t t = bodyStart; t <= bodyEnd; ++t) {
        if (inst[t].op == IROp::StoreGlobal && inst[t].sym == sym) {
          stored = true;
          break;
        }
      }
      if (stored) {
        continue;
      }
      if (inst[k].op == IROp::LoadGlobal) {
        if (symToLoadVreg.count(sym)) {
          continue;
        }
        int h = fn.allocVreg();
        symToLoadVreg[sym] = h;
        IRInst ld = inst[k];
        ld.dst = h;
        prelude.push_back({sym, ld});
      } else {
        if (symToLeaVreg.count(sym)) {
          continue;
        }
        int h = fn.allocVreg();
        symToLeaVreg[sym] = h;
        IRInst lea = inst[k];
        lea.dst = h;
        prelude.push_back({sym, lea});
      }
    }
    if (prelude.empty()) {
      continue;
    }
    vector<IRInst> out;
    out.reserve(inst.size() + prelude.size());
    size_t p = 0;
    while (p < n) {
      if (p == i) {
        out.push_back(inst[i]);
        for (const auto &pr : prelude) {
          out.push_back(pr.second);
        }
        for (size_t k = bodyStart; k <= bodyEnd; ++k) {
          IRInst w = inst[k];
          if (w.op == IROp::LoadGlobal && symToLoadVreg.count(w.sym)) {
            IRInst cp;
            cp.op = IROp::Copy;
            cp.dst = w.dst;
            cp.u = symToLoadVreg[w.sym];
            cp.isFloat = w.isFloat;
            out.push_back(cp);
          } else if (w.op == IROp::LeaGlobal && symToLeaVreg.count(w.sym)) {
            IRInst cp;
            cp.op = IROp::Copy;
            cp.dst = w.dst;
            cp.u = symToLeaVreg[w.sym];
            cp.isFloat = false;
            out.push_back(cp);
          } else {
            out.push_back(w);
          }
        }
        out.push_back(inst[j - 1]);
        p = j;
      } else {
        out.push_back(inst[p++]);
      }
    }
    inst.swap(out);
    irHoistInvariantLoadGlobalSimpleWhile(fn);
    return;
  }
}

void irOptimizeBlock(IRFunction &fn) {
  irHoistInvariantLoadGlobalSimpleWhile(fn);
  irRefreshCFG(fn);
  if (fn.insts.empty()) {
    return;
  }

  const int n = static_cast<int>(fn.nextVreg);
  vector<int> copyUF(max(n, 1));
  for (int i = 0; i < n; ++i) {
    copyUF[i] = i;
  }

  auto remap = [&](int r) -> int {
    if (r < 0) {
      return r;
    }
    if (r >= static_cast<int>(copyUF.size())) {
      return r;
    }
    return findRoot(copyUF, r);
  };

  struct Key {
    IROp op = IROp::Nop;
    uint64_t a = 0;
    uint64_t b = 0;
    int32_t imm = 0;
    bool fl = false;
    bool operator==(const Key &o) const {
      return op == o.op && a == o.a && b == o.b && imm == o.imm && fl == o.fl;
    }
  };
  struct KeyHash {
    size_t operator()(const Key &k) const noexcept {
      size_t h = static_cast<size_t>(k.op);
      h = h * 1315423911u ^ static_cast<size_t>(k.a);
      h = h * 1315423911u ^ static_cast<size_t>(k.b);
      h = h * 1315423911u ^ static_cast<uint32_t>(k.imm);
      return h ^ (k.fl ? 1u : 0u);
    }
  };

  unordered_map<Key, int, KeyHash> cse;
  cse.reserve(256);

  auto makeKeyComm = [&](const IRInst &in) -> Key {
    Key k{in.op, 0, 0, in.immI, in.isFloat};
    int ru = remap(in.u);
    int rv = remap(in.v);
    if (in.op == IROp::Add || in.op == IROp::Mul || in.op == IROp::FAdd ||
        in.op == IROp::FMul) {
      if (ru > rv) {
        swap(ru, rv);
      }
    }
    k.a = static_cast<uint64_t>(static_cast<uint32_t>(ru));
    k.b = static_cast<uint64_t>(static_cast<uint32_t>(rv));
    return k;
  };

  auto makeKeyPlain = [&](const IRInst &in) -> Key {
    Key k{in.op, static_cast<uint64_t>(static_cast<uint32_t>(remap(in.u))),
          static_cast<uint64_t>(static_cast<uint32_t>(remap(in.v))), in.immI, in.isFloat};
    return k;
  };

  vector<optional<int32_t>> knownInt(max(n, 1));
  vector<optional<float>> knownFloat(max(n, 1));

  auto followConstI = [&](int r) -> optional<int32_t> {
    if (r < 0 || r >= static_cast<int>(knownInt.size())) {
      return nullopt;
    }
    int root = findRoot(copyUF, r);
    if (root < 0 || root >= static_cast<int>(knownInt.size())) {
      return nullopt;
    }
    return knownInt[root];
  };

  auto followConstF = [&](int r) -> optional<float> {
    if (r < 0 || r >= static_cast<int>(knownFloat.size())) {
      return nullopt;
    }
    int root = findRoot(copyUF, r);
    if (root < 0 || root >= static_cast<int>(knownFloat.size())) {
      return nullopt;
    }
    return knownFloat[root];
  };

  // Store-to-load forwarding state (integrated into main loop)
  unordered_map<Symbol *, int> fwdStore;   // symbol → last stored vreg

  vector<IRInst> out;
  out.reserve(fn.insts.size());

  for (IRInst in : fn.insts) {
    // Memory-clobbering operations invalidate forwarding
    if (in.op == IROp::StoreMem || in.op == IROp::Call) {
      fwdStore.clear();
    }
    if (sideEffecting(in)) {
      cse.clear();
    }
    if (in.op == IROp::Label) {
      cse.clear();
    }

    switch (in.op) {
    case IROp::Label:
      cse.clear();
      fwdStore.clear();
      for (int t = 0; t < n; ++t) {
        copyUF[static_cast<size_t>(t)] = t;
      }
      fill(knownInt.begin(), knownInt.end(), nullopt);
      fill(knownFloat.begin(), knownFloat.end(), nullopt);
      out.push_back(in);
      continue;
    case IROp::J:
      cse.clear();
      fwdStore.clear();
      for (int t = 0; t < n; ++t) {
        copyUF[static_cast<size_t>(t)] = t;
      }
      fill(knownInt.begin(), knownInt.end(), nullopt);
      fill(knownFloat.begin(), knownFloat.end(), nullopt);
      out.push_back(in);
      continue;
    case IROp::Beqz:
      cse.clear();
      fwdStore.clear();
      in.u = remap(in.u);
      out.push_back(in);
      continue;
    case IROp::Copy: {
      int u = remap(in.u);
      if (u >= 0 && in.dst >= 0 && in.dst < static_cast<int>(copyUF.size())) {
        copyUF[in.dst] = u;
      }
      out.push_back(in);
      continue;
    }
    case IROp::StoreLocal:
      // Only real stores record the last stored vreg. LeaGlobal/LeaStr must not
      // touch fwdStore: their `u` is unused (-1) and would poison LoadGlobal.
      fwdStore[in.sym] = in.u;
      in.u = remap(in.u);
      in.v = remap(in.v);
      out.push_back(in);
      continue;
    case IROp::StoreGlobal:
      fwdStore[in.sym] = in.u;
      in.u = remap(in.u);
      in.v = remap(in.v);
      out.push_back(in);
      continue;
    case IROp::LoadLocal: {
      auto it = fwdStore.find(in.sym);
      if (it != fwdStore.end() && it->second >= 0 && in.dst >= 0) {
        // Forward to Copy: reuse stored vreg
        IRInst cp;
        cp.op = IROp::Copy;
        cp.dst = in.dst;
        cp.u = remap(it->second);
        out.push_back(cp);
        if (in.dst >= 0 && in.dst < static_cast<int>(copyUF.size())) {
          copyUF[in.dst] = findRoot(copyUF, remap(it->second));
        }
        continue;
      }
      fwdStore.erase(in.sym);
      in.u = remap(in.u);
      out.push_back(in);
      continue;
    }
    case IROp::LoadGlobal: {
      auto it = fwdStore.find(in.sym);
      if (it != fwdStore.end() && it->second >= 0 && in.dst >= 0) {
        IRInst cp;
        cp.op = IROp::Copy;
        cp.dst = in.dst;
        cp.u = remap(it->second);
        out.push_back(cp);
        if (in.dst >= 0 && in.dst < static_cast<int>(copyUF.size())) {
          copyUF[in.dst] = findRoot(copyUF, remap(it->second));
        }
        continue;
      }
      fwdStore.erase(in.sym);
      in.u = remap(in.u);
      out.push_back(in);
      continue;
    }
    case IROp::LeaStr:
    case IROp::LeaGlobal:
    case IROp::LeaLocal:
    case IROp::LoadParamAddr:
    case IROp::StoreMem:
    case IROp::Call:
    case IROp::Ret:
    case IROp::Nop:
      in.u = remap(in.u);
      in.v = remap(in.v);
      for (int &a : in.args) {
        a = remap(a);
      }
      out.push_back(in);
      continue;
    default:
      break;
    }

    in.u = remap(in.u);
    in.v = remap(in.v);

    auto tryCse = [&](const Key &k) -> bool {
      auto it = cse.find(k);
      if (it == cse.end()) {
        return false;
      }
      IRInst cp;
      cp.op = IROp::Copy;
      cp.dst = in.dst;
      cp.u = it->second;
      out.push_back(cp);
      if (in.dst >= 0 && in.dst < static_cast<int>(copyUF.size())) {
        copyUF[in.dst] = findRoot(copyUF, it->second);
      }
      return true;
    };

    auto emitCopy = [&](int dst, int src) {
      IRInst cp;
      cp.op = IROp::Copy;
      cp.dst = dst;
      cp.u = src;
      out.push_back(cp);
      if (dst >= 0 && dst < static_cast<int>(copyUF.size())) {
        copyUF[dst] = findRoot(copyUF, src);
      }
    };

    auto foldIntConst = [&](int32_t result) -> bool {
      Key ck{IROp::ConstI, static_cast<uint64_t>(static_cast<uint32_t>(result)), 0, 0, false};
      if (tryCse(ck)) {
        if (in.dst >= 0) knownInt[in.dst] = result;
        return true;
      }
      cse[ck] = in.dst;
      in.op = IROp::ConstI;
      in.immI = result;
      in.isFloat = false;
      in.u = -1;
      in.v = -1;
      if (in.dst >= 0) knownInt[in.dst] = result;
      return false;
    };

    auto foldFloatConst = [&](float result) -> bool {
      Key ck{IROp::ConstF, static_cast<uint64_t>(floatBits(result)), 0, 0, true};
      if (tryCse(ck)) {
        if (in.dst >= 0) knownFloat[in.dst] = result;
        return true;
      }
      cse[ck] = in.dst;
      in.op = IROp::ConstF;
      in.immF = result;
      in.isFloat = true;
      in.u = -1;
      in.v = -1;
      if (in.dst >= 0) knownFloat[in.dst] = result;
      return false;
    };

    bool folded = false;
    Key k{};
    switch (in.op) {
    case IROp::ConstI: {
      Key ck{IROp::ConstI, static_cast<uint64_t>(static_cast<uint32_t>(in.immI)), 0, 0, false};
      folded = tryCse(ck);
      if (!folded) {
        cse[ck] = in.dst;
      }
      if (in.dst >= 0) knownInt[in.dst] = in.immI;
      break;
    }
    case IROp::ConstF: {
      Key ck{IROp::ConstF, static_cast<uint64_t>(floatBits(in.immF)), 0, 0, true};
      folded = tryCse(ck);
      if (!folded) {
        cse[ck] = in.dst;
      }
      if (in.dst >= 0) knownFloat[in.dst] = in.immF;
      break;
    }
    case IROp::Add: {
      auto ci = followConstI(in.u);
      auto cj = followConstI(in.v);
      if (ci && cj) {
        folded = foldIntConst(*ci + *cj);
        break;
      }
      if (ci && *ci == 0) {
        emitCopy(in.dst, in.v);
        folded = true;
        break;
      }
      if (cj && *cj == 0) {
        emitCopy(in.dst, in.u);
        folded = true;
        break;
      }
      if (remap(in.u) == remap(in.v)) {
        IRInst sh;
        sh.op = IROp::Sll;
        sh.dst = in.dst;
        sh.u = in.u;
        sh.immI = 1;
        out.push_back(sh);
        if (in.dst >= 0 && in.dst < static_cast<int>(knownInt.size())) {
          knownInt[in.dst].reset();
        }
        folded = true;
        break;
      }
      k = makeKeyComm(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::Sub: {
      auto ci = followConstI(in.u);
      auto cj = followConstI(in.v);
      if (ci && cj) {
        folded = foldIntConst(*ci - *cj);
        break;
      }
      if (cj && *cj == 0) {
        emitCopy(in.dst, in.u);
        folded = true;
        break;
      }
      if (ci && *ci == 0) {
        IRInst ng;
        ng.op = IROp::Neg;
        ng.dst = in.dst;
        ng.u = in.v;
        out.push_back(ng);
        if (in.dst >= 0 && in.dst < static_cast<int>(knownInt.size())) {
          knownInt[in.dst].reset();
        }
        folded = true;
        break;
      }
      if (remap(in.u) == remap(in.v)) {
        folded = foldIntConst(0);
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::Mul: {
      auto ci = followConstI(in.u);
      auto cj = followConstI(in.v);
      if (ci && cj) {
        folded = foldIntConst(*ci * *cj);
        break;
      }
      if ((ci && *ci == 0) || (cj && *cj == 0)) {
        folded = foldIntConst(0);
        break;
      }
      if (ci && *ci == 1) {
        emitCopy(in.dst, in.v);
        folded = true;
        break;
      }
      if (cj && *cj == 1) {
        emitCopy(in.dst, in.u);
        folded = true;
        break;
      }
      if (ci && *ci == -1) {
        IRInst ng;
        ng.op = IROp::Neg;
        ng.dst = in.dst;
        ng.u = in.v;
        out.push_back(ng);
        if (in.dst >= 0 && in.dst < static_cast<int>(knownInt.size())) {
          knownInt[in.dst].reset();
        }
        folded = true;
        break;
      }
      if (cj && *cj == -1) {
        IRInst ng;
        ng.op = IROp::Neg;
        ng.dst = in.dst;
        ng.u = in.u;
        out.push_back(ng);
        if (in.dst >= 0 && in.dst < static_cast<int>(knownInt.size())) {
          knownInt[in.dst].reset();
        }
        folded = true;
        break;
      }
      auto emitNegPow2Mul = [&](int32_t negK, int srcV) -> bool {
        if (negK >= -1 || negK == INT32_MIN) {
          return false;
        }
        int32_t ak = static_cast<int32_t>(-static_cast<int64_t>(negK));
        int shamt = intLog2PositivePow2_32(ak);
        if (shamt <= 0) {
          return false;
        }
        IRInst sh;
        sh.op = IROp::Sll;
        sh.dst = in.dst;
        sh.u = srcV;
        sh.immI = shamt;
        out.push_back(sh);
        IRInst ng;
        ng.op = IROp::Neg;
        ng.dst = in.dst;
        ng.u = in.dst;
        out.push_back(ng);
        if (in.dst >= 0 && in.dst < static_cast<int>(knownInt.size())) {
          knownInt[in.dst].reset();
        }
        return true;
      };
      if (ci && emitNegPow2Mul(*ci, in.v)) {
        folded = true;
        break;
      }
      if (cj && emitNegPow2Mul(*cj, in.u)) {
        folded = true;
        break;
      }
      if (ci && *ci > 1) {
        int shamt = intLog2PositivePow2_32(*ci);
        if (shamt > 0) {
          IRInst sh;
          sh.op = IROp::Sll;
          sh.dst = in.dst;
          sh.u = in.v;
          sh.immI = shamt;
          out.push_back(sh);
          if (in.dst >= 0 && in.dst < static_cast<int>(knownInt.size())) {
            knownInt[in.dst].reset();
          }
          folded = true;
          break;
        }
      }
      if (cj && *cj > 1) {
        int shamt = intLog2PositivePow2_32(*cj);
        if (shamt > 0) {
          IRInst sh;
          sh.op = IROp::Sll;
          sh.dst = in.dst;
          sh.u = in.u;
          sh.immI = shamt;
          out.push_back(sh);
          if (in.dst >= 0 && in.dst < static_cast<int>(knownInt.size())) {
            knownInt[in.dst].reset();
          }
          folded = true;
          break;
        }
      }
      k = makeKeyComm(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::Div: {
      auto ci = followConstI(in.u);
      auto cj = followConstI(in.v);
      if (ci && cj && *cj != 0) {
        folded = foldIntConst(*ci / *cj);
        break;
      }
      if (cj && *cj == 1) {
        emitCopy(in.dst, in.u);
        folded = true;
        break;
      }
      if (cj && *cj == -1) {
        IRInst ng;
        ng.op = IROp::Neg;
        ng.dst = in.dst;
        ng.u = in.u;
        out.push_back(ng);
        if (in.dst >= 0 && in.dst < static_cast<int>(knownInt.size())) {
          knownInt[in.dst].reset();
        }
        folded = true;
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::Rem: {
      auto ci = followConstI(in.u);
      auto cj = followConstI(in.v);
      if (ci && cj && *cj != 0) {
        folded = foldIntConst(*ci % *cj);
        break;
      }
      if (cj && (*cj == 1 || *cj == -1)) {
        folded = foldIntConst(0);
        break;
      }
      if (remap(in.u) == remap(in.v)) {
        folded = foldIntConst(0);
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::Neg: {
      auto ci = followConstI(in.u);
      if (ci) {
        folded = foldIntConst(-*ci);
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::Slt: {
      auto ci = followConstI(in.u);
      auto cj = followConstI(in.v);
      if (ci && cj) {
        folded = foldIntConst(*ci < *cj ? 1 : 0);
        break;
      }
      if (remap(in.u) == remap(in.v)) {
        folded = foldIntConst(0);
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::Seqz: {
      auto ci = followConstI(in.u);
      if (ci) {
        folded = foldIntConst(*ci == 0 ? 1 : 0);
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::Snez: {
      auto ci = followConstI(in.u);
      if (ci) {
        folded = foldIntConst(*ci != 0 ? 1 : 0);
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::Sll: {
      int sh = in.immI & 31;
      auto cu = followConstI(in.u);
      if (cu) {
        uint32_t bits = static_cast<uint32_t>(*cu);
        folded = foldIntConst(static_cast<int32_t>(bits << sh));
        break;
      }
      if (sh == 0) {
        emitCopy(in.dst, in.u);
        folded = true;
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::FAdd: {
      auto fi = followConstF(in.u);
      auto fj = followConstF(in.v);
      if (fi && fj) {
        folded = foldFloatConst(*fi + *fj);
        break;
      }
      if (fj && *fj == 0.0f) {
        emitCopy(in.dst, in.u);
        folded = true;
        break;
      }
      if (fi && *fi == 0.0f) {
        emitCopy(in.dst, in.v);
        folded = true;
        break;
      }
      k = makeKeyComm(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::FSub: {
      auto fi = followConstF(in.u);
      auto fj = followConstF(in.v);
      if (fi && fj) {
        folded = foldFloatConst(*fi - *fj);
        break;
      }
      if (fj && *fj == 0.0f) {
        emitCopy(in.dst, in.u);
        folded = true;
        break;
      }
      if (fi && *fi == 0.0f) {
        IRInst ng;
        ng.op = IROp::FNeg;
        ng.dst = in.dst;
        ng.u = in.v;
        ng.isFloat = true;
        out.push_back(ng);
        if (in.dst >= 0 && in.dst < static_cast<int>(knownFloat.size())) {
          knownFloat[in.dst].reset();
        }
        folded = true;
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::FMul: {
      auto fi = followConstF(in.u);
      auto fj = followConstF(in.v);
      if (fi && fj) {
        folded = foldFloatConst(*fi * *fj);
        break;
      }
      if ((fi && *fi == 0.0f) || (fj && *fj == 0.0f)) {
        folded = foldFloatConst(0.0f);
        break;
      }
      if (fi && *fi == 1.0f) {
        emitCopy(in.dst, in.v);
        folded = true;
        break;
      }
      if (fj && *fj == 1.0f) {
        emitCopy(in.dst, in.u);
        folded = true;
        break;
      }
      if (fi && *fi == -1.0f) {
        IRInst ng;
        ng.op = IROp::FNeg;
        ng.dst = in.dst;
        ng.u = in.v;
        ng.isFloat = true;
        out.push_back(ng);
        if (in.dst >= 0 && in.dst < static_cast<int>(knownFloat.size())) {
          knownFloat[in.dst].reset();
        }
        folded = true;
        break;
      }
      if (fj && *fj == -1.0f) {
        IRInst ng;
        ng.op = IROp::FNeg;
        ng.dst = in.dst;
        ng.u = in.u;
        ng.isFloat = true;
        out.push_back(ng);
        if (in.dst >= 0 && in.dst < static_cast<int>(knownFloat.size())) {
          knownFloat[in.dst].reset();
        }
        folded = true;
        break;
      }
      k = makeKeyComm(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::FDiv: {
      auto fi = followConstF(in.u);
      auto fj = followConstF(in.v);
      if (fi && fj && *fj != 0.0f) {
        folded = foldFloatConst(*fi / *fj);
        break;
      }
      if (fi && *fi == 0.0f) {
        folded = foldFloatConst(0.0f);
        break;
      }
      if (fj && *fj == 1.0f) {
        emitCopy(in.dst, in.u);
        folded = true;
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::FNeg: {
      auto fi = followConstF(in.u);
      if (fi) {
        folded = foldFloatConst(-*fi);
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::ICvtF: {
      auto ci = followConstI(in.u);
      if (ci) {
        folded = foldFloatConst(static_cast<float>(*ci));
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::FCvtI: {
      auto fi = followConstF(in.u);
      if (fi) {
        folded = foldIntConst(static_cast<int32_t>(*fi));
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::FCmp: {
      auto fi = followConstF(in.u);
      auto fj = followConstF(in.v);
      if (fi && fj) {
        bool r = false;
        switch (in.immI) {
        case FCMP_EQ: r = *fi == *fj; break;
        case FCMP_NE: r = *fi != *fj; break;
        case FCMP_LT: r = *fi < *fj; break;
        case FCMP_GT: r = *fi > *fj; break;
        case FCMP_LE: r = *fi <= *fj; break;
        case FCMP_GE: r = *fi >= *fj; break;
        default: break;
        }
        folded = foldIntConst(r ? 1 : 0);
        break;
      }
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    }
    case IROp::LoadMem:
      k = Key{in.op, static_cast<uint64_t>(static_cast<uint32_t>(in.u)), 0, 0, in.isFloat};
      folded = tryCse(k);
      if (!folded) {
        cse[k] = in.dst;
      }
      break;
    default:
      break;
    }

    if (sideEffecting(in)) {
      cse.clear();
    }

    if (!folded) {
      out.push_back(in);
    }
  }

  fn.insts.swap(out);

  // Dead code elimination: iterate to fixed point
  bool changed = true;
  while (changed) {
    changed = false;
    const int nv = static_cast<int>(fn.nextVreg);
    vector<int> useCount(max(nv, 1));
    for (const auto &inst : fn.insts) {
      if (inst.u >= 0 && inst.u < nv) ++useCount[inst.u];
      if (inst.v >= 0 && inst.v < nv) ++useCount[inst.v];
      for (int a : inst.args) {
        if (a >= 0 && a < nv) ++useCount[a];
      }
    }

    vector<IRInst> live;
    live.reserve(fn.insts.size());
    for (auto &inst : fn.insts) {
      if (!sideEffecting(inst) && inst.dst >= 0 && inst.dst < nv &&
          useCount[inst.dst] == 0) {
        changed = true;
        continue;
      }
      live.push_back(inst);
    }
    fn.insts.swap(live);
  }
  // 刷新 CFG；跨块常量传播可在此后基于 blocks/succ 增量接入
  irRefreshCFG(fn);
}

static void irAugmentLastUseWithCFG(IRFunction &fn, vector<size_t> &lastUse) {
  irRefreshCFG(fn);
  const int nb = static_cast<int>(fn.blocks.size());
  const int nv = static_cast<int>(fn.nextVreg);
  if (nb <= 0 || nv <= 0) {
    return;
  }

  vector<vector<char>> useB(static_cast<size_t>(nb)),
      defB(static_cast<size_t>(nb));
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

  vector<vector<char>> inn(static_cast<size_t>(nb)),
      out(static_cast<size_t>(nb));
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
      for (int v = 0; v < nv; ++v) {
        if (out[static_cast<size_t>(b)][static_cast<size_t>(v)] !=
            nOut[static_cast<size_t>(v)]) {
          chg = true;
        }
      }
      out[static_cast<size_t>(b)].swap(nOut);
    }
    for (int b = 0; b < nb; ++b) {
      vector<char> nIn(static_cast<size_t>(nv), 0);
      for (int v = 0; v < nv; ++v) {
        const size_t vi = static_cast<size_t>(v);
        if (useB[static_cast<size_t>(b)][vi] ||
            (out[static_cast<size_t>(b)][vi] && !defB[static_cast<size_t>(b)][vi])) {
          nIn[vi] = 1;
        }
      }
      for (int v = 0; v < nv; ++v) {
        const size_t vi = static_cast<size_t>(v);
        if (inn[static_cast<size_t>(b)][vi] != nIn[vi]) {
          chg = true;
        }
      }
      inn[static_cast<size_t>(b)].swap(nIn);
    }
    if (!chg) {
      break;
    }
  }

  for (int b = 0; b < nb; ++b) {
    const IRBlock &blk = fn.blocks[static_cast<size_t>(b)];
    vector<char> live(static_cast<size_t>(nv), 0);
    for (int v = 0; v < nv; ++v) {
      live[static_cast<size_t>(v)] = inn[static_cast<size_t>(b)][static_cast<size_t>(v)];
    }
    for (size_t i = blk.begin; i < blk.end; ++i) {
      const IRInst &in = fn.insts[i];
      auto bump = [&](int r) {
        if (r >= 0 && r < nv) {
          lastUse[static_cast<size_t>(r)] =
              max(lastUse[static_cast<size_t>(r)], i);
        }
      };
      for (int v = 0; v < nv; ++v) {
        if (live[static_cast<size_t>(v)]) {
          lastUse[static_cast<size_t>(v)] =
              max(lastUse[static_cast<size_t>(v)], i);
        }
      }
      bump(in.u);
      bump(in.v);
      for (int a : in.args) {
        bump(a);
      }
      if (in.dst >= 0 && in.dst < nv) {
        live[static_cast<size_t>(in.dst)] = 0;
      }
      if (in.dst >= 0 && in.dst < nv) {
        live[static_cast<size_t>(in.dst)] = 1;
      }
    }
  }
}

void irAssignSlots(IRFunction &fn) {
  const int nv = static_cast<int>(fn.nextVreg);
  fn.vregSlots.assign(static_cast<size_t>(std::max(nv, 0)), -1);
  if (nv <= 0) {
    return;
  }

  // Compute last-use position for each vreg (linear scan)
  vector<size_t> lastUse(static_cast<size_t>(nv), 0);
  for (size_t idx = 0; idx < fn.insts.size(); ++idx) {
    const IRInst &in = fn.insts[idx];
    if (in.u >= 0 && in.u < nv) lastUse[static_cast<size_t>(in.u)] = idx;
    if (in.v >= 0 && in.v < nv) lastUse[static_cast<size_t>(in.v)] = idx;
    for (int a : in.args) {
      if (a >= 0 && a < nv) lastUse[static_cast<size_t>(a)] = idx;
    }
  }
  irAugmentLastUseWithCFG(fn, lastUse);

  // Linear scan slot assignment: reuse slots after last use
  vector<int> slotFreeAt; // slot → freed after instruction index
  vector<int> allocated(static_cast<size_t>(nv), -1);

  for (size_t idx = 0; idx < fn.insts.size(); ++idx) {
    const IRInst &in = fn.insts[idx];

    // Free slots whose owner's last use was at a previous instruction
    for (size_t s = 0; s < slotFreeAt.size(); ++s) {
      if (slotFreeAt[s] >= 0 && static_cast<size_t>(slotFreeAt[s]) < idx) {
        slotFreeAt[s] = -1; // mark as free
      }
    }

    // Allocate slot for dst
    if (in.dst >= 0 && in.dst < nv && allocated[static_cast<size_t>(in.dst)] < 0) {
      int slot = -1;
      if (in.op == IROp::Copy && in.u >= 0 && in.u < nv &&
          lastUse[static_cast<size_t>(in.u)] == idx &&
          allocated[static_cast<size_t>(in.u)] >= 0) {
        slot = allocated[static_cast<size_t>(in.u)];
        allocated[static_cast<size_t>(in.dst)] = slot;
        int lu = static_cast<int>(lastUse[static_cast<size_t>(in.dst)]);
        size_t su = static_cast<size_t>(slot);
        if (lu > slotFreeAt[su]) {
          slotFreeAt[su] = lu;
        }
      } else {
        // Find a freed slot
        for (size_t s = 0; s < slotFreeAt.size(); ++s) {
          if (slotFreeAt[s] < 0) {
            slot = static_cast<int>(s);
            break;
          }
        }
        if (slot < 0) {
          // Allocate new slot
          slot = static_cast<int>(slotFreeAt.size());
          slotFreeAt.push_back(-1);
        }
        slotFreeAt[static_cast<size_t>(slot)] =
            static_cast<int>(lastUse[static_cast<size_t>(in.dst)]);
        allocated[static_cast<size_t>(in.dst)] = slot;
      }
    }
  }

  fn.vregSlots = allocated;
}

int irSlotCount(const IRFunction &fn) {
  int maxSlot = -1;
  for (int s : fn.vregSlots) {
    if (s > maxSlot) maxSlot = s;
  }
  return maxSlot + 1;
}
