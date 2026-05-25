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

## 暂不动（高风险）

- `mlir_rv/Schedule.cpp`, `InstCombine.cpp` — 指令/阶段顺序敏感
- `cfg/CFGToLegacy.cpp` — 结构桥接（`OpBase` 仅已改 dom 局部 helper）
- `mlir_rv/StrengthReduct.cpp` — 已单独修 sort；MUL/REM 默认关
- `opt/LICM.cpp` — `variant` 与 `VariantAttr` 易误伤

## 后续可选（仍仅 helper 命名）

- `dialect_parse/Lexer.cpp`, `Parser.cpp`, `Sema.cpp`（无独立 static helper 时收益小）
- `ir_regalloc.cpp`, `rv/rv_asm.cpp`, `utils/Matcher.cpp`
- `opt/Reassociate.cpp`, `opt/DSE.cpp` / `DLE.cpp`（多为成员函数）

## 评测前

```bash
unset SYSY_RV_ENABLE_STRENGTH_REDUCT
make clean && make -j4 && make libsysy.a
make runtime-eval SUITE=performance OPT=O1
```
