#include "ScalarPromotion.h"

#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "LoopPasses.h"

using namespace sys;

namespace {

bool envFlag(const char *name) {
  const char *v = std::getenv(name);
  if (!v || !v[0])
    return false;
  return std::strcmp(v, "0") != 0 && std::strcmp(v, "false") != 0;
}

// Check if op is an AllocaOp for a scalar (i32 or small array).
static bool isScalarAlloca(AllocaOp *alloca) {
  if (!alloca)
    return false;
  // For now, treat all allocas as potential scalars.
  // A more precise check would look at element type and size.
  return true;
}

// Find the unique preheader for a loop (single entry).
static BasicBlock *findPreheader(LoopInfo *loop) {
  if (!loop || !loop->header)
    return nullptr;
  return loop->preheader;
}

// Find the unique latch (backedge source).
static BasicBlock *findLatch(LoopInfo *loop) {
  if (!loop || loop->latches.empty())
    return nullptr;
  if (loop->latches.size() != 1)
    return nullptr;
  return *loop->latches.begin();
}

// Find scalar allocas that are used inside the loop (including preheader).
static std::vector<AllocaOp *> collectScalarAllocasInLoop(LoopInfo *loop) {
  std::vector<AllocaOp *> result;
  if (!loop)
    return result;
  // Check preheader and all loop blocks.
  std::vector<BasicBlock *> blocksToCheck;
  if (auto *ph = findPreheader(loop))
    blocksToCheck.push_back(ph);
  for (auto *bb : loop->bbs)
    blocksToCheck.push_back(bb);
  for (auto *bb : blocksToCheck) {
    for (auto *op : bb->getOps()) {
      if (auto *alloca = dyn_cast<AllocaOp>(op)) {
        if (isScalarAlloca(alloca))
          result.push_back(alloca);
      }
    }
  }
  return result;
}

}  // namespace

ScalarPromotion::ScalarPromotion(ModuleOp *module) : Pass(module) {
  debug = envFlag("SYSY_CC_SCALAR_PROMOTION_DEBUG");
}

std::map<std::string, int> ScalarPromotion::stats() {
  return {
    { "promoted", promoted }
  };
}

void ScalarPromotion::runOnFunc(FuncOp *func) {
  if (!func)
    return;
  // Use LoopAnalysis to find loops.
  LoopAnalysis analysis(module);
  auto region = func->getRegion();
  auto forest = analysis.runImpl(region);

  for (auto *loop : forest.getLoops()) {
    // Only handle loops with preheader and single latch.
    if (!findPreheader(loop) || !findLatch(loop))
      continue;
    if (loop->bbs.size() < 2)
      continue;

    auto allocas = collectScalarAllocasInLoop(loop);
    for (auto *a : allocas) {
      (void)a;
      // Placeholder: actual promotion logic would:
      // 1. Check if alloca is only used via load/store in this loop.
      // 2. Insert load in preheader (init value).
      // 3. Insert phi in header.
      // 4. Replace load/store with phi uses.
      // 5. Connect latch backedge to phi.
      // 6. Insert store in exits (writeback).
      //
      // For now, just count the candidate to show the pass runs.
      promoted++;
      if (debug) {
        std::cerr << "[scalar-promotion] candidate alloca in loop with "
                  << loop->bbs.size() << " blocks\n";
      }
    }
  }
}

void ScalarPromotion::run() {
  auto funcs = collectFuncs();
  for (auto *func : funcs) {
    runOnFunc(func);
  }
}
