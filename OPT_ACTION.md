# OPT_ACTION — 编译器优化实施行动文件

> **用法**：每次让我改代码前，把本文件 `@OPT_ACTION.md` 发给我，并说明「做到 Phase X / 步骤 X.Y」。
> 我按本文件顺序执行，不跳步、不扩 scope，改前先 asm A/B，改后必跑验收命令。

---

## 0. 项目约束（始终遵守）

| 项 | 规则 |
|----|------|
| 工作目录 | 只改 `compiler2026-x`，**不改** reference 项目 |
| Git | 不 revert、不 force push；对比 baseline 用 `git worktree`，不动主工作树 checkout |
| 提交 | 只有你明确说「commit / push」时才做 |
| 改代码原则 | 最小 diff；一次只做一个 pass / 一条 reject 规则；不顺手重构 |
| 性能结论 | 以 **asm 是否变化** 为准；QEMU 需同机连跑 2–3 次看趋势，单次不比 |
| **Pass 环境** | 若 shell 里设了 `SYSY_CC_NO_GUARDED_ACCUM=1` 等，pass **不会运行**；验证前 `unset` 或 `env -u` |

**Baseline 编译器（固定对比点）**：

```bash
git worktree add /tmp/compiler2026-baseline <commit> --detach
cd /tmp/compiler2026-baseline && make -j4 && make libsysy.a
```

**当前分支工作区**：

```bash
cd /mnt/c/Users/asus/compiler2026-x && make -j4 && make libsysy.a
```

---

## 1. 每次会话标准流程

```
[1] 读 OPT_ACTION.md → 确认目标 Phase/步骤
[2] 对靶题跑 asm A/B + pass stats（Phase 0 命令）
[3] 只改本步骤列出的文件/逻辑
[4] 跑本步骤验收命令
[5] 汇报：改了什么 / asm 是否变 / stats / 能否进入下一步
```

**Asm A/B（单题）**：

```bash
BASE=/tmp/compiler2026-baseline/compiler   # 或你指定的 commit
CAND=$PWD/compiler
CASE=performance/matmul2.sy                # 本步骤靶题

$BASE -S -O1 -o /tmp/base.s "$CASE"
$CAND  -S -O1 -o /tmp/cand.s "$CASE"
diff -q /tmp/base.s /tmp/cand.s && echo "asm SAME" || echo "asm DIFFERS"
```

**Pass 统计**：

```bash
SYSY_CC_PASS_STATS=1 ./compiler -S -O1 -o /tmp/x.s "$CASE" 2>&1 \
  | grep -iE 'row-scratch|loop-tiling|guarded|mat-transpose|replaced|tiled|lifted|rewrite|reject'
```

**RowScratch 拒绝原因（诊断用）**：

```bash
SYSY_CC_ROW_SCRATCH_DEBUG=1 SYSY_CC_PASS_STATS=1 ./compiler -S -O1 -o /tmp/x.s "$CASE" 2>&1 \
  | grep 'row-scratch-k'
```

**功能冒烟**：

```bash
export LIBSYSY=$PWD/libsysy.a
./scripts/cmp_o1_tiers.sh performance/matmul2.sy performance/transpose2.sy
```

**性能全集（仅 Phase 完成或你明确要求时）**：

```bash
RUNTIME_PERF_TIMEOUT_SEC=120 make runtime-eval SUITE=performance OPT=O1
make runtime-summary
```

---

## 2. 靶题清单（按优先级）

| 优先级 | 用例 | 主要 pass |
|--------|------|-----------|
| P0 | `matmul2`, `01_mm2`, `many_mat_cal-1` | RowScratchMatmul |
| P1 | `matmul1/3`, `01_mm1/3`, `many_mat_cal-2/3` | RowScratchMatmul |
| P1 | `transpose2`, `matmul2` | LoopTiling |
| P2 | `transpose0/1`, `shuffle1/2`, `sl2` | LoopTiling / MatTransposePair |
| P2 | `conv2d-1/2/3` | 仅回归正确性（GuardedAccum 勿误匹配） |
| P3 | `optimization_scheduling*`, `knapsack*` | GuardedAccum |

**Pass 开关（默认全开；`scripts/opt-passes-on.sh` 会 unset 残留 NO_*）**：

```bash
source ./scripts/opt-passes-on.sh   # GuardedAccum + MatTransposePair（eval 已自动 source）
source ./scripts/opt-passes-on-experimental.sh   # 额外 Reassociate + Fusion，需单独 eval
# 仅 bisect 时临时关闭：
SYSY_CC_NO_ROW_SCRATCH_MATMUL=1
SYSY_CC_NO_LOOP_TILING=1
SYSY_CC_NO_GUARDED_ACCUM=1
SYSY_CC_NO_MAT_TRANSPOSE_PAIR=1
SYSY_CC_TILE_SIZE=32          # LoopTiling 试验
SYSY_CC_ENABLE_NESTED_LOOP_TILING=1
```

---

## 3. 分阶段任务（按顺序做，完成打 `[x]`）

### Phase 0 — 基础设施（先做，或与新 Phase 并行）

- [x] **0.1** 确认 baseline worktree 可编译，`/tmp/compiler2026-baseline/compiler` 可用
- [x] **0.2** 对 P0+P1 共 ~15 题跑 asm A/B 表（asm SAME/DIFF + 主要 pass stats 一行/题）
- [x] **0.3** 记录 baseline kernel 合计（`make runtime-summary` 或 CSV），作后续对比锚点

**验收**：能回答「哪几题 asm 已变、哪几题 RSM rejected 最多」。

---

### Phase 1 — RowScratchMatmul 覆盖面（最高 ROI）

**文件**：`src/opt/RowScratchMatmul.cpp`，pipeline 已在 `src/dialect_pipeline.cpp`

- [x] **1.1** 对 P0 三题跑 `SYSY_CC_ROW_SCRATCH_DEBUG=1`，汇总 Top reject 原因
- [x] **1.2** 只放宽 **一条** 最高频 reject（`loads`：merge phi  guarded accum 形式）
- [ ] **1.3** 验收 P0：`replaced ≥ 1` 且 asm DIFFERS；`cmp_o1_tiers` 通过
- [ ] **1.4** 重复 1.2–1.3 直到 P0 三题至少 2 题 replaced，或 reject 无法再降
- [ ] **1.5** 扩展到 P1 matmul / many_mat_cal 变体

**禁止**：一次改多条 reject；未 asm 变化就调 QEMU 报「提升」。

**完成标准**：`matmul2` + `many_mat_cal-1` asm 相对 baseline 有 intentional diff，且 60 题仍 AC。

---

### Phase 2 — LoopTiling 覆盖面

**文件**：`src/opt/LoopTiling.cpp`（dialect）；AST 分块在 `src/loop_tiling.cpp` 仅 D 档相关

- [ ] **2.1** P0/P1 题统计 `tiled / rejected-shape / rejected-profit`
- [ ] **2.2** 试 `SYSY_CC_TILE_SIZE=16|32|64` 对 `matmul2`、`transpose2` asm A/B
- [ ] **2.3** 若 profit 拒绝多：只改 profit 或上界阈值，**单点** patch
- [ ] **2.4** 评估 `SYSY_CC_ENABLE_NESTED_LOOP_TILING=1` 对 `01_mm2`（先 asm+正确性）

**完成标准**：至少 2 题 `tiled ≥ 1` 且 asm 变化；transpose2 或 matmul2 之一有 diff。

---

### Phase 3 — MatTransposePair / GuardedAccum（低优先级，有 IR 再做）

**文件**：`src/opt/MatKernelOpt.cpp`，pipeline `src/dialect_pipeline.cpp`

- [x] **3.1** transpose2 / many_mat_cal-1：MatTransposePair `rewrites=0`（performance 集无 b[i][j]=a[j][i] 在 pass 前存活）
- [ ] **3.2** 若无 swap 索引：改 `mkoParseGrid` / 用 `SubscriptAttr`，或认定「performance 无此 IR」并文档化
- [ ] **3.3** 若有：保证 pass 在 DSE 前；`rewrites > 0` + asm diff 的小测例回归
- [x] **3.4** GuardedAccum：保持 **merge 仅单 acc phi**；扩 else 分支 + compact merge（含 Lt/branch）

**完成标准**：转置相关题 `rewrites > 0` 或书面结论「performance 集无匹配 IR」；conv2d 三题 link 正常。

---

### Phase 4 — 全量 gate（你要求跑分再做）

- [ ] **4.1** `RUNTIME_PERF_TIMEOUT_SEC=120 make runtime-eval` → **60/60**（`eval-runtime.sh` 已自动 `opt-passes-on`）
- [ ] **4.2** 与 baseline CSV 比：`./scripts/eval-vs-baseline.sh baseline.csv candidate.csv`
- [ ] **4.3** 合并门槛：median_ratio ≤ 1.05，且至少 3 题 asm 有 intentional 改进

---

## 4. 已完成记录（实施时更新）

| 日期 | 步骤 | 摘要 |
|------|------|------|
| 2026-05-29 | 0.3 | baseline @ abbf8a4：60/60，kernel 合计 **3876.724ms**（CSV: sysy-performance-O1.csv） |
| 2026-05-29 | 1.2b | `load-global-set`：三矩阵 matmul 按 (i,k)/(k,j) 索引解析；helper 增 outPtr；guarded-k 拒绝条件累加 |
| 2026-05-29 | 3.4 | GuardedAccum：else 分支 + compact merge；matmul2 `lifted=1` asm 204→205；conv2d OK |
| 2026-05-29 | 4.2 | kernel 合计 6819 vs baseline 3877（×1.76）；wall median 2.15 失真；eval 改 kernel 列 |
| 2026-05-29 | opt-on | `opt-passes-on.sh` 仅 GuardedAccum+MatTransposePair；Reassociate/Fusion 改 experimental |
| | | performance 集上两 pass asm 大多 SAME，暂无涨分 |
| 2026-05-30 | bisect | 同 session 全量 O1 bisect（60/60，kernel 列）；anchor abbf8a4 **3876.724ms** |

**2026-05-30 bisect（`opt-passes-on` + `RUNTIME_PERF_TIMEOUT_SEC=120`）**

| 配置 | kernel 合计 | vs abbf8a4 | vs all-on |
|------|------------:|-----------:|----------:|
| ALL_ON | **7933.167ms** | ×2.05 | — |
| `NO_GUARDED_ACCUM=1` | 11068.504ms | ×2.86 | +3135ms |
| `NO_ROW_SCRATCH_MATMUL=1` | 9136.241ms | ×2.36 | +1203ms |
| `NO_LOOP_TILING=1` | 9866.658ms | ×2.55 | +1934ms |

CSV：`tests/.out/runtime/bisect-{all-on,no-guarded,no-rsm,no-tiling}.csv`

结论（相对 all-on 关 pass 更慢 → pass 净收益为正，但仍远差于 abbf8a4）：
- **GuardedAccum**：净 +3135ms；帮 matmul2/sl/shuffle2；害 crypto-1/03_sort1/01_mm2
- **RowScratchMatmul**：净 +1203ms；帮 many_mat_cal/matmul2；害 crypto-1/shuffle1/transpose2
- **LoopTiling**：净 +1934ms；帮 matmul3/matmul2/many_mat_cal；害 crypto-1/shuffle1/transpose2

Phase 4 gate 仍 fail；单靠关任一 pass 无法回到 baseline，需按 case 选择性禁用或修 crypto/sort 回归。

---

## 5. 你给我指令时的推荐格式

```
@OPT_ACTION.md
做到 Phase 1 步骤 1.2
baseline commit: abbf8a4
额外：只改 matmul2 相关 reject，不要动 LoopTiling
```

我会：读本文件 → 执行对应步骤 → 跑验收 → 更新 §4 已完成记录（若你要求）。

---

## 6. 明确不做的事（除非你在指令里写例外）

- 不整 pipeline 重排
- 不默认开 `SYSY_CC_ENABLE_IR_NORMALIZE`
- 不为涨分放宽 GuardedAccum merge 条件
- 不在 asm 未变时声称「性能优化有效」
- 不修改 `performance/` 官方用例源码
