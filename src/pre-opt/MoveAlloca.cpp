#include "PrePasses.h"


// compiler2026-x phase-1 (pass surface)

using namespace sys;

void MoveAlloca::run() {
  auto funcs = collectFuncs();
  
  for (auto func : funcs) {
    auto allocas = func->findAll<AllocaOp>();
    auto region = func->getRegion();
    auto begin = region->insert(region->getFirstBlock());
    for (auto alloca : allocas)
      alloca->moveToEnd(begin);
  }
}
