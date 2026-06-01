#include "LoopNestSplit.h"

#include <cstdlib>
#include <cstring>

#include "LoopPasses.h"
#include "../pre-opt/PreAttrs.h"

using namespace sys;

namespace {

bool envFlag(const char *name) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return false;
  return std::strcmp(v, "0") != 0 && std::strcmp(v, "false") != 0;
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

}  // namespace

LoopNestSplit::LoopNestSplit(ModuleOp *module) : Pass(module) {
  debug = envFlag("SYSY_CC_LOOP_NEST_SPLIT_DEBUG");
}

std::map<std::string, int> LoopNestSplit::stats() {
  return {
    { "candidates", candidates },
    { "marked", marked },
    { "split", marked },
  };
}

void LoopNestSplit::runOnFunc(FuncOp *func) {
  if (!func)
    return;

  LoopAnalysis analysis(module);
  auto forest = analysis.runImpl(func->getRegion());
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
    lnsMarkKLoopInterior(kLoop);
    marked++;
    if (debug) {
      std::cerr << "[loop-nest-split] marked interior k header="
                << bbmap[kLoop->header] << "\n";
    }
  }
}

void LoopNestSplit::run() {
  for (auto *func : collectFuncs())
    runOnFunc(func);
}
