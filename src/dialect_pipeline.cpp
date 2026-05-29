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

void dpipeFail(std::vector<std::string> &errors, const std::string &msg) {
  errors.push_back(msg);
}

bool dpipeDialectPassEnabled(const char *disableVar, bool defaultOn = true) {
  if (envFlagTruthy(disableVar))
    return false;
  return defaultOn;
}

bool dpipeUseStructuredCodegen() {
  if (envFlagTruthy("SYSY_CC_USE_CFG_IR"))
    return false;
  return true;
}

int dpipeInlineThreshold() {
  if (const char *v = std::getenv("SYSY_CC_INLINE_THRESHOLD")) {
    char *end = nullptr;
    long n = std::strtol(v, &end, 10);
    if (end != v && n > 0)
      return static_cast<int>(n);
  }
  return 200;
}

int dpipeLateInlineThreshold() {
  if (const char *v = std::getenv("SYSY_CC_LATE_INLINE_THRESHOLD")) {
    char *end = nullptr;
    long n = std::strtol(v, &end, 10);
    if (end != v && n > 0)
      return static_cast<int>(n);
  }
  return dpipeInlineThreshold();
}

void dpipeAppendPreOptPasses(sys::PassManager &pm) {
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
  if (dpipeDialectPassEnabled("SYSY_CC_ENABLE_PREOPT_EARLY_INLINE", true))
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
  // Adjacent structured-loop fusion (reference: LoopFusionPass); must run before Lower.
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_FUSION"))
    pm.addPass<sys::Fusion>();
  if (!envFlagTruthy("SYSY_CC_NO_PREOPT_LOWER"))
    pm.addPass<sys::Lower>();
}

void dpipeAppendPostFlattenPasses(sys::PassManager &pm) {
  pm.addPass<sys::GVN>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  if (!envFlagTruthy("SYSY_CC_NO_INLINE"))
    pm.addPass<sys::Inline>(dpipeInlineThreshold());
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  pm.addPass<sys::Localize>(/*beforeFlattenCFG=*/false);
  if (!envFlagTruthy("SYSY_CC_NO_GLOBALIZE"))
    pm.addPass<sys::Globalize>();
}

void dpipeAppendMemoryOptPasses(sys::PassManager &pm) {
  pm.addPass<sys::Alias>();
  if (!envFlagTruthy("SYSY_CC_NO_MAT_TRANSPOSE_PAIR"))
    pm.addPass<sys::MatTransposePair>();
  if (!envFlagTruthy("SYSY_CC_NO_REGULAR_FOLD"))
    pm.addPass<sys::RegularFold>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  pm.addPass<sys::DAE>();
  pm.addPass<sys::Alias>();
  // Reshape add trees before GVN (reference: AddChainReduction + reassociate for CSE).
  if (!envFlagTruthy("SYSY_CC_NO_REASSOCIATE"))
    pm.addPass<sys::Reassociate>();
  if (!envFlagTruthy("SYSY_CC_NO_DSE_DLE")) {
    pm.addPass<sys::DSE>();
    pm.addPass<sys::DLE>();
  }
  pm.addPass<sys::GVN>();
}

void dpipeAppendLoopOptPasses(sys::PassManager &pm) {
  pm.addPass<sys::CanonicalizeLoop>(/*lcssa=*/true);
  pm.addPass<sys::LoopRotate>();
  pm.addPass<sys::CanonicalizeLoop>(/*lcssa=*/false);
  if (!envFlagTruthy("SYSY_CC_NO_ROW_SCRATCH_MATMUL"))
    pm.addPass<sys::RowScratchMatmul>();
  if (!envFlagTruthy("SYSY_CC_NO_LOOP_TILING"))
    pm.addPass<sys::LoopTiling>();
  pm.addPass<sys::LICM>();
  if (!envFlagTruthy("SYSY_CC_NO_HOIST_LOOP_GLOBAL"))
    pm.addPass<sys::HoistLoopGlobal>();
  if (!envFlagTruthy("SYSY_CC_NO_GUARDED_ACCUM"))
    pm.addPass<sys::GuardedAccum>();
  if (!envFlagTruthy("SYSY_CC_NO_MAT_TRANSPOSE_PAIR"))
    pm.addPass<sys::MatTransposePair>();
  if (dpipeDialectPassEnabled("SYSY_CC_ENABLE_CONST_LOOP_UNROLL", true))
    pm.addPass<sys::ConstLoopUnroll>();
  if (!envFlagTruthy("SYSY_CC_NO_SCEV"))
    pm.addPass<sys::SCEV>();
  pm.addPass<sys::AggressiveDCE>();
  pm.addPass<sys::GVN>();
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::RemoveEmptyLoop>();
  if (!envFlagTruthy("SYSY_CC_NO_MAT_TRANSPOSE_PAIR"))
    pm.addPass<sys::MatTransposePair>();
}

void dpipeAppendMiscOptPasses(sys::PassManager &pm) {
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
  if (!envFlagTruthy("SYSY_CC_NO_GUARDED_ACCUM"))
    pm.addPass<sys::GuardedAccum>();
  if (!envFlagTruthy("SYSY_CC_NO_SELECT"))
    pm.addPass<sys::Select>();
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  if (!envFlagTruthy("SYSY_CC_NO_GCM"))
    pm.addPass<sys::GCM>();
  pm.addPass<sys::GVN>();
  pm.addPass<sys::AggressiveDCE>();
}

void dpipeAppendLateInlinePasses(sys::PassManager &pm) {
  if (!envFlagTruthy("SYSY_CC_NO_LATE_INLINE"))
    pm.addPass<sys::LateInline>(dpipeLateInlineThreshold());
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::GVN>();
  pm.addPass<sys::Alias>();
  if (!envFlagTruthy("SYSY_CC_NO_DSE_DLE")) {
    pm.addPass<sys::DSE>();
    pm.addPass<sys::DLE>();
  }
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  if (dpipeDialectPassEnabled("SYSY_CC_ENABLE_INLINE_STORE", false))
    pm.addPass<sys::InlineStore>();
  if (!envFlagTruthy("SYSY_CC_NO_SYNTH_CONST_ARRAY"))
    pm.addPass<sys::SynthConstArray>();
  pm.addPass<sys::RegularFold>();
  pm.addPass<sys::DCE>(/*elimBlocks=*/true);
  if (!envFlagTruthy("SYSY_CC_NO_GCM"))
    pm.addPass<sys::GCM>();
  pm.addPass<sys::GVN>();
}

void dpipeAppendLoopRoundPasses(sys::PassManager &pm) {
  pm.addPass<sys::CanonicalizeLoop>(/*lcssa=*/true);
  pm.addPass<sys::LICM>();
  if (!envFlagTruthy("SYSY_CC_NO_HOIST_LOOP_GLOBAL"))
    pm.addPass<sys::HoistLoopGlobal>();
  if (!envFlagTruthy("SYSY_CC_NO_SCEV"))
    pm.addPass<sys::SCEV>();
  if (!envFlagTruthy("SYSY_CC_NO_GUARDED_ACCUM"))
    pm.addPass<sys::GuardedAccum>();
  pm.addPass<sys::RemoveEmptyLoop>();
  if (!envFlagTruthy("SYSY_CC_NO_MAT_TRANSPOSE_PAIR"))
    pm.addPass<sys::MatTransposePair>();
  pm.addPass<sys::GVN>();
  if (!envFlagTruthy("SYSY_CC_NO_REGULAR_FOLD"))
    pm.addPass<sys::RegularFold>();
}

void dpipeAppendFinalCleanupPasses(sys::PassManager &pm) {
  pm.addPass<sys::AggressiveDCE>();
  pm.addPass<sys::SimplifyCFG>();
  if (!envFlagTruthy("SYSY_CC_NO_INST_SCHEDULE"))
    pm.addPass<sys::InstSchedule>();
}

std::unique_ptr<sys::ModuleOp> dpipeBuildFromStructuredAst(sys::ASTNode *node) {
  sys::CodeGen cg(node);
  return std::unique_ptr<sys::ModuleOp>(cg.getModule());
}

std::unique_ptr<sys::ModuleOp> dpipeBuildFromCfgLowering(
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
    dpipeFail(errors, e.what());
    if (node)
      delete node;
    return nullptr;
  } catch (const std::exception &e) {
    dpipeFail(errors, std::string("dialect frontend: ") + e.what());
    if (node)
      delete node;
    return nullptr;
  }

  std::unique_ptr<ModuleOp> module;
  if (dpipeUseStructuredCodegen())
    module = dpipeBuildFromStructuredAst(node);
  else
    module = dpipeBuildFromCfgLowering(node, errors);

  delete node;
  return module;
}

void appendDialectMidEndPasses(PassManager &pm) {
  if (dpipeUseStructuredCodegen() && !envFlagTruthy("SYSY_CC_NO_DIALECT_PRE_OPT"))
    dpipeAppendPreOptPasses(pm);

  if (dpipeUseStructuredCodegen())
    pm.addPass<sys::FlattenCFG>();

  // Before function inlining: avoid cloning 32-iter bitwise simulators into hot loops.
  if (!envFlagTruthy("SYSY_CC_NO_BIT_STUB_FOLD"))
    pm.addPass<sys::BitStubFold>();

  if (dpipeDialectPassEnabled("SYSY_CC_NO_POST_FLATTEN_OPT"))
    dpipeAppendPostFlattenPasses(pm);

  pm.addPass<sys::Mem2Reg>();

  if (dpipeDialectPassEnabled("SYSY_CC_NO_DIALECT_MEM_OPT"))
    dpipeAppendMemoryOptPasses(pm);

  if (dpipeDialectPassEnabled("SYSY_CC_NO_DIALECT_LOOP_OPT"))
    dpipeAppendLoopOptPasses(pm);

  if (dpipeDialectPassEnabled("SYSY_CC_ENABLE_DIALECT_MISC_OPT", true))
    dpipeAppendMiscOptPasses(pm);

  if (dpipeDialectPassEnabled("SYSY_CC_NO_DIALECT_LATE_OPT"))
    dpipeAppendLateInlinePasses(pm);

  if (dpipeDialectPassEnabled("SYSY_CC_ENABLE_DIALECT_LOOP_ROUND_OPT", true)) {
    for (int i = 0; i < 3; i++)
      dpipeAppendLoopRoundPasses(pm);
  }

  if (dpipeDialectPassEnabled("SYSY_CC_NO_DIALECT_FINAL_OPT"))
    dpipeAppendFinalCleanupPasses(pm);

  // Pinning loop vars to fixed callee-saved regs is opt-in (hurts many_mat_cal by default).
  if (envFlagTruthy("SYSY_CC_ENABLE_RSM_HELPER_PIN"))
    pm.addPass<sys::RsmHelperPin>();
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
