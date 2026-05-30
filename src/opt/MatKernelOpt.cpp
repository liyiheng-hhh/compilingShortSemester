#include "Passes.h"
#include "LoopPasses.h"
#include "../pre-opt/PreAttrs.h"
#include "../common.h"

#include <functional>
#include <set>
#include <unordered_set>

using namespace sys;

namespace {

// --- shared helpers -------------------------------------------------------

Op *mkoResolve(Op *v) {
  std::set<Op*> seen;
  while (v && !seen.count(v)) {
    seen.insert(v);
    if (isa<PhiOp>(v) && v->getOperandCount() == 1)
      v = v->DEF(0);
    else
      break;
  }
  return v;
}

bool mkoSame(Op *a, Op *b) {
  return mkoResolve(a) == mkoResolve(b);
}

bool mkoGlobalName(Op *op, std::string &name) {
  op = mkoResolve(op);
  if (auto *glob = dyn_cast<GetGlobalOp>(op)) {
    name = NAME(glob);
    return !name.empty();
  }
  return false;
}

std::set<std::string> mkoGlobalsIn(Op *op) {
  std::set<std::string> names;
  std::set<Op*> seen;
  std::function<void(Op*)> walk = [&](Op *x) {
    if (!x || seen.count(x))
      return;
    seen.insert(x);
    if (x->has<BaseAttr>()) {
      std::string name;
      if (mkoGlobalName(BASE(x), name))
        names.insert(name);
      return;
    }
    std::string name;
    if (mkoGlobalName(x, name)) {
      names.insert(name);
      return;
    }
    for (auto operand : x->getOperands())
      walk(operand.defining);
  };
  walk(op);
  return names;
}

bool mkoUsesNeedle(Op *op, Op *needle, std::set<Op*> &seen) {
  if (!op || !needle || seen.count(op))
    return false;
  if (op == needle)
    return true;
  seen.insert(op);
  for (auto operand : op->getOperands()) {
    if (mkoUsesNeedle(operand.defining, needle, seen))
      return true;
  }
  return false;
}

bool mkoUsesNeedle(Op *op, Op *needle) {
  std::set<Op*> seen;
  return mkoUsesNeedle(op, needle, seen);
}

bool mkoGlobalMatrix(const std::map<std::string, GlobalOp*> &gMap,
                     const std::string &name, int &rows, int &cols) {
  auto it = gMap.find(name);
  if (it == gMap.end())
    return false;
  auto glob = it->second;
  if (!glob->has<DimensionAttr>())
    return false;
  const auto &dims = DIM(glob);
  if (dims.size() != 2)
    return false;
  rows = dims[0];
  cols = dims[1];
  if (rows <= 0 || cols <= 0)
    return false;
  return true;
}

bool mkoUnmulImm(Op *op, int imm, Op *&idx) {
  if (!op || !isa<MulIOp>(op) || op->getOperandCount() != 2)
    return false;
  Op *a = op->DEF(0);
  Op *b = op->DEF(1);
  if (isa<IntOp>(b) && V(b) == imm) {
    idx = a;
    return true;
  }
  if (isa<IntOp>(a) && V(a) == imm) {
    idx = b;
    return true;
  }
  return false;
}

struct GridAccess {
  std::string glob;
  Op *row = nullptr;
  Op *col = nullptr;
};

bool mkoParseGrid(Op *addr, int rowStrideBytes, GridAccess &out) {
  out = {};
  Op *cur = addr;
  Op *row = nullptr;
  Op *col = nullptr;
  std::set<Op*> seen;
  while (cur && !seen.count(cur)) {
    seen.insert(cur);
    if (cur->has<BaseAttr>()) {
      if (!mkoGlobalName(BASE(cur), out.glob))
        return false;
      out.row = row;
      out.col = col;
      return row && col;
    }
    if (isa<GetGlobalOp>(cur)) {
      out.glob = NAME(cur);
      out.row = row;
      out.col = col;
      return !out.glob.empty() && row && col;
    }
    if (!isa<AddLOp>(cur))
      break;
    Op *a = cur->DEF(0);
    Op *b = cur->DEF(1);
    Op *idx = nullptr;
    if (mkoUnmulImm(b, 4, idx)) {
      col = idx;
      cur = a;
      continue;
    }
    if (mkoUnmulImm(a, 4, idx)) {
      col = idx;
      cur = b;
      continue;
    }
    if (mkoUnmulImm(b, rowStrideBytes, idx)) {
      row = idx;
      cur = a;
      continue;
    }
    if (mkoUnmulImm(a, rowStrideBytes, idx)) {
      row = idx;
      cur = b;
      continue;
    }
    break;
  }
  return false;
}

Op *mkoMatrixAddr(Builder &builder, Op *base, Op *row, Op *col, int rowStrideBytes) {
  auto rowOff = builder.create<MulIOp>({ row, builder.create<IntOp>({ new IntAttr(rowStrideBytes) }) });
  auto rowBase = builder.create<AddLOp>({ base, rowOff });
  auto colOff = builder.create<MulIOp>({ col, builder.create<IntOp>({ new IntAttr(4) }) });
  return builder.create<AddLOp>({ rowBase, colOff });
}

Op *mkoGlobalPtr(Builder &builder, const std::map<std::string, GlobalOp*> &gMap,
                 const std::string &name) {
  auto it = gMap.find(name);
  if (it == gMap.end())
    return nullptr;
  return builder.create<GetGlobalOp>({ new NameAttr(name) });
}

bool mkoSameBound(Op *a, Op *b) {
  a = mkoResolve(a);
  b = mkoResolve(b);
  if (isa<IntOp>(a) && isa<IntOp>(b))
    return V(a) == V(b);
  return a == b;
}

LoopInfo *mkoInnermostLoop(const LoopForest &forest, BasicBlock *bb) {
  LoopInfo *best = nullptr;
  size_t bestSize = SIZE_MAX;
  for (auto *loop : forest.getLoops()) {
    if (!loop || !loop->contains(bb))
      continue;
    if (loop->bbs.size() <= bestSize) {
      best = loop;
      bestSize = loop->bbs.size();
    }
  }
  return best;
}

LoopInfo *mkoParentLoop(LoopInfo *inner, const LoopForest &forest) {
  if (!inner)
    return nullptr;
  LoopInfo *best = nullptr;
  size_t bestSize = SIZE_MAX;
  for (auto *cand : forest.getLoops()) {
    if (!cand || cand == inner)
      continue;
    if (!cand->contains(inner->header))
      continue;
    if (cand->bbs.size() < bestSize) {
      best = cand;
      bestSize = cand->bbs.size();
    }
  }
  return best;
}

Op *mkoLoopIv(LoopInfo *loop) {
  if (!loop)
    return nullptr;
  Op *iv = loop->induction;
  if (iv)
    return iv;
  for (auto phi : loop->header->getPhis()) {
    if (phi->getResultType() == Value::i32)
      return phi;
  }
  return nullptr;
}

Op *mkoLoopBound(LoopInfo *loop) {
  if (!loop)
    return nullptr;
  if (loop->stop)
    return loop->stop;
  auto term = loop->header->getLastOp();
  if (auto br = dyn_cast<BranchOp>(term)) {
    auto cond = br->DEF(0);
    if (cond && isa<LtOp>(cond))
      return cond->DEF(1);
  }
  return nullptr;
}

Op *mkoPhiIncoming(PhiOp *phi, BasicBlock *fromBB) {
  if (!phi || !fromBB)
    return nullptr;
  const auto &attrs = phi->getAttrs();
  for (int i = 0; i < phi->getOperandCount(); i++) {
    if (i < (int)attrs.size() && FROM(attrs[i]) == fromBB)
      return phi->DEF(i);
  }
  return nullptr;
}

bool mkoReachableFrom(BasicBlock *from, BasicBlock *to) {
  if (!from || !to)
    return false;
  std::unordered_set<BasicBlock*> seen;
  std::vector<BasicBlock*> stack = { from };
  while (!stack.empty()) {
    auto *cur = stack.back();
    stack.pop_back();
    if (cur == to)
      return true;
    if (!seen.insert(cur).second)
      continue;
    for (auto *succ : cur->succs)
      stack.push_back(succ);
  }
  return false;
}

// --- GuardedAccum ---------------------------------------------------------

bool mkoPureThenBody(BasicBlock *thenBB, Op *addInst) {
  for (auto *op : thenBB->getOps()) {
    if (op == addInst || isa<GotoOp>(op))
      continue;
    if (isa<StoreOp>(op) || isa<CallOp>(op) || op->has<ImpureAttr>())
      return false;
  }
  return true;
}

// Matmul k-loop: acc += a[i][k]*b[k][j]. Lifting to select+add regresses on real RISC-V
// (platform matmul1/2/3 +6.36 vs pre-GuardedAccum); conv/sl patterns don't use mul in then.
// Phase 1.2 relaxation: allow Mul in then if the block is side-effect free (only load/mul/addi).
// This matches the spirit of CondGuardedAccumulatePass (hoist + cond*val) without copying code.
bool mkoGuardedMatmulStep(BasicBlock *thenBB, AddIOp *addInst, PhiOp *accPhi = nullptr) {
  // If thenBB only contains load + mul + add (no call/store/impure), allow it.
  for (auto *op : thenBB->getOps()) {
    if (op == addInst || isa<GotoOp>(op) || isa<BranchOp>(op))
      continue;
    if (isa<LoadOp>(op) || isa<MulIOp>(op) || isa<AddIOp>(op) || isa<IntOp>(op))
      continue;
    if (isa<StoreOp>(op) || isa<CallOp>(op) || op->has<ImpureAttr>())
      return true;  // still reject if has side effects
  }
  return false;  // side-effect free thenBB with mul is now allowed
}

AddIOp *mkoFindGuardedAdd(BasicBlock *thenBB, Op *&acc, Op *&addend) {
  acc = addend = nullptr;
  auto &ops = thenBB->getOps();
  for (auto it = ops.rbegin(); it != ops.rend(); ++it) {
    auto *op = *it;
    if (isa<GotoOp>(op) || isa<BranchOp>(op))
      continue;
    auto *add = dyn_cast<AddIOp>(op);
    if (!add)
      return nullptr;
    acc = add->DEF(0);
    addend = add->DEF(1);
    if (acc == addend)
      return nullptr;
    return add;
  }
  return nullptr;
}

bool mkoLatchMerge(BasicBlock *mergeBB, BasicBlock *&header) {
  if (!mergeBB)
    return false;
  auto *term = mergeBB->getLastOp();
  if (!term || !term->has<TargetAttr>())
    return false;
  header = TARGET(term);
  return header != nullptr;
}

// Merge must be a single-acc latch: one acc phi plus optional iv bump / loop branch.
bool mkoAccLatchMerge(BasicBlock *mergeBB, PhiOp *accPhi) {
  if (!mergeBB || !accPhi)
    return false;
  int phis = 0;
  for (auto *op : mergeBB->getOps()) {
    if (isa<PhiOp>(op)) {
      if (op != accPhi)
        return false;
      phis++;
      continue;
    }
    if (isa<StoreOp>(op) || isa<CallOp>(op) || op->has<ImpureAttr>())
      return false;
  }
  return phis == 1;
}


bool mkoGuardedAddOrientation(BasicBlock *bb, BranchOp *br, BasicBlock *&thenBB,
                             BasicBlock *&mergeBB, bool &addInElse, Op *&acc,
                             Op *&addend, AddIOp *&addInst) {
  thenBB = mergeBB = nullptr;
  addInElse = false;
  acc = addend = nullptr;
  addInst = nullptr;
  if (!bb || !br)
    return false;

  BasicBlock *candidates[2] = { TARGET(br), ELSE(br) };
  for (int i = 0; i < 2; i++) {
    BasicBlock *thenCand = candidates[i];
    BasicBlock *mergeCand = candidates[1 - i];
    if (!thenCand || !mergeCand)
      continue;
    auto *thenTerm = thenCand->getLastOp();
    if (!thenTerm || !thenTerm->has<TargetAttr>() || TARGET(thenTerm) != mergeCand)
      continue;
    Op *accCand = nullptr;
    Op *addendCand = nullptr;
    auto *addCand = mkoFindGuardedAdd(thenCand, accCand, addendCand);
    if (!addCand || !mkoPureThenBody(thenCand, addCand))
      continue;
    thenBB = thenCand;
    mergeBB = mergeCand;
    addInElse = (thenCand == ELSE(br));
    acc = accCand;
    addend = addendCand;
    addInst = addCand;
    return true;
  }
  return false;
}

void mkoHoistThenCompute(BasicBlock *dst, BasicBlock *thenBB, AddIOp *addInst) {
  auto br = dst->getLastOp();
  std::vector<Op*> toMove;
  for (auto *op : thenBB->getOps()) {
    if (op == addInst || isa<GotoOp>(op))
      continue;
    toMove.push_back(op);
  }
  for (auto *op : toMove)
    op->moveBefore(br);
}

void mkoFinalizeGuardedAccumCompact(FuncOp *func, BasicBlock *bb, BasicBlock *thenBB,
                                    BasicBlock *mergeBB, BranchOp *br, Op *cond, Op *acc,
                                    Op *addend, AddIOp *addInst, PhiOp *accPhi,
                                    bool addInElse) {
  Builder builder;
  builder.setBeforeOp(br);

  mkoHoistThenCompute(bb, thenBB, addInst);

  auto zero = builder.create<IntOp>({ new IntAttr(0) });
  Op *scaled = addInElse
                   ? static_cast<Op*>(builder.create<SelectOp>({ cond, zero, addend }))
                   : static_cast<Op*>(builder.create<SelectOp>({ cond, addend, zero }));
  auto newAcc = builder.create<AddIOp>(std::vector<Value>{ acc, scaled });

  builder.replace<GotoOp>(br, { new TargetAttr(mergeBB) });

  for (int i = accPhi->getOperandCount() - 1; i >= 0; i--) {
    const auto &attrs = accPhi->getAttrs();
    if (i < (int)attrs.size() && FROM(attrs[i]) == thenBB)
      accPhi->removeOperand(i);
  }
  for (int i = 0; i < accPhi->getOperandCount(); i++) {
    const auto &attrs = accPhi->getAttrs();
    if (i < (int)attrs.size() && FROM(attrs[i]) == bb)
      accPhi->setOperand(i, newAcc);
  }

  func->getRegion()->updatePreds();
  thenBB->forceErase();
  func->getRegion()->updatePreds();
}

void mkoFinalizeGuardedAccum(FuncOp *func, BasicBlock *bb, BasicBlock *thenBB,
                             BasicBlock *mergeBB, BasicBlock *header,
                             BranchOp *br, Op *cond, Op *acc, Op *addend,
                             AddIOp *addInst, PhiOp *accPhi) {
  Builder builder;
  builder.setBeforeOp(br);

  mkoHoistThenCompute(bb, thenBB, addInst);

  auto zero = builder.create<IntOp>({ new IntAttr(0) });
  auto scaled = builder.create<SelectOp>({ cond, addend, zero });
  auto newAcc = builder.create<AddIOp>({ acc, scaled });

  std::vector<Op*> toMove;
  for (auto *op : mergeBB->getOps()) {
    if (isa<GotoOp>(op) || isa<PhiOp>(op) || op == accPhi)
      continue;
    toMove.push_back(op);
  }
  for (auto *op : toMove)
    op->moveBefore(br);

  builder.replace<GotoOp>(br, { new TargetAttr(header) });

  for (auto *phi : header->getPhis()) {
    auto &attrs = phi->getAttrs();
    for (int i = 0; i < phi->getOperandCount(); i++) {
      if (FROM(attrs[i]) != mergeBB)
        continue;
      cast<FromAttr>(attrs[i])->bb = bb;
      Op *incoming = phi->DEF(i);
      if (mkoSame(incoming, acc) || mkoSame(incoming, addInst) ||
          (accPhi && incoming == accPhi))
        phi->setOperand(i, newAcc);
    }
  }

  if (accPhi) {
    accPhi->replaceAllUsesWith(newAcc);
    accPhi->erase();
  }

  func->getRegion()->updatePreds();
  thenBB->forceErase();
  mergeBB->forceErase();
  func->getRegion()->updatePreds();
}

bool mkoTryGuardedAccum(FuncOp *func, BasicBlock *bb, int &converted) {
  const bool debug = envFlagTruthy("SYSY_CC_GUARDED_ACCUM_DEBUG");
  auto dbg = [&](const char *why) {
    if (debug)
      std::cerr << "[guarded-accum] reject " << why << "\n";
  };

  auto *br = dyn_cast<BranchOp>(bb->getLastOp());
  if (!br || br->getOperandCount() != 1 || !br->has<TargetAttr>() || !br->has<ElseAttr>())
    return false;

  BasicBlock *thenBB = nullptr;
  BasicBlock *mergeBB = nullptr;
  bool addInElse = false;
  Op *acc = nullptr;
  Op *addend = nullptr;
  AddIOp *addInst = nullptr;
  if (!mkoGuardedAddOrientation(bb, br, thenBB, mergeBB, addInElse, acc, addend, addInst))
    return false;

  Op *cond = br->DEF(0);
  if (!cond || cond->getResultType() != Value::i32) {
    dbg("cond");
    return false;
  }

  auto *mergeTerm = mergeBB->getLastOp();
  bool compact = mergeTerm && isa<BranchOp>(mergeTerm);

  for (auto *phi : mergeBB->getPhis()) {
    if (phi->getOperandCount() != 2)
      continue;
    Op *fromPred = mkoPhiIncoming(cast<PhiOp>(phi), bb);
    Op *fromThen = mkoPhiIncoming(cast<PhiOp>(phi), thenBB);
    if (!fromPred || !fromThen)
      continue;
    if (!mkoSame(fromPred, acc)) {
      dbg("acc-mismatch");
      continue;
    }
    if (!mkoSame(fromThen, addInst)) {
      dbg("add-mismatch");
      continue;
    }
    auto *accPhi = cast<PhiOp>(phi);
    if (!mkoAccLatchMerge(mergeBB, accPhi)) {
      dbg("merge-shape");
      continue;
    }
    if (mkoGuardedMatmulStep(thenBB, addInst, accPhi)) {
      dbg("matmul-step");
      continue;
    }

    if (compact) {
      mkoFinalizeGuardedAccumCompact(func, bb, thenBB, mergeBB, br, cond, acc, addend,
                                     addInst, accPhi, addInElse);
    } else {
      if (addInElse)
        continue;
      BasicBlock *header = nullptr;
      if (!mkoLatchMerge(mergeBB, header))
        continue;
      mkoFinalizeGuardedAccum(func, bb, thenBB, mergeBB, header, br, cond, acc, addend,
                              addInst, accPhi);
    }
    if (debug)
      std::cerr << "[guarded-accum] lifted compact=" << compact
                << " addInElse=" << addInElse << "\n";
    converted++;
    return true;
  }
  dbg("no-phi");
  return false;
}

// --- MatTransposePair -------------------------------------------------------

struct TransposeCtx {
  std::string srcGlob;
  std::string dstGlob;
  int dstCols = 0;
  BasicBlock *outerExit = nullptr;
  std::unordered_set<BasicBlock*> skip;
};

bool mkoMatchTransposeStore(StoreOp *store, Op *iIv, Op *jIv,
                            const std::map<std::string, GlobalOp*> &gMap,
                            TransposeCtx &ctx) {
  auto *val = store->DEF(0);
  auto *load = dyn_cast<LoadOp>(val);
  if (!load)
    return false;

  GridAccess st, ld;
  auto storeNames = mkoGlobalsIn(store->DEF(1));
  auto loadNames = mkoGlobalsIn(load->DEF(0));
  if (storeNames.size() != 1 || loadNames.size() != 1)
    return false;
  if (*storeNames.begin() == *loadNames.begin())
    return false;

  int stRows = 0, stCols = 0;
  int ldRows = 0, ldCols = 0;
  if (!mkoGlobalMatrix(gMap, *storeNames.begin(), stRows, stCols))
    return false;
  if (!mkoGlobalMatrix(gMap, *loadNames.begin(), ldRows, ldCols))
    return false;

  if (!mkoParseGrid(store->DEF(1), stCols * 4, st))
    return false;
  if (!mkoParseGrid(load->DEF(0), ldCols * 4, ld))
    return false;
  if (st.glob != *storeNames.begin() || ld.glob != *loadNames.begin())
    return false;

  if (!mkoSame(st.row, iIv) || !mkoSame(st.col, jIv))
    return false;
  if (!mkoSame(ld.row, jIv) || !mkoSame(ld.col, iIv))
    return false;

  ctx.srcGlob = ld.glob;
  ctx.dstGlob = st.glob;
  ctx.dstCols = stCols;
  return true;
}

bool mkoFindTransposeNest(FuncOp *func, const LoopForest &forest,
                          const std::map<std::string, GlobalOp*> &gMap,
                          TransposeCtx &ctx) {
  for (auto *bb : func->getRegion()->getBlocks()) {
    for (auto *op : bb->getOps()) {
      auto *store = dyn_cast<StoreOp>(op);
      if (!store)
        continue;

      auto *jLoop = mkoInnermostLoop(forest, bb);
      if (!jLoop || jLoop->bbs.size() > 4)
        continue;
      auto *iLoop = mkoParentLoop(jLoop, forest);
      if (!iLoop)
        continue;

      Op *iIv = mkoLoopIv(iLoop);
      Op *jIv = mkoLoopIv(jLoop);
      Op *iBound = mkoLoopBound(iLoop);
      Op *jBound = mkoLoopBound(jLoop);
      if (!iIv || !jIv || !iBound || !jBound)
        continue;
      if (!mkoSameBound(iBound, jBound))
        continue;

      TransposeCtx cand;
      if (!mkoMatchTransposeStore(store, iIv, jIv, gMap, cand))
        continue;

      BasicBlock *outerExit = nullptr;
      auto term = iLoop->header->getLastOp();
      if (auto *ibr = dyn_cast<BranchOp>(term)) {
        for (BasicBlock *succ : { TARGET(ibr), ELSE(ibr) }) {
          if (succ && !iLoop->contains(succ)) {
            outerExit = succ;
            break;
          }
        }
      }
      if (!outerExit)
        continue;

      ctx = cand;
      ctx.outerExit = outerExit;
      ctx.skip.clear();
      for (auto *lb : iLoop->bbs)
        ctx.skip.insert(lb);
      return true;
    }
  }
  return false;
}

bool mkoIsKJLoad(GridAccess &ga, Op *kIv, Op *jIv) {
  return mkoSame(ga.row, kIv) && !mkoSame(ga.col, kIv) && mkoUsesNeedle(ga.col, jIv);
}

bool mkoFindIJK(const LoopForest &forest, BasicBlock *bb,
                Op *&iIv, Op *&jIv, Op *&kIv) {
  auto *kLoop = mkoInnermostLoop(forest, bb);
  auto *jLoop = mkoParentLoop(kLoop, forest);
  auto *iLoop = mkoParentLoop(jLoop, forest);
  if (!iLoop || !jLoop || !kLoop)
    return false;
  iIv = mkoLoopIv(iLoop);
  jIv = mkoLoopIv(jLoop);
  kIv = mkoLoopIv(kLoop);
  return iIv && jIv && kIv;
}

bool mkoRewriteKJLoad(LoadOp *load, BasicBlock *bb,
                      const TransposeCtx &ctx, const LoopForest &forest,
                      const std::map<std::string, GlobalOp*> &gMap,
                      Builder &builder, int &rewrites) {
  if (ctx.skip.count(bb))
    return false;
  if (!mkoReachableFrom(ctx.outerExit, bb))
    return false;

  Op *iIv = nullptr, *jIv = nullptr, *kIv = nullptr;
  if (!mkoFindIJK(forest, bb, iIv, jIv, kIv))
    return false;

  GridAccess ga;
  int srcRows = 0, srcCols = 0;
  if (!mkoGlobalMatrix(gMap, ctx.srcGlob, srcRows, srcCols))
    return false;
  if (!mkoParseGrid(load->DEF(0), srcCols * 4, ga))
    return false;
  if (ga.glob != ctx.srcGlob)
    return false;
  if (!mkoIsKJLoad(ga, kIv, jIv))
    return false;

  builder.setBeforeOp(load);
  auto *dstBase = mkoGlobalPtr(builder, gMap, ctx.dstGlob);
  if (!dstBase)
    return false;
  auto *newAddr = mkoMatrixAddr(builder, dstBase, ga.col, ga.row, ctx.dstCols * 4);
  builder.replace<LoadOp>(load, load->getResultType(), { newAddr }, load->getAttrs());
  rewrites++;
  return true;
}

} // namespace

// --- GuardedAccum pass ------------------------------------------------------

std::map<std::string, int> GuardedAccum::stats() {
  return { { "lifted", lifted } };
}

void GuardedAccum::run() {
  for (auto *func : collectFuncs())
    func->getRegion()->updatePreds();

  for (auto *func : collectFuncs()) {
    bool local = true;
    while (local) {
      local = false;
      for (auto *bb : func->getRegion()->getBlocks()) {
        if (mkoTryGuardedAccum(func, bb, lifted)) {
          local = true;
          break;
        }
      }
    }
  }
}

// --- MatTransposePair pass --------------------------------------------------

std::map<std::string, int> MatTransposePair::stats() {
  return { { "rewrites", rewrites } };
}

void MatTransposePair::run() {
  LoopAnalysis loops(module);
  loops.run();
  auto gMap = getGlobalMap();
  Builder builder;

  for (auto *func : collectFuncs()) {
    func->getRegion()->updatePreds();
    auto forest = loops.runImpl(func->getRegion());
    TransposeCtx ctx;
    if (!mkoFindTransposeNest(func, forest, gMap, ctx))
      continue;

    std::vector<LoadOp*> candidates;
    for (auto *bb : func->getRegion()->getBlocks()) {
      if (ctx.skip.count(bb))
        continue;
      for (auto *op : bb->getOps()) {
        if (auto *load = dyn_cast<LoadOp>(op))
          candidates.push_back(load);
      }
    }

    for (auto *load : candidates) {
      mkoRewriteKJLoad(load, load->getParent(), ctx, forest, gMap, builder, rewrites);
    }
  }
}
