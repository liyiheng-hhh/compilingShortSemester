# OPT_SCORE_ROADMAP — 60 题真正涨分实施计划

> **目标**：平台 `compiler -S -O1` 默认管线涨分，不靠 env、不靠手工开关。  
> **参考**：`compiler2026-main (2)` lightsmile 的 O1 管线（~50 IR pass + 后端 CSE/LICM 三件套）。  
> **用法**：每轮实现只做一个 Phase 内的一个步骤；验收通过再进下一步。配合 `OPT_ACTION.md` 的 asm A/B 流程。

---

## 0. 涨分判据（别再用错指标）

| 指标 | 用途 | 通过标准 |
|------|------|----------|
| **功能** | 平台 AC | `make check` + 代表题 `cmp_o1_tiers.sh` |
| **Asm DIFF** | pass 真生效 | 靶题 `.s` 与 baseline **DIFF**，且 diff 在热点 loop（如 matmul2 `.Lbb9`） |
| **Pass stats** | rewrite 发生 | 关键计数 **>0** 且 asm 变（只计数不改 IR 不算完成） |
| **Kernel 时间** | 最终涨分 | `make runtime-eval SUITE=performance` 或平台上 matmul/many_mat_cal 分数下降 |

**禁止**：「框架 + 检测 + folded++」当完成；「operand swap 但行数不变」当大涨分。

**Baseline 固定点**：当前 `big` 分支 HEAD（或你指定的 abbf8a4），每 Phase 记录 kernel 合计 ms。

---

## 1. 代表题矩阵（60 题抽样，每 Phase 必测）

| 代号 | 用例 | 测什么 |
|------|------|--------|
| M2 | `performance/matmul2.sy` | guarded accum、%2、temp 标量、RowScratch |
| M1 | `performance/matmul1.sy` | 纯 matmul |
| MMC | `performance/many_mat_cal-1.sy` | RowScratch + Tiling |
| TR | `performance/transpose2.sy` | 转置访存 |
| CV | `performance/conv2d-1.sy` | 卷积 nest |
| CR | `performance/crypto-1.sy` | mod/div 规约 |
| SO | `performance/03_sort1.sy` | 排序 / 数组 |
| KN | `performance/knapsack_naive-1.sy` | DP / alloca |
| H5 | `performance/h-5-01.sy` | 隐藏小题 |
| CRC | `performance/crc1.sy` | 位运算 |

**每步最少**：M2 + 本 Phase 相关 1 题 + `make check` 全冒烟 3 题。

**Phase 结束**：上表 10 题全跑 `cmp_o1_tiers.sh` + asm A/B 表。

---

## 2. 参考项目 vs 我们 — 缺口一览

### 2.1 后端（文本 asm，平台必走）

| 参考 | 我们 | 状态 |
|------|------|------|
| BlockLocalCSE（完整 live + replace） | `rv/BlockLocalCSE` | ⚠️ 骨架，需补全 |
| LICM（la/li 外提到 preheader） | 无 | ❌ |
| 顺序：CSE → LICM → CSE(laOnly) | InstCombine 后单次 CSE | ❌ |
| 寄存器分配后 SecondPeep | RegPeephole + InstCombine | 部分 |

### 2.2 前端 IR（Dialect，涨分主战场）

| 参考 pass | 我们 | 优先级 |
|-----------|------|--------|
| GlobalScalarPromotion | ScalarPromotion 空壳 | **P0** |
| CondGuardedAccumulate | GuardedAccum 只松绑 | **P0** |
| MultiplyPass + ModLoopReduction | 无 | **P0** |
| RemoveRedundantStore | DSE 部分 | **P1** |
| NormalizationPass | 默认关 | **P1** |
| ArrayElimination + RemoveOnlyWriteArray | 无 | **P1** |
| AllocaCoalesce | MoveAlloca 不同 | **P1** |
| ArrayStoreLoadForward | 无 | **P1** |
| LoopInterchange | 无 | **P1** |
| LoopFusion + BitwiseLoopFusion | pre-opt Fusion 有限 | **P2** |
| LoopNestInteriorSplit | LoopNestSplit 空壳 | **P2** |
| GEPChainFold（真 rewrite） | 仅 normalize | **P2** |
| AddChainReduction | Reassociate 部分 | **P2** |
| LoopInductionSR / SRFixed | SCEV + rv SR 部分 | **P2** |

### 2.3 已有但需修（不是新建，是让它命中）

| Pass | 问题 | 动作 |
|------|------|------|
| RowScratchMatmul | matmul2 `loads` / `k-canonical` reject | Phase 3 修 matcher |
| GuardedAccum | 无 branchless | Phase 2 重写 |
| LoopTiling | mod/guard 内层不 tile | Phase 3 放宽或 NestSplit 真拆 |
| GepChainFold | 无 CSE 子表达式 | Phase 4 或并入 Reassociate |

---

## 3. 实施 Phase（严格顺序）

### Phase A — 后端三件套（全 60 题基础分）

**为什么先做**：参考固定跑；改动 isolated；不破坏 IR 语义；matmul/transpose 重复 `la` 立刻少指令。

#### A.1 完善 `rv/BlockLocalCSE.cpp`

- [ ] 实现 `isRegDefinedSince` / `isAvailEntryLive` / `areSourceOperandsLiveSinceDef`
- [ ] 实现 `replaceUsesWithCanon`：重复定义 → `mv rd, canon` 或删 dead def
- [ ] 循环 header 跳过 `li` CSE（参考 `skipMaterializeCSE`）
- [ ] `sp` 变化时 `invalidateForSp`
- **验收**：matmul2 asm 热点块 `la`/`li` 条数减少；`blockCSE_*` stats > 0；10 代表题功能 AC

#### A.2 新增 `rv/LICM.cpp` + `rv/LICM.h`

- [ ] 文本 asm 上识别 loop（backedge / 标签模式，或复用 `splitBasicBlocks` + 简单 loop 检测）
- [ ] 外提循环不变 `la`（全局地址）和虚拟 reg 的 `li` 到 preheader
- [ ] 合并相同 invariant 的 `la`（参考 `mergeLAReg`）
- **验收**：matmul2 preheader 出现外提的 `la`；循环体内 `la` 减少

#### A.3 改 `rv/rv_passes.cpp` 顺序

```text
strengthReduct → branchPeephole → regPeephole → instCombine
→ BlockLocalCSE(false) → LICM → BlockLocalCSE(true)  // laOnly
→ [schedule]
```

- **验收**：与 A.1+A.2 叠加后 kernel 时间 M2/MMC/TR 至少 2/3 下降（本地 runtime-eval）

**Phase A 完成标准**：10 代表题 asm 多数 DIFF；kernel 合计 ms 相对 baseline 下降 ≥5%（本地）。

---

### Phase B — 标量提升 + branchless（matmul2 核心）

**为什么**：matmul2 的 `temp` 在栈上反复 load/store；k 环有 `if + mod + add` 分支。

#### B.1 `GlobalScalarPromotion` → 完善 `ScalarPromotion`

**文件**：`src/opt/ScalarPromotion.cpp`（重写）

**逻辑**（对齐参考，适配 Dialect IR）：
1. 收集循环内 **AllocaOp** 标量（及可选 scalar global）的 load/store
2. 检查：所有 store 值 derive from 该 slot 的 load（参考 `loopStoresDeriveFromGlobalLoad`）
3. preheader：`load` 初值；header：`PhiOp`；循环内：替换 load/store 为 phi 链；latch：phi incoming；exit：`store` 写回
4. **插入点**：`dpipeAppendLoopOptPasses` 中 **LoopRotate 之后、RowScratch 之前**（与参考：inline 前 promotion 类似）

- **验收**：M2 asm 热点 **sw/lw temp 消失或明显减少**；`promoted >= 1`；功能 AC

#### B.2 `CondGuardedAccumulate` → 重写 `GuardedAccum` / `MatKernelOpt`

**逻辑**：
- 匹配 `if (cond) acc = acc + val`（then 无副作用）
- 生成 `acc = acc + (cond != 0) * val` 或 select/mul 等价（Dialect：MulIOp + AddIOp）
- 删除 branch 与 empty then/merge

- **验收**：M2 asm 热点 **分支数下降**（beq/bne 减少）；`lifted`/`converted >= 1`

#### B.3 `MultiplyPass` + `ModLoopReduction`（新 pass 或扩 `MatKernelOpt`）

**逻辑**：
- `x % 2 == 0` → `(x & 1) == 0` 或 parity
- 循环内 `% N` 若 N 为常数， strength reduce（参考 ModLoopReduction）
- **顺序**：在 CondGuardedAccum **之前**（参考：Mod 在 NestSplit 后、Guard 前 — 我们无 NestSplit 时可 B.2 前）

- **验收**：M2 asm 中 `remw` 热点减少；CR/crypto 代表题 asm DIFF

**Phase B 完成标准**：M2 本地 kernel 时间相对 Phase A 再降 **≥20%**；平台 matmul2 目标从 ~22s 向 ~15s 靠近（不保证追平，但需明显动）。

---

### Phase C — 内存与数组（many_mat_cal / sort / knapsack）

#### C.1 `RemoveRedundantStore`（扩 DSE 或新 pass）

- store 的值与同地址上次 load 相同 → 删 store
- **插入**：Mem2Reg 后、LoopOpt 前一轮

#### C.2 `ArrayStoreLoadForward`

- 同块同 index 的 store→load 转发

#### C.3 `ArrayElimination` + `RemoveOnlyWriteArray`

- 只写局部数组删除；可提升为 global 的局部 buffer（参考 ArrayPass）
- **适配**：Dialect 的 AllocaOp + GetGlobalOp + Alias

#### C.4 `AllocaCoalesce`（扩 `MoveAlloca`）

- alloca 推迟到首次使用块；合并同大小 alloca

- **验收**：MMC、SO、KN 代表题 asm DIFF；many_mat_cal 分数接近或超过榜首

**Phase C 完成标准**：MMC 三题 kernel 与榜首差距 <10%。

---

### Phase D — 循环变换（matmul / transpose / conv）

#### D.1 `LoopInterchange`（新 pass）

- 识别 i-j-k nest；交换使内层 stride 最小
- **插入**：RowScratch 失败时仍可用；在 Tiling 前

#### D.2 完善 `LoopNestInteriorSplit`（重写 `LoopNestSplit`）

- 参考 `kPad=2`, `kKernelSize=5`；拆 interior/border
- 让 RowScratch / Tiling 作用于 interior

#### D.3 `LoopFusion` + `BitwiseLoopFusion`（扩 pre-opt Fusion 或 opt 层）

- 相邻同界循环合并

#### D.4 `TransposePairLoadRewrite`（增强 `MatTransposePair`）

- 成对 load/store 重写

- **验收**：M1/M2/M3、TR、CV asm 热点 loop 指令数下降

---

### Phase E — 地址与收尾（全题微调 + 稳定性）

#### E.1 `AddChainReduction` + 真 `GepChainFold` rewrite

- 合并 `base + iv*stride`；提取公共 `mul(iv,N)`
- **顺序**：Reassociate → GepChainFold → GVN（参考）

#### E.2 `NormalizationPass`（可控开启）

- commutative 常数右置；比较符 canonical
- **默认**：仅 O1 开启；crypto 题若回归则加 reject

#### E.3 管线清扫

- 参考式 **DCE + SimplifyCFG** 在激进 pass 后各跑一轮
- 关掉 proven 有害 pass（Reassociate/Fusion 对 shuffle-h5 的回归用 env 保守）

#### E.4 RowScratch / LoopTiling 修 reject

- matmul2：`loads`、`k-canonical` 根因修复
- 目标：`candidates >= 1`, `replaced >= 1` on M2

**Phase E 完成标准**：60 题 `cmp_o1_tiers.sh` 全 AC；kernel 合计相对最初 baseline 下降 **≥30%**（本地）。

---

## 4. 默认 O1 管线目标形态（终态）

```text
[pre-opt] 现有 + Normalization(开) + Fusion(保守)

FlattenCFG → BitStubFold → PostFlatten → Mem2Reg

[memory] Alias → MatTransposePair → RegularFold → DCE → DAE
       → Reassociate → GepChainFold(rewrite) → DSE/DLE → GVN

[loop-early] Canonicalize → Rotate → Canonicalize
       → ScalarPromotion          ← Phase B
       → ModLoopReduction         ← Phase B
       → LoopNestSplit(真 split)  ← Phase D
       → RowScratch → LoopTiling → LICM → HoistLoopGlobal
       → GuardedAccum(branchless) ← Phase B
       → MatTransposePair → ConstUnroll → SCEV → ADCE → GVN

[misc] RegularFold → DCE → GVN → SimplifyCFG → DAE → DSE
       → GuardedAccum → Select → GCM → GVN

[late] LateInline → … → SynthConstArray

[loop-round ×3] 现有

[final] ADCE → SimplifyCFG → InstSchedule

--- codegen → rv ---

strengthReduct → branchPeephole → regPeephole → instCombine
→ BlockLocalCSE → LICM → BlockLocalCSE(laOnly)   ← Phase A
```

---

## 5. 每步工作包（复制即用）

```bash
# 1. 功能
make check

# 2. 代表题 AC
export LIBSYSY=$PWD/libsysy.a
./scripts/cmp_o1_tiers.sh performance/matmul2.sy performance/many_mat_cal-1.sy performance/transpose2.sy

# 3. Asm A/B
BASE=/tmp/compiler2026-baseline/compiler
CAND=$PWD/compiler
for c in matmul2.sy many_mat_cal-1.sy transpose2.sy; do
  $BASE -S -O1 -o /tmp/b.s performance/$c
  $CAND -S -O1 -o /tmp/c.s performance/$c
  echo -n "$c: "; diff -q /tmp/b.s /tmp/c.s || true
done

# 4. Pass stats
SYSY_CC_PASS_STATS=1 ./compiler -S -O1 -o /dev/null performance/matmul2.sy 2>&1 \
  | grep -iE 'scalar-promotion|guarded|mod|blockCSE|licm|row-scratch|gep-chain'

# 5. 性能（Phase 结束）
RUNTIME_PERF_TIMEOUT_SEC=120 make runtime-eval SUITE=performance OPT=O1
```

---

## 6. 风险与禁止项

| 不要做 | 原因 |
|--------|------|
| 一次合入多个 Phase | 无法 bisect 回归 |
| 只加 stats 不 rewrite | 平台分数不变（已踩坑） |
| 默认开 Reassociate+Fusion 全量 | 历史 shuffle/h-5 退化 |
| 跳过 make check 直接交平台 | 功能 WA 零分 |
| 复制参考 LLVM GEP pass  verbatim | IR 不同，必崩 |

| 回归时 | 动作 |
|--------|------|
| 单题 WA | 该 pass 加 conservative reject |
| 单题变慢 | asm 对比找多余 load/branch |
| 全题变慢 | 关闭本 Phase 默认，env  opt-in |

---

## 7. 建议排期（可并行一人）

| 周 | 内容 | 预期平台感知 |
|----|------|--------------|
| 1 | Phase A 完成 | 全题小幅 ↑ |
| 2 | Phase B.1–B.2 | **matmul 明显 ↑** |
| 3 | Phase B.3 + C.1–C.2 | crypto/sort 改善 |
| 4 | Phase C.3–D.2 | many_mat_cal / transpose |
| 5 | Phase D + E + 60 题扫 | 总分冲刺 |

---

## 8. 下一步（立即执行）

**从 Phase A.1 开始**：完善 `rv/BlockLocalCSE.cpp` 的 live 分析与 `replaceUsesWithCanon`。

对我说：

> `@OPT_SCORE_ROADMAP.md @OPT_ACTION.md 做到 Phase A.1`

即按本路线图执行，不跳步。
