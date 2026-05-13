#include "ir.h"

#include "common.h"

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

void irOptimizeBlock(IRFunction &fn) {
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
  cse.reserve(64);

  auto makeKeyComm = [&](const IRInst &in) -> Key {
    Key k{in.op, 0, 0, in.immI, in.isFloat};
    int ru = remap(in.u);
    int rv = remap(in.v);
    if (in.op == IROp::Add || in.op == IROp::Mul) {
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

  vector<IRInst> out;
  out.reserve(fn.insts.size());

  for (IRInst in : fn.insts) {
    if (sideEffecting(in)) {
      cse.clear();
    }

    switch (in.op) {
    case IROp::Copy: {
      int u = remap(in.u);
      if (u >= 0 && in.dst >= 0 && in.dst < static_cast<int>(copyUF.size())) {
        copyUF[in.dst] = u;
      }
      out.push_back(in);
      continue;
    }
    case IROp::LeaStr:
    case IROp::LeaGlobal:
    case IROp::LeaLocal:
    case IROp::LoadParamAddr:
    case IROp::StoreMem:
    case IROp::StoreLocal:
    case IROp::StoreGlobal:
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
      if (in.u == in.v) {
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
      if (in.u == in.v) {
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
    case IROp::Sll:
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) cse[k] = in.dst;
      break;
    case IROp::FAdd: {
      auto fi = followConstF(in.u);
      auto fj = followConstF(in.v);
      if (fi && fj) {
        folded = foldFloatConst(*fi + *fj);
        break;
      }
      k = makeKeyPlain(in);
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
      k = makeKeyPlain(in);
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
    case IROp::LoadLocal:
    case IROp::LoadGlobal:
      k = Key{in.op, reinterpret_cast<uint64_t>(in.sym), 0, 0, in.isFloat};
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
}
