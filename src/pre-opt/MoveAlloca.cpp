#include "PrePasses.h"

// compiler2026-x phase-D (trivial opt dedup)

using namespace sys;

void MoveAlloca::run() {
  for (auto func : collectFuncs()) {
    auto prologue = func->getRegion()->insert(func->getRegion()->getFirstBlock());
    for (auto slot : func->findAll<AllocaOp>())
      slot->moveToEnd(prologue);
  }
}
