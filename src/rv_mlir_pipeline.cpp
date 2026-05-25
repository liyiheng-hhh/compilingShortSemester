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

bool rvmpEnvFlag(const char *name, bool fallback) {
  const char *v = std::getenv(name);
  if (!v || !v[0]) return fallback;
  if (std::strcmp(v, "0") == 0 || std::strcmp(v, "false") == 0) return false;
  return true;
}

template <class T, class... Args>
void rvmpRunPass(ModuleOp *module, Args &&...args) {
  T pass(module, std::forward<Args>(args)...);
  pass.run();
}

}  // namespace

namespace sys {

bool rvMlirBackendEnabled() {
  return rvmpEnvFlag("SYSY_CC_ENABLE_MLIR_RV", false) ||
         rvmpEnvFlag("SYSY_CC_ENABLE_RV_IR_BACKEND", false);
}

std::string runRvMlirPipeline(ModuleOp *module, bool enableSchedule,
                              const std::string &onlyFunc) {
  rvmpRunPass<rv::Lower>(module);
  if (rv::envEnabledSuffix("ENABLE_INST_COMBINE", true))
    rvmpRunPass<rv::InstCombine>(module);
  rvmpRunPass<rv::RvDCE>(module);
  if (!rvmpEnvFlag("SYSY_RV_DISABLE_GVN", false) && rv::envEnabledSuffix("ENABLE_GVN", true))
    rvmpRunPass<GVN>(module);
  if (rv::envEnabledSuffix("ENABLE_STRENGTH_REDUCT", true))
    rvmpRunPass<rv::StrengthReduct>(module);
  if (enableSchedule && rv::envEnabledSuffix("ENABLE_SCHEDULE", true))
    rvmpRunPass<rv::Schedule>(module);
  const bool fast = rvmpEnvFlag("SYSY_CC_MLIR_RV_FAST_RA", false);
  rvmpRunPass<rv::RegAlloc>(module, fast);
  std::ostringstream oss;
  rv::Dump dump(module, "");
  dump.dump(oss, onlyFunc);
  return oss.str();
}

}  // namespace sys
