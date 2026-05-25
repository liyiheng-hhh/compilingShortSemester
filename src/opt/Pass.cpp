#include "Pass.h"
#include "../codegen/Attrs.h"

#include <set>

// compiler2026-x phase-D (trivial opt dedup)

using namespace sys;

namespace {

const std::set<std::string> &passRuntimeExterns() {
  static const std::set<std::string> names = {
    "getint", "getch", "getfloat", "getarray", "getfarray",
    "putint", "putch", "putfloat", "putarray", "putfarray",
    "_sysy_starttime", "_sysy_stoptime", "starttime", "stoptime",
  };
  return names;
}

template <class OpTy>
static std::map<std::string, OpTy*> passIndexByName(ModuleOp *module) {
  std::map<std::string, OpTy*> out;
  for (auto op : module->getRegion()->getFirstBlock()->getOps()) {
    if (auto typed = dyn_cast<OpTy>(op))
      out[NAME(op)] = typed;
  }
  return out;
}

} // namespace

bool sys::isExtern(const std::string &name) {
  return passRuntimeExterns().count(name);
}

std::map<std::string, FuncOp*> Pass::getFunctionMap() {
  return passIndexByName<FuncOp>(module);
}

std::map<std::string, GlobalOp*> Pass::getGlobalMap() {
  return passIndexByName<GlobalOp>(module);
}

std::vector<FuncOp*> Pass::collectFuncs() {
  std::vector<FuncOp*> result;
  for (auto op : module->getRegion()->getFirstBlock()->getOps()) {
    if (auto fn = dyn_cast<FuncOp>(op))
      result.push_back(fn);
  }
  return result;
}

std::vector<GlobalOp*> Pass::collectGlobals() {
  std::vector<GlobalOp*> result;
  for (auto op : module->getRegion()->getFirstBlock()->getOps()) {
    if (auto glob = dyn_cast<GlobalOp>(op))
      result.push_back(glob);
  }
  return result;
}

DomTree Pass::getDomTree(Region *region) {
  region->updateDoms();
  DomTree tree;
  for (auto bb : region->getBlocks()) {
    if (auto idom = bb->getIdom())
      tree[idom].push_back(bb);
  }
  return tree;
}

void Pass::cleanup() {
  Op::release();
  runRewriter([&](PhiOp *op) {
    if (op->getResultType() == Value::f32)
      return false;
    for (auto operand : op->getOperands()) {
      if (operand.defining->getResultType() == Value::f32) {
        op->setResultType(Value::f32);
        return true;
      }
    }
    return false;
  });
}

Op *Pass::nonalloca(Region *region) {
  auto entry = region->getFirstBlock();
  Op *cursor = entry->getFirstOp();
  while (!cursor->atBack()) {
    if (isa<AllocaOp>(cursor))
      cursor = cursor->nextOp();
    else
      break;
  }
  if (cursor->atBack())
    cursor = entry->nextBlock()->getFirstOp();
  return cursor;
}

Op *Pass::nonphi(BasicBlock *bb) {
  Op *cursor = bb->getFirstOp();
  while (!cursor->atBack()) {
    if (isa<PhiOp>(cursor))
      cursor = cursor->nextOp();
    else
      break;
  }
  return cursor;
}
