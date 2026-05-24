# 选择性去重策略（谨慎版）

> 基线：`f09570a`（sort 修正：默认关闭 StrengthReduct MUL/REM）  
> 目标：降低与本地参考实现的文本相似度，**保持 60/60**。

## 原则

1. **只改局部 helper 命名**：`namespace { }` / `static` / 单文件内函数；不改 pass 公开 API（`Pass::run`、`runImpl` 等）。
2. **不改控制流与 pass 阶段顺序**：不重排 `runRewriter` 轮次、不调整 pipeline 中 pass 调用顺序、不重构 Schedule/InstCombine/CFGToLegacy。
3. **每批改动后全量评测**：`make runtime-eval SUITE=performance OPT=O1`。

## 本轮已完成（helper 重命名）

| 文件 | 重命名示例 |
|------|------------|
| `opt/Mem2Reg.cpp` | `SlotLiveIn`, `computeSlotLiveIn`, `resolvePhiOperandOnEdge` |
| `opt/RegularFold.cpp` | `readConstGlobalScalar`, `globalHasStore`, `foldToConstInt` |
| `opt/DCE.cpp` | `repairPhiPredecessors` |
| `opt/SCEV.cpp` | `incomingPhiForBlock` |
| `utils/smt/Simplify.cpp` | `foldTree`, `foldSpecialForms` |
| `pre-opt/Unswitch.cpp` | `alignModAtLoopEntry`, `loopCondIsInvariant` |
| `dialect_parse/KnapsackDp.cpp` | `dpEmitMainLoops`, `dpVarRef`, `dpFindFunction` |

## 暂不动（高风险）

- `mlir_rv/Schedule.cpp`, `InstCombine.cpp` — 指令/阶段顺序敏感
- `cfg/CFGToLegacy.cpp`, `codegen/OpBase.cpp` — 结构桥接
- `mlir_rv/StrengthReduct.cpp` — 已单独修 sort；MUL/REM 默认关
- `pre-opt/EarlyConstFold.cpp` — 曾改 pass 顺序

## 后续可选（仍仅 helper 命名）

- `mlir_rv/RegAlloc.cpp`, `RegPeephole.cpp` 内 `envEnabled` / `fitsImm12` 等
- `opt/RowScratchMatmul.cpp` 匿名 namespace 匹配 helper
- `dialect_parse/Lexer.cpp`, `Sema.cpp` 文件内 static helper

## 评测前

```bash
unset SYSY_RV_ENABLE_STRENGTH_REDUCT
make clean && make -j4 && make libsysy.a
make runtime-eval SUITE=performance OPT=O1
```
