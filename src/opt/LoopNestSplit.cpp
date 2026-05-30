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

}  // namespace

LoopNestSplit::LoopNestSplit(ModuleOp *module) : Pass(module) {
  debug = envFlag("SYSY_CC_LOOP_NEST_SPLIT_DEBUG");
}

std::map<std::string, int> LoopNestSplit::stats() {
  return {
    { "split", split }
  };
}

void LoopNestSplit::runOnFunc(FuncOp *func) {
  if (!func)
    return;
  // Phase 3.3: detect 3+ level nests (matmul-like) and mark the innermost k-loop
  // with RsmInteriorAttr so RowScratchMatmul can skip guarded-k reject.
  LoopAnalysis analysis(module);
  auto region = func->getRegion();
  auto forest = analysis.runImpl(region);
  for (auto *loop : forest.getLoops()) {
    if (loop->subloops.size() >= 2) {
      // 3+ level nest detected (matmul-like). Phase 3 uses env flag to tell
      // RowScratchMatmul to skip guarded-k. No attr marking in this stable version.
      split++;
      if (debug) {
        std::cerr << "[loop-nest-split] candidate nest with " << loop->subloops.size() << " subloops\n";
      }
    }
  }
}

void LoopNestSplit::run() {
  auto funcs = collectFuncs();
  for (auto *func : funcs) {
    runOnFunc(func);
  }
}