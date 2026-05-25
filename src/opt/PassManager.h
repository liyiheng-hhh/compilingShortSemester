// compiler2026-x phase-C (header layout)
#pragma once

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "Pass.h"


namespace sys {

struct PassDebugOptions {
  bool verify = false;
  bool stats = false;
  bool verbose = false;
  bool compareSparse = false;
  std::string printAfter;
  std::string printBefore;
  std::string compareWith;
  std::string simulateInput;
  std::string stopAfterPass;
  std::string compareOnlyPass;
};

PassDebugOptions loadPassDebugOptionsFromEnv();

class PassManager {
  std::vector<std::unique_ptr<Pass>> passes;
  ModuleOp *module;

  PassDebugOptions debug;
  bool pastFlatten = false;
  bool pastMem2Reg = false;
  int expectedExit = 0;
  std::string expectedOutput;
  std::string simulateInput;

public:
  PassManager(ModuleOp *module, PassDebugOptions debug = {});

  template <class T, class... Args>
  void addPass(Args &&...args) {
    passes.emplace_back(std::make_unique<T>(module, std::forward<Args>(args)...));
  }

  void run();
  ModuleOp *getModule() { return module; }

  void dumpPipeline(std::ostream &os) const;
};

}  // namespace sys
