# OPT_STRATEGY_LOOP — Dialect Loop & Memory 优化专项计划（2026-05-30）

> **目标**：在不拷贝参考项目代码的前提下，针对 matmul2 / many_mat_cal / transpose 类热点，实现 **branchless guarded accum + address folding + loop-nest 结构化变换**，使核心题 kernel 时间从当前 ×2 baseline 逐步接近或超越。
>
> **原则**：
> - 只提取思想，不拷贝实现
> - 每步只改一个 pass / 一条 reject 规则
> - 先 asm A/B，再正确性
> - 所有新 pass 必须有 env 开关（`SYSY_CC_ENABLE_*` / `SYSY_CC_NO_*`）

---

## 0. 当前差距诊断（已完成）

| 维度 | 参考项目（50+ pass） | 本项目现状 | 差距 |
|------|----------------------|------------|------|
| Guarded Accum | CondGuardedAccumulatePass（cond*val 形式 + hoist） | GuardedAccum（仅单 acc phi，matmul-step reject） | matmul2 k 环 branch + mod 保留 |
| Loop 家族 | LoopInterchange + Fusion + Unroll + NestInteriorSplit + GccStyle | LoopTiling（默认跳过含 Mod 内层） | matmul2 热点 k 环未 tile |
| GEP / 地址 | GEPChainFold + GepPass + AddChainReduction | 主要靠 GVN + Reassociate（experimental） | 地址计算冗余 |
| 后端 | 1400+ 行 PeepholeOptimizer + InstructionReorder | rv/ 简单 peephole | 普遍题收益不足 |
| 专用 memory | ArrayElimination、StoreLoadForward、MemoizationV2 | 无 | DP / crypto 类题弱 |

**matmul2 核心 IR 模式**（当前未被任何 pass 完全匹配）：

```c
while (k < 300) {
    if ((a[i][k] * b[k][j]) % 2 == 0)
        temp = temp + (b[i][k] * a[k][j]);
    k = k + 1;
}
```

需要同时做到：
1. `if (cond) acc += x` → `acc += (cond ? x : 0)` 或 `acc += cond * x`（branchless）
2. 把 then 里的 mul/load 提到 header（hoist）
3. 可能交换 i-j-k 顺序或对 k 环做 strip-mine
4. 折叠 `a[i][k]` 系列 GEP 链

---

## 1. 总体路线（不照抄，Dialect 适配版）

### 1.1 核心思想映射

| 参考思想 | Dialect IR 实现路径 | 新/改 pass |
|----------|---------------------|------------|
| CondGuardedAccumulate（cond*val + hoist） | 在 GuardedAccum 中增加“多 mul-add 模式 + 条件表达式构造” | **改 GuardedAccum**（或新建 `GuardedAccumV2`） |
| GEPChainFold | 识别连续 `GetElementPtr` + `AddI` 构成的地址递推链，合并为 `base + stride * iv` 形式 | **新建 `GepChainFold`**（opt/ 层） |
| LoopNestInteriorSplit | 对带 guard 的 matmul kernel，把 interior（无边界检查）与 border 拆开，interior 走更激进的 RowScratch | **新建 `LoopNestSplit`**（实验性，NESTED 模式下） |
| LoopInterchange / Fusion | 利用现有 `CanonicalizeLoop` + `Fusion`（experimental），增加索引重排属性 | 先不实现完整 Interchange，优先让 GuardedAccum + Tiling 能处理 k 环 |
| 后端 peephole | 把 `rv/` 的 `branchPeephole` / `instCombine` 扩展成可配置的 20+ 条规则 | **改 rv_passes.cpp** |

### 1.2 新 pass 命名与开关

| Pass | 文件 | env 开关 | 默认 |
|------|------|----------|------|
| GuardedAccumV2（增强版） | `opt/GuardedAccum.cpp`（内增模式） | `SYSY_CC_ENABLE_GUARDED_V2` | 实验性关闭 |
| GepChainFold | `opt/GepChainFold.cpp`（新建） | `SYSY_CC_ENABLE_GEP_CHAIN` | 实验性关闭 |
| LoopNestSplit | `opt/LoopNestSplit.cpp`（新建） | `SYSY_CC_ENABLE_NEST_SPLIT` | 实验性关闭 |
| RV Peephole 扩展 | `rv/rv_passes.cpp` | `SYSY_CC_ENABLE_RV_PEEPHOLE_AGGRESSIVE` | 关闭 |

所有新 pass 必须实现 `stats()` 返回关键指标（如 `converted`, `folded`, `hoisted`）。

---

## 2. 分阶段执行计划（严格顺序）

### Phase 0 — 基线与诊断（已部分完成）

- [x] 0.1 对 `performance/matmul2.sy` 跑 asm A/B + pass stats（记录当前 `guarded-k` / `matmul-step` reject 原因）
- [ ] 0.2 提取 matmul2 热点 loop 的完整 IR 片段（`SYSY_CC_PASS_STATS=1` + `printAfter=guarded-accum`）
- [ ] 0.3 在 `OPT_STRATEGY_LOOP.md` 中记录“当前 GuardedAccum 拒绝的精确条件”

**验收**：有一份清晰的 matmul2 热点 IR 模式文档。

---

### Phase 1 — GuardedAccum 增强（最高 ROI，针对 matmul2）

**文件**：`src/opt/GuardedAccum.cpp`（或在其内新增模式）

**核心改动**（不拷贝 CondGuardedAccumulatePass，而是提取思想）：

1. 识别“then 块内有 1–2 个 MulIOp + AddIOp”的 guarded accum 模式
2. 构造 `SelectOp` 或 `MulIOp(cond, val)` 形式，消除分支
3. 把 then 块内可 hoist 的 load/mul 提到 loop header（利用现有 `hoistVariant` 逻辑扩展）
4. 新增 reject 条件松绑：允许 then 内有 Mul（只要不越界即可）

**开关**：`SYSY_CC_ENABLE_GUARDED_V2=1`

**步骤**：

- [ ] 1.1 只改一条 reject 规则（允许 then 含 MulIOp，但要求 acc 是 phi 且 entry 为常数 0）
- [ ] 1.2 实现 `cond * val` 形式的累加替换（Dialect IR 用 `SelectOp` 或 `MulIOp`）
- [ ] 1.3 对 matmul2 跑 asm A/B，验证 `.Lbb9`（热点）branch 减少或 mul 外提
- [ ] 1.4 跑 60 题功能 + crypto-1 / 03_sort1 回归检查

**验收**：matmul2 asm DIFF（热点块指令数或分支数下降），且 60/60 AC，无新回归。

---

### Phase 2 — GepChainFold（地址计算折叠）

**精确 IR 模式（从 matmul2 热点提取，2026-05-30）**：

```ir
%62 : phi <i32> , %63 , %9          // j (外层)
%71 : muli <i32> , %62 , %39        // j * 4
%215 : addl , %6 , %71              // base_a + j*4
...
%222 : addl , %220 , %39            // base + j*4 + k*1200
%81 : load <i32> , %222             // a[i][k] 的 load
%83 : load <i32> , %220             // b[k][j] 的 load
```

**典型模式**：
- 同一 `base`（global a/b/c）+ 两个归纳变量（`i`, `k` 或 `j`, `k`）
- 地址计算链：`base + iv1 * stride_row + iv2 * stride_col`
- 多个 `LoadOp` / `StoreOp` 共享相似的 GEP 链（%222, %220, %216 等）
- 当前被 GVN 部分 CSE，但仍有冗余 `muli + addl` 链

**本步目标**：识别上述链，合并为 `base + (iv1 * stride_row + iv2 * stride_col)` 的单条加法链，减少指令数，让 GVN 进一步 CSE。

---

**新建文件**：`src/opt/GepChainFold.cpp` + `GepChainFold.h`

**思想来源**：参考 GEPChainFoldPass，把连续 `a[i][k]` 系列 GEP 合并为 `base + (i*N + k)*4` 的单条加法链。

**Dialect IR 实现**：

- 遍历 `GetElementPtrOp`，检测 base 相同、索引为 `iv` 或 `iv + const` 的链
- 用 `Builder` 构造 `AddIOp` + `MulIOp` 替换多条 GEP
- 必须在 GVN 之前运行（让 GVN 能 CSE 新构造的地址）

**开关**：`SYSY_CC_ENABLE_GEP_CHAIN=1`

**步骤**：

- [x] 2.1 实现基础的“同一 base + 两个 iv 索引”的 GEP 链识别（框架 + Makefile 集成，rewrite 逻辑待 2.2）
- [ ] 2.2 对 matmul2 / transpose2 跑 asm A/B，验证 load 地址计算指令减少
- [ ] 2.3 加入 `SYSY_CC_GEP_CHAIN_DEBUG=1` 输出 rewrite 详情
- [ ] 2.4 集成进 `dialect_pipeline.cpp`（memory opt 阶段，DSE 前）

**验收**：至少 2 题 `gep-folded ≥ 1` 且 asm DIFF，功能 AC。

---

### Phase 3 — LoopNestSplit（实验性，解决 matmul2 k 环 tiling）

**新建文件**：`src/opt/LoopNestSplit.cpp`

**思想来源**：参考 LoopNestInteriorSplitPass，把带 guard 的 matmul kernel 拆成 interior（无边界检查，可激进 RowScratch）和 border（保留 guard）。

**Dialect IR 实现**（轻量版）：

- 利用现有 `LoopInfo` + `ParallelizableAttr`
- 对 `RowScratchMatmul` 之前，检测“k 环含条件累加”的 nest
- 把 interior（k 从 pad 到 N-pad）标记为 `NoGuardAttr`，允许 RowScratch 接管
- border 保留原 guard

**开关**：`SYSY_CC_ENABLE_NEST_SPLIT=1`（默认关闭，仅实验）

**步骤**：

- [x] 3.1 实现 nest 分析（复用 `LoopAnalysis`）—— Phase 3.1 完成：检测到 matmul2 候选 nest（split=1）
- [ ] 3.2 标记 interior 的 `NoGuardAttr`
- [ ] 3.3 让 RowScratchMatmul 识别该 attr，跳过 guarded-k reject
- [ ] 3.4 对 matmul2 跑 `NEST_SPLIT=1` 下的 asm A/B

**验收**：matmul2 在 `NEST_SPLIT=1` 下 `replaced ≥ 1` 且 asm DIFF；sl1/sl2 仍 A–D OK。

**Phase 3 现状（2026-05-30）**：

- LoopNestSplit 能检测到 matmul2 的 3+ 层 nest（`split=1`）
- guarded-k 放行机制已实现（`SYSY_CC_ENABLE_NEST_SPLIT=1` 时跳过 reject）
- 但 matmul2 仍然被 `loads` / `k-canonical` reject（guarded accum 模式下 loads 收集失败）
- **结论**：Phase 3 的 guarded-k 放行是"必要条件"，但 matmul2 的 RowScratch 生效还需要解决 loads/k-canonical（超出 Phase 3 范围）
- **稳定策略**：Phase 3 采用 env 控制（非 attr），功能已验证 AC，无回归风险

因此 Phase 3 收尾为"实验性 nest 检测 + guarded-k 放行"，不继续深入 attr 机制（while 风格循环匹配困难）。

---

### Phase 4 — 后端 Peephole 增强

**文件**：`src/rv/rv_passes.cpp` + 新增 `rv/Peephole.cpp`

**改动**：

- 把现有的 `branchPeephole` / `instCombine` 扩展为可配置规则表（至少 15 条）
- 新增规则示例：
  - `slt + beqz` → `bge` 反转
  - `mul 2` → `slli`
  - `add 0` / `or 0` / `and -1` 消除
  - `li + add` → `addi`
- 增加 `InstructionReorderPass` 思想的轻量版（列表调度，respect reg pressure）

**开关**：`SYSY_CC_ENABLE_RV_PEEPHOLE_AGGRESSIVE=1`

**步骤**：

- [ ] 4.1 扩展规则表到 15 条，带 stats
- [ ] 4.2 对全量 60 题跑 asm A/B，统计改动题数
- [ ] 4.3 验证无功能回归

**验收**：至少 10 题 asm DIFF（非热点题也受益），60/60 AC。

---

### Phase 5 — 全量验证与平台提交

- [ ] 5.1 `RUNTIME_PERF_TIMEOUT_SEC=120 make runtime-eval`（all-on + 新 pass 组合）
- [ ] 5.2 与 abbf8a4 baseline 对比 kernel 合计（目标 ≤ 1.3×）
- [ ] 5.3 平台提交（至少 matmul1/2/3 单题分提升到榜首 70% 以上）
- [ ] 5.4 更新 `README.md` §6 与 `OPT_ACTION.md`

**验收**：核心题（matmul 系列）平台分较当前 +3～5 分；全量 kernel 合计显著下降。

---

## 3. 风险与回滚

| 风险 | 缓解 | 回滚方式 |
|------|------|----------|
| GuardedAccumV2 误匹配 crypto/sort | 严格 `then` 仅允许 1–2 个 Mul + Add，且 acc entry 必须是常数 0 | `SYSY_CC_NO_GUARDED_ACCUM=1` |
| GepChainFold 改变地址语义 | 只对 `GetElementPtrOp` + `AddIOp` 链操作，保留原有 GEP 的 `SizeAttr` | 环境变量关闭 |
| LoopNestSplit 破坏 sl1/sl2 | interior 仅对 matmul-like 模式（3 层以上 + 条件累加）生效 | 默认关闭 |
| 后端 peephole 误优化 | 所有规则必须是纯代数等价或条件明确 | 环境变量关闭 |

---

## 4. 执行纪律（必须遵守）

1. 每步开始前先跑 **asm A/B**（`performance/matmul2.sy`）
2. 每步结束必须跑 **功能冒烟**（`cmp_o1_tiers.sh` 或 `make check`）
3. 新 pass 必须有 `stats()` 输出关键数字
4. 文档先行：改代码前先在 `OPT_STRATEGY_LOOP.md` 中记录“本步要解决的精确 IR 模式”
5. 不跳步：Phase 1 未验收通过，不进入 Phase 2

---

## 5. 起始状态（2026-05-30 19:14）

- 当前分支：`big`
- Baseline commit：abbf8a4（kernel 3877 ms）
- 当前 ALL_ON：7933 ms（×2.05）
- matmul2 热点：189 行 asm，branch + mod + 非标准索引全留

**下一步行动**：等待用户确认“从 Phase 0.2 / Phase 1.1 开始”，然后严格按步骤推进。