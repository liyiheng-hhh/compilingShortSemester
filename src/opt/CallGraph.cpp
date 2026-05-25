#include "Analysis.h"

using namespace sys;

static void cgAttachCallerAttrs(ModuleOp *module,
                                std::map<std::string, std::set<std::string>> &calledBy) {
  std::vector<FuncOp *> funcs;
  for (auto op : module->getRegion()->getFirstBlock()->getOps()) {
    if (auto fn = dyn_cast<FuncOp>(op))
      funcs.push_back(fn);
  }
  for (auto func : funcs) {
    func->remove<CallerAttr>();
    const auto &name = NAME(func);
    const auto &callersSet = calledBy[name];
    std::vector<std::string> callers(callersSet.begin(), callersSet.end());
    func->add<CallerAttr>(callers);
  }
}

void CallGraph::run() {
  // Construct a call graph.
  // Actually Pureness can rely on this, but as it runs I wouldn't bother to change.
  std::map<std::string, std::set<std::string>> cgCalledBy;

  auto calls = module->findAll<CallOp>();
  // We consider `clone()` syscall also as calling the worker function.
  auto workers = module->findAll<CloneOp>();
  calls.reserve(calls.size() + workers.size());
  std::copy(workers.begin(), workers.end(), std::back_inserter(calls));
  for (auto call : calls) {
    auto func = call->getParentOp<FuncOp>();
    auto calledName = NAME(call);
    if (!isExtern(calledName))
      cgCalledBy[calledName].insert(NAME(func));
  }

  cgAttachCallerAttrs(module, cgCalledBy);
}
