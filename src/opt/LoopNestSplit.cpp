#include "LoopNestSplit.h"

#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <vector>

#include "LoopPasses.h"
#include "../pre-opt/PreAttrs.h"

using namespace sys;

namespace {

constexpr int kDefaultKPad = 2;
constexpr int kMinKernelTrip = 5;

bool envFlag(const char *name) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return false;
  return std::strcmp(v, "0") != 0 && std::strcmp(v, "false") != 0;
}

int envInt(const char *name, int fallback) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return fallback;
  int x = std::atoi(v);
  return x > 0 ? x : fallback;
}

bool cfgSplitEnabled() {
  // Opt-in: k-border peel doubles matmul kernel size on platform (~185→360 asm lines)
  // without RowScratch win on guarded matmul2. Keep interior marking only by default.
  return envFlag("SYSY_CC_ENABLE_NEST_CFG_SPLIT");
}

bool lnsKLoopHasInnerGuard(LoopInfo *kLoop) {
  if (!kLoop)
    return false;
  auto header = kLoop->header;
  BasicBlock *latch = kLoop->latches.size() == 1 ? kLoop->getLatch() : nullptr;
  for (auto *bb : kLoop->getBlocks()) {
    if (bb == header || bb == latch)
      continue;
    for (auto *op : bb->getOps()) {
      if (isa<ModIOp>(op) || isa<SelectOp>(op))
        return true;
      if (isa<BranchOp>(op) && cast<BranchOp>(op)->has<ElseAttr>())
        return true;
    }
  }
  return false;
}

bool lnsIsMatmulLikeNest(LoopInfo *kLoop) {
  if (!kLoop || !kLoop->subloops.empty())
    return false;
  LoopInfo *jLoop = kLoop->parent;
  if (!jLoop)
    return false;
  LoopInfo *iLoop = jLoop->parent;
  return iLoop != nullptr;
}

void lnsMarkKLoopInterior(LoopInfo *kLoop) {
  if (!kLoop || !kLoop->header)
    return;
  for (auto *op : kLoop->header->getOps()) {
    if (!op->has<RsmInteriorAttr>())
      op->add<RsmInteriorAttr>();
  }
  if (auto *term = kLoop->header->getLastOp()) {
    if (!term->has<RsmInteriorAttr>())
      term->add<RsmInteriorAttr>();
  }
  if (kLoop->induction && !kLoop->induction->has<RsmInteriorAttr>())
    kLoop->induction->add<RsmInteriorAttr>();
}

Op *lnsResolve(Op *v) {
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

int lnsConstInt(Op *op) {
  op = lnsResolve(op);
  if (isa<IntOp>(op))
    return V(op);
  return -1;
}

Op *lnsFindUnitIvPhi(LoopInfo *loop) {
  if (!loop || loop->latches.size() != 1)
    return nullptr;
  auto *latch = loop->getLatch();
  auto *header = loop->header;
  if (!latch || !header)
    return nullptr;
  for (auto *phi : header->getPhis()) {
    if (phi->getResultType() != Value::i32)
      continue;
    const auto &ops = phi->getOperands();
    const auto &attrs = phi->getAttrs();
    Op *latchVal = nullptr;
    for (int i = 0; i < (int)attrs.size(); i++) {
      auto *from = dyn_cast<FromAttr>(attrs[i]);
      if (!from)
        continue;
      if (from->bb == latch)
        latchVal = ops[i].defining;
    }
    if (!latchVal || !isa<AddIOp>(latchVal))
      continue;
    Op *a = latchVal->DEF(0);
    Op *b = latchVal->DEF(1);
    bool unit = (a == phi && isa<IntOp>(b) && V(b) == 1) ||
                (b == phi && isa<IntOp>(a) && V(a) == 1);
    if (unit)
      return phi;
  }
  return nullptr;
}

Op *lnsFindLoopStop(LoopInfo *loop) {
  if (!loop || loop->latches.size() != 1)
    return nullptr;
  auto *latch = loop->getLatch();
  auto *term = latch->getLastOp();
  if (isa<BranchOp>(term) && term->getOperandCount() == 1) {
    if (auto *lt = dyn_cast<LtOp>(term->DEF(0)))
      return lt->DEF(1);
  }
  return loop->stop;
}

struct LnsGuardShape {
  BasicBlock *guardBB = nullptr;
  BranchOp *guardBr = nullptr;
  BasicBlock *thenBB = nullptr;
  BasicBlock *latchBB = nullptr;
  PhiOp *headerAccPhi = nullptr;
  PhiOp *latchAccPhi = nullptr;
  Op *kIv = nullptr;
  Op *stop = nullptr;
  Op *accPred = nullptr;
  AddIOp *thenAdd = nullptr;
  Op *guardCond = nullptr;
  Op *thenAddend = nullptr;
  bool addInElse = false;
};

AddIOp *lnsFindThenAdd(BasicBlock *thenBB, Op *&acc, Op *&addend) {
  acc = addend = nullptr;
  for (auto it = thenBB->getOps().rbegin(); it != thenBB->getOps().rend(); ++it) {
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

Op *lnsPhiIncoming(PhiOp *phi, BasicBlock *from) {
  if (!phi || !from)
    return nullptr;
  const auto &attrs = phi->getAttrs();
  for (int i = 0; i < phi->getOperandCount(); i++) {
    if (i < (int)attrs.size() && FROM(attrs[i]) == from)
      return phi->DEF(i);
  }
  return nullptr;
}

bool lnsParseGuardedKLoop(LoopInfo *kLoop, LnsGuardShape &shape) {
  shape = {};
  if (!kLoop || kLoop->latches.size() != 1)
    return false;
  shape.latchBB = kLoop->getLatch();
  shape.kIv = lnsFindUnitIvPhi(kLoop);
  if (!shape.kIv)
    return false;
  shape.stop = lnsFindLoopStop(kLoop);
  if (!shape.stop || lnsConstInt(shape.stop) <= 0)
    return false;

  for (auto *bb : kLoop->getBlocks()) {
    if (bb == kLoop->header || bb == shape.latchBB)
      continue;
    for (auto *op : bb->getOps()) {
      auto *br = dyn_cast<BranchOp>(op);
      if (!br || !br->has<ElseAttr>() || br->getOperandCount() != 1)
        continue;
      BasicBlock *a = TARGET(br);
      BasicBlock *b = ELSE(br);
      for (int ori = 0; ori < 2; ori++) {
        BasicBlock *then = ori ? b : a;
        BasicBlock *merge = ori ? a : b;
        if (!then || !merge || merge != shape.latchBB)
          continue;
        if (then->getLastOp() == nullptr || !then->getLastOp()->has<TargetAttr>() ||
            TARGET(then->getLastOp()) != merge)
          continue;
        Op *acc = nullptr;
        Op *addend = nullptr;
        auto *add = lnsFindThenAdd(then, acc, addend);
        if (!add)
          continue;
        shape.guardBB = bb;
        shape.guardBr = br;
        shape.thenBB = then;
        shape.thenAdd = add;
        shape.accPred = acc;
        shape.guardCond = br->DEF(0);
        shape.thenAddend = addend;
        shape.addInElse = (then == ELSE(br));
        break;
      }
      if (shape.guardBB)
        break;
    }
    if (shape.guardBB)
      break;
  }
  if (!shape.guardBB || !shape.thenBB)
    return false;

  for (auto *op : shape.latchBB->getOps()) {
    auto *phi = dyn_cast<PhiOp>(op);
    if (!phi)
      continue;
    if (lnsPhiIncoming(phi, shape.guardBB) == shape.accPred ||
        lnsPhiIncoming(phi, shape.thenBB) == shape.thenAdd) {
      shape.latchAccPhi = phi;
      break;
    }
  }
  for (auto *op : kLoop->header->getPhis()) {
    auto *phi = dyn_cast<PhiOp>(op);
    if (!phi || phi == shape.kIv)
      continue;
    if (shape.latchAccPhi && lnsPhiIncoming(shape.latchAccPhi, shape.guardBB) == phi)
      shape.headerAccPhi = phi;
  }
  return shape.latchAccPhi && shape.headerAccPhi;
}

bool lnsIsMod2Guard(const LnsGuardShape &shape) {
  auto *cond = shape.guardCond;
  if (auto *mod = dyn_cast<ModIOp>(cond))
    return mod->getOperandCount() == 2 && isa<IntOp>(mod->DEF(1)) && V(mod->DEF(1)) == 2;
  return false;
}

bool lnsRedirectTerminator(BasicBlock *from, BasicBlock *to) {
  auto *term = from->getLastOp();
  if (auto *go = dyn_cast<GotoOp>(term)) {
    TARGET(go) = to;
    return true;
  }
  if (auto *br = dyn_cast<BranchOp>(term)) {
    if (br->has<TargetAttr>())
      TARGET(br) = to;
    else if (br->has<ElseAttr>())
      ELSE(br) = to;
    else
      return false;
    return true;
  }
  return false;
}

Op *lnsMapOperand(Op *v, Op *kIv, Op *kConst, const std::set<BasicBlock*> &loopBlocks,
                  std::map<Op*, Op*> &map, Builder &builder);

Op *lnsCloneExpr(Op *op, Builder &builder, Op *kIv, Op *kConst,
                 const std::set<BasicBlock*> &loopBlocks,
                 std::map<Op*, Op*> &map) {
  if (!op)
    return nullptr;
  if (!loopBlocks.count(op->getParent()) && op != kIv)
    return op;
  auto it = map.find(op);
  if (it != map.end())
    return it->second;

  if (isa<PhiOp>(op))
    return op;

  std::vector<Value> ops;
  for (int i = 0; i < op->getOperandCount(); i++)
    ops.push_back(lnsMapOperand(op->DEF(i), kIv, kConst, loopBlocks, map, builder));

  Op *cloned = nullptr;
  if (isa<IntOp>(op))
    cloned = builder.create<IntOp>(op->getAttrs());
  else if (isa<AddIOp>(op))
    cloned = builder.create<AddIOp>(ops);
  else if (isa<AddLOp>(op))
    cloned = builder.create<AddLOp>(ops, op->getAttrs());
  else if (isa<MulIOp>(op))
    cloned = builder.create<MulIOp>(ops);
  else if (isa<ModIOp>(op))
    cloned = builder.create<ModIOp>(ops);
  else if (isa<AndIOp>(op))
    cloned = builder.create<AndIOp>(ops);
  else if (isa<SubIOp>(op))
    cloned = builder.create<SubIOp>(ops);
  else if (isa<EqOp>(op))
    cloned = builder.create<EqOp>(ops);
  else if (isa<LtOp>(op))
    cloned = builder.create<LtOp>(ops);
  else if (isa<LoadOp>(op))
    cloned = builder.create<LoadOp>(op->getResultType(), ops, op->getAttrs());
  else
    return op;

  map[op] = cloned;
  return cloned;
}

Op *lnsMapOperand(Op *v, Op *kIv, Op *kConst, const std::set<BasicBlock*> &loopBlocks,
                  std::map<Op*, Op*> &map, Builder &builder) {
  if (v == kIv)
    return kConst;
  auto it = map.find(v);
  if (it != map.end())
    return it->second;
  if (v && loopBlocks.count(v->getParent()))
    return lnsCloneExpr(v, builder, kIv, kConst, loopBlocks, map);
  return v;
}

Op *lnsBuildScaledAddend(Builder &builder, Op *cond, Op *addend, bool addInElse) {
  if (!addInElse)
    return builder.create<MulIOp>(std::vector<Value>{ cond, addend });

  if (auto *mod = dyn_cast<ModIOp>(cond)) {
    if (mod->getOperandCount() == 2 && isa<IntOp>(mod->DEF(1)) && V(mod->DEF(1)) == 2) {
      auto one = builder.create<IntOp>({ new IntAttr(1) });
      auto bit = builder.create<AndIOp>(std::vector<Value>{ mod->DEF(0), one });
      auto even = builder.create<SubIOp>(std::vector<Value>{ one, bit });
      return builder.create<MulIOp>(std::vector<Value>{ addend, even });
    }
  }

  auto zero = builder.create<IntOp>({ new IntAttr(0) });
  auto active = builder.create<EqOp>(std::vector<Value>{ cond, zero });
  return builder.create<MulIOp>(std::vector<Value>{ addend, active });
}

bool lnsRetargetPhiIncoming(PhiOp *phi, BasicBlock *oldFrom, BasicBlock *newFrom,
                            Op *val) {
  if (!phi || !oldFrom || !newFrom)
    return false;
  const auto &attrs = phi->getAttrs();
  for (int i = 0; i < phi->getOperandCount(); i++) {
    if (i < (int)attrs.size() && FROM(attrs[i]) == oldFrom) {
      phi->setOperand(i, val);
      cast<FromAttr>(attrs[i])->bb = newFrom;
      return true;
    }
  }
  return false;
}

bool lnsSetLoopStop(LoopInfo *kLoop, Op *kIv, Op *newStop) {
  if (!kLoop || !kIv || !newStop)
    return false;
  auto *latch = kLoop->getLatch();
  if (!latch)
    return false;
  auto *term = latch->getLastOp();
  if (!isa<BranchOp>(term) || term->getOperandCount() != 1)
    return false;
  Op *cond = term->DEF(0);
  auto *lt = dyn_cast<LtOp>(cond);
  if (!lt)
    return false;
  Op *cmpIv = lt->DEF(0);
  if (auto *add = dyn_cast<AddIOp>(cmpIv)) {
    if (add->DEF(0) != kIv && add->DEF(1) != kIv)
      return false;
  } else if (cmpIv != kIv) {
    return false;
  }
  lt->setOperand(1, Value(newStop));
  return true;
}

Op *lnsEmitGuardedStep(BasicBlock *stepBB, const LnsGuardShape &shape, Op *kConst,
                       Op *accIn, BasicBlock *nextBB,
                       const std::set<BasicBlock*> &loopBlocks) {
  Builder builder;
  builder.setToBlockEnd(stepBB);
  std::map<Op*, Op*> map;

  for (auto *op : shape.guardBB->getOps()) {
    if (op == shape.guardBr)
      break;
    lnsCloneExpr(op, builder, shape.kIv, kConst, loopBlocks, map);
  }

  Op *cond = lnsMapOperand(shape.guardCond, shape.kIv, kConst, loopBlocks, map, builder);
  Op *addend = lnsMapOperand(shape.thenAddend, shape.kIv, kConst, loopBlocks, map, builder);

  for (auto *op : shape.thenBB->getOps()) {
    if (op == shape.thenAdd || isa<GotoOp>(op))
      break;
    lnsCloneExpr(op, builder, shape.kIv, kConst, loopBlocks, map);
  }

  Op *scaled = lnsBuildScaledAddend(builder, cond, addend, shape.addInElse);
  auto *accOut = builder.create<AddIOp>(std::vector<Value>{ accIn, scaled });
  builder.create<GotoOp>({ new TargetAttr(nextBB) });
  return accOut;
}

bool lnsPeelLowBorder(FuncOp *func, LoopInfo *kLoop, const LnsGuardShape &shape,
                      int kPad, const std::set<BasicBlock*> &loopBlocks,
                      BasicBlock *&lastPeel, Op *&accOut) {
  Region *region = func->getRegion();
  BasicBlock *pre = kLoop->preheader;
  Op *acc = lnsPhiIncoming(shape.headerAccPhi, pre);
  if (!acc)
    return false;

  std::vector<BasicBlock*> steps;
  steps.reserve(kPad);
  for (int k = 0; k < kPad; k++)
    steps.push_back(region->insertAfter(k == 0 ? pre : steps.back()));
  if (!lnsRedirectTerminator(pre, steps.front()))
    return false;

  for (int k = 0; k < kPad; k++) {
    BasicBlock *next = (k + 1 == kPad) ? kLoop->header : steps[k + 1];
    Builder bk;
    bk.setToBlockEnd(steps[k]);
    auto *kConst = bk.create<IntOp>({ new IntAttr(k) });
    acc = lnsEmitGuardedStep(steps[k], shape, kConst, acc, next, loopBlocks);
  }

  lastPeel = steps.back();
  Builder bk;
  bk.setBeforeOp(lastPeel->getLastOp());
  auto *kStart = bk.create<IntOp>({ new IntAttr(kPad) });
  if (!lnsRetargetPhiIncoming(shape.headerAccPhi, pre, lastPeel, acc))
    return false;
  if (!lnsRetargetPhiIncoming(cast<PhiOp>(shape.kIv), pre, lastPeel, kStart))
    return false;

  accOut = acc;
  region->updatePreds();
  return true;
}

bool lnsPeelHighBorder(FuncOp *func, LoopInfo *kLoop, const LnsGuardShape &shape,
                       int kPad, int trip, const std::set<BasicBlock*> &loopBlocks) {
  Region *region = func->getRegion();
  BasicBlock *exitBB = kLoop->getExit();
  auto *latchBr = dyn_cast<BranchOp>(shape.latchBB->getLastOp());
  if (!latchBr)
    return false;

  BasicBlock *oldExit = (TARGET(latchBr) == exitBB) ? exitBB :
                        (ELSE(latchBr) == exitBB ? exitBB : nullptr);
  if (!oldExit)
    return false;

  std::vector<BasicBlock*> steps;
  steps.reserve(kPad);
  for (int i = 0; i < kPad; i++)
    steps.push_back(region->insert(exitBB));

  if (TARGET(latchBr) == oldExit)
    TARGET(latchBr) = steps.front();
  else
    ELSE(latchBr) = steps.front();

  Op *acc = shape.latchAccPhi;
  if (!acc)
    acc = shape.accPred;

  for (int i = 0; i < kPad; i++) {
    int k = trip - kPad + i;
    BasicBlock *next = (i + 1 == kPad) ? exitBB : steps[i + 1];
    Builder bk;
    bk.setToBlockEnd(steps[i]);
    auto *kConst = bk.create<IntOp>({ new IntAttr(k) });
    acc = lnsEmitGuardedStep(steps[i], shape, kConst, acc, next, loopBlocks);
  }

  BasicBlock *lastStep = steps.back();
  for (auto *op : exitBB->getPhis()) {
    auto *phi = dyn_cast<PhiOp>(op);
    if (!phi)
      continue;
    for (int i = 0; i < phi->getOperandCount(); i++) {
      if (i < (int)phi->getAttrs().size() && FROM(phi->getAttrs()[i]) == shape.latchBB) {
        phi->setOperand(i, acc);
        cast<FromAttr>(phi->getAttrs()[i])->bb = lastStep;
      }
    }
  }

  region->updatePreds();
  (void)kLoop;
  return true;
}

bool lnsTryCfgSplit(FuncOp *func, LoopInfo *kLoop, int kPad, bool debug) {
  LnsGuardShape shape;
  if (!lnsParseGuardedKLoop(kLoop, shape)) {
    if (debug)
      std::cerr << "[loop-nest-split] cfg-split reject parse header="
                << bbmap[kLoop->header] << "\n";
    return false;
  }

  int trip = lnsConstInt(shape.stop);
  if (trip < kMinKernelTrip || trip < 2 * kPad + 1) {
    if (debug)
      std::cerr << "[loop-nest-split] skip cfg-split short trip=" << trip << "\n";
    return false;
  }

  if (!kLoop->preheader) {
    if (debug)
      std::cerr << "[loop-nest-split] cfg-split reject no-preheader\n";
    return false;
  }

  if (!lnsIsMod2Guard(shape)) {
    if (debug)
      std::cerr << "[loop-nest-split] cfg-split reject non-mod2-guard\n";
    return false;
  }

  std::set<BasicBlock*> loopBlocks;
  for (auto *bb : kLoop->getBlocks())
    loopBlocks.insert(bb);

  BasicBlock *lastPeel = nullptr;
  Op *accAfterLow = nullptr;
  if (!lnsPeelLowBorder(func, kLoop, shape, kPad, loopBlocks, lastPeel, accAfterLow)) {
    if (debug)
      std::cerr << "[loop-nest-split] cfg-split reject peel-low\n";
    return false;
  }

  Builder bStop;
  bStop.setBeforeOp(shape.latchBB->getLastOp());
  auto *kStopInterior = bStop.create<IntOp>({ new IntAttr(trip - kPad) });
  if (!lnsSetLoopStop(kLoop, shape.kIv, kStopInterior)) {
    if (debug)
      std::cerr << "[loop-nest-split] cfg-split reject set-stop\n";
    return false;
  }

  if (!lnsPeelHighBorder(func, kLoop, shape, kPad, trip, loopBlocks)) {
    if (debug)
      std::cerr << "[loop-nest-split] cfg-split reject peel-high\n";
    return false;
  }

  lnsMarkKLoopInterior(kLoop);
  func->getRegion()->updatePreds();
  func->getRegion()->updateDoms();

  if (debug)
    std::cerr << "[loop-nest-split] cfg-split kPad=" << kPad << " trip=" << trip
              << " header=" << bbmap[kLoop->header] << "\n";
  (void)accAfterLow;
  (void)lastPeel;
  return true;
}

}  // namespace

LoopNestSplit::LoopNestSplit(ModuleOp *module, LnsPhase phaseIn) : Pass(module), phase(phaseIn) {
  debug = envFlag("SYSY_CC_LOOP_NEST_SPLIT_DEBUG");
}

std::map<std::string, int> LoopNestSplit::stats() {
  return {
    { "candidates", candidates },
    { "marked", marked },
    { "split", cfgSplit },
  };
}

void LoopNestSplit::runOnFunc(FuncOp *func) {
  if (!func)
    return;

  LoopAnalysis analysis(module);
  auto forest = analysis.runImpl(func->getRegion());
  const int kPad = envInt("SYSY_CC_NEST_SPLIT_KPAD", kDefaultKPad);

  for (auto *kLoop : forest.getLoops()) {
    if (!lnsIsMatmulLikeNest(kLoop))
      continue;
    candidates++;
    if (!lnsKLoopHasInnerGuard(kLoop)) {
      if (debug)
        std::cerr << "[loop-nest-split] skip unguarded k header="
                  << bbmap[kLoop->header] << "\n";
      continue;
    }

    if (phase != LnsPhase::Mark && cfgSplitEnabled() &&
        lnsTryCfgSplit(func, kLoop, kPad, debug)) {
      cfgSplit++;
      marked++;
      continue;
    }

    if (phase != LnsPhase::CfgSplit) {
      lnsMarkKLoopInterior(kLoop);
      marked++;
      if (debug) {
        std::cerr << "[loop-nest-split] marked interior k header="
                  << bbmap[kLoop->header] << "\n";
      }
    }
  }
}

void LoopNestSplit::run() {
  for (auto *func : collectFuncs())
    runOnFunc(func);
}
