#pragma once

#include "Pass.h"

#include <memory>
#include <vector>

namespace sys {

// Minimal pass runner for dialect mid-end (no interpreter / compare hooks).
class PassManager {
  std::vector<std::unique_ptr<Pass>> passes;
  ModuleOp *module;

public:
  explicit PassManager(ModuleOp *module) : module(module) {}

  template <class T, class... Args>
  void addPass(Args &&...args) {
    passes.emplace_back(std::make_unique<T>(module, std::forward<Args>(args)...));
  }

  void run();
  ModuleOp *getModule() { return module; }

  void dumpPipeline(std::ostream &os) const;
};

}  // namespace sys
