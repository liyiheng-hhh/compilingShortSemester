#include "ir.h"

#include <cstdint>
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
    case IROp::ConstI:
    case IROp::ConstF:
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

    bool folded = false;
    Key k{};
    switch (in.op) {
    case IROp::Add:
    case IROp::Mul:
      k = makeKeyComm(in);
      folded = tryCse(k);
      if (!folded) {
        cse[k] = in.dst;
      }
      break;
    case IROp::Sub:
    case IROp::Div:
    case IROp::Rem:
    case IROp::Neg:
    case IROp::Slt:
    case IROp::Seqz:
    case IROp::Snez:
    case IROp::Sll:
    case IROp::FAdd:
    case IROp::FSub:
    case IROp::FMul:
    case IROp::FDiv:
    case IROp::FNeg:
    case IROp::ICvtF:
    case IROp::FCvtI:
    case IROp::FCmp:
      k = makeKeyPlain(in);
      folded = tryCse(k);
      if (!folded) {
        cse[k] = in.dst;
      }
      break;
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
}
