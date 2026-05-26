#include "LoopPasses.h"
#include "Passes.h"

using namespace sys;

namespace {

bool tracesToGlobal(Op *addr, std::string &outName) {
  while (addr) {
    if (isa<GetGlobalOp>(addr)) {
      outName = NAME(addr);
      return true;
    }
    if (isa<AddIOp>(addr) || isa<AddLOp>(addr) || isa<MulIOp>(addr) || isa<MulLOp>(addr))
      addr = addr->DEF(0);
    else if (addr->getOperandCount() == 1)
      addr = addr->DEF(0);
    else
      break;
  }
  return false;
}

bool loopStoresGlobal(LoopInfo *loop, const std::string &gname) {
  for (auto *bb : loop->getBlocks()) {
    for (auto *op : bb->getOps()) {
      if (!isa<StoreOp>(op))
        continue;
      std::string stored;
      if (tracesToGlobal(op->DEF(1), stored) && stored == gname)
        return true;
    }
  }
  return false;
}

}  // namespace

std::map<std::string, int> HoistLoopGlobal::stats() {
  return { { "hoisted-globals", hoisted } };
}

void HoistLoopGlobal::run() {
  LoopAnalysis analysis(module);
  auto funcs = collectFuncs();

  Builder builder;
  for (auto *func : funcs) {
    auto *region = func->getRegion();
    auto forest = analysis.runImpl(region);
    for (auto *loop : forest.getLoops()) {
      if (loop->getParent())
        continue;
      runOnLoop(loop);
      for (auto *sub : loop->subloops)
        runOnLoop(sub);
    }
  }
}

void HoistLoopGlobal::runOnLoop(LoopInfo *loop) {
  auto *preheader = loop->preheader;
  if (!preheader)
    return;

  Builder builder;
  std::map<std::string, Op*> hoisted;
  std::vector<Op*> toReplace;

  for (auto *bb : loop->getBlocks()) {
    for (auto *op : bb->getOps()) {
      if (!isa<GetGlobalOp>(op))
        continue;
      const auto &gname = NAME(op);
      if (loopStoresGlobal(loop, gname))
        continue;
      toReplace.push_back(op);
    }
  }

  if (toReplace.empty())
    return;

  auto *term = preheader->getLastOp();
  for (auto *op : toReplace) {
    const auto &gname = NAME(op);
    if (hoisted.count(gname) || loopStoresGlobal(loop, gname))
      continue;
    builder.setBeforeOp(term);
    hoisted[gname] = builder.create<GetGlobalOp>({ new NameAttr(gname) });
    this->hoisted++;
  }

  for (auto *op : toReplace) {
    auto it = hoisted.find(NAME(op));
    if (it == hoisted.end())
      continue;
    op->replaceAllUsesWith(it->second);
    if (op->getUses().empty())
      op->erase();
  }
}
