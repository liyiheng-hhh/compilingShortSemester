#include "dialect_pipeline.h"

#include "rv_mlir_pipeline.h"

#include "cfg/CFGToLegacy.h"
#include "cfg/HIRToCFG.h"
#include "dialect_hir/DhirBuilder.h"
#include "dialect_parse/Parser.h"
#include "dialect_parse/Sema.h"
#include "dialect_parse/TypeContext.h"
#include "opt/CleanupPasses.h"
#include "opt/PassManager.h"
#include "dialect_parse/CompileError.h"
#include "common.h"
#include "opt/Passes.h"

#include <cstdlib>
#include <cstring>
#include <sstream>

bool sys::dialectPipelineEnabled() {
  if (const char *v = std::getenv("SYSY_CC_NO_DIALECT_PIPELINE"))
    if (v[0] && std::strcmp(v, "0") != 0)
      return false;
  const char *en = std::getenv("SYSY_CC_ENABLE_DIALECT_PIPELINE");
  if (en && en[0])
    return envFlagTruthy("SYSY_CC_ENABLE_DIALECT_PIPELINE");
  // 评测平台通常不设环境变量：-O1 默认走方言管线（可用 NO_=1 或 ENABLE=0 关闭）
  return true;
}

namespace {

void fail(std::vector<std::string> &errors, const std::string &msg) {
  errors.push_back(msg);
}

}  // namespace

std::unique_ptr<sys::ModuleOp> sys::buildDialectModuleFromSource(
    const std::string &source, std::vector<std::string> &errors) {
  errors.clear();
  sys::TypeContext ctx;
  sys::ASTNode *node = nullptr;
  try {
    sys::Parser parser(source, ctx);
    node = parser.parse();
    sys::Sema sema(node, ctx);
  } catch (const sys::CompileError &e) {
    fail(errors, e.what());
    if (node)
      delete node;
    return nullptr;
  } catch (const std::exception &e) {
    fail(errors, std::string("dialect frontend: ") + e.what());
    if (node)
      delete node;
    return nullptr;
  }

  sys::dhir::Module hir = sys::dhir::Builder().build(node);

  sys::cfg::Module cfgMod = sys::cfg::lowerFromHIR(hir, errors);
  if (!errors.empty()) {
    delete node;
    return nullptr;
  }

  auto module = sys::cfg::lowerToLegacyIR(cfgMod, errors);
  delete node;
  return module;
}

void sys::appendDialectMidEndPasses(sys::PassManager &pm) {
  pm.addPass<sys::Mem2Reg>();
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  pm.addPass<sys::SimplifyCFG>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
}

std::string sys::emitDialectModuleAsm(sys::ModuleOp *module, bool enableSchedule) {
  sys::PassManager pm(module);
  sys::appendDialectMidEndPasses(pm);
  pm.run();
  return sys::runRvMlirPipeline(module, enableSchedule, "");
}
