# 编译器优化路线图（2026.05）

## 当前状态（2026.05.23 更新）

- 性能测试（O1）：60/60 pass，median_sum ≈ 53,653 ms（已启用安全 RV peephole 基线）
- 已实现：Mem2Reg、mm_hoist、loop_interchange、loop_tiling、RowScratchMatmul（AST/HIR）、双层 IR 骨架、**Stage 3 安全 RV peephole（默认开启）**
- 主要瓶颈：shuffle1（~8000 ms）、h-5* 系列（~2500-3800 ms）、h-8*、conv2d-1、many_mat_cal*、shuffle2
- 新基线开关：`SYSY_CC_ENABLE_RV_ASM_PASSES=1` 已改为默认 ON（仅安全 mv/addi 0 删除）；激进 peephole 和 Schedule 仍需显式开启

目标：把 geometric mean 性能得分拉到高分区间（参考 Sisyphus ~70 分水平）。

---

## 整体优化策略

采用 **“快速提分 + 架构升级 + 后端强化”** 三轨并行策略。

### 阶段 1：快速提分（优先级最高，1–2 周见效）

目标：median_sum ≤ 55,000 ms

| 优先级 | 优化项 | 位置 | 预期收益 | 当前状态 | 验证命令 |
|--------|--------|------|----------|----------|----------|
| 1 | RowScratchMatmul 真正实现（hoist / scratch rewrite） | AST + HIR | matmul / many_mat_cal / conv2d 大幅提速 | 骨架已完成 | `make runtime-eval` |
| 2 | RV StrengthReduct（魔法除法 + mul 分解） | `codegen.cpp` / 新建 `rv/` | crypto / h-* / 除法密集用例 | 未做 | 同上 |
| 3 | 常量乘法分解增强（popcount 形式） | `emitMulByConst` | h-* / shuffle 部分收益 | 已部分实现 | 同上 |
| 4 | 小循环展开（带安全 guard） | `loop_unroll.cpp` | 03_sort* / shuffle* | 已实现但默认关 | 同上 |
| 5 | 后端简单 peephole（load-load CSE、算术合并） | codegen 尾部 | 整体小幅提速 | 未做 | 同上 |

**阶段 1 原则**：
- 每加一个 Pass 必须有独立 env flag（`SYSY_CC_ENABLE_*`）
- 重点跟踪用例：shuffle1、h-5-01、01_mm1、matmul1
- 每完成一个子任务立即跑 `make runtime-eval SUITE=performance OPT=O1`

---

### 阶段 2：完善双层 IR 架构（中长期基础建设）

目标：让 HIR 真正成为可优化的中间表示

| 任务 | 说明 | 优先级 | 状态 |
|------|------|--------|------|
| 实现真正的 `lowerToLegacyIR` | HIR → 现有 IRInst / 新 Lowered IR | 高 | 占位符 |
| 把 RowScratchMatmul 彻底迁移到 HIR 层 | 结构化 While/For 模式识别 + 实际变换 | 高 | 正在实现 |
| HIR 版 Loop Interchange / Tiling | 利用 HIR 的 While/For 结构做更准确的变换 | 中 | 参考已有 AST 版 |
| HIR 版 LoopUnroll（const + factor） | 带 trip 分析和 body 安全检查 | 中 | 已有 AST 版 |
| HIR 版 LICM / ScalarReplace | 在结构化控制流上做不变量外提 | 低 | 按需 |

**阶段 2 价值**：为阶段 3 的复杂优化提供安全、强大的基础，同时代码更易维护。

---

### 阶段 3：RISC-V 后端强化（高性价比提分）

Sisyphus 在 RV 后端做了大量针对性工作，这是当前项目最缺的一环。

| 优化 | 文件位置 | 预期效果 | 优先级 |
|------|----------|----------|--------|
| rv-schedule（指令调度） | 新建 `src/rv/Schedule.cpp` | shuffle / sort / h-5/h-8 明显改善 | 高 |
| StrengthReduct 完整版 | `src/rv/StrengthReduct.cpp` | 除法/模/乘法密集用例 | 高 |
| RegPeephole / InstCombine | `src/rv/RegPeephole.cpp` | load-load CSE、算术再结合 | 中 |
| 更好的图着色分配调优 | `src/ir_regalloc.cpp` | 减少 spill | 中 |
| 循环不变量 li 提升 | StrengthReduct 中 | 小幅提速 | 低 |

**阶段 3 验证重点**：shuffle1、03_sort*、h-5*、h-8*

---

### 阶段 4：中端激进优化（高风险，按需开启）

仅在前面三阶段完成后，且性能仍不达标时才考虑。

- 多轮支配树 GVN + 固定点
- 更激进的 Reassociate + CSE
- Global Code Motion (GCM)
- 跨函数内联 + 常量参数特化
- Range / EqClass 分析

**风险**：极易导致 sort/shuffle 类用例变慢或 WA，必须加严格复杂度控制和独立开关。

---

## 验证与风险控制

1. **每阶段必须有独立验证**
   - `make runtime-eval SUITE=performance OPT=O1`
   - `make runtime-summary`
   - 重点用例单独记录时间（shuffle1、h-5-01、01_mm1、matmul1）

2. **Env Flag 规范**
   - 所有新 Pass 必须有 `SYSY_CC_ENABLE_*` / `SYSY_CC_NO_*` 开关
   - 默认保守（对性能有风险的 Pass 默认关闭）

3. **保持干净 baseline**
   - 重要 commit 前创建 tag（如 `baseline-2026-05-22`）
   - 任何大改动必须可快速回退

4. **文档同步**
   - 每次重要优化都要更新本文件和 `EVAL_BUGLOG.md`

---

## 当前优先任务（2026.05.23 之后，新基线：安全 RV peephole 默认开启）

1. **诊断并修复 shuffle1 性能回退**（安全 RV 后反而变慢，需 asm diff + spill 分析）
2. **尝试开启 Schedule（SYSY_CC_ENABLE_RV_SCHEDULE=1）**，看是否能把 shuffle1 拉回来
3. **RowScratchMatmul 真正落地**（HIR 版完整接上 CodeGen）
4. **RV StrengthReduct 增强**（asm 层 mul→shift、rem 优化）
5. 继续跟踪 h-5*、h-8*、conv2d-1、many_mat_cal* 等用例

---

## 参考资料

- Sisyphus 仓库：`compiler2026-sisyphus-pure-rv`
  - `src/opt/RowScratchMatmul.cpp`
  - `src/rv/StrengthReduct.cpp`
  - `src/rv/Schedule.cpp`
  - `src/main/PipelineProfiles.cpp`
- 当前项目：`OPTIMIZATION_ROADMAP.md`（本文件）
- 性能数据：`tests/.out/runtime/sysy-performance-O1.csv`

---

**更新日期**：2026-05-22

**维护者**：zky + Cursor Agent

本路线图将随实际进展持续更新。