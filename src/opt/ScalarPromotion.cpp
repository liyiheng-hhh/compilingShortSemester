#include "ScalarPromotion.h"

#include "../codegen/Attrs.h"
#include "../codegen/CodeGen.h"
#include "LoopPasses.h"
#include "Passes.h"

#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace sys;

namespace {

bool envFlag(const char *name) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return false;
  return std::strcmp(v, "0") != 0 && std::strcmp(v, "false") != 0;
}

bool isScalarAlloca(AllocaOp *alloca) {
  if (!alloca)
    return false;
  if (!alloca->has<SizeAttr>())
    return false;
  const size_t sz = SIZE(alloca);
  if (sz != 4 && sz != 8)
    return false;
  for (auto *use : alloca->getUses()) {
    if (!isa<LoadOp>(use) && !isa<StoreOp>(use))
      return false;
    if (isa<StoreOp>(use) && use->DEF(0) == alloca)
      return false;
  }
  return true;
}

void collectAllocasUsedInLoop(LoopInfo *loop, std::set<AllocaOp *> &out) {
  auto scan = [&](BasicBlock *bb) {
    if (!bb)
      return;
    for (auto *op : bb->getOps()) {
      if (auto *load = dyn_cast<LoadOp>(op)) {
        if (auto *a = dyn_cast<AllocaOp>(load->getOperand().defining))
          out.insert(a);
      }
      if (auto *store = dyn_cast<StoreOp>(op)) {
        if (auto *a = dyn_cast<AllocaOp>(store->DEF(1)))
          out.insert(a);
      }
    }
  };
  scan(loop->preheader);
  for (auto *bb : loop->bbs)
    scan(bb);
}

bool loopHasCall(LoopInfo *loop) {
  for (auto *bb : loop->bbs) {
    for (auto *op : bb->getOps()) {
      if (isa<CallOp>(op))
        return true;
    }
  }
  return false;
}

bool hasLoadOrStoreOf(LoopInfo *loop, AllocaOp *slot) {
  auto usesSlot = [&](BasicBlock *bb) {
    for (auto *op : bb->getOps()) {
      if (auto *load = dyn_cast<LoadOp>(op)) {
        if (load->getOperand().defining == slot)
          return true;
      }
      if (auto *store = dyn_cast<StoreOp>(op)) {
        if (store->DEF(1) == slot)
          return true;
      }
    }
    return false;
  };
  if (usesSlot(loop->preheader))
    return true;
  for (auto *bb : loop->bbs) {
    if (usesSlot(bb))
      return true;
  }
  return false;
}

bool valueUsesLoadOfSlot(Op *v, AllocaOp *slot, std::unordered_set<Op *> &vis) {
  if (!v || !vis.insert(v).second)
    return false;
  if (auto *load = dyn_cast<LoadOp>(v))
    return load->getOperand().defining == slot;
  if (isa<PhiOp>(v) || isa<CallOp>(v) || isa<LoadOp>(v))
    return false;
  for (int i = 0; i < v->getOperandCount(); ++i) {
    if (valueUsesLoadOfSlot(v->DEF(i), slot, vis))
      return true;
  }
  return false;
}

bool storesDeriveFromLoad(LoopInfo *loop, AllocaOp *slot) {
  auto check = [&](BasicBlock *bb) {
    for (auto *op : bb->getOps()) {
      auto *store = dyn_cast<StoreOp>(op);
      if (!store || store->DEF(1) != slot)
        continue;
      std::unordered_set<Op *> vis;
      if (!valueUsesLoadOfSlot(store->DEF(0), slot, vis))
        return false;
    }
    return true;
  };
  if (!check(loop->preheader))
    return false;
  for (auto *bb : loop->bbs) {
    if (!check(bb))
      return false;
  }
  return true;
}

Value::Type slotValueType(LoopInfo *loop, AllocaOp *slot) {
  auto pick = [&](BasicBlock *bb) -> Value::Type {
    for (auto *op : bb->getOps()) {
      if (auto *load = dyn_cast<LoadOp>(op)) {
        if (load->getOperand().defining == slot)
          return load->getResultType();
      }
    }
    return Value::i32;
  };
  if (loop->preheader) {
    auto ty = pick(loop->preheader);
    if (ty != Value::i32)
      return ty;
  }
  for (auto *bb : loop->bbs) {
    auto ty = pick(bb);
    if (ty != Value::i32)
      return ty;
  }
  return slot->has<FPAttr>() ? Value::f32 : Value::i32;
}

LoadOp *findLastLoadInBlock(BasicBlock *bb, AllocaOp *slot) {
  LoadOp *last = nullptr;
  if (!bb)
    return last;
  for (auto *op : bb->getOps()) {
    if (auto *load = dyn_cast<LoadOp>(op)) {
      if (load->getOperand().defining == slot)
        last = load;
    }
  }
  return last;
}

Op *mergePredOutVals(BasicBlock *bb, LoopInfo *loop,
                     const std::map<BasicBlock *, Op *> &outVal) {
  Op *merged = nullptr;
  for (auto *pred : bb->preds) {
    if (!loop->contains(pred))
      continue;
    auto it = outVal.find(pred);
    if (it == outVal.end())
      return nullptr;
    if (!merged)
      merged = it->second;
    else if (merged != it->second)
      return nullptr;
  }
  return merged;
}

Op *simulateBlock(AllocaOp *slot, BasicBlock *bb, Op *inVal) {
  Op *cur = inVal;
  for (auto *op : bb->getOps()) {
    if (auto *store = dyn_cast<StoreOp>(op)) {
      if (store->DEF(1) == slot)
        cur = store->DEF(0);
    }
  }
  return cur;
}

void applySlotInBlock(AllocaOp *slot, BasicBlock *bb, Op *startVal) {
  Op *cur = startVal;
  std::vector<Op *> toErase;
  for (auto *op : bb->getOps()) {
    if (auto *load = dyn_cast<LoadOp>(op)) {
      if (load->getOperand().defining == slot) {
        load->replaceAllUsesWith(cur);
        toErase.push_back(load);
      }
    }
    if (auto *store = dyn_cast<StoreOp>(op)) {
      if (store->DEF(1) == slot) {
        cur = store->DEF(0);
        toErase.push_back(store);
      }
    }
  }
  for (auto *op : toErase)
    op->erase();
}

void insertPhiAfterExistingPhis(Builder &builder, BasicBlock *header) {
  builder.setToBlockStart(header);
  auto phis = header->getPhis();
  if (!phis.empty())
    builder.setAfterOp(phis.back());
}

bool promoteAllocaInLoop(LoopInfo *loop, AllocaOp *slot, Builder &builder, bool debug) {
  if (!loop->preheader || loop->latches.size() != 1)
    return false;
  if (!isScalarAlloca(slot))
    return false;
  if (loopHasCall(loop))
    return false;
  if (!hasLoadOrStoreOf(loop, slot))
    return false;
  if (!storesDeriveFromLoad(loop, slot))
    return false;

  BasicBlock *header = loop->header;
  BasicBlock *latch = loop->getLatch();
  BasicBlock *preheader = loop->preheader;

  const Value::Type ty = slotValueType(loop, slot);
  const size_t sz = SIZE(slot);

  Op *preScalar = findLastLoadInBlock(preheader, slot);
  if (!preScalar) {
    builder.setBeforeOp(preheader->getLastOp());
    preScalar = builder.create<LoadOp>(ty, { Value(slot) }, { new SizeAttr(sz) });
  }

  insertPhiAfterExistingPhis(builder, header);
  PhiOp *phi = builder.create<PhiOp>();
  phi->pushOperand({ preScalar });
  phi->add<FromAttr>(preheader);

  std::map<BasicBlock *, Op *> outVal;
  bool progress = true;
  int limit = static_cast<int>(loop->bbs.size()) + 4;
  while (progress && limit-- > 0) {
    progress = false;
    for (auto *bb : loop->bbs) {
      Op *inVal = (bb == header) ? static_cast<Op *>(phi) : mergePredOutVals(bb, loop, outVal);
      if (!inVal)
        continue;
      Op *newOut = simulateBlock(slot, bb, inVal);
      auto it = outVal.find(bb);
      if (it == outVal.end() || it->second != newOut) {
        outVal[bb] = newOut;
        progress = true;
      }
    }
  }

  if (!outVal.count(latch))
    return false;

  phi->pushOperand({ outVal[latch] });
  phi->add<FromAttr>(latch);

  for (auto *bb : loop->bbs) {
    Op *inVal = (bb == header) ? static_cast<Op *>(phi) : mergePredOutVals(bb, loop, outVal);
    if (!inVal)
      continue;
    applySlotInBlock(slot, bb, inVal);
  }

  // Remove preheader stores that only seed the promoted slot if unused afterward.
  {
    std::vector<Op *> preErase;
    for (auto *op : preheader->getOps()) {
      if (auto *store = dyn_cast<StoreOp>(op)) {
        if (store->DEF(1) == slot)
          preErase.push_back(store);
      }
    }
    for (auto *op : preErase)
      op->erase();
  }

  for (auto *exitBB : loop->exits) {
    Op *wbVal = outVal.count(latch) ? outVal[latch] : static_cast<Op *>(phi);
    for (auto *exiting : loop->exitings) {
      if (!exiting->succs.count(exitBB))
        continue;
      auto it = outVal.find(exiting);
      if (it != outVal.end())
        wbVal = it->second;
      break;
    }
    builder.setToBlockStart(exitBB);
    auto phis = exitBB->getPhis();
    if (!phis.empty())
      builder.setAfterOp(phis.back());
    builder.create<StoreOp>({ Value(wbVal), Value(slot) }, { new SizeAttr(sz) });
  }

  if (debug) {
    std::cerr << "[scalar-promotion] promoted alloca in loop header "
              << header << " (" << loop->bbs.size() << " blocks)\n";
  }
  return true;
}

}  // namespace

ScalarPromotion::ScalarPromotion(ModuleOp *module) : Pass(module) {
  debug = envFlag("SYSY_CC_SCALAR_PROMOTION_DEBUG");
}

std::map<std::string, int> ScalarPromotion::stats() {
  return { { "promoted", promoted } };
}

void ScalarPromotion::runOnFunc(FuncOp *func) {
  if (!func)
    return;

  // Loop rotate / canonicalize can expose allocas that early Mem2Reg skipped.
  Mem2Reg lateMem2Reg(module);
  lateMem2Reg.promoteFunc(func);
  promoted += lateMem2Reg.stats().at("lowered-alloca");

  LoopAnalysis analysis(module);
  auto forest = analysis.runImpl(func->getRegion());

  std::vector<LoopInfo *> ordered = forest.getLoops();
  std::sort(ordered.begin(), ordered.end(), [](LoopInfo *a, LoopInfo *b) {
    return a->bbs.size() < b->bbs.size();
  });

  Builder builder;
  for (auto *loop : ordered) {
    if (!loop->preheader || loop->latches.size() != 1)
      continue;
    if (loop->bbs.size() < 2)
      continue;

    std::set<AllocaOp *> slots;
    collectAllocasUsedInLoop(loop, slots);
    for (auto *slot : slots) {
      if (promoteAllocaInLoop(loop, slot, builder, debug))
        promoted++;
    }
  }
}

void ScalarPromotion::run() {
  for (auto *func : collectFuncs())
    runOnFunc(func);
}
