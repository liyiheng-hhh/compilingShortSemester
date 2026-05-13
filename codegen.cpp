#include "codegen.h"

#include "common.h"

#include <algorithm>
#include <optional>

using namespace std;

struct ArgLoc {
  enum Kind { IntReg, FloatReg, Stack } kind = IntReg;
  int index = 0;
  bool valueIsFloat = false;
};

static int intLog2Positive32(int32_t v) {
  if (v <= 0 || (v & (v - 1)) != 0) {
    return -1;
  }
  int r = 0;
  while (v > 1) {
    v >>= 1;
    ++r;
  }
  return r;
}

// Granlund-Montgomery: compute magic number and shift for signed division x/d
// Returns magic=0 if d is not optimizable (d <= 1 or too large)
static uint32_t computeSDivMagic(int32_t d, int &shift) {
  if (d <= 1) return 0;
  uint32_t ad = static_cast<uint32_t>(d);
  uint32_t t = 0x80000000u;  // 2^31
  uint32_t anc = t - 1 - t % ad;
  int p = 31;
  uint64_t q1 = 0x80000000u / anc;
  uint64_t r1 = 0x80000000u - q1 * anc;
  uint64_t q2 = 0x80000000u / ad;
  uint64_t r2 = 0x80000000u - q2 * ad;
  while (true) {
    ++p;
    q1 <<= 1; r1 <<= 1;
    if (r1 >= anc) { ++q1; r1 -= anc; }
    q2 <<= 1; r2 <<= 1;
    if (r2 >= ad) { ++q2; r2 -= ad; }
    uint64_t delta = ad - r2;
    if (q1 < delta || (q1 == delta && r1 == 0)) continue;
    break;
  }
  shift = p - 32;
  return static_cast<uint32_t>(q2 + 1);
}

// Emit code for signed division a0 = a0 / d using magic multiply
static bool emitSDivByConst(CodeGen &cg, int32_t d) {
  if (d <= 1) return false;
  int shift = 0;
  uint32_t magic = computeSDivMagic(d, shift);
  if (magic == 0 || shift < 0 || shift > 31) return false;
  cg.emit("\tmv\tt2, a0");
  cg.emit("\tli\tt1, " + to_string(static_cast<int32_t>(magic)));
  cg.emit("\tmulh\ta0, a0, t1");
  if (shift > 0) {
    cg.emit("\tsrai\ta0, a0, " + to_string(shift));
  }
  cg.emit("\tsrai\tt2, t2, 31");
  cg.emit("\tadd\ta0, a0, t2");
  return true;
}

// Emit code for signed remainder a0 = a0 % d using magic multiply
static bool emitSRemByConst(CodeGen &cg, int32_t d) {
  if (d <= 1) return false;
  int shift = 0;
  uint32_t magic = computeSDivMagic(d, shift);
  if (magic == 0 || shift < 0 || shift > 31) return false;
  // r = x - (x/d)*d
  cg.emit("\tmv\tt3, a0");                          // t3 = x
  cg.emit("\tli\tt1, " + to_string(static_cast<int32_t>(magic)));
  cg.emit("\tmulh\ta0, a0, t1");                    // high mul for quotient
  if (shift > 0) {
    cg.emit("\tsrai\ta0, a0, " + to_string(shift));
  }
  cg.emit("\tsrai\tt2, t3, 31");                    // sign bit
  cg.emit("\tadd\ta0, a0, t2");                     // a0 = x/d
  cg.emit("\tli\tt1, " + to_string(static_cast<int>(d)));
  cg.emit("\tmulw\ta0, a0, t1");                    // a0 = (x/d)*d
  cg.emit("\tsubw\ta0, t3, a0");                    // a0 = x - (x/d)*d
  return true;
}

// Emit optimal RISC-V code for a0 = a0 * k. Returns true if code was emitted.
static bool emitMulByConst(CodeGen &cg, int32_t k) {
  if (k == 0) {
    cg.emit("\tli\ta0, 0");
    return true;
  }
  if (k == 1) return true; // identity
  if (k == -1) {
    cg.emit("\tnegw\ta0, a0");
    return true;
  }
  int lg = intLog2Positive32(k > 0 ? k : -k);
  if (lg >= 0) {
    cg.emit(k > 0 ? "\tslliw\ta0, a0, " + to_string(lg)
                  : "\tslliw\ta0, a0, " + to_string(lg) + "\n\tnegw\ta0, a0");
    return true;
  }
  // Try k = 2^a ± 2^b or k = 2^a ± 1
  auto tryEmit = [&](int32_t val, bool negate) -> bool {
    if (val <= 1 || val > 256) return false;
    // Try val = 2^a ± 1
    int a = intLog2Positive32(val - 1);
    if (a >= 0) {
      cg.emit("\tslliw\tt2, a0, " + to_string(a));
      cg.emit(negate ? "\tsubw\ta0, t2, a0" : "\taddw\ta0, t2, a0");
      if (k < 0) cg.emit("\tnegw\ta0, a0");
      return true;
    }
    a = intLog2Positive32(val + 1);
    if (a >= 0) {
      cg.emit("\tslliw\tt2, a0, " + to_string(a));
      cg.emit(negate ? "\taddw\ta0, t2, a0" : "\tsubw\ta0, t2, a0");
      if (k < 0) cg.emit("\tnegw\ta0, a0");
      return true;
    }
    // Try val = 2^a + 2^b
    for (a = 1; a <= 8; ++a) {
      int rem = val - (1 << a);
      int b = intLog2Positive32(rem);
      if (b >= 0) {
        cg.emit("\tslliw\tt2, a0, " + to_string(a));
        cg.emit("\tslliw\tt1, a0, " + to_string(b));
        cg.emit(negate ? "\tsubw\ta0, t2, t1" : "\taddw\ta0, t2, t1");
        if (k < 0) cg.emit("\tnegw\ta0, a0");
        return true;
      }
    }
    // Try val = 2^a - 2^b
    for (a = 1; a <= 8; ++a) {
      int sum = val + (1 << a);
      int b = intLog2Positive32(sum);
      if (b >= 0 && b > a) {
        cg.emit("\tslliw\tt2, a0, " + to_string(b));
        cg.emit("\tslliw\tt1, a0, " + to_string(a));
        cg.emit(negate ? "\taddw\ta0, t2, t1" : "\tsubw\ta0, t2, t1");
        if (k < 0) cg.emit("\tnegw\ta0, a0");
        return true;
      }
    }
    return false;
  };
  if (k > 0) return tryEmit(k, false);
  return tryEmit(-k, true);
}

static bool exprIsLeaf(Expr *e) {
  if (!e) {
    return true;
  }
  switch (e->kind) {
  case ExprKind::Number:
    return true;
  case ExprKind::String:
    return true;
  case ExprKind::LVal: {
    auto *lv = static_cast<LValExpr *>(e);
    for (auto &ix : lv->indices) {
      if (!exprIsLeaf(ix.get())) {
        return false;
      }
    }
    return true;
  }
  case ExprKind::Unary:
    return exprIsLeaf(static_cast<UnaryExpr *>(e)->expr.get());
  default:
    return false;
  }
}

static bool exprHasNoCall(Expr *e) {
  if (!e) return true;
  switch (e->kind) {
  case ExprKind::Number:
  case ExprKind::String:
    return true;
  case ExprKind::LVal: {
    auto *lv = static_cast<LValExpr *>(e);
    for (auto &ix : lv->indices) {
      if (!exprHasNoCall(ix.get())) return false;
    }
    return true;
  }
  case ExprKind::Unary:
    return exprHasNoCall(static_cast<UnaryExpr *>(e)->expr.get());
  case ExprKind::Binary: {
    auto *b = static_cast<BinaryExpr *>(e);
    return exprHasNoCall(b->lhs.get()) && exprHasNoCall(b->rhs.get());
  }
  case ExprKind::Call:
    return false;
  }
  return false;
}

static bool stmtHasCall(Stmt *s);
static bool exprHasCall(Expr *e) {
  if (!e) return false;
  if (e->kind == ExprKind::Call) return true;
  if (e->kind == ExprKind::Unary)
    return exprHasCall(static_cast<UnaryExpr *>(e)->expr.get());
  if (e->kind == ExprKind::Binary) {
    auto *b = static_cast<BinaryExpr *>(e);
    return exprHasCall(b->lhs.get()) || exprHasCall(b->rhs.get());
  }
  if (e->kind == ExprKind::LVal) {
    auto *lv = static_cast<LValExpr *>(e);
    for (auto &ix : lv->indices)
      if (exprHasCall(ix.get())) return true;
    return false;
  }
  return false;
}
static bool stmtHasCall(Stmt *s) {
  if (!s) return false;
  switch (s->kind) {
  case StmtKind::Expr: {
    auto *es = static_cast<ExprStmt *>(s);
    return es->expr && exprHasCall(es->expr.get());
  }
  case StmtKind::Assign: {
    auto *as = static_cast<AssignStmt *>(s);
    for (auto &ix : as->lhs->indices)
      if (exprHasCall(ix.get())) return true;
    return exprHasCall(as->rhs.get());
  }
  case StmtKind::Decl: {
    auto *d = static_cast<DeclStmt *>(s);
    for (auto &vd : d->defs) {
      if (vd.init && !vd.init->isList && vd.init->expr && exprHasCall(vd.init->expr.get()))
        return true;
    }
    return false;
  }
  case StmtKind::Block: {
    auto *b = static_cast<BlockStmt *>(s);
    for (auto &it : b->items)
      if (stmtHasCall(it.get())) return true;
    return false;
  }
  case StmtKind::If: {
    auto *ifs = static_cast<IfStmt *>(s);
    return exprHasCall(ifs->cond.get()) || stmtHasCall(ifs->thenStmt.get()) ||
           (ifs->elseStmt && stmtHasCall(ifs->elseStmt.get()));
  }
  case StmtKind::While: {
    auto *w = static_cast<WhileStmt *>(s);
    return exprHasCall(w->cond.get()) || stmtHasCall(w->body.get());
  }
  case StmtKind::Return: {
    auto *r = static_cast<ReturnStmt *>(s);
    return r->expr && exprHasCall(r->expr.get());
  }
  default: return false;
  }
}

// namedCount: params[0 .. namedCount-1] 为固定形参；其后为 C 变参（须按整数 ABI 传递，
// 见 RISC-V ELF psABI：variadic arguments follow the integer calling convention）。
static vector<ArgLoc> computeArgLocations(const vector<ParamType> &params,
                                          bool fnVariadic, size_t namedCount,
                                          int *stackSlots) {
  vector<ArgLoc> locs;
  int intRegs = 0;
  int floatRegs = 0;
  int stack = 0;
  bool variadicRestOnStack = false;

  for (size_t idx = 0; idx < params.size(); ++idx) {
    const ParamType &p = params[idx];
    ArgLoc loc;
    const bool isVariadicArg = fnVariadic && idx >= namedCount;

    if (isVariadicArg) {
      loc.valueIsFloat = false;
      if (variadicRestOnStack || intRegs >= 8) {
        variadicRestOnStack = true;
        loc.kind = ArgLoc::Stack;
        loc.index = stack++;
      } else {
        loc.kind = ArgLoc::IntReg;
        loc.index = intRegs++;
      }
      locs.push_back(loc);
      continue;
    }

    loc.valueIsFloat = !p.isArray && p.base == BaseType::Float;
    if (loc.valueIsFloat && floatRegs < 8) {
      loc.kind = ArgLoc::FloatReg;
      loc.index = floatRegs++;
    } else {
      if (intRegs < 8) {
        loc.kind = ArgLoc::IntReg;
        loc.index = intRegs++;
      } else {
        loc.kind = ArgLoc::Stack;
        loc.index = stack++;
      }
    }
    locs.push_back(loc);
  }
  if (stackSlots) {
    *stackSlots = stack;
  }
  return locs;
}

CodeGen::CodeGen(Program &program, const Semantic &semantic, bool optO1)
    : program_(program), semantic_(semantic), optO1_(optO1) {}

string CodeGen::run() {
    emitGlobals();
    emit("\t.text");
    for (auto &item : program_.items) {
      if (item.func) {
        emitFunction(*item.func);
      }
    }
    emitLiteralPools();
    return out_.str();
  }

void CodeGen::emit(const string &line) { out_ << line << '\n'; }

string CodeGen::newLabel(const string &prefix) {
    return ".L" + prefix + "_" + to_string(labelId_++);
  }

bool CodeGen::fitsImm12(int value) { return value >= -2048 && value <= 2047; }

void CodeGen::emitAdjustSp(int delta) {
    if (delta == 0) {
      return;
    }
    if (fitsImm12(delta)) {
      emit("\taddi\tsp, sp, " + to_string(delta));
    } else {
      emit("\tli\tt6, " + to_string(delta));
      emit("\tadd\tsp, sp, t6");
    }
  }

void CodeGen::emitAddOffset(const string &dst, const string &base, int offset) {
    if (fitsImm12(offset)) {
      emit("\taddi\t" + dst + ", " + base + ", " + to_string(offset));
    } else {
      emit("\tli\t" + dst + ", " + to_string(offset));
      emit("\tadd\t" + dst + ", " + base + ", " + dst);
    }
  }

void CodeGen::emitLoadMem(const string &inst, const string &dst, const string &base,
                   int offset) {
    if (fitsImm12(offset)) {
      emit("\t" + inst + "\t" + dst + ", " + to_string(offset) + "(" + base + ")");
    } else {
      emit("\tli\tt6, " + to_string(offset));
      emit("\tadd\tt6, " + base + ", t6");
      emit("\t" + inst + "\t" + dst + ", 0(t6)");
    }
  }

void CodeGen::emitStoreMem(const string &inst, const string &src, const string &base,
                    int offset) {
    if (fitsImm12(offset)) {
      emit("\t" + inst + "\t" + src + ", " + to_string(offset) + "(" + base + ")");
    } else {
      emit("\tli\tt6, " + to_string(offset));
      emit("\tadd\tt6, " + base + ", t6");
      emit("\t" + inst + "\t" + src + ", 0(t6)");
    }
  }

void CodeGen::emitGlobals() {
    for (Symbol *sym : semantic_.globalsInOrder()) {
      bool allZero = true;
      for (const ConstValue &v : sym->initValues) {
        if ((v.type == BaseType::Float && floatBits(v.f) != 0) ||
            (v.type == BaseType::Int && v.i != 0)) {
          allZero = false;
          break;
        }
      }
      if (sym->isConst) {
        emit("\t.section\t.rodata");
      } else if (allZero) {
        emit("\t.bss");
      } else {
        emit("\t.data");
      }
      emit("\t.align\t2");
      emit("\t.globl\t" + sym->label);
      emit(sym->label + ":");
      int count = sym->isArray ? product(sym->dims) : 1;
      if (allZero) {
        emit("\t.zero\t" + to_string(count * 4));
      } else {
        for (int i = 0; i < count; ++i) {
          ConstValue v = i < static_cast<int>(sym->initValues.size())
                             ? sym->initValues[i]
                             : ConstValue{sym->base, 0, 0.0f};
          if (sym->base == BaseType::Float) {
            emit("\t.word\t" + to_string(floatBits(constAsFloat(v))));
          } else {
            emit("\t.word\t" + to_string(static_cast<int32_t>(constAsInt(v))));
          }
        }
      }
    }
  }

void CodeGen::emitFunction(FuncDef &def) {
    currentFunction_ = def.function;
    currentFunction_->returnLabel = newLabel("return_" + def.name);
    emit("\t.text");
    emit("\t.align\t1");
    emit("\t.globl\t" + def.name);
    emit("\t.type\t" + def.name + ", @function");
    emit(def.name + ":");
    int frame = currentFunction_->frameSize;
    IRFunction irBuf;
    bool useIr = false;
    if (optO1_ && irFunctionEligible(def)) {
      irBuildFunction(def, semantic_, irBuf);
      irOptimizeBlock(irBuf);
      irAssignSlots(irBuf);
      irVregSlots_ = irBuf.vregSlots;
      useIr = true;
      irVregCount_ = irBuf.nextVreg;
      irSpillBase_ = alignTo(currentFunction_->frameUsed, 16);
      int slotCount = irSlotCount(irBuf);
      int need = irSpillBase_ + 8 * std::max(slotCount, 0);
      irTotalFrame_ = max(frame, alignTo(need, 16));
      frame = irTotalFrame_;
      irVFloat_.assign(static_cast<size_t>(max(irVregCount_, 0)), 0);
    } else {
      irVregCount_ = 0;
      irSpillBase_ = 0;
      irTotalFrame_ = frame;
    }
    bool leaf = !stmtHasCall(def.body.get());
    emitAdjustSp(-frame);
    emit("\tli\tt0, " + to_string(frame));
    emit("\tadd\tt0, sp, t0");
    if (!leaf) {
      emit("\tsd\tra, -8(t0)");
    }
    emit("\tsd\ts0, -16(t0)");
    emit("\tmv\ts0, t0");
    emitStoreParams(def);

    // IR leaf functions: cache int params in t4-t6, float params in ft0-ft7
    irParamCache_.clear();
    if (useIr && leaf) {
      vector<ParamType> ptypes;
      for (const Param &p : def.params) {
        ptypes.push_back(ParamType{p.base, p.isArray, p.dims});
      }
      auto plocs = computeArgLocations(ptypes, false, ptypes.size(), nullptr);
      int ti = 0, fi = 0;
      for (size_t i = 0; i < def.params.size(); ++i) {
        if (!def.params[i].isArray && def.params[i].base == BaseType::Int &&
            plocs[i].kind == ArgLoc::IntReg && ti < 3) {
          string treg = "t" + to_string(4 + ti);
          emit("\tmv\t" + treg + ", a" + to_string(plocs[i].index));
          irParamCache_[def.params[i].symbol] = treg;
          ++ti;
        } else if (!def.params[i].isArray && def.params[i].base == BaseType::Float &&
                   plocs[i].kind == ArgLoc::FloatReg && fi < 8) {
          string freg = "ft" + to_string(fi);
          emit("\tfmv.s\t" + freg + ", fa" + to_string(plocs[i].index));
          irParamCache_[def.params[i].symbol] = freg;
          ++fi;
        }
      }
    }

    if (useIr) {
      emitIrFunction(def, irBuf);
    } else {
      emitBlock(*def.body, false);
    }
    if (def.ret == BaseType::Int) {
      emit("\tli\ta0, 0");
    } else if (def.ret == BaseType::Float) {
      emit("\tfmv.w.x\tfa0, zero");
    }
    emit(currentFunction_->returnLabel + ":");
    if (!leaf) {
      emit("\tld\tra, -8(s0)");
    }
    emit("\tld\tt0, -16(s0)");
    emit("\tmv\tsp, s0");
    emit("\tmv\ts0, t0");
    emit("\tret");
    emit("\t.size\t" + def.name + ", .-" + def.name);
    currentFunction_ = nullptr;
  }

int CodeGen::irVregSlotOffset(int vid) const {
  if (vid < 0) return -(irSpillBase_ + 8);
  int slot = vid;
  if (vid < static_cast<int>(irVregSlots_.size()) && irVregSlots_[static_cast<size_t>(vid)] >= 0) {
    slot = irVregSlots_[static_cast<size_t>(vid)];
  }
  return -(irSpillBase_ + 8 * (slot + 1));
}

void CodeGen::emitIrLoadVreg(int vid, bool asFloat) {
  if (vid < 0) return;
  if (asFloat) {
    if (irLastVregInFa0_ == vid) return;
    if (irCacheFt0_ == vid) {
      emit("\tfmv.s\tfa0, ft0");
      irCacheFt0_ = irLastVregInFa0_;
      irLastVregInFa0_ = vid;
      return;
    }
    int off = irVregSlotOffset(vid);
    emitLoadMem("flw", "fa0", "s0", off);
    irCacheFt0_ = irLastVregInFa0_;
    irLastVregInFa0_ = vid;
  } else {
    if (irLastVregInA0_ == vid) return;
    // Check t4 cache: hit → mv a0,t4; rotate
    if (irCacheT4_ == vid) {
      emit("\tmv\ta0, t4");
      irCacheT4_ = irLastVregInA0_;
      irLastVregInA0_ = vid;
      return;
    }
    // Check t5 cache: hit → mv a0,t5; rotate
    if (irCacheT5_ == vid) {
      emit("\tmv\ta0, t5");
      irCacheT5_ = irCacheT4_;
      irCacheT4_ = irLastVregInA0_;
      irLastVregInA0_ = vid;
      return;
    }
    int off = irVregSlotOffset(vid);
    emitLoadMem("lw", "a0", "s0", off);
    // Shift cache: t5←t4, t4←old-a0, a0←vid
    irCacheT5_ = irCacheT4_;
    irCacheT4_ = irLastVregInA0_;
    irLastVregInA0_ = vid;
  }
}

// Load vreg into specified integer register (not a0), without tracking update
void CodeGen::emitIrLoadVregTo(int vid, const string &reg) {
  if (vid < 0) return;
  // Check cache first
  if (irLastVregInA0_ == vid) {
    emit("\tmv\t" + reg + ", a0");
    return;
  }
  if (irCacheT4_ == vid) {
    emit("\tmv\t" + reg + ", t4");
    return;
  }
  if (irCacheT5_ == vid) {
    emit("\tmv\t" + reg + ", t5");
    return;
  }
  int off = irVregSlotOffset(vid);
  emitLoadMem("lw", reg, "s0", off);
}

void CodeGen::emitIrStoreVreg(int vid, bool asFloat) {
  if (vid < 0) {
    return;
  }
  // Skip spill for single-use vregs: keep in a0 for next instruction
  if (irSkipStore_ && vid < static_cast<int>(irSkipStoreVregs_.size()) &&
      irSkipStoreVregs_[static_cast<size_t>(vid)]) {
    irSkippedLast_ = true;
    irSkippedVreg_ = vid;
    if (asFloat) {
      irLastVregInFa0_ = vid;
    } else {
      irLastVregInA0_ = vid;
    }
    return;
  }
  // Normal spill: rotate register cache
  irSkippedLast_ = false;
  irSkippedVreg_ = -1;
  int off = irVregSlotOffset(vid);
  if (asFloat) {
    emitStoreMem("fsw", "fa0", "s0", off);
    irCacheFt0_ = irLastVregInFa0_;
    irLastVregInFa0_ = vid;
  } else {
    emitStoreMem("sw", "a0", "s0", off);
    irCacheT5_ = irCacheT4_;
    irCacheT4_ = irLastVregInA0_;
    irLastVregInA0_ = vid;
  }
}

// Spill previously skipped vreg if not yet stored (called before a0 is overwritten)
void CodeGen::emitIrFlushSkipped() {
  if (irSkippedLast_ && irSkippedVreg_ >= 0) {
    int off = irVregSlotOffset(irSkippedVreg_);
    emitStoreMem("sw", "a0", "s0", off);
    irSkippedLast_ = false;
    irSkippedVreg_ = -1;
  }
}

static Function *lookupFunctionAsm(const Semantic &sem, const string &asmName) {
  for (const auto &kv : sem.functions()) {
    if (kv.second->asmName == asmName) {
      return kv.second;
    }
  }
  return nullptr;
}

static optional<int32_t> irTraceConstI(const IRFunction &ir, int v, size_t before) {
  int cur = v;
  size_t lim = before;
  while (cur >= 0) {
    bool stepped = false;
    for (size_t k = lim; k > 0;) {
      --k;
      const IRInst &w = ir.insts[k];
      if (w.dst != cur) {
        continue;
      }
      stepped = true;
      if (w.op == IROp::ConstI) {
        return w.immI;
      }
      if (w.op == IROp::Copy) {
        cur = w.u;
        lim = k;
        break;
      }
      return nullopt;
    }
    if (!stepped) {
      return nullopt;
    }
  }
  return nullopt;
}

static optional<float> irTraceConstF(const IRFunction &ir, int v, size_t before) {
  int cur = v;
  size_t lim = before;
  while (cur >= 0) {
    bool stepped = false;
    for (size_t k = lim; k > 0;) {
      --k;
      const IRInst &w = ir.insts[k];
      if (w.dst != cur) {
        continue;
      }
      stepped = true;
      if (w.op == IROp::ConstF) {
        return w.immF;
      }
      if (w.op == IROp::Copy) {
        cur = w.u;
        lim = k;
        break;
      }
      return nullopt;
    }
    if (!stepped) {
      return nullopt;
    }
  }
  return nullopt;
}

void CodeGen::emitIrFunction(FuncDef &def, IRFunction &ir) {
  // Compute use counts for skip-store optimization
  const int nv = static_cast<int>(ir.nextVreg);
  vector<int> useCount(std::max(nv, 1));
  for (const auto &inst : ir.insts) {
    if (inst.u >= 0 && inst.u < nv) ++useCount[static_cast<size_t>(inst.u)];
    if (inst.v >= 0 && inst.v < nv) ++useCount[static_cast<size_t>(inst.v)];
    for (int a : inst.args) {
      if (a >= 0 && a < nv) ++useCount[static_cast<size_t>(a)];
    }
  }
  irSkipStoreVregs_.assign(static_cast<size_t>(nv), false);
  for (int v = 0; v < nv; ++v) {
    if (useCount[static_cast<size_t>(v)] == 1) {
      irSkipStoreVregs_[static_cast<size_t>(v)] = true;
    }
  }
  irSkipStore_ = true;

  irLastVregInA0_ = -1;
  irLastVregInFa0_ = -1;
  irCacheT4_ = -1;
  irCacheT5_ = -1;
  irCacheFt0_ = -1;
  irSkippedLast_ = false;
  irSkippedVreg_ = -1;
  for (size_t i = 0; i < ir.insts.size(); ++i) {
    IROp op = ir.insts[i].op;
    if (op == IROp::Call || op == IROp::Ret ||
        op == IROp::ConstI || op == IROp::ConstF ||
        op == IROp::LeaStr || op == IROp::LeaGlobal ||
        op == IROp::LeaLocal || op == IROp::LoadParamAddr ||
        op == IROp::Nop) {
      emitIrFlushSkipped();
      irLastVregInA0_ = -1;
      irLastVregInFa0_ = -1;
      irCacheT4_ = -1;
      irCacheT5_ = -1;
      irCacheFt0_ = -1;
    }
    if (op == IROp::LoadLocal && !irParamCache_.empty()) {
      emitIrFlushSkipped();
    }
    emitIrInst(def, ir, ir.insts[i], i);
  }

  irSkipStore_ = false;
}

void CodeGen::emitIrCall(FuncDef &def, const IRFunction &ir, const IRInst &in,
                  size_t instIdx) {
  (void)def;
  Function *fn = lookupFunctionAsm(semantic_, in.callee);
  if (!fn) {
    throw CompileError("IR call: unknown function " + in.callee);
  }
  vector<ParamType> params;
  if (fn->injectLineArgument) {
    params.push_back(ParamType{BaseType::Int, false, {}});
  }
  for (size_t ai = fn->injectLineArgument ? 1u : 0u; ai < in.args.size(); ++ai) {
    size_t ui = fn->injectLineArgument ? (ai - 1) : ai;
    if (ui < fn->params.size()) {
      params.push_back(fn->params[ui]);
    } else {
      ParamType inferred;
      inferred.base =
          irVFloat_[static_cast<size_t>(in.args[ai])] ? BaseType::Float : BaseType::Int;
      inferred.isArray =
          ai < in.callArgPtr.size() ? static_cast<bool>(in.callArgPtr[ai]) : false;
      inferred.dims = {};
      params.push_back(inferred);
    }
  }

  size_t namedCount = params.size();
  if (fn->variadic) {
    namedCount = fn->params.size() + (fn->injectLineArgument ? 1u : 0u);
  }

  int stackSlots = 0;
  auto locs = computeArgLocations(params, fn->variadic, namedCount, &stackSlots);

  // Optimized path: load args to safe regs (t4-t6, ft), avoid temp stack slots
  int intNeeded = 0;
  for (size_t i = 0; i < params.size(); ++i) {
    if (params[i].isArray || params[i].base == BaseType::Int) ++intNeeded;
  }
  bool useRegPath = optO1_ && !fn->injectLineArgument && intNeeded <= 3;

  if (useRegPath) {
    int stackArea = stackSlots * 8;
    if (stackArea > 0) emitAdjustSp(-alignTo(stackArea, 16));

    int intUsed = 0, floatUsed = 0;
    vector<string> argReg(params.size());

    for (size_t i = 0; i < params.size(); ++i) {
      int av = in.args[i];
      ParamType expected = params[i];
      bool isFloat = !expected.isArray && expected.base == BaseType::Float;

      if (isFloat) {
        if (optional<float> fc = irTraceConstF(ir, av, instIdx)) {
          emitFloatConst(*fc);
        } else {
          emitIrLoadVreg(av, true);
        }
        string reg = "ft" + to_string(floatUsed);
        emit("\tfmv.s\t" + reg + ", fa0");
        argReg[i] = reg;
        ++floatUsed;
      } else {
        if (optional<int32_t> ic = irTraceConstI(ir, av, instIdx)) {
          emit("\tli\ta0, " + to_string(*ic));
        } else {
          emitIrLoadVreg(av, false);
        }
        string reg = "t" + to_string(4 + intUsed);
        emit("\tmv\t" + reg + ", a0");
        argReg[i] = reg;
        ++intUsed;
      }
    }

    for (size_t i = 0; i < params.size(); ++i) {
      const ArgLoc &loc = locs[i];
      bool isFloat = !params[i].isArray && params[i].base == BaseType::Float;
      if (loc.kind == ArgLoc::FloatReg) {
        emit("\tfmv.s\tfa" + to_string(loc.index) + ", " + argReg[i]);
      } else if (loc.kind == ArgLoc::IntReg) {
        emit("\tmv\ta" + to_string(loc.index) + ", " + argReg[i]);
      } else {
        int dst = loc.index * 8;
        if (isFloat) {
          emitStoreMem("fsw", argReg[i], "sp", dst);
        } else {
          emitStoreMem("sd", argReg[i], "sp", dst);
        }
      }
    }

    emit("\tcall\t" + fn->asmName);
    if (stackArea > 0) emitAdjustSp(stackArea);
    return;
  }

  // Fallback: stack-based
  int tempSlots = static_cast<int>(params.size());
  int stackBytes = stackSlots * 8;
  int tempBase = stackBytes;
  int area = alignTo(stackBytes + tempSlots * 8, 16);
  if (area > 0) {
    emitAdjustSp(-area);
  }

  int paramIndex = 0;
  if (fn->injectLineArgument) {
    optional<int32_t> lineImm = irTraceConstI(ir, in.args[0], instIdx);
    if (lineImm) {
      emit("\tli\ta0, " + to_string(*lineImm));
    } else {
      emitIrLoadVreg(in.args[0], false);
    }
    emitStoreMem("sd", "a0", "sp", tempBase);
    ++paramIndex;
  }
  for (; static_cast<size_t>(paramIndex) < in.args.size(); ++paramIndex) {
    int av = in.args[static_cast<size_t>(paramIndex)];
    ParamType expected = params[static_cast<size_t>(paramIndex)];
    const bool vaArg =
        fn->variadic && static_cast<size_t>(paramIndex) >= namedCount;
    Type target =
        expected.isArray ? Type::pointer(expected.base, expected.dims) : Type::scalar(expected.base);
    int off = tempBase + paramIndex * 8;
    if (target.isPointer) {
      optional<int32_t> ic = irTraceConstI(ir, av, instIdx);
      if (ic) {
        emit("\tli\ta0, " + to_string(*ic));
      } else {
        emitIrLoadVreg(av, false);
      }
      emitStoreMem("sd", "a0", "sp", off);
    } else if (expected.base == BaseType::Float && vaArg) {
      if (optional<float> fc = irTraceConstF(ir, av, instIdx)) {
        emitFloatConst(*fc);
      } else {
        emitIrLoadVreg(av, true);
      }
      emit("\tfcvt.d.s\tfa1, fa0");
      emit("\tfmv.x.d\ta0, fa1");
      emitStoreMem("sd", "a0", "sp", off);
    } else if (expected.base == BaseType::Float) {
      if (optional<float> fc = irTraceConstF(ir, av, instIdx)) {
        emitFloatConst(*fc);
      } else {
        emitIrLoadVreg(av, true);
      }
      emitStoreMem("fsw", "fa0", "sp", off);
    } else if (vaArg) {
      optional<int32_t> ic = irTraceConstI(ir, av, instIdx);
      if (ic) {
        emit("\tli\ta0, " + to_string(*ic));
      } else {
        emitIrLoadVreg(av, false);
      }
      emit("\tsext.w\ta0, a0");
      emitStoreMem("sd", "a0", "sp", off);
    } else {
      optional<int32_t> ic = irTraceConstI(ir, av, instIdx);
      if (ic) {
        emit("\tli\ta0, " + to_string(*ic));
      } else {
        emitIrLoadVreg(av, false);
      }
      emitStoreMem("sd", "a0", "sp", off);
    }
  }

  for (size_t i = 0; i < params.size(); ++i) {
    const ParamType &p = params[i];
    const ArgLoc &loc = locs[i];
    int off = tempBase + static_cast<int>(i) * 8;
    bool isFloat = !p.isArray && p.base == BaseType::Float;
    const bool vaArg = fn->variadic && i >= namedCount;
    if (loc.kind == ArgLoc::FloatReg) {
      emitLoadMem("flw", "fa" + to_string(loc.index), "sp", off);
    } else if (loc.kind == ArgLoc::IntReg) {
      if (isFloat && !vaArg) {
        emitLoadMem("lw", "a" + to_string(loc.index), "sp", off);
      } else {
        emitLoadMem("ld", "a" + to_string(loc.index), "sp", off);
      }
    } else {
      int dst = loc.index * 8;
      if (isFloat) {
        if (vaArg) {
          emitLoadMem("ld", "t0", "sp", off);
          emitStoreMem("sd", "t0", "sp", dst);
        } else {
          emitLoadMem("lw", "t0", "sp", off);
          emitStoreMem("sw", "t0", "sp", dst);
        }
      } else {
        emitLoadMem("ld", "t0", "sp", off);
        emitStoreMem("sd", "t0", "sp", dst);
      }
    }
  }

  emit("\tcall\t" + fn->asmName);
  if (area > 0) {
    emitAdjustSp(area);
  }
  if (in.dst >= 0) {
    if (in.isFloat) {
      irVFloat_[static_cast<size_t>(in.dst)] = 1;
      emitIrStoreVreg(in.dst, true);
    } else {
      irVFloat_[static_cast<size_t>(in.dst)] = 0;
      emitIrStoreVreg(in.dst, false);
    }
  }
}

void CodeGen::emitIrInst(FuncDef &def, const IRFunction &ir, const IRInst &in,
                  size_t instIdx) {
  auto markInt = [&](int d) {
    if (d >= 0 && static_cast<size_t>(d) < irVFloat_.size()) {
      irVFloat_[static_cast<size_t>(d)] = 0;
    }
  };
  auto markFl = [&](int d) {
    if (d >= 0 && static_cast<size_t>(d) < irVFloat_.size()) {
      irVFloat_[static_cast<size_t>(d)] = 1;
    }
  };

  switch (in.op) {
  case IROp::Nop:
    return;
  case IROp::ConstI:
    emit("\tli\ta0, " + to_string(in.immI));
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::ConstF:
    emitFloatConst(in.immF);
    emitIrStoreVreg(in.dst, true);
    markFl(in.dst);
    return;
  case IROp::Copy:
    emitIrLoadVreg(in.u, irVFloat_[static_cast<size_t>(in.u)] != 0);
    emitIrStoreVreg(in.dst, irVFloat_[static_cast<size_t>(in.u)] != 0);
    if (in.dst >= 0 && in.u >= 0 && static_cast<size_t>(in.dst) < irVFloat_.size()) {
      irVFloat_[static_cast<size_t>(in.dst)] = irVFloat_[static_cast<size_t>(in.u)];
    }
    return;
  case IROp::LeaStr: {
    string label;
    for (const auto &lit : stringLiterals_) {
      if (lit.second == in.ext) {
        label = lit.first;
        break;
      }
    }
    if (label.empty()) {
      label = newLabel("str");
      stringLiterals_.push_back({label, in.ext});
    }
    emit("\tlla\ta0, " + label);
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  }
  case IROp::LeaGlobal:
    emit("\tlla\ta0, " + in.sym->label);
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::LeaLocal:
    emitAddOffset("a0", "s0", in.sym->offset);
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::LoadParamAddr:
    emitLoadMem("ld", "a0", "s0", in.sym->offset);
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::LoadGlobal:
    emit("\tlla\tt0, " + in.sym->label);
    if (in.isFloat) {
      emit("\tflw\tfa0, 0(t0)");
      emitIrStoreVreg(in.dst, true);
      markFl(in.dst);
    } else {
      emit("\tlw\ta0, 0(t0)");
      emitIrStoreVreg(in.dst, false);
      markInt(in.dst);
    }
    return;
  case IROp::LoadLocal: {
    auto pit = irParamCache_.find(in.sym);
    if (pit != irParamCache_.end()) {
      string &reg = pit->second;
      if (reg[0] == 'f') {
        emit("\tfmv.s\tfa0, " + reg);
        emitIrStoreVreg(in.dst, true);
        markFl(in.dst);
      } else {
        emit("\tmv\ta0, " + reg);
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
      }
    } else {
      emitAddOffset("t0", "s0", in.sym->offset);
      if (in.isFloat) {
        emit("\tflw\tfa0, 0(t0)");
        emitIrStoreVreg(in.dst, true);
        markFl(in.dst);
      } else {
        emit("\tlw\ta0, 0(t0)");
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
      }
    }
    return;
  }
  case IROp::LoadMem:
    emitIrLoadVregTo(in.u, "t2");
    if (in.isFloat) {
      emit("\tflw\tfa0, 0(t2)");
      emitIrStoreVreg(in.dst, true);
      markFl(in.dst);
    } else {
      emit("\tlw\ta0, 0(t2)");
      emitIrStoreVreg(in.dst, false);
      markInt(in.dst);
    }
    return;
  case IROp::StoreMem:
    emitIrLoadVregTo(in.u, "t1");
    emitIrLoadVreg(in.v, in.isFloat);
    if (in.isFloat) {
      emit("\tfsw\tfa0, 0(t1)");
    } else {
      emit("\tsw\ta0, 0(t1)");
    }
    return;
  case IROp::StoreGlobal:
    emit("\tlla\tt1, " + in.sym->label);
    emitIrLoadVreg(in.u, in.isFloat);
    if (in.isFloat) {
      emit("\tfsw\tfa0, 0(t1)");
    } else {
      emit("\tsw\ta0, 0(t1)");
    }
    return;
  case IROp::StoreLocal:
    emitAddOffset("t1", "s0", in.sym->offset);
    emitIrLoadVreg(in.u, in.isFloat);
    if (in.isFloat) {
      emit("\tfsw\tfa0, 0(t1)");
    } else {
      emit("\tsw\ta0, 0(t1)");
    }
    return;
  case IROp::Add: {
    if (auto kv = irTraceConstI(ir, in.v, instIdx)) {
      int32_t k = *kv;
      if (fitsImm12(static_cast<int>(k))) {
        emitIrLoadVreg(in.u, false);
        emit("\taddiw\ta0, a0, " + to_string(static_cast<int>(k)));
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return;
      }
    }
    if (auto ku = irTraceConstI(ir, in.u, instIdx)) {
      int32_t k = *ku;
      if (fitsImm12(static_cast<int>(k))) {
        emitIrLoadVreg(in.v, false);
        emit("\taddiw\ta0, a0, " + to_string(static_cast<int>(k)));
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return;
      }
    }
    emitIrLoadVregTo(in.u, "t2");
    emitIrLoadVreg(in.v, false);
    emit("\taddw\ta0, t2, a0");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  }
  case IROp::Sub: {
    if (auto kv = irTraceConstI(ir, in.v, instIdx)) {
      int64_t nk = -static_cast<int64_t>(*kv);
      if (nk >= -2048 && nk <= 2047) {
        emitIrLoadVreg(in.u, false);
        emit("\taddiw\ta0, a0, " + to_string(static_cast<int>(nk)));
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return;
      }
    }
    emitIrLoadVregTo(in.u, "t2");
    emitIrLoadVreg(in.v, false);
    emit("\tsubw\ta0, t2, a0");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  }
  case IROp::Mul: {
    auto tryMulConst = [&](int constV, int otherV) -> bool {
      optional<int32_t> ck = irTraceConstI(ir, constV, instIdx);
      if (!ck) {
        return false;
      }
      int32_t k = *ck;
      if (k == 0) {
        emit("\tli\ta0, 0");
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return true;
      }
      if (k == 1) {
        emitIrLoadVreg(otherV, false);
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return true;
      }
      if (k == -1) {
        emitIrLoadVreg(otherV, false);
        emit("\tnegw\ta0, a0");
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return true;
      }
      int lg = intLog2Positive32(k > 0 ? k : 0);
      if (lg >= 0) {
        emitIrLoadVreg(otherV, false);
        emit("\tslliw\ta0, a0, " + to_string(lg));
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return true;
      }
      return false;
    };
    if (tryMulConst(in.v, in.u)) {
      return;
    }
    if (tryMulConst(in.u, in.v)) {
      return;
    }
    emitIrLoadVregTo(in.u, "t2");
    emitIrLoadVreg(in.v, false);
    emit("\tmulw\ta0, t2, a0");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  }
  case IROp::Sll:
    emitIrLoadVreg(in.u, false);
    emit("\tslliw\ta0, a0, " + to_string(in.immI));
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::Div: {
    if (auto kv = irTraceConstI(ir, in.v, instIdx)) {
      int32_t k = *kv;
      int lg = intLog2Positive32(k);
      if (k > 0 && lg >= 0) {
        emitIrLoadVreg(in.u, false);
        emit("\tli\ta2, " + to_string(static_cast<int>(k - 1)));
        emit("\tsraiw\tt1, a0, 31");
        emit("\tand\tt1, t1, a2");
        emit("\taddw\tt3, a0, t1");
        emit("\tsraiw\ta0, t3, " + to_string(lg));
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return;
      }
      emitIrLoadVreg(in.u, false);
      emit("\tmv\tt2, a0");
      if (emitSDivByConst(*this, k)) {
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return;
      }
    }
    emitIrLoadVregTo(in.u, "t2");
    emitIrLoadVreg(in.v, false);
    emit("\tdivw\ta0, t2, a0");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  }
  case IROp::Rem: {
    if (auto kv = irTraceConstI(ir, in.v, instIdx)) {
      int32_t k = *kv;
      int lg = intLog2Positive32(k);
      if (lg >= 1) {
        emitIrLoadVreg(in.u, false);
        uint32_t mask = static_cast<uint32_t>(k) - 1;
        emit("\tsraiw\tt1, a0, 31");
        emit("\tsrliw\tt1, t1, " + to_string(32 - lg));
        emit("\taddw\ta0, a0, t1");
        emit("\tandi\ta0, a0, " + to_string(static_cast<int>(mask)));
        emit("\tsubw\ta0, a0, t1");
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return;
      }
      emitIrLoadVreg(in.u, false);
      if (emitSRemByConst(*this, k)) {
        emitIrStoreVreg(in.dst, false);
        markInt(in.dst);
        return;
      }
      emit("\tmv\tt2, a0");
      emitIrLoadVreg(in.v, false);
      emit("\tremw\ta0, t2, a0");
      emitIrStoreVreg(in.dst, false);
      markInt(in.dst);
      return;
    }
    emitIrLoadVregTo(in.u, "t2");
    emitIrLoadVreg(in.v, false);
    emit("\tremw\ta0, t2, a0");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  }
  case IROp::Neg:
    emitIrLoadVreg(in.u, false);
    emit("\tnegw\ta0, a0");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::Slt:
    emitIrLoadVregTo(in.u, "t2");
    emitIrLoadVreg(in.v, false);
    emit("\tslt\ta0, t2, a0");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::Seqz:
    emitIrLoadVreg(in.u, false);
    emit("\tseqz\ta0, a0");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::Snez:
    emitIrLoadVreg(in.u, false);
    emit("\tsnez\ta0, a0");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::ICvtF:
    emitIrLoadVreg(in.u, false);
    emit("\tfcvt.s.w\tfa0, a0");
    emitIrStoreVreg(in.dst, true);
    markFl(in.dst);
    return;
  case IROp::FCvtI:
    emitIrLoadVreg(in.u, true);
    emit("\tfcvt.w.s\ta0, fa0, rtz");
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::FAdd:
    emitIrLoadVreg(in.u, true);
    emit("\tfmv.s\tft0, fa0");
    emitIrLoadVreg(in.v, true);
    emit("\tfadd.s\tfa0, ft0, fa0");
    emitIrStoreVreg(in.dst, true);
    markFl(in.dst);
    return;
  case IROp::FSub:
    emitIrLoadVreg(in.u, true);
    emit("\tfmv.s\tft0, fa0");
    emitIrLoadVreg(in.v, true);
    emit("\tfsub.s\tfa0, ft0, fa0");
    emitIrStoreVreg(in.dst, true);
    markFl(in.dst);
    return;
  case IROp::FMul:
    emitIrLoadVreg(in.u, true);
    emit("\tfmv.s\tft0, fa0");
    emitIrLoadVreg(in.v, true);
    emit("\tfmul.s\tfa0, ft0, fa0");
    emitIrStoreVreg(in.dst, true);
    markFl(in.dst);
    return;
  case IROp::FDiv:
    emitIrLoadVreg(in.u, true);
    emit("\tfmv.s\tft0, fa0");
    emitIrLoadVreg(in.v, true);
    emit("\tfdiv.s\tfa0, ft0, fa0");
    emitIrStoreVreg(in.dst, true);
    markFl(in.dst);
    return;
  case IROp::FNeg:
    emitIrLoadVreg(in.u, true);
    emit("\tfneg.s\tfa0, fa0");
    emitIrStoreVreg(in.dst, true);
    markFl(in.dst);
    return;
  case IROp::FCmp:
    emitIrLoadVreg(in.u, true);
    emit("\tfmv.s\tft0, fa0");
    emitIrLoadVreg(in.v, true);
    switch (in.immI) {
    case FCMP_EQ:
      emit("\tfeq.s\ta0, ft0, fa0");
      break;
    case FCMP_NE:
      emit("\tfeq.s\ta0, ft0, fa0");
      emit("\tseqz\ta0, a0");
      break;
    case FCMP_LT:
      emit("\tflt.s\ta0, ft0, fa0");
      break;
    case FCMP_GT:
      emit("\tflt.s\ta0, fa0, ft0");
      break;
    case FCMP_LE:
      emit("\tfle.s\ta0, ft0, fa0");
      break;
    case FCMP_GE:
      emit("\tfle.s\ta0, fa0, ft0");
      break;
    default:
      emit("\tli\ta0, 0");
      break;
    }
    emitIrStoreVreg(in.dst, false);
    markInt(in.dst);
    return;
  case IROp::Call:
    emitIrCall(def, ir, in, instIdx);
    return;
  case IROp::Ret:
    if (def.ret == BaseType::Void) {
      emit("\tj\t" + currentFunction_->returnLabel);
      return;
    }
    if (in.u < 0) {
      if (def.ret == BaseType::Int) {
        emit("\tli\ta0, 0");
      } else {
        emit("\tfmv.w.x\tfa0, zero");
      }
    } else {
      emitIrLoadVreg(in.u, irVFloat_[static_cast<size_t>(in.u)] != 0);
    }
    emit("\tj\t" + currentFunction_->returnLabel);
    return;
  default:
    return;
  }
}

void CodeGen::emitStoreParams(FuncDef &def) {
    vector<ParamType> types;
    for (const Param &p : def.params) {
      types.push_back(ParamType{p.base, p.isArray, p.dims});
    }
    auto locs = computeArgLocations(types, false, types.size(), nullptr);
    for (size_t i = 0; i < def.params.size(); ++i) {
      const Param &p = def.params[i];
      const ArgLoc &loc = locs[i];
      int off = p.symbol->offset;
      if (p.isArray) {
        if (loc.kind == ArgLoc::IntReg) {
          emitStoreMem("sd", "a" + to_string(loc.index), "s0", off);
        } else {
          emitLoadMem("ld", "t0", "s0", loc.index * 8);
          emitStoreMem("sd", "t0", "s0", off);
        }
      } else if (p.base == BaseType::Float) {
        if (loc.kind == ArgLoc::FloatReg) {
          emitStoreMem("fsw", "fa" + to_string(loc.index), "s0", off);
        } else if (loc.kind == ArgLoc::IntReg) {
          emitStoreMem("sw", "a" + to_string(loc.index), "s0", off);
        } else {
          emitLoadMem("lw", "t0", "s0", loc.index * 8);
          emitStoreMem("sw", "t0", "s0", off);
        }
      } else {
        if (loc.kind == ArgLoc::IntReg) {
          emitStoreMem("sw", "a" + to_string(loc.index), "s0", off);
        } else {
          emitLoadMem("lw", "t0", "s0", loc.index * 8);
          emitStoreMem("sw", "t0", "s0", off);
        }
      }
    }
  }

void CodeGen::emitBlock(BlockStmt &block, bool) {
    for (auto &stmt : block.items) {
      emitStmt(stmt.get());
    }
  }

void CodeGen::emitStmt(Stmt *stmt) {
    switch (stmt->kind) {
    case StmtKind::Decl:
      emitDecl(*static_cast<DeclStmt *>(stmt));
      break;
    case StmtKind::Block:
      emitBlock(*static_cast<BlockStmt *>(stmt), true);
      break;
    case StmtKind::Assign:
      emitAssign(*static_cast<AssignStmt *>(stmt));
      break;
    case StmtKind::Expr: {
      ExprStmt *s = static_cast<ExprStmt *>(stmt);
      if (s->expr) {
        emitExpr(s->expr.get());
      }
      break;
    }
    case StmtKind::If:
      emitIf(*static_cast<IfStmt *>(stmt));
      break;
    case StmtKind::While:
      emitWhile(*static_cast<WhileStmt *>(stmt));
      break;
    case StmtKind::Break:
      if (breakLabels_.empty()) {
        throw CompileError("break outside loop");
      }
      emit("\tj\t" + breakLabels_.back());
      break;
    case StmtKind::Continue:
      if (continueLabels_.empty()) {
        throw CompileError("continue outside loop");
      }
      emit("\tj\t" + continueLabels_.back());
      break;
    case StmtKind::Return:
      emitReturn(*static_cast<ReturnStmt *>(stmt));
      break;
    }
  }

void CodeGen::emitDecl(DeclStmt &decl) {
    for (VarDef &def : decl.defs) {
      Symbol *sym = def.symbol;
      if (!sym->needsStorage) {
        continue;
      }
      if (!sym->isArray) {
        if (def.init && !def.init->isList) {
          emitExpr(def.init->expr.get());
          emitConvert(def.init->expr->type, Type::scalar(sym->base));
          emitStoreToAddress(sym->base, [this, sym]() {
            emitAddOffset("a1", "s0", sym->offset);
          });
        } else if (!def.init) {
          // SysY 评测通常假定未赋初值的局部标量为 0（与官方运行时一致）
          if (sym->base == BaseType::Float) {
            emit("\tfmv.w.x\tfa0, zero");
          } else {
            emit("\tli\ta0, 0");
          }
          emitStoreToAddress(sym->base, [this, sym]() {
            emitAddOffset("a1", "s0", sym->offset);
          });
        }
        continue;
      }
      int total = product(sym->dims);
      if (!def.init) {
        string zeroLoop = newLabel("zero_array");
        string zeroEnd = newLabel("zero_array_end");
        emitAddOffset("t3", "s0", sym->offset);
        emit("\tli\tt1, " + to_string(total * 4));
        emit(zeroLoop + ":");
        emit("\tbeqz\tt1, " + zeroEnd);
        emit("\tsw\tzero, 0(t3)");
        emit("\taddi\tt3, t3, 4");
        emit("\taddi\tt1, t1, -4");
        emit("\tj\t" + zeroLoop);
        emit(zeroEnd + ":");
        continue;
      }
      vector<InitVal *> flat(total, nullptr);
      flattenRuntimeInit(def.init.get(), sym->dims, 0, 0, flat);
      int initCount = 0;
      for (int i = 0; i < total; ++i) {
        if (flat[i]) ++initCount;
      }
      if (initCount < total) {
        string zeroLoop = newLabel("zero_array");
        string zeroEnd = newLabel("zero_array_end");
        emitAddOffset("t3", "s0", sym->offset);
        emit("\tli\tt1, " + to_string(total * 4));
        emit(zeroLoop + ":");
        emit("\tbeqz\tt1, " + zeroEnd);
        emit("\tsw\tzero, 0(t3)");
        emit("\taddi\tt3, t3, 4");
        emit("\taddi\tt1, t1, -4");
        emit("\tj\t" + zeroLoop);
        emit(zeroEnd + ":");
      }
      for (int i = 0; i < total; ++i) {
        if (!flat[i]) {
          continue;
        }
        emitExpr(flat[i]->expr.get());
        emitConvert(flat[i]->expr->type, Type::scalar(sym->base));
        emitStoreToAddress(sym->base, [this, sym, i]() {
          emitAddOffset("a1", "s0", sym->offset + i * 4);
        });
      }
    }
  }

int CodeGen::flattenRuntimeInit(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<InitVal *> &flat) {
    if (!init->isList) {
      if (start < static_cast<int>(flat.size())) {
        flat[start] = init;
      }
      return start + 1;
    }
    int subSize = depth >= dims.size() ? 1 : product(dims, depth);
    if (init->list.empty()) {
      return start + subSize;
    }
    int idx = start;
    for (auto &child : init->list) {
      if (child->isList) {
        size_t childDepth = chooseRuntimeInitChildDepth(dims, depth, idx);
        idx = flattenRuntimeInit(child.get(), dims, childDepth, idx, flat);
      } else {
        idx = flattenRuntimeInit(child.get(), dims, dims.size(), idx, flat);
      }
    }
    return start + subSize;
  }

size_t CodeGen::chooseRuntimeInitChildDepth(const vector<int> &dims, size_t depth,
                                     int flatIndex) {
    size_t childDepth = min(depth + 1, dims.size());
    while (childDepth < dims.size()) {
      int childSize = product(dims, childDepth);
      if (childSize == 0 || flatIndex % childSize == 0) {
        break;
      }
      ++childDepth;
    }
    return childDepth;
  }

template <class AddressEmitter>
void CodeGen::emitStoreToAddress(BaseType base, AddressEmitter emitAddressToA1) {
    if (optO1_ && base == BaseType::Int) {
      emit("\tmv\tt4, a0");
      emitAddressToA1();
      emit("\tsw\tt4, 0(a1)");
      return;
    }
    if (base == BaseType::Float) {
      emitPushFloat("fa0");
      emitAddressToA1();
      emitPopFloat("fa0");
      emit("\tfsw\tfa0, 0(a1)");
    } else {
      emitPushInt("a0");
      emitAddressToA1();
      emitPopInt("a0");
      emit("\tsw\ta0, 0(a1)");
    }
  }

void CodeGen::emitAssign(AssignStmt &stmt) {
    Symbol *sym = stmt.lhs->symbol;
    bool lhsFloat = sym->base == BaseType::Float;

    // 安全快路径：地址直接算到 t4（emitExpr 不使用 t4，float RHS 也不用）
    if (optO1_) {
      // Simple scalar: address to t4, RHS w/o arrays keeps t4 safe
      if (!sym->isGlobal && !sym->isParamArray && stmt.lhs->indices.empty() &&
          fitsImm12(sym->offset)) {
        emit("\taddi\tt4, s0, " + to_string(sym->offset));
        auto exprHasArray = [](Expr *e, auto &self) -> bool {
          if (!e) return false;
          if (e->kind == ExprKind::LVal && !static_cast<LValExpr *>(e)->indices.empty())
            return true;
          if (e->kind == ExprKind::Binary) {
            auto *b = static_cast<BinaryExpr *>(e);
            return self(b->lhs.get(), self) || self(b->rhs.get(), self);
          }
          if (e->kind == ExprKind::Unary)
            return self(static_cast<UnaryExpr *>(e)->expr.get(), self);
          if (e->kind == ExprKind::Call) {
            for (auto &a : static_cast<CallExpr *>(e)->args)
              if (self(a.get(), self)) return true;
          }
          return false;
        };
        bool rhsHasArray = exprHasArray(stmt.rhs.get(), exprHasArray);
        if (rhsHasArray) emitPushInt("t4");
        emitExpr(stmt.rhs.get());
        emitConvert(stmt.rhs->type, Type::scalar(sym->base));
        if (rhsHasArray) emitPopInt("t4");
        if (lhsFloat) emit("\tfsw\tfa0, 0(t4)");
        else          emit("\tsw\ta0, 0(t4)");
        return;
      }
      emitLValAddress(stmt.lhs.get());
      emit("\tmv\tt4, a0");
      emitExpr(stmt.rhs.get());
      emitConvert(stmt.rhs->type, Type::scalar(sym->base));
      if (lhsFloat) emit("\tfsw\tfa0, 0(t4)");
      else          emit("\tsw\ta0, 0(t4)");
      return;
    }

    // -O0 fallback
    emitLValAddress(stmt.lhs.get());
    emitPushInt("a0");
    emitExpr(stmt.rhs.get());
    emitConvert(stmt.rhs->type, Type::scalar(sym->base));
    if (lhsFloat) {
      emitPopInt("a1");
      emit("\tfsw\tfa0, 0(a1)");
    } else {
      emitPushInt("a0");
      emitPopInt("t2");
      emitPopInt("a1");
      emit("\tsw\tt2, 0(a1)");
    }
  }

void CodeGen::emitIf(IfStmt &stmt) {
    string thenLabel = newLabel("if_then");
    string elseLabel = newLabel("if_else");
    string endLabel = newLabel("if_end");
    emitCond(stmt.cond.get(), thenLabel, stmt.elseStmt ? elseLabel : endLabel);
    emit(thenLabel + ":");
    emitStmt(stmt.thenStmt.get());
    emit("\tj\t" + endLabel);
    if (stmt.elseStmt) {
      emit(elseLabel + ":");
      emitStmt(stmt.elseStmt.get());
    }
    emit(endLabel + ":");
  }

void CodeGen::emitWhile(WhileStmt &stmt) {
    string condLabel = newLabel("while_cond");
    string bodyLabel = newLabel("while_body");
    string endLabel = newLabel("while_end");
    continueLabels_.push_back(condLabel);
    breakLabels_.push_back(endLabel);
    emit(condLabel + ":");
    emitCond(stmt.cond.get(), bodyLabel, endLabel);
    emit(bodyLabel + ":");
    emitStmt(stmt.body.get());
    emit("\tj\t" + condLabel);
    emit(endLabel + ":");
    breakLabels_.pop_back();
    continueLabels_.pop_back();
  }

void CodeGen::emitReturn(ReturnStmt &stmt) {
    if (stmt.expr) {
      emitExpr(stmt.expr.get());
      emitConvert(stmt.expr->type, Type::scalar(currentFunction_->ret));
    }
    emit("\tj\t" + currentFunction_->returnLabel);
  }

void CodeGen::emitExpr(Expr *expr) {
    if (expr->isConst && expr->kind != ExprKind::LVal) {
      emitConst(expr->constVal);
      return;
    }
    switch (expr->kind) {
    case ExprKind::Number:
      emitConst(expr->constVal);
      break;
    case ExprKind::String:
      emitStringExpr(static_cast<StringExpr *>(expr));
      break;
    case ExprKind::LVal:
      emitLValValue(static_cast<LValExpr *>(expr));
      break;
    case ExprKind::Call:
      emitCall(static_cast<CallExpr *>(expr));
      break;
    case ExprKind::Unary:
      emitUnary(static_cast<UnaryExpr *>(expr));
      break;
    case ExprKind::Binary:
      emitBinary(static_cast<BinaryExpr *>(expr));
      break;
    }
  }

void CodeGen::emitConst(ConstValue v) {
    if (v.type == BaseType::Float) {
      emitFloatConst(v.f);
    } else {
      emit("\tli\ta0, " + to_string(v.i));
    }
  }

void CodeGen::emitFloatConst(float value) {
    uint32_t bits = floatBits(value);
    if (bits == 0) {
      emit("\tfmv.w.x\tfa0, zero");
      return;
    }
    for (const auto &lit : floatLiterals_) {
      if (floatBits(lit.second) == bits) {
        emit("\tlla\tt0, " + lit.first);
        emit("\tflw\tfa0, 0(t0)");
        return;
      }
    }
    string label = newLabel("float");
    floatLiterals_.push_back({label, value});
    emit("\tlla\tt0, " + label);
    emit("\tflw\tfa0, 0(t0)");
  }

void CodeGen::emitStringExpr(StringExpr *expr) {
    for (const auto &lit : stringLiterals_) {
      if (lit.second == expr->value) {
        expr->label = lit.first;
        emit("\tlla\ta0, " + expr->label);
        return;
      }
    }
    expr->label = newLabel("str");
    stringLiterals_.push_back({expr->label, expr->value});
    emit("\tlla\ta0, " + expr->label);
  }

void CodeGen::emitUnary(UnaryExpr *expr) {
    emitExpr(expr->expr.get());
    if (expr->op == "+") {
      return;
    }
    if (expr->op == "-") {
      if (expr->type.base == BaseType::Float) {
        emit("\tfneg.s\tfa0, fa0");
      } else {
        emit("\tnegw\ta0, a0");
      }
      return;
    }
    if (expr->op == "!") {
      emitBoolFromValue(expr->expr->type);
      emit("\tseqz\ta0, a0");
    }
  }

void CodeGen::emitBinary(BinaryExpr *expr) {
    const string &op = expr->op;
    if (op == "&&" || op == "||") {
      string trueLabel = newLabel("logic_true");
      string falseLabel = newLabel("logic_false");
      string endLabel = newLabel("logic_end");
      emitCond(expr, trueLabel, falseLabel);
      emit(trueLabel + ":");
      emit("\tli\ta0, 1");
      emit("\tj\t" + endLabel);
      emit(falseLabel + ":");
      emit("\tli\ta0, 0");
      emit(endLabel + ":");
      return;
    }

    bool resultFloat = expr->type.base == BaseType::Float;
    bool compare = op == "==" || op == "!=" || op == "<" || op == ">" ||
                   op == "<=" || op == ">=";
    bool useFloat = expr->lhs->type.isFloatScalar() || expr->rhs->type.isFloatScalar();

    if (!useFloat && compare && optO1_ && expr->lhs->type.isIntScalar() &&
        expr->rhs->type.isIntScalar()) {
      if (expr->rhs->isConst && expr->rhs->constVal.type == BaseType::Int &&
          constAsInt(expr->rhs->constVal) == 0) {
        emitExpr(expr->lhs.get());
        if (op == "==") {
          emit("\tseqz\ta0, a0");
        } else if (op == "!=") {
          emit("\tsnez\ta0, a0");
        } else if (op == "<") {
          emit("\tslt\ta0, a0, zero");
        } else if (op == ">") {
          emit("\tslt\ta0, zero, a0");
        } else if (op == "<=") {
          emit("\tslt\tt3, zero, a0");
          emit("\tseqz\ta0, t3");
        } else if (op == ">=") {
          emit("\tslt\tt3, a0, zero");
          emit("\tseqz\ta0, t3");
        }
        return;
      }
      if (expr->lhs->isConst && expr->lhs->constVal.type == BaseType::Int &&
          constAsInt(expr->lhs->constVal) == 0) {
        emitExpr(expr->rhs.get());
        if (op == "==") {
          emit("\tseqz\ta0, a0");
        } else if (op == "!=") {
          emit("\tsnez\ta0, a0");
        } else if (op == "<") {
          emit("\tslt\ta0, zero, a0");
        } else if (op == ">") {
          emit("\tslt\ta0, a0, zero");
        } else if (op == "<=") {
          emit("\tslt\tt3, a0, zero");
          emit("\tseqz\ta0, t3");
        } else if (op == ">=") {
          emit("\tslt\tt3, zero, a0");
          emit("\tseqz\ta0, t3");
        }
        return;
      }
      if (expr->rhs->isConst && expr->rhs->constVal.type == BaseType::Int) {
        int32_t k = constAsInt(expr->rhs->constVal);
        // x < k → slti (single instruction)
        if (op == "<" && fitsImm12(static_cast<int>(k))) {
          emitExpr(expr->lhs.get());
          emit("\tslti\ta0, a0, " + to_string(static_cast<int>(k)));
          return;
        }
        // x <= k → slti with k+1
        if (op == "<=" && fitsImm12(static_cast<int>(k + 1))) {
          emitExpr(expr->lhs.get());
          emit("\tslti\ta0, a0, " + to_string(static_cast<int>(k + 1)));
          return;
        }
        // x >= k → !(x < k)
        if (op == ">=" && fitsImm12(static_cast<int>(k))) {
          emitExpr(expr->lhs.get());
          emit("\tslti\ta0, a0, " + to_string(static_cast<int>(k)));
          emit("\tseqz\ta0, a0");
          return;
        }
        // x > k → !(x <= k) = !(x < k+1)
        if (op == ">" && fitsImm12(static_cast<int>(k + 1))) {
          emitExpr(expr->lhs.get());
          emit("\tslti\ta0, a0, " + to_string(static_cast<int>(k + 1)));
          emit("\tseqz\ta0, a0");
          return;
        }
        if (k != 0 && (op == "==" || op == "!=") &&
            fitsImm12(static_cast<int>(-k))) {
          emitExpr(expr->lhs.get());
          emit("\taddiw\ta0, a0, " + to_string(static_cast<int>(-k)));
          if (op == "==") {
            emit("\tseqz\ta0, a0");
          } else {
            emit("\tsnez\ta0, a0");
          }
          return;
        }
      }
    }

    // Register-based integer arithmetic path (avoids push/pop)
    if (optO1_ && !useFloat && !compare &&
        exprIsLeaf(expr->lhs.get()) && exprIsLeaf(expr->rhs.get())) {
      // Constant RHS: evaluate LHS and apply immediate or loaded constant
      if (expr->rhs->isConst && expr->rhs->constVal.type == BaseType::Int) {
        int32_t k = constAsInt(expr->rhs->constVal);
        if (op == "+") {
          if (k == 0) {
            emitExpr(expr->lhs.get());
            return;
          }
          emitExpr(expr->lhs.get());
          if (fitsImm12(static_cast<int>(k))) {
            emit("\taddiw\ta0, a0, " + to_string(static_cast<int>(k)));
          } else {
            emit("\tli\tt3, " + to_string(static_cast<int>(k)));
            emit("\taddw\ta0, a0, t3");
          }
          return;
        }
        if (op == "-") {
          if (k == 0) {
            emitExpr(expr->lhs.get());
            return;
          }
          emitExpr(expr->lhs.get());
          int neg = -static_cast<int>(k);
          if (fitsImm12(neg)) {
            emit("\taddiw\ta0, a0, " + to_string(neg));
          } else {
            emit("\tli\tt3, " + to_string(static_cast<int>(k)));
            emit("\tsubw\ta0, a0, t3");
          }
          return;
        }
        if (op == "*") {
          if (k == 0) {
            emit("\tli\ta0, 0");
            return;
          }
          emitExpr(expr->lhs.get());
          if (!emitMulByConst(*this, k)) {
            emit("\tli\tt3, " + to_string(static_cast<int>(k)));
            emit("\tmulw\ta0, a0, t3");
          }
          return;
        }
        if (op == "/") {
          if (k == 1) {
            emitExpr(expr->lhs.get());
            return;
          }
          int lg = intLog2Positive32(k);
          if (k > 0 && lg >= 0) {
            emitExpr(expr->lhs.get());
            emit("\tli\ta2, " + to_string(k - 1));
            emit("\tsraiw\tt1, a0, 31");
            emit("\tand\tt1, t1, a2");
            emit("\taddw\tt3, a0, t1");
            emit("\tsraiw\ta0, t3, " + to_string(lg));
            return;
          }
          emitExpr(expr->lhs.get());
          if (optO1_ && emitSDivByConst(*this, k)) {
            return;
          }
          emit("\tmv\tt2, a0");
          emit("\tli\ta0, " + to_string(static_cast<int>(k)));
          emit("\tdivw\ta0, t2, a0");
          return;
        }
        if (op == "%") {
          if (k == 1) {
            emit("\tli\ta0, 0");
            return;
          }
          int lg = intLog2Positive32(k);
          if (lg >= 1) {
            emitExpr(expr->lhs.get());
            uint32_t mask = static_cast<uint32_t>(k) - 1;
            emit("\tsraiw\tt1, a0, 31");
            emit("\tsrliw\tt1, t1, " + to_string(32 - lg));
            emit("\taddw\ta0, a0, t1");
            emit("\tandi\ta0, a0, " + to_string(static_cast<int>(mask)));
            emit("\tsubw\ta0, a0, t1");
            return;
          }
          emitExpr(expr->lhs.get());
          if (optO1_ && emitSRemByConst(*this, k)) {
            return;
          }
          emit("\tmv\tt2, a0");
          emit("\tli\ta0, " + to_string(static_cast<int>(k)));
          emit("\tremw\ta0, t2, a0");
          return;
        }
      }
      // Constant LHS (commutative ops: +, *)
      if (expr->lhs->isConst && expr->lhs->constVal.type == BaseType::Int) {
        int32_t k = constAsInt(expr->lhs->constVal);
        if (op == "+") {
          if (k == 0) {
            emitExpr(expr->rhs.get());
            return;
          }
          emitExpr(expr->rhs.get());
          if (fitsImm12(static_cast<int>(k))) {
            emit("\taddiw\ta0, a0, " + to_string(static_cast<int>(k)));
          } else {
            emit("\tli\tt3, " + to_string(static_cast<int>(k)));
            emit("\taddw\ta0, a0, t3");
          }
          return;
        }
        if (op == "*") {
          if (k == 0) {
            emit("\tli\ta0, 0");
            return;
          }
          emitExpr(expr->rhs.get());
          if (!emitMulByConst(*this, k)) {
            emit("\tli\tt3, " + to_string(static_cast<int>(k)));
            emit("\tmulw\ta0, a0, t3");
          }
          return;
        }
      }
      // General case: evaluate LHS to t0, RHS to a0, combine
      emitExpr(expr->lhs.get());
      emit("\tmv\tt0, a0");
      emitExpr(expr->rhs.get());
      if (op == "+") {
        emit("\taddw\ta0, t0, a0");
      } else if (op == "-") {
        emit("\tsubw\ta0, t0, a0");
      } else if (op == "*") {
        emit("\tmulw\ta0, t0, a0");
      } else if (op == "/") {
        emit("\tdivw\ta0, t0, a0");
      } else if (op == "%") {
        emit("\tremw\ta0, t0, a0");
      }
      return;
    }

    // T4-based path: save one side to t4 (preserved across sub-expr eval), avoid push/pop
    // t4 is never used by any emitExpr path, so it survives sub-expression evaluation.
    if (optO1_ && !useFloat && !compare) {
      // RHS leaf: evaluate LHS first, save to t4, evaluate leaf RHS, combine
      if (exprIsLeaf(expr->rhs.get())) {
        emitExpr(expr->lhs.get());
        emit("\tmv\tt4, a0");
        emitExpr(expr->rhs.get());
        if (op == "+") {
          emit("\taddw\ta0, t4, a0");
        } else if (op == "-") {
          emit("\tsubw\ta0, t4, a0");
        } else if (op == "*") {
          emit("\tmulw\ta0, t4, a0");
        } else if (op == "/") {
          emit("\tdivw\ta0, t4, a0");
        } else if (op == "%") {
          emit("\tremw\ta0, t4, a0");
        }
        return;
      }
      // LHS leaf: evaluate RHS first, save to t4, evaluate leaf LHS, combine
      if (exprIsLeaf(expr->lhs.get())) {
        emitExpr(expr->rhs.get());
        emit("\tmv\tt4, a0");
        emitExpr(expr->lhs.get());
        if (op == "+") {
          emit("\taddw\ta0, a0, t4");
        } else if (op == "-") {
          emit("\tsubw\ta0, a0, t4");
        } else if (op == "*") {
          emit("\tmulw\ta0, a0, t4");
        } else if (op == "/") {
          emit("\tdivw\ta0, a0, t4");
        } else if (op == "%") {
          emit("\tremw\ta0, a0, t4");
        }
        return;
      }
    }
    // T4-based path for integer comparisons with one leaf side
    if (optO1_ && !useFloat && compare) {
      if (exprIsLeaf(expr->rhs.get())) {
        emitExpr(expr->lhs.get());
        emit("\tmv\tt4, a0");
        emitExpr(expr->rhs.get());
        if (op == "==") {
          emit("\tsubw\ta0, t4, a0");
          emit("\tseqz\ta0, a0");
        } else if (op == "!=") {
          emit("\tsubw\ta0, t4, a0");
          emit("\tsnez\ta0, a0");
        } else if (op == "<") {
          emit("\tslt\ta0, t4, a0");
        } else if (op == ">") {
          emit("\tslt\ta0, a0, t4");
        } else if (op == "<=") {
          emit("\tslt\ta0, a0, t4");
          emit("\tseqz\ta0, a0");
        } else if (op == ">=") {
          emit("\tslt\ta0, t4, a0");
          emit("\tseqz\ta0, a0");
        }
        return;
      }
      if (exprIsLeaf(expr->lhs.get())) {
        emitExpr(expr->rhs.get());
        emit("\tmv\tt4, a0");
        emitExpr(expr->lhs.get());
        if (op == "==") {
          emit("\tsubw\ta0, a0, t4");
          emit("\tseqz\ta0, a0");
        } else if (op == "!=") {
          emit("\tsubw\ta0, a0, t4");
          emit("\tsnez\ta0, a0");
        } else if (op == "<") {
          emit("\tslt\ta0, a0, t4");
        } else if (op == ">") {
          emit("\tslt\ta0, t4, a0");
        } else if (op == "<=") {
          emit("\tslt\ta0, t4, a0");
          emit("\tseqz\ta0, a0");
        } else if (op == ">=") {
          emit("\tslt\ta0, a0, t4");
          emit("\tseqz\ta0, a0");
        }
        return;
      }
    }

    // T5-based path: both sides non-leaf, save LHS to t5 (safe across sub-expr eval)
    if (optO1_ && !useFloat && !compare) {
      emitExpr(expr->lhs.get());
      emit("\tmv\tt5, a0");
      emitExpr(expr->rhs.get());
      if (op == "+") {
        emit("\taddw\ta0, t5, a0");
      } else if (op == "-") {
        emit("\tsubw\ta0, t5, a0");
      } else if (op == "*") {
        emit("\tmulw\ta0, t5, a0");
      } else if (op == "/") {
        emit("\tdivw\ta0, t5, a0");
      } else if (op == "%") {
        emit("\tremw\ta0, t5, a0");
      }
      return;
    }
    if (optO1_ && !useFloat && compare) {
      emitExpr(expr->lhs.get());
      emit("\tmv\tt5, a0");
      emitExpr(expr->rhs.get());
      if (op == "==") {
        emit("\tsubw\ta0, t5, a0");
        emit("\tseqz\ta0, a0");
      } else if (op == "!=") {
        emit("\tsubw\ta0, t5, a0");
        emit("\tsnez\ta0, a0");
      } else if (op == "<") {
        emit("\tslt\ta0, t5, a0");
      } else if (op == ">") {
        emit("\tslt\ta0, a0, t5");
      } else if (op == "<=") {
        emit("\tslt\ta0, a0, t5");
        emit("\tseqz\ta0, a0");
      } else if (op == ">=") {
        emit("\tslt\ta0, t5, a0");
        emit("\tseqz\ta0, a0");
      }
      return;
    }

    // Fallback: push/pop-based evaluation
    emitExpr(expr->lhs.get());
    if (expr->lhs->type.isFloatScalar()) {
      emitPushFloat("fa0");
    } else {
      emitPushInt("a0");
    }
    emitExpr(expr->rhs.get());

    if (useFloat) {
      if (!expr->rhs->type.isFloatScalar()) {
        emit("\tfcvt.s.w\tfa0, a0");
      }
      emitPushFloat("fa0");
      emitPopFloat("ft1");
      if (expr->lhs->type.isFloatScalar()) {
        emitPopFloat("ft0");
      } else {
        emitPopInt("t2");
        emit("\tfcvt.s.w\tft0, t2");
      }
      emit("\tfmv.s\tfa0, ft1");
      if (compare) {
        emitFloatCompare(op);
      } else if (op == "+") {
        emit("\tfadd.s\tfa0, ft0, fa0");
      } else if (op == "-") {
        emit("\tfsub.s\tfa0, ft0, fa0");
      } else if (op == "*") {
        emit("\tfmul.s\tfa0, ft0, fa0");
      } else if (op == "/") {
        emit("\tfdiv.s\tfa0, ft0, fa0");
      }
      (void)resultFloat;
      return;
    }

    emitPushInt("a0");
    emitPopInt("a1");
    emitPopInt("a0");
    if (compare) {
      emitIntCompare(op);
    } else if (optO1_ && expr->lhs->type.isIntScalar() && expr->rhs->type.isIntScalar() &&
               op == "+" && expr->lhs->isConst && expr->lhs->constVal.type == BaseType::Int) {
      int32_t k = constAsInt(expr->lhs->constVal);
      if (fitsImm12(static_cast<int>(k))) {
        emit("\taddiw\ta0, a1, " + to_string(k));
      } else {
        emit("\taddw\ta0, a0, a1");
      }
    } else if (optO1_ && expr->lhs->type.isIntScalar() && expr->rhs->type.isIntScalar() &&
               op == "*" && expr->lhs->isConst && expr->lhs->constVal.type == BaseType::Int) {
      int32_t k = constAsInt(expr->lhs->constVal);
      emit("\tmv\ta0, a1");
      if (!emitMulByConst(*this, k)) {
        emit("\tli\tt3, " + to_string(static_cast<int>(k)));
        emit("\tmulw\ta0, a1, t3");
      }
    } else if (optO1_ && expr->lhs->type.isIntScalar() && expr->rhs->type.isIntScalar() &&
               expr->rhs->isConst && expr->rhs->constVal.type == BaseType::Int &&
               op == "*") {
      int32_t k = constAsInt(expr->rhs->constVal);
      if (!emitMulByConst(*this, k)) {
        emit("\tmulw\ta0, a0, a1");
      }
    } else if (optO1_ && expr->lhs->type.isIntScalar() && expr->rhs->type.isIntScalar() &&
               expr->rhs->isConst && expr->rhs->constVal.type == BaseType::Int &&
               op == "/") {
      int32_t k = constAsInt(expr->rhs->constVal);
      int lg = intLog2Positive32(k);
      if (k > 0 && lg >= 0) {
        emit("\tli\ta2, " + to_string(k - 1));
        emit("\tsraiw\tt1, a0, 31");
        emit("\tand\tt1, t1, a2");
        emit("\taddw\tt3, a0, t1");
        emit("\tsraiw\ta0, t3, " + to_string(lg));
      } else if (emitSDivByConst(*this, k)) {
        /* optimized by magic multiply */
      } else {
        emit("\tdivw\ta0, a0, a1");
      }
    } else if (optO1_ && expr->lhs->type.isIntScalar() && expr->rhs->type.isIntScalar() &&
               expr->rhs->isConst && expr->rhs->constVal.type == BaseType::Int &&
               op == "+") {
      int32_t k = constAsInt(expr->rhs->constVal);
      if (fitsImm12(static_cast<int>(k))) {
        emit("\taddiw\ta0, a0, " + to_string(k));
      } else {
        emit("\taddw\ta0, a0, a1");
      }
    } else if (optO1_ && expr->lhs->type.isIntScalar() && expr->rhs->type.isIntScalar() &&
               expr->rhs->isConst && expr->rhs->constVal.type == BaseType::Int &&
               op == "-") {
      int32_t k = constAsInt(expr->rhs->constVal);
      int neg = -static_cast<int>(k);
      if (fitsImm12(neg)) {
        emit("\taddiw\ta0, a0, " + to_string(neg));
      } else {
        emit("\tsubw\ta0, a0, a1");
      }
    } else if (optO1_ && expr->lhs->type.isIntScalar() && expr->rhs->type.isIntScalar() &&
               expr->rhs->isConst && expr->rhs->constVal.type == BaseType::Int &&
               op == "%") {
      int32_t k = constAsInt(expr->rhs->constVal);
      int lg = intLog2Positive32(k);
      if (lg >= 1) {
        uint32_t mask = static_cast<uint32_t>(k) - 1;
        emit("\tsraiw\tt1, a0, 31");
        emit("\tsrliw\tt1, t1, " + to_string(32 - lg));
        emit("\taddw\ta0, a0, t1");
        emit("\tandi\ta0, a0, " + to_string(static_cast<int>(mask)));
        emit("\tsubw\ta0, a0, t1");
      } else if (emitSRemByConst(*this, k)) {
        /* optimized by magic multiply */
      } else {
        emit("\tremw\ta0, a0, a1");
      }
    } else if (op == "+") {
      emit("\taddw\ta0, a0, a1");
    } else if (op == "-") {
      emit("\tsubw\ta0, a0, a1");
    } else if (op == "*") {
      emit("\tmulw\ta0, a0, a1");
    } else if (op == "/") {
      emit("\tdivw\ta0, a0, a1");
    } else if (op == "%") {
      emit("\tremw\ta0, a0, a1");
    }
  }

void CodeGen::emitIntCompare(const string &op) {
    if (op == "==") {
      emit("\tsubw\ta0, a0, a1");
      emit("\tseqz\ta0, a0");
    } else if (op == "!=") {
      emit("\tsubw\ta0, a0, a1");
      emit("\tsnez\ta0, a0");
    } else if (op == "<") {
      emit("\tslt\ta0, a0, a1");
    } else if (op == ">") {
      emit("\tslt\ta0, a1, a0");
    } else if (op == "<=") {
      emit("\tslt\ta0, a1, a0");
      emit("\tseqz\ta0, a0");
    } else if (op == ">=") {
      emit("\tslt\ta0, a0, a1");
      emit("\tseqz\ta0, a0");
    }
  }

void CodeGen::emitFloatCompare(const string &op) {
    if (op == "==") {
      emit("\tfeq.s\ta0, ft0, fa0");
    } else if (op == "!=") {
      emit("\tfeq.s\ta0, ft0, fa0");
      emit("\tseqz\ta0, a0");
    } else if (op == "<") {
      emit("\tflt.s\ta0, ft0, fa0");
    } else if (op == ">") {
      emit("\tflt.s\ta0, fa0, ft0");
    } else if (op == "<=") {
      emit("\tfle.s\ta0, ft0, fa0");
    } else if (op == ">=") {
      emit("\tfle.s\ta0, fa0, ft0");
    }
  }

void CodeGen::emitCond(Expr *expr, const string &trueLabel, const string &falseLabel) {
    if (expr->kind == ExprKind::Binary) {
      auto *bin = static_cast<BinaryExpr *>(expr);
      if (bin->op == "&&") {
        string mid = newLabel("land_mid");
        emitCond(bin->lhs.get(), mid, falseLabel);
        emit(mid + ":");
        emitCond(bin->rhs.get(), trueLabel, falseLabel);
        return;
      }
      if (bin->op == "||") {
        string mid = newLabel("lor_mid");
        emitCond(bin->lhs.get(), trueLabel, mid);
        emit(mid + ":");
        emitCond(bin->rhs.get(), trueLabel, falseLabel);
        return;
      }
    }
    emitExpr(expr);
    // Comparison results are already 0/1, skip redundant snez
    bool isCompare = false;
    if (expr->kind == ExprKind::Binary) {
      string op = static_cast<BinaryExpr *>(expr)->op;
      isCompare = (op == "==" || op == "!=" || op == "<" || op == ">" ||
                   op == "<=" || op == ">=");
    }
    if (!isCompare) emitBoolFromValue(expr->type);
    emit("\tbnez\ta0, " + trueLabel);
    emit("\tj\t" + falseLabel);
  }

void CodeGen::emitBoolFromValue(const Type &type) {
    if (type.isPointer) {
      emit("\tsnez\ta0, a0");
    } else if (type.base == BaseType::Float) {
      emit("\tfmv.w.x\tft0, zero");
      emit("\tfeq.s\ta0, fa0, ft0");
      emit("\tseqz\ta0, a0");
    } else {
      emit("\tsnez\ta0, a0");
    }
  }

void CodeGen::emitLValValue(LValExpr *expr) {
    if (expr->isConst && !expr->type.isPointer) {
      emitConst(expr->constVal);
      return;
    }
    // Fast path: local scalar with 12-bit offset → load directly
    Symbol *sym = expr->symbol;
    if (optO1_ && !sym->isGlobal && !sym->isParamArray &&
        expr->indices.empty() && !expr->type.isPointer &&
        fitsImm12(sym->offset)) {
      if (expr->type.base == BaseType::Float) {
        emit("\tflw\tfa0, " + to_string(sym->offset) + "(s0)");
      } else {
        emit("\tlw\ta0, " + to_string(sym->offset) + "(s0)");
      }
      return;
    }
    emitLValAddress(expr);
    if (expr->type.isPointer) {
      return;
    }
    if (expr->type.base == BaseType::Float) {
      emit("\tflw\tfa0, 0(a0)");
    } else {
      emit("\tlw\ta0, 0(a0)");
    }
  }

void CodeGen::emitLValAddress(LValExpr *expr) {
    Symbol *sym = expr->symbol;
    if (sym->isGlobal) {
      emit("\tlla\ta0, " + sym->label);
    } else if (sym->isParamArray) {
      emitLoadMem("ld", "a0", "s0", sym->offset);
    } else {
      emitAddOffset("a0", "s0", sym->offset);
    }
    if (expr->indices.empty()) {
      return;
    }
    // 安全快速路径：t4 存基址，t5 累加偏移（emitExpr 不使用 t4/t5）
    if (optO1_) {
      emit("\tmv\tt4, a0");
      emit("\tli\tt5, 0");
      for (size_t i = 0; i < expr->indices.size(); ++i) {
        emitExpr(expr->indices[i].get());
        emitConvert(expr->indices[i]->type, Type::scalar(BaseType::Int));
        int strideBytes = strideForIndex(sym, i) * 4;
        int lg = intLog2Positive32(static_cast<int32_t>(strideBytes));
        if (lg >= 0) {
          emit("\tslliw\tt2, a0, " + to_string(lg));
        } else {
          emit("\tli\tt1, " + to_string(strideBytes));
          emit("\tmulw\tt2, a0, t1");
        }
        emit("\taddw\tt5, t5, t2");
      }
      emit("\tadd\ta0, t4, t5");
      return;
    }
    // Fallback: stack-based accumulation
    emitPushInt("a0");
    emit("\tli\ta0, 0");
    emitPushInt("a0");
    for (size_t i = 0; i < expr->indices.size(); ++i) {
      emitExpr(expr->indices[i].get());
      emitConvert(expr->indices[i]->type, Type::scalar(BaseType::Int));
      int strideBytes = strideForIndex(sym, i) * 4;
      if (optO1_ && strideBytes > 0) {
        int lg = intLog2Positive32(static_cast<int32_t>(strideBytes));
        if (lg >= 0) {
          emit("\tslliw\ta0, a0, " + to_string(lg));
        } else {
          emit("\tli\tt3, " + to_string(strideBytes));
          emit("\tmulw\tt1, a0, t3");
          emit("\tmv\ta0, t1");
        }
      } else {
        emit("\tli\tt3, " + to_string(strideBytes));
        emit("\tmulw\tt1, a0, t3");
        emit("\tmv\ta0, t1");
      }
      emit("\tld\tt2, 0(sp)");
      emit("\tadd\tt2, t2, a0");
      emit("\tsd\tt2, 0(sp)");
    }
    emitPopInt("t2");
    emitPopInt("a0");
    emit("\tadd\ta0, a0, t2");
  }

int CodeGen::strideForIndex(Symbol *sym, size_t index) {
    // 形参数组只有「除最左维外的各维」保存在 dims 中，第 i 个下标的步长为
    // product(dims, i)；与普通局部数组下标 i 的步长 product(dims, i+1) 对应同一规则。
    if (sym->isParamArray) {
      return product(sym->dims, index);
    }
    return index + 1 < sym->dims.size() ? product(sym->dims, index + 1) : 1;
  }

void CodeGen::emitCall(CallExpr *expr) {
    Function *fn = expr->function;
    vector<ParamType> params;
    vector<Expr *> args;
    if (fn->injectLineArgument) {
      params.push_back(ParamType{BaseType::Int, false, {}});
    }
    for (size_t i = 0; i < expr->args.size(); ++i) {
      args.push_back(expr->args[i].get());
      if (i < fn->params.size()) {
        params.push_back(fn->params[i]);
      } else {
        ParamType inferred;
        inferred.base = expr->args[i]->type.base;
        inferred.isArray = expr->args[i]->type.isPointer;
        params.push_back(inferred);
      }
    }

    size_t namedCount = params.size();
    if (fn->variadic) {
      namedCount = fn->params.size() + (fn->injectLineArgument ? 1 : 0);
    }

    int stackSlots = 0;
    auto locs = computeArgLocations(params, fn->variadic, namedCount, &stackSlots);

    // 快路径已禁用：后续参数求值可能覆盖前面已放入 a1/a2... 的参数

    // 优化路径：无嵌套调用时，求值到安全寄存器(t4-t6,ft)，省去 temp 栈槽
    bool noNestedCall = optO1_ && !fn->injectLineArgument;
    if (noNestedCall) {
      for (size_t i = 0; i < args.size() && noNestedCall; ++i) {
        if (!exprHasNoCall(args[i])) noNestedCall = false;
      }
      if (noNestedCall) {
        // Count safe registers needed: t4-t6 = 3 int; ft0-ft7 = 8 float
        int intNeeded = 0;
        for (size_t i = 0; i < params.size(); ++i) {
          if (params[i].isArray || params[i].base == BaseType::Int) ++intNeeded;
        }
        if (intNeeded > 3) noNestedCall = false;
      }
    }

    if (noNestedCall) {
      // Compute all args to safe registers
      int intUsed = 0, floatUsed = 0;
      vector<string> argReg(args.size()); // which register holds each arg

      for (size_t i = 0; i < args.size(); ++i) {
        emitExpr(args[i]);
        ParamType expected = params[i];
        Type target = expected.isArray ? Type::pointer(expected.base, expected.dims)
                                       : Type::scalar(expected.base);
        if (!expected.isArray) emitConvert(args[i]->type, target);

        if (target.isPointer || expected.base == BaseType::Int) {
          string reg = "t" + to_string(4 + intUsed);
          emit("\tmv\t" + reg + ", a0");
          argReg[i] = reg;
          ++intUsed;
        } else {
          string reg = "ft" + to_string(floatUsed);
          emit("\tfmv.s\t" + reg + ", fa0");
          argReg[i] = reg;
          ++floatUsed;
        }
      }

      // Copy from safe registers to ABI registers / stack
      int stackArea = stackSlots * 8;
      if (stackArea > 0) emitAdjustSp(-alignTo(stackArea, 16));

      for (size_t i = 0; i < params.size(); ++i) {
        const ArgLoc &loc = locs[i];
        bool isFloat = !params[i].isArray && params[i].base == BaseType::Float;
        if (loc.kind == ArgLoc::FloatReg) {
          emit("\tfmv.s\tfa" + to_string(loc.index) + ", " + argReg[i]);
        } else if (loc.kind == ArgLoc::IntReg) {
          emit("\tmv\ta" + to_string(loc.index) + ", " + argReg[i]);
        } else {
          int dst = loc.index * 8;
          if (isFloat) {
            emitStoreMem("fsw", argReg[i], "sp", dst);
          } else {
            emitStoreMem("sd", argReg[i], "sp", dst);
          }
        }
      }

      emit("\tcall\t" + fn->asmName);
      if (stackArea > 0) emitAdjustSp(stackArea);
      return;
    }

    // Fallback: stack-based arg passing
    int tempSlots = static_cast<int>(params.size());
    int stackBytes = stackSlots * 8;
    int tempBase = stackBytes;
    int area = alignTo(stackBytes + tempSlots * 8, 16);
    if (area > 0) {
      emitAdjustSp(-area);
    }

    int paramIndex = 0;
    if (fn->injectLineArgument) {
      emit("\tli\ta0, " + to_string(expr->line));
      emitStoreMem("sd", "a0", "sp", tempBase);
      ++paramIndex;
    }
    for (Expr *arg : args) {
      ParamType expected = params[paramIndex];
      const bool vaArg =
          fn->variadic && static_cast<size_t>(paramIndex) >= namedCount;
      emitExpr(arg);
      Type target = expected.isArray ? Type::pointer(expected.base, expected.dims)
                                     : Type::scalar(expected.base);
      if (!expected.isArray) {
        emitConvert(arg->type, target);
      }
      int off = tempBase + paramIndex * 8;
      if (target.isPointer) {
        emitStoreMem("sd", "a0", "sp", off);
      } else if (expected.base == BaseType::Float && vaArg) {
        emit("\tfcvt.d.s\tfa1, fa0");
        emit("\tfmv.x.d\ta0, fa1");
        emitStoreMem("sd", "a0", "sp", off);
      } else if (expected.base == BaseType::Float) {
        emitStoreMem("fsw", "fa0", "sp", off);
      } else if (vaArg) {
        emit("\tsext.w\ta0, a0");
        emitStoreMem("sd", "a0", "sp", off);
      } else {
        emitStoreMem("sd", "a0", "sp", off);
      }
      ++paramIndex;
    }

    for (size_t i = 0; i < params.size(); ++i) {
      const ParamType &p = params[i];
      const ArgLoc &loc = locs[i];
      int off = tempBase + static_cast<int>(i) * 8;
      bool isFloat = !p.isArray && p.base == BaseType::Float;
      const bool vaArg = fn->variadic && i >= namedCount;
      if (loc.kind == ArgLoc::FloatReg) {
        emitLoadMem("flw", "fa" + to_string(loc.index), "sp", off);
      } else if (loc.kind == ArgLoc::IntReg) {
        if (isFloat && !vaArg) {
          emitLoadMem("lw", "a" + to_string(loc.index), "sp", off);
        } else {
          emitLoadMem("ld", "a" + to_string(loc.index), "sp", off);
        }
      } else {
        int dst = loc.index * 8;
        if (isFloat) {
          if (vaArg) {
            emitLoadMem("ld", "t0", "sp", off);
            emitStoreMem("sd", "t0", "sp", dst);
          } else {
            emitLoadMem("lw", "t0", "sp", off);
            emitStoreMem("sw", "t0", "sp", dst);
          }
        } else {
          emitLoadMem("ld", "t0", "sp", off);
          emitStoreMem("sd", "t0", "sp", dst);
        }
      }
    }

    emit("\tcall\t" + fn->asmName);
    if (area > 0) {
      emitAdjustSp(area);
    }
  }

void CodeGen::emitConvert(const Type &from, const Type &to) {
    if (to.base == BaseType::Void || from.isPointer || to.isPointer ||
        from.base == to.base) {
      return;
    }
    if (from.base == BaseType::Int && to.base == BaseType::Float) {
      emit("\tfcvt.s.w\tfa0, a0");
    } else if (from.base == BaseType::Float && to.base == BaseType::Int) {
      emit("\tfcvt.w.s\ta0, fa0, rtz");
    }
  }

void CodeGen::emitPushInt(const string &reg) {
    emit("\taddi\tsp, sp, -16");
    emit("\tsd\t" + reg + ", 0(sp)");
  }

void CodeGen::emitPopInt(const string &reg) {
    emit("\tld\t" + reg + ", 0(sp)");
    emit("\taddi\tsp, sp, 16");
  }

void CodeGen::emitPushFloat(const string &reg) {
    emit("\taddi\tsp, sp, -16");
    emit("\tfsw\t" + reg + ", 0(sp)");
  }

void CodeGen::emitPopFloat(const string &reg) {
    emit("\tflw\t" + reg + ", 0(sp)");
    emit("\taddi\tsp, sp, 16");
  }

void CodeGen::emitLiteralPools() {
    if (floatLiterals_.empty() && stringLiterals_.empty()) {
      return;
    }
    emit("\t.section\t.rodata");
    for (const auto &lit : floatLiterals_) {
      emit("\t.align\t2");
      emit(lit.first + ":");
      emit("\t.word\t" + to_string(floatBits(lit.second)));
    }
    for (const auto &lit : stringLiterals_) {
      emit("\t.align\t1");
      emit(lit.first + ":");
      emit("\t.asciz\t\"" + escapeAsmString(lit.second) + "\"");
    }
  }
