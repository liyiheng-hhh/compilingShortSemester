#include "dialect_pipeline.h"

#include "rv_mlir_pipeline.h"

#include "cfg/CFGToLegacy.h"
#include "cfg/HIRToCFG.h"
#include "codegen/CodeGen.h"
#include "dialect_hir/DhirBuilder.h"
#include "dialect_parse/KnapsackDp.h"
#include "dialect_parse/Parser.h"
#include "dialect_parse/Sema.h"
#include "dialect_parse/TypeContext.h"
#include "opt/Analysis.h"
#include "opt/CleanupPasses.h"
#include "opt/LowerPasses.h"
#include "opt/PassManager.h"
#include "dialect_parse/CompileError.h"
#include "common.h"
#include "opt/Passes.h"
#include "opt/SMTPasses.h"
#include "opt/LoopPasses.h"
#include "pre-opt/PrePasses.h"
#include "pre-opt/PreLoopPasses.h"
#include "pre-opt/PreAnalysis.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>

bool sys::dialectPipelineEnabled() {
  if (const char *v = std::getenv("SYSY_CC_NO_DIALECT_PIPELINE"))
    if (v[0] && std::strcmp(v, "0") != 0)
      return false;
  const char *en = std::getenv("SYSY_CC_ENABLE_DIALECT_PIPELINE");
  if (en && en[0])
    return envFlagTruthy("SYSY_CC_ENABLE_DIALECT_PIPELINE");
  return true;
}

namespace {

void fail(std::vector<std::string> &errors, const std::string &msg) {
  errors.push_back(msg);
}

bool dialectPassEnabled(const char *disableVar, bool defaultOn = true) {
  if (envFlagTruthy(disableVar))
    return false;
  return defaultOn;
}

bool useStructuredCodegen() {
  if (envFlagTruthy("SYSY_CC_USE_CFG_IR"))
    return false;
  return true;
}

int inlineThreshold() {
  if (const char *v = std::getenv("SYSY_CC_INLINE_THRESHOLD")) {
    char *end = nullptr;
    long n = std::strtol(v, &end, 10);
    if (end != v && n > 0)
      return static_cast<int>(n);
  }
  return 200;
}

int lateInlineThreshold() {
  if (const char *v = std::getenv("SYSY_CC_LATE_INLINE_THRESHOLD")) {
    char *end = nullptr;
    long n = std::strtol(v, &end, 10);
    if (end != v && n > 0)
      return static_cast<int>(n);
  }
  return inlineThreshold();
}

void appendPreOptPasses(sys::PassManager &pm) {
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_CALLGRAPH"))
    pm.addPass<sys::CallGraph>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_ATMOSTONCE"))
    pm.addPass<sys::AtMostOnce>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_LOCALIZE"))
    pm.addPass<sys::Localize>(/*beforeFlattenCFG=*/true);
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_MOVE_ALLOCA"))
    pm.addPass<sys::MoveAlloca>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_EARLY_CONST_FOLD"))
    pm.addPass<sys::EarlyConstFold>(/*beforePureness=*/true);
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_PURENESS"))
    pm.addPass<sys::Pureness>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_EARLY_CONST_FOLD_AFTER"))
    pm.addPass<sys::EarlyConstFold>(/*beforePureness=*/false);
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_TCO"))
    pm.addPass<sys::TCO>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_REMERGE"))
    pm.addPass<sys::Remerge>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_RAISE_TO_FOR"))
    pm.addPass<sys::RaiseToFor>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_DCE"))
    pm.addPass<sys::DCE>(/*elimBlocks=*/false);
  if (dialectPassEnabled("SYSY_CC_ENABLE_PREOPT_EARLY_INLINE", true))
    pm.addPass<sys::EarlyInline>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_REGULAR_FOLD"))
    pm.addPass<sys::RegularFold>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_VIEW"))
    pm.addPass<sys::View>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_LOOP_DCE"))
    pm.addPass<sys::LoopDCE>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_TIDY_MEM"))
    pm.addPass<sys::TidyMemory>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_DCE"))
    pm.addPass<sys::DCE>(/*elimBlocks=*/false);
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_COLUMN_MAJOR"))
    pm.addPass<sys::ColumnMajor>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_PARALLELIZABLE"))
    pm.addPass<sys::Parallelizable>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_LOOP_DCE"))
    pm.addPass<sys::LoopDCE>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_LOWER"))
    pm.addPass<sys::Lower>();
}

void appendPostFlattenPasses(sys::PassManager &pm) {
  pm.addPass<sys::GVN>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  if (!envFlagTruthy("SYSY_CC_NO_INLINE"))
    pm.addPass<sys::Inline>(inlineThreshold());
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  pm.addPass<sys::Localize>(/*beforeFlattenCFG=*/false);
  if (!envFlagTruthy("SYSY_CC_NO_GLOBALIZE"))
    pm.addPass<sys::Globalize>();
}

void appendMemoryOptPasses(sys::PassManager &pm) {
  pm.addPass<sys::Alias>();
  if (!envFlagTruthy("SYSY_CC_NO_REGULAR_FOLD"))
    pm.addPass<sys::RegularFold>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  pm.addPass<sys::DAE>();
  pm.addPass<sys::Alias>();
  if (!envFlagTruthy("SYSY_CC_NO_DSE_DLE")) {
    pm.addPass<sys::DSE>();
    pm.addPass<sys::DLE>();
  }
  pm.addPass<sys::GVN>();
}

void appendLoopOptPasses(sys::PassManager &pm) {
  pm.addPass<sys::CanonicalizeLoop>(/*lcssa=*/true);
  pm.addPass<sys::LoopRotate>();
  pm.addPass<sys::CanonicalizeLoop>(/*lcssa=*/false);
  if (!envFlagTruthy("SYSY_CC_NO_ROW_SCRATCH_MATMUL"))
    pm.addPass<sys::RowScratchMatmul>();
  if (!envFlagTruthy("SYSY_CC_NO_LOOP_TILING"))
    pm.addPass<sys::LoopTiling>();
  pm.addPass<sys::LICM>();
  if (dialectPassEnabled("SYSY_CC_ENABLE_CONST_LOOP_UNROLL", true))
    pm.addPass<sys::ConstLoopUnroll>();
  if (!envFlagTruthy("SYSY_CC_NO_SCEV"))
    pm.addPass<sys::SCEV>();
  pm.addPass<sys::AggressiveDCE>();
  pm.addPass<sys::GVN>();
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::RemoveEmptyLoop>();
}

void appendMiscOptPasses(sys::PassManager &pm) {
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  pm.addPass<sys::GVN>();
  pm.addPass<sys::SimplifyCFG>();
  pm.addPass<sys::Alias>();
  pm.addPass<sys::DAE>();
  if (!envFlagTruthy("SYSY_CC_NO_DSE_DLE")) {
    pm.addPass<sys::DSE>();
    pm.addPass<sys::DLE>();
  }
  if (!envFlagTruthy("SYSY_CC_NO_SELECT"))
    pm.addPass<sys::Select>();
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  if (!envFlagTruthy("SYSY_CC_NO_GCM"))
    pm.addPass<sys::GCM>();
  pm.addPass<sys::GVN>();
  pm.addPass<sys::AggressiveDCE>();
}

void appendLateInlinePasses(sys::PassManager &pm) {
  if (!envFlagTruthy("SYSY_CC_NO_LATE_INLINE"))
    pm.addPass<sys::LateInline>(lateInlineThreshold());
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::GVN>();
  pm.addPass<sys::Alias>();
  if (!envFlagTruthy("SYSY_CC_NO_DSE_DLE")) {
    pm.addPass<sys::DSE>();
    pm.addPass<sys::DLE>();
  }
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  if (dialectPassEnabled("SYSY_CC_ENABLE_INLINE_STORE", false))
    pm.addPass<sys::InlineStore>();
  if (!envFlagTruthy("SYSY_CC_NO_SYNTH_CONST_ARRAY"))
    pm.addPass<sys::SynthConstArray>();
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  if (!envFlagTruthy("SYSY_CC_NO_GCM"))
    pm.addPass<sys::GCM>();
  pm.addPass<sys::GVN>();
}

void appendLoopRoundPasses(sys::PassManager &pm) {
  pm.addPass<sys::CanonicalizeLoop>(/*lcssa=*/true);
  pm.addPass<sys::LICM>();
  if (!envFlagTruthy("SYSY_CC_NO_SCEV"))
    pm.addPass<sys::SCEV>();
  pm.addPass<sys::RemoveEmptyLoop>();
  pm.addPass<sys::GVN>();
  if (!envFlagTruthy("SYSY_CC_NO_REGULAR_FOLD"))
    pm.addPass<sys::RegularFold>();
}

void appendFinalCleanupPasses(sys::PassManager &pm) {
  pm.addPass<sys::AggressiveDCE>();
  pm.addPass<sys::SimplifyCFG>();
  if (!envFlagTruthy("SYSY_CC_NO_INST_SCHEDULE"))
    pm.addPass<sys::InstSchedule>();
}

std::unique_ptr<sys::ModuleOp> buildFromStructuredAst(sys::ASTNode *node) {
  sys::CodeGen cg(node);
  return std::unique_ptr<sys::ModuleOp>(cg.getModule());
}

std::unique_ptr<sys::ModuleOp> buildFromCfgLowering(
    sys::ASTNode *node, std::vector<std::string> &errors) {
  static bool warned = false;
  if (!warned) {
    warned = true;
    std::cerr << "warning: SYSY_CC_USE_CFG_IR=1 uses deprecated HIR/CFG lowering; "
                 "prefer default CodeGen structured IR\n";
  }
  sys::dhir::Module hir = sys::dhir::Builder().build(node);
  sys::cfg::Module cfgMod = sys::cfg::lowerFromHIR(hir, errors);
  if (!errors.empty())
    return nullptr;
  return sys::cfg::lowerToLegacyIR(cfgMod, errors);
}

}  // namespace

namespace sys {

std::unique_ptr<ModuleOp> buildDialectModuleFromSource(
    const std::string &source, std::vector<std::string> &errors) {
  errors.clear();
  sys::TypeContext ctx;
  sys::ASTNode *node = nullptr;
  try {
    sys::Parser parser(source, ctx);
    node = parser.parse();
    sys::Sema sema(node, ctx);
    if (!envFlagTruthy("SYSY_CC_NO_KNAPSACK_DP"))
      applyKnapsackDpDialect(node, ctx);
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

  std::unique_ptr<ModuleOp> module;
  if (useStructuredCodegen())
    module = buildFromStructuredAst(node);
  else
    module = buildFromCfgLowering(node, errors);

  delete node;
  return module;
}

void appendDialectMidEndPasses(PassManager &pm) {
  if (useStructuredCodegen() && !envFlagTruthy("SYSY_CC_NO_DIALECT_PRE_OPT"))
    appendPreOptPasses(pm);

  if (useStructuredCodegen())
    pm.addPass<sys::FlattenCFG>();

  if (dialectPassEnabled("SYSY_CC_NO_POST_FLATTEN_OPT"))
    appendPostFlattenPasses(pm);

  pm.addPass<sys::Mem2Reg>();

  if (dialectPassEnabled("SYSY_CC_NO_DIALECT_MEM_OPT"))
    appendMemoryOptPasses(pm);

  if (dialectPassEnabled("SYSY_CC_NO_DIALECT_LOOP_OPT"))
    appendLoopOptPasses(pm);

  if (dialectPassEnabled("SYSY_CC_ENABLE_DIALECT_MISC_OPT", true))
    appendMiscOptPasses(pm);

  if (dialectPassEnabled("SYSY_CC_NO_DIALECT_LATE_OPT"))
    appendLateInlinePasses(pm);

  if (dialectPassEnabled("SYSY_CC_ENABLE_DIALECT_LOOP_ROUND_OPT", true)) {
    for (int i = 0; i < 3; i++)
      appendLoopRoundPasses(pm);
  }

  if (dialectPassEnabled("SYSY_CC_NO_DIALECT_FINAL_OPT"))
    appendFinalCleanupPasses(pm);
}

std::string emitDialectModuleAsm(ModuleOp *module, bool enableSchedule,
                                 const PassDebugOptions &debug) {
  PassDebugOptions opts = loadPassDebugOptionsFromEnv();
  if (!debug.compareWith.empty())
    opts.compareWith = debug.compareWith;
  if (!debug.simulateInput.empty())
    opts.simulateInput = debug.simulateInput;
  if (debug.verify)
    opts.verify = true;
  if (debug.stats)
    opts.stats = true;
  if (debug.verbose)
    opts.verbose = true;
  if (!debug.printAfter.empty())
    opts.printAfter = debug.printAfter;
  if (!debug.printBefore.empty())
    opts.printBefore = debug.printBefore;

  PassManager pm(module, opts);
  appendDialectMidEndPasses(pm);
  pm.run();
  return runRvMlirPipeline(module, enableSchedule, "");
}

}  // namespace sys
