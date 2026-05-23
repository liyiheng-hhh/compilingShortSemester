#include "rv_mlir_pipeline.h"

#include "mlir_rv/RvConfig.h"
#include "mlir_rv/RvDupPasses.h"
#include "mlir_rv/RvPasses.h"

#include <cstdlib>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>

#include "opt/Pass.h"
#include "opt/Passes.h"

using namespace sys;

namespace {

bool envFlag(const char *name, bool fallback) {
  const char *v = std::getenv(name);
  if (!v || !v[0]) return fallback;
  if (std::strcmp(v, "0") == 0 || std::strcmp(v, "false") == 0) return false;
  return true;
}

template <class T, class... Args>
void runPass(ModuleOp *module, Args &&...args) {
  T pass(module, std::forward<Args>(args)...);
  pass.run();
}

}  // namespace

namespace sys {

bool rvMlirBackendEnabled() {
  return envFlag("SYSY_CC_ENABLE_MLIR_RV", false) ||
         envFlag("SYSY_CC_ENABLE_RV_IR_BACKEND", false);
}

std::string runRvMlirPipeline(ModuleOp *module, bool enableSchedule,
                              const std::string &onlyFunc) {
  runPass<rv::Lower>(module);
  if (rv::envEnabledSuffix("ENABLE_INST_COMBINE", true))
    runPass<rv::InstCombine>(module);
  runPass<rv::RvDCE>(module);
  if (!envFlag("SYSY_RV_DISABLE_GVN", false) && rv::envEnabledSuffix("ENABLE_GVN", true))
    runPass<GVN>(module);
  if (enableSchedule && rv::envEnabledSuffix("ENABLE_SCHEDULE", true))
    runPass<rv::Schedule>(module);
  const bool fast = envFlag("SYSY_CC_MLIR_RV_FAST_RA", false);
  runPass<rv::RegAlloc>(module, fast);
  std::ostringstream oss;
  rv::Dump dump(module, "");
  dump.dump(oss, onlyFunc);
  return oss.str();
}

}  // namespace sys
