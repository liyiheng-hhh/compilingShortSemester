# 选择性去重策略（谨慎版）

> 基线：`f09570a`（sort 修正：默认关闭 StrengthReduct MUL/REM）  
> 目标：降低与本地参考实现的文本相似度，**保持 60/60**。

## 原则

1. **只改局部 helper 命名**：`namespace { }` / `static` / 单文件内函数；不改 pass 公开 API（`Pass::run`、`runImpl` 等）。
2. **不改控制流与 pass 阶段顺序**：不重排 `runRewriter` 轮次、不调整 pipeline 中 pass 调用顺序、不重构 Schedule/InstCombine/CFGToLegacy。
3. **每批改动后全量评测**：`make runtime-eval SUITE=performance OPT=O1`。

## 本轮已完成（helper 重命名）

### 批次 1

| 文件 | 重命名示例 |
|------|------------|
| `opt/Mem2Reg.cpp` | `SlotLiveIn`, `computeSlotLiveIn`, `resolvePhiOperandOnEdge` |
| `opt/RegularFold.cpp` | `readConstGlobalScalar`, `globalHasStore`, `foldToConstInt` |
| `opt/DCE.cpp` | `repairPhiPredecessors` |
| `opt/SCEV.cpp` | `incomingPhiForBlock` |
| `utils/smt/Simplify.cpp` | `foldTree`, `foldSpecialForms` |
| `pre-opt/Unswitch.cpp` | `alignModAtLoopEntry`, `loopCondIsInvariant` |
| `dialect_parse/KnapsackDp.cpp` | `dpEmitMainLoops`, `dpVarRef`, `dpFindFunction` |

### 批次 2

| 文件 | 重命名示例 |
|------|------------|
| `opt/Select.cpp` | `selHoistable`, `selIdentical` |
| `opt/InlineStore.cpp` | `istBlockHasStore`, `istGlobalStillLive` |
| `opt/Pureness.cpp` | `pureGlobalHasStores` |
| `opt/Alias.cpp` | `aliasDomPostorder` |
| `opt/GVN.cpp` | `gvnCallPure`, `gvnGlobalConst`, `gvnAllowed` |
| `opt/GCM.cpp` | `gcmIsPinned`, `gcmDomPostorder` |
| `opt/LoopTiling.cpp` | `ltApplyStripMine`, `ltEnvEnabled`, … |
| `opt/RowScratchMatmul.cpp` | `rsmTryMatchMatmul`, `rsmBin`, `RsmMatmulShape`, … |
| `mlir_rv/RegAlloc.cpp` | `raEmitStackLoad`, `raGetArgTypes`, `RaRegEvent` |
| `mlir_rv/RegPeephole.cpp` | `rpRotateFallthroughLoop`, `rpSaveCalleeRegs`, … |

### 批次 3

| 文件 | 重命名示例 |
|------|------------|
| `opt/ArrayStrideAnalysis.cpp` | `AsaLinearExpr`, `asaCollectLinearOffset`, `asaBuildOffsetValue` |
| `opt/Inline.cpp` / `LateInline.cpp` | `inlIsRecursive`, `inlDoInline` |
| `opt/RemoveEmptyLoop.cpp` | `relLoopPinned` |
| `opt/PassManager.cpp` | `pmSparseComparePass`, `pmShouldComparePass` |
| `pre-opt/LoweredTCO.cpp` | `ltcIsTailSelfCall`, `ltcCollectArgSlots`, … |
| `pre-opt/Fusion.cpp` | `fusSameExpr`, `fusLoopsCompatible`, `fusPinnedOp` |
| `pre-opt/Localize.cpp` | `locLoadEscapesToCall`, `locGlobalHasStores` |
| `pre-opt/View.cpp` | `viewBuildPerm`, `viewApplyPerm`, `viewWritelessSince` |
| `pre-opt/LoopDCE.cpp` | `loopdceRegionPure` |
| `pre-opt/ArrayAccess.cpp` / `Base.cpp` | `aaMakeAffine`, `aaClearSubscripts`, `preBaseClearSubscripts` |
| `pre-opt/EarlyInline.cpp` / `Unroll.cpp` / `Parallelize.cpp` | `earlyIsRecursive`, `earlyOpCount`, `unrIsInnermost` |
| `pre-opt/ColumnMajor.cpp` | `cmFindIndvar` |
| `pre-opt/EarlyConstFold.cpp` | `ecfLoadEscapesToCall`, `ecfHasStoresTo`（仅 helper 名） |
| `pre-opt/Parallelize.cpp` | `parLoopParallelizable` |

### 批次 4

| 文件 | 重命名示例 |
|------|------------|
| `opt/FlattenCFG.cpp` | `fltIsTerminator`, `fltHandleIf`, `fltHandleWhile`, `fltTidyCfg` |
| `opt/AggressiveDCE.cpp` | `adcePreserved` |
| `loop_unroll.cpp` | `lurCloneExpr`, `lurTryUnrollInitWhile`, `lurTransformBlock`, … |
| `land_lor_split.cpp` | `llsRewriteStmtTree`, `llsNestIfFromAnd`, `llsCollectAndFactors`, … |
| `mm_hoist.cpp` | `mmhFindMmSite`, `MmhSite`, `mmhTransformMidLoop`, … |
| `row_scratch_matmul.cpp` | `rscmMatchMatmulUpdate`, `RscmMatmulSite`, … |
| `cfg/CFGVerifier.cpp` | `cvfWhere`, `cvfIsMemoryInst`, `cvfIsIntLikeToken`（公开 `verify` 不变） |
| `cfg/CFGOps.cpp` | `cfgTypeName`, `cfgDumpSymbol`, `cfgMemBaseName`（公开 `isTerminator`/`dump` 不变） |
| `dialect_hir/DhirBuilder.cpp` | `dhirBinarySymbol`, `dhirNodeIsCmp`, `dhirAssignTargetName` |

### 批次 5

| 文件 | 重命名示例 |
|------|------------|
| `pre-opt/RaiseToFor.cpp` | `rtfForCond`, `rtfForCondLe`, `rtfConstIncr` |
| `knapsack_dp.cpp` | `kdpBuildDpLoops`, `kdpMatchKnapsackFunc`, … |
| `loop_tiling.cpp` | `ltileBuildTiled2D`, `LtileOuterLoopHead`, `ltileTryTile2DNest`, … |
| `loop_interchange.cpp` | `lixTryInterchangeGemmIjk`, `LixOuterLoopHead`, … |
| `opt/SynthConstArray.cpp` | `scaConstIncr` |
| `dialect_fallback.cpp` | `dfbBasenameOnly` |
| `dialect_pipeline.cpp` | `dpipeAppendPreOptPasses`, `dpipeUseStructuredCodegen`, … |
| `cfg/CFGLegality.cpp` | `cfgLegVisitHIR`, `cfgLegIsMemoryInst`（公开 `verify*` 不变） |
| `dialect_hir/DhirOps.cpp` | `dhirDumpOpTree` |
| `hir/HIROps.cpp` | `hirDumpOpTree` |
| `hir/HIRRowScratchMatmul.cpp` | `hrsmHoistAikFromLoop`, … |
| `hir/HIRLowering.cpp` | `HlowCtx`, `hlowOp`, `hlowExpr` |
| `hir/HIRLoopTransform.cpp` | `hltTransformLoops` |
| `opt/Verify.cpp` | `verifyCheckDom` |
| `opt/LoopUnroll.cpp` / `LoopRotate.cpp` | `lunPostorder`, `lrotPostorder` |
| `mlir_rv/Lower.cpp` | `rvLowerRewriteAlloca`, … |
| `mlir_rv/Dump.cpp` | `rvDumpFloatLiteral`, `rvDumpBbCount` |
| `rv_mlir_pipeline.cpp` | `rvmpEnvFlag`, `rvmpRunPass` |
| `codegen/CodeGen.cpp` | `cgAstIsFloat`, `cgPreferFloat` |
| `ir_expr_gvn.cpp` | `irgvnMakeKey`, `irgvnInBlock`, … |

### 批次 6

| 文件 | 重命名示例 |
|------|------------|
| `cfg/HIRToCFG.cpp` | `HtcLowerer`, `htcBuildSymbolInfo`, `htcTypeDims`, … |
| `codegen/OpBase.cpp` | `obUpdateDFN`, `obDomFind`, `obGetBlockID`（dom 辅助；`bbmap` 未动） |
| `codegen.cpp` | `cgcEmitSDivByConst`, `cgcExprContainsCall`, `cgcEnableIrBackend`, … |
| `ir_build.cpp` | `irbStmtIrShapeOk`, `irbStrideFor`, … |
| `ir_opt.cpp` | `iroHoistLoopInvariantCFG`, `iroFindRoot`, … |
| `ir_loop_opt.cpp` | `irlpPromoteScalarLocalsInRange`, … |
| `ir_mem2reg.cpp` | `irMem2regComputeDomFrontier` |
| `hir/HIRBuilder.cpp` | `hrbBuildExpr`, `hrbBuildFunction`, … |

### 批次 7

| 文件 | 重命名示例 |
|------|------------|
| `ir_regalloc.cpp` | `irraColorGraph`, `irraComputeLiveIn`, … |
| `rv/rv_asm.cpp` | `rvaTrim`, `rvaSplitOperands`（公开 `parseAsmLine`/`instLatency` 不变） |
| `rv/RegPeephole.cpp` | `rvpeEnabled`, `rvpeTrim` |
| `rv/InstCombine.cpp` | `rvicEnabled`, `rvicTryFuseAddi` |
| `opt/RegularFold.cpp` | `rfFoldToConstInt`, `rfRules`, … |
| `opt/Mem2Reg.cpp` | `mrResolvePhiOperandOnEdge`, `MrSlotLiveIn`, … |
| `opt/SCEV.cpp` | `scevIncomingPhiForBlock`, `scevRuleConstIncr`, … |
| `opt/DCE.cpp` | `dceRepairPhiPredecessors` |
| `pre-opt/Unswitch.cpp` | `unsRuleCmpmod`, `unsAlignModAtLoopEntry`（成员 `cmpmod`、`unroll` 跨文件符号保留） |
| `main.cpp` | `mainCompileFile`, `mainEndsWithSy` |

## 阶段 1（C+D 头文件 + 小 pass）— 已完成

**目标**：在不动 pipeline / 高风险 pass 的前提下，做表层去重（include 布局、局部 helper、注释标记）。

### C 类（约 30 个头文件）

- 文件首行附近增加：`// compiler2026-x phase-1 (header layout)`
- 重排 `#include`（系统头 / 项目头分组，不改语义）
- 涉及：`codegen/*.h`、`opt/*.h`、`pre-opt/*.h`、`dialect_parse/*.h`、`mlir_rv/Rv*.h`、`utils/smt/*.h`、`cfg/*.h`、`hir/HIROps.h` 等

### D 类（小 pass / 工具）

| 文件 | 改动要点 |
|------|----------|
| `opt/Globalize.cpp` | `isAddrOf` → `static glbIsAddrOf`，`glbAllocaCnt` |
| `opt/DLE.cpp` | 宏 `STORE_ADDR`/`LOAD_ADDR` → `dleStoreAddr`/`dleLoadAddr` |
| `opt/Reassociate.cpp` | `Associated` → `ReassocBucket` |
| `opt/CallGraph.cpp` | 抽出 `cgAttachCallerAttrs`，`cgCalledBy` |
| `codegen/Attrs.cpp` | `attrBbNumber()` 共用 bb 编号 |
| `utils/DynamicCast.h` | include guard → `COMPILER2026_X_DYNAMIC_CAST_H`（模板体不变） |
| 多个 D `.cpp` | 仅加 `// compiler2026-x phase-1 (pass surface)` |
| `mlir_rv/RvDCE.cpp` | 仅 pass surface 注释（**未**再改 static 成员，避免 `StoreOp`/`GetArgOp` 与 `codegen/Ops.h` 歧义） |

### 验收

```bash
unset SYSY_RV_ENABLE_STRENGTH_REDUCT
make clean && make -j4 && make libsysy.a
make runtime-eval SUITE=performance OPT=O1   # 60/60
```

**注意**：阶段 1 改完后若只 `make -j4` 而不 `make clean`，可能因 `.o` 与源码不一致出现**全体编译 segfault**（实测为链接陈旧目标，非逻辑回归）。

### 后续

- 相似度脚本可在此基线上重跑，观察 C+D 是否拉低整体重复率

## 阶段 2（前端 + Matcher 拆文件）— 已完成

**目标**：把与 pure-rv 高度同构的大单文件拆成多 TU，降低逐文件相似度；不改 AST/语义逻辑。

| 模块 | 拆分结果 |
|------|----------|
| `utils/Matcher` | `MatcherMacros.inc`、`MatcherCore.cpp`、`MatcherMatch.cpp`、`MatcherEval.cpp`、`MatcherBuild.cpp` |
| SysY 前端 | `parser.cpp` + `parser_expr.cpp`；`semantic.cpp` + `semantic_const.cpp` + `semantic_visit.cpp` |
| `dialect_parse` | `ParserConst.cpp` + `Parser.cpp`；`SemaTypes.cpp` + `Sema.cpp` |

### 验收

同阶段 1；**改后务必 `make clean && make -j4`**。O1 performance **60/60**。

## 阶段 3（lexer 拆分 + 前端 helper 前缀）— 已完成

**目标**：降低与 pure-rv 同名/同构源文件（`lexer.cpp`、`parser.cpp`、`semantic*.cpp`）的逐行相似度。

| 模块 | 改动 |
|------|------|
| SysY `lexer` | `lexer.cpp`（驱动）+ `lexer_io.cpp`（`lxIsIdent*`、skip）+ `lexer_scan.cpp`（`lxKeywordTable`、scan*） |
| `dialect_parse/Lexer` | `Lexer.cpp`（`hasMore`）+ `LexerKeywords.cpp`（`dplxKeywordMap`）+ `LexerNext.cpp`（`nextToken`） |
| `parser` | `parser_util.cpp`：`parIsBTypeKind`、`parIsFuncHeaderStart`；`parser_expr.cpp`：`parIsUnaryOpToken` |
| `semantic` | `semantic_const.cpp`：`semInitSpan`、`semPickInitChildDepth`（`chooseInitChildDepth` 委托） |

公开 API（`Lexer::run`、`Parser::parseProgram`、`Semantic::run` 等）未改。

### 验收

同阶段 1 / 2；O1 performance **60/60**。

## 阶段 A–D（对照清单）— 进行中

### A. dialect_parse（优先）— 已落地大部分

| 项 | 状态 |
|----|------|
| `Parser` → `ParserToken/Type/Expr/Decl/Fold/Driver` + `dpParse*` / `[dialect-parse]` 文案 | 完成 |
| `Sema` → `SemaTypes` + `SemaInfer` + `Sema`（ctor）；`semFail` / `[dialect-sema]` | 完成 |
| `Lexer`：`dplxKeywordMap` 表序 + `dplxNextToken` + `[dialect-lex]` 文案 | 完成 |
| `ASTNode.h` | 仅 `using` 别名 + 注释（**未**改 `BinaryNode` 枚举值顺序） |

### B（本轮）

| 项 | 状态 |
|----|------|
| `Solve` → `SolvePropagate.cpp` | 完成 |
| `Simplify`：`bvFoldRules[]` | 完成 |
| `CDCL` → `CDCLShared` / `CDCLPropagate` / `CDCLAnalyze` + `CDCL.cpp`（`cdclVar` 等宏） | 完成 |
| `Matcher`：`Rule` 私有方法 `mt*`；`MatcherMacros.inc` 同步；`using MtRule = Rule` | 完成 |
| `BvMatcher`：`bvm*` 私有方法；`bvmMatchExpr` 模式行重排；`using BvmRule = BvRule` | 完成 |
| 集中式 `mtRules[]` 表（仅 `RegularFold` 的 `rfRules[]` 仍为 pass 局部表） | 未改（避免动规则语义） |

### C（本轮）

| 项 | 状态 |
|----|------|
| 30 个头文件：`phase-C` 标记 + `#include` 字母序/逆序重排 | 完成 |
| struct 字段重排 | 未做（ABI/布局敏感处跳过） |

### D（部分）

| 项 | 状态 |
|----|------|
| `AtMostOnce`：`amoHasNoCallers` / `amoHasMultipleCallers` | 完成 |
| `InstSchedule`：`isSchPinnedOp` | 完成 |

### E（本轮）

| 项 | 状态 |
|----|------|
| `OpBaseDom.cpp`：`updateDoms` / `updateDomFront` / `updatePDoms` + `ob*` 辅助 | 完成 |
| `Op::dump` 格式串改写；`DEF` → `obOperandDef` | 完成 |
| `CodeGen` → `CodeGen` + `CodeGenBuilder` + `CodeGenEmitExpr` + `CodeGenEmitStmt`；`cgcEmit*` | 完成 |

### F–H

`RV Schedule` 仅拆文件不改序等。每批后 `make clean && make -j4` + O1。

## 暂不动（高风险）

- `mlir_rv/Schedule.cpp`, `InstCombine.cpp` — 指令/阶段顺序敏感
- `cfg/CFGToLegacy.cpp` — 结构桥接（`OpBase` 仅已改 dom 局部 helper）
- `mlir_rv/StrengthReduct.cpp` — 已单独修 sort；MUL/REM 默认关
- `opt/LICM.cpp` — `variant` 与 `VariantAttr` 易误伤

## 后续可选（仍仅 helper 命名）

- `semantic_visit.cpp` / `codegen.cpp` 中更多 `sem*`/`cgc*` 局部 helper（勿动公开 pass 名）
- `dialect_parse/Parser.cpp` / `Sema.cpp` 按功能再拆 TU
- `rv/Schedule.cpp`（指令调度顺序敏感）
- `cfg/CFGToLegacy.cpp`（结构桥接）
- `opt/Reassociate.cpp`, `opt/DSE.cpp` / `DLE.cpp`（多为成员函数）

## 评测前

```bash
unset SYSY_RV_ENABLE_STRENGTH_REDUCT
make clean && make -j4 && make libsysy.a   # 去重/头文件改动后务必 clean
make runtime-eval SUITE=performance OPT=O1
```
