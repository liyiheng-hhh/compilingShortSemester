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

## 暂不动（高风险）

- `mlir_rv/Schedule.cpp`, `InstCombine.cpp` — 指令/阶段顺序敏感
- `cfg/CFGToLegacy.cpp`, `codegen/OpBase.cpp` — 结构桥接
- `mlir_rv/StrengthReduct.cpp` — 已单独修 sort；MUL/REM 默认关
- `opt/LICM.cpp` — `variant` 与 `VariantAttr` 易误伤

## 后续可选（仍仅 helper 命名）

- `dialect_parse/Lexer.cpp`, `Parser.cpp`, `Sema.cpp` 内 lambda/局部逻辑（无独立 helper 时收益小）
- `opt/Reassociate.cpp`, `opt/DSE.cpp` / `DLE.cpp`（多为成员函数）
- `pre-opt/TidyMemory.cpp`, `MoveAlloca.cpp`, `RaiseToFor.cpp`（静态 Rule 名）

## 评测前

```bash
unset SYSY_RV_ENABLE_STRENGTH_REDUCT
make clean && make -j4 && make libsysy.a
make runtime-eval SUITE=performance OPT=O1
```
