# Compiler2026-x — SysY RISC-V 编译器

2026 年全国大学生计算机系统能力大赛 · 编译系统设计赛 · **RISC-V 赛道**参赛工程。

本仓库在官方 baseline 上自研 **Dialect IR 中端 + 模式匹配优化 pass 管线**。优化实施纪律见 [`OPT_ACTION.md`](OPT_ACTION.md)，评测 bug 记录见 [`EVAL_BUGLOG.md`](EVAL_BUGLOG.md)。

**仓库**：[栈上开花队 / Compiler2026-X](https://gitlab.eduxiji.net/T2026104862010524/compiler2026-x)（希冀 GitLab）  
**开发分支**：`big`（提交时可指定 `master` 或 `big`，见 §3）

---

## 目录

1. [项目概述与原创性](#1-项目概述与原创性)
2. [项目结构](#2-项目结构)
3. [官方评测说明](#3-官方评测说明)
4. [合规性要求](#4-合规性要求)
5. [本地测评方法](#5-本地测评方法)
6. [中间代码优化](#6-中间代码优化o1-dialect-pipeline)
7. [现阶段问题](#7-现阶段问题)
8. [后续优化策略](#8-后续优化策略)
9. [构建与常用命令](#9-构建与常用命令)
10. [待你补充的信息](#10-待你补充的信息)

---

## 1. 项目概述与原创性

### 核心思路

| 维度 | 本仓库做法 | 与常见参考实现的差异 |
|------|-----------|---------------------|
| IR | 自研 **结构化 Dialect IR**（`ModuleOp` / `FuncOp` / `WhileOp`），flatten 后走 Mem2Reg + GVN | 非 LLVM IR 直搬；非纯 AST 直出汇编 |
| 优化 | **IR 结构模式匹配 pass**（RowScratchMatmul、GuardedAccum、LoopTiling 等），带显式 reject 条件 | 非函数名/输入硬编码；每个 pass 有 stats / env 开关 |
| Pipeline | `dialect_pipeline.cpp` 分阶段注册（pre-opt → flatten → mem → loop → late → rv） | 可 bisect 单 pass，见 `scripts/bisect-kernel-passes.sh` |
| 后端 | `mlir_rv/` 图着色 RegAlloc + `rv/` asm peephole | 性能 `-O1` 专用后端 |
| 工程 | `OPT_ACTION.md` 分 Phase；**asm A/B 为涨分判据** | 避免 asm 未变就声称提升 |

### 原创 pass（Dialect O1）

RowScratchMatmul · GuardedAccum · LoopTiling · MatTransposePair · Pureness — 均在 `src/opt/`，见 [`OPT_ACTION.md`](OPT_ACTION.md)。

---

## 2. 项目结构

### 为什么不是「双路径 O1」？

历史上存在两套 **O1** 实现（legacy 档 D AST/IR + 新 dialect），评测时 **只有 dialect 一条在用**：

| 官方命令 | 实际路径 | 说明 |
|---------|---------|------|
| `compiler -S -o out.s f.sy` | **Legacy O0** | 功能测 **不带 `-O1`**，走 `lexer→parser→codegen.cpp` 直出汇编 |
| `compiler -S -O1 -o out.s f.sy` | **Dialect O1** | 性能测，走 `dialect_parse→opt/*→mlir_rv` 全管线 |

**2026-05-30 已删除** 最近一次测评未使用的 legacy O1 代码：`loop_interchange/tiling/unroll`、`row_scratch_matmul`（AST 版）、`knapsack_dp`（AST 版）、`mm_hoist`、`land_lor_split`、`hir/`、`cfg/` 等。

**仍保留**：legacy **O0 最小路径**（官方功能测必需）+ `codegen.cpp` 内嵌的 `ir_*` 链接（O0 不触发 IR 后端，但尚未从 codegen 剥离）。

```
compiler2026-x/
├── src/
│   ├── main.cpp                 # O1→dialect；O0→legacy 最小路径
│   │
│   ├── dialect_parse/           # Dialect 前端
│   ├── codegen/                 # 结构化 IR（ModuleOp）+ CodeGen
│   ├── pre-opt/                 # Flatten 前 pass
│   ├── opt/                     # Flatten 后 pass（RowScratch/LoopTiling/…）
│   ├── mlir_rv/                 # Dialect→RISC-V
│   ├── rv_mlir_pipeline.cpp
│   ├── dialect_pipeline.cpp     # 中端 pipeline 工厂
│   │
│   ├── lexer*.cpp parser*.cpp   # Legacy O0 前端
│   ├── semantic*.cpp codegen.cpp # Legacy O0 汇编发射
│   ├── ir_*.cpp                 # codegen 内 IR 后端（仅链接，O0 不启用）
│   └── rv/                      # asm peephole（O1 dialect 后处理）
│
├── scripts/                     # eval / bisect / Docker
├── examples/                    # 冒烟 golden
├── local_eval_cases/
├── performance/                 # 性能用例（.gitignore）
├── runtime/sysy_runtime.c
├── OPT_ACTION.md
├── EVAL_BUGLOG.md
└── Makefile                     # SRCS 白名单
```

### 编译路径（当前）

```
O0（官方功能测）:
  lexer → parser → semantic → codegen.cpp（无优化）→ .s

O1（官方性能测）:
  dialect_parse → codegen/ → pre-opt → FlattenCFG → Mem2Reg
    → opt/* → mlir_rv → rv/ peephole → .s
```

`-O1` 若设 `SYSY_CC_NO_DIALECT_PIPELINE=1` 将 **直接报错**（不再 fallback 到 legacy O1）。

### 关于 Makefile vs 官方编译

- **本地**：`make` 只编译 `Makefile` 中 **`SRCS` 白名单**（当前 ~139 个 `.cpp`）。
- **官方希冀**：`clang++` **递归扫描**仓库内所有 `.cpp` 并链接。
- **务必**：不要提交未列入 `SRCS` 且含 `main`/重复符号的 `.cpp` 副本，否则 **CE（duplicate symbol）**。
- 2026-05-30 已清理 ~6000 行废弃代码（`hir/`、`cfg/`、`dialect_hir/`、orphan `ir_*`、未接入 pipeline 的 pre-opt），降低官方递归编译风险。

---

## 3. 官方评测说明

| 项 | 要求 |
|----|------|
| 拉取分支 | 默认 **`master`**；其它分支在仓库 URL 后空格指定（如 `…git big`） |
| 可执行文件 | 必须名为 **`compiler`** |
| 语言 | C++17；**唯一** `main`（`src/main.cpp`） |
| 功能测试 | `compiler -S -o testcase.s testcase.sy` |
| 性能测试 | `compiler -S -o testcase.s testcase.sy -O1` |
| 目标 | 64-bit RISC-V，`-mcmodel=medany`，链接官方 `libsysy.a` |
| 计时 | 官方使用 `starttime()` / `stoptime()`（`gettimeofday`） |

### 观察官方结果的方式

希冀平台提交后查看：**功能分 / 性能分 / CE / RE / WA / TLE**。本地无法完全复现官方 `libsysy.a` 与机器，以 **AC/WA + kernel 时间趋势** 为准，见下节。

---

## 4. 合规性要求

大赛技术委员会明确 **反对投机性优化**。本仓库策略：

| 允许 ✅ | 禁止 ❌ |
|--------|--------|
| 基于 IR 结构（phi、loop nest、load/store 索引）的通用变换 | 匹配函数名 / 特定字符串触发优化 |
| 循环交换、分块、guard 累加折叠等通用 loop transform | 对特定测试用例硬编码结果 |
| 利用 Pureness / Alias 分析保证语义 | 利用 UB 换性能 |
| env 开关 bisect（`SYSY_CC_NO_*`） | 依赖特定输入大小/模式的投机路径 |

当前 pass 均通过 **IR 形状 + reject 规则** 匹配，详见 [`OPT_ACTION.md`](OPT_ACTION.md) 与各 pass 内 `reject` 计数。

---

## 5. 本地测评方法

### 5.1 编译冒烟（无需 QEMU）

```sh
make && make check
```

`make check` 对 `examples/` 下多组 golden 做 **O0 + O1 编译 CE 检查**（不链接运行）。

### 5.2 功能回归（需 RISC-V 工具链 + libsysy.a）

```sh
make libsysy.a
export LIBSYSY=$PWD/libsysy.a

# 单目录批测
make sytest-o0 TESTDIR=examples/golden_smoke
make sytest-o1 TESTDIR=performance

# 本地小套 + O0/O1 对拍（判断 WA 是否只在优化路径出现）
make local-eval
```

### 5.3 性能测评（kernel 时间，推荐）

```sh
make && make libsysy.a

# 跑 performance 全集，CSV 输出
RUNTIME_PERF_TIMEOUT_SEC=120 make runtime-eval SUITE=performance OPT=O1

# 汇总：通过率、最慢题、WA 列表
make runtime-summary

# 改优化前后对比
RUNTIME_CSV=tests/.out/runtime/before.csv make runtime-eval SUITE=performance OPT=O1
# … 改代码 make …
RUNTIME_CSV=tests/.out/runtime/after.csv make runtime-eval SUITE=performance OPT=O1
./scripts/eval-vs-baseline.sh tests/.out/runtime/before.csv tests/.out/runtime/after.csv
```

**如何读 CSV**

| 列 | 含义 |
|----|------|
| `median_ms` | warmup 后 3 次运行的 **中位数**（主指标） |
| `pass` | AC/WA/TLE/CE |
| kernel 合计 | `runtime-summary` 对 `pass=AC` 的题求和，用于 pass bisect |

**Pass bisect**（哪个 pass 帮/害整体）：

```sh
./scripts/bisect-kernel-passes.sh   # 或见 OPT_ACTION.md §4 bisect 表
```

**Asm 是否真正变化**（涨分前置条件）：

```sh
./scripts/asm-ab-perf.sh performance/matmul2.sy
# 或
SYSY_CC_PASS_STATS=1 ./compiler -S -O1 -o /tmp/x.s performance/matmul2.sy 2>&1 \
  | grep -iE 'replaced|tiled|lifted|reject'
```

### 5.4 Docker（无本机工具链时）

```sh
make docker-init
make docker-local-eval          # local_eval_cases
make docker-test-functional     # 功能集 AC/WA
make docker-runtime-perf        # performance O1 + CSV
make docker-runtime-gate        # 功能硬门 + 性能软门
```

### 5.5 关键环境变量

| 变量 | 作用 |
|------|------|
| `SYSY_CC_NO_DIALECT_PIPELINE=1` | `-O1` 直接失败（已无 legacy O1 fallback） |
| `SYSY_CC_NO_ROW_SCRATCH_MATMUL=1` | 关 RowScratchMatmul |
| `SYSY_CC_NO_LOOP_TILING=1` | 关 LoopTiling |
| `SYSY_CC_NO_GUARDED_ACCUM=1` | 关 GuardedAccum |
| `SYSY_CC_PASS_STATS=1` | 打印 pass 统计 |
| `SYSY_O1_TIER=B/C/D` | legacy 路径 O1 档位（见 opt_config.h） |

默认 eval 脚本会 `source scripts/opt-passes-on.sh`（开启 GuardedAccum + MatTransposePair）。

---

## 6. 中间代码优化（O1 Dialect Pipeline）

> Pass 注册顺序见 [`src/dialect_pipeline.cpp`](src/dialect_pipeline.cpp)。  
> 涨分判据与 bisect 数据见 [`OPT_ACTION.md` §4](OPT_ACTION.md#4-已完成记录实施时更新)。

### 6.1 Pipeline 阶段

```
pre-opt（结构化 IR）→ FlattenCFG → Mem2Reg
  → 内存优化（Alias / DSE / DLE / GVN / Reassociate）
  → 循环优化（Canonicalize / Rotate / RowScratch / LoopTiling / LICM / …）
  → 杂项清理（SimplifyCFG / GCM / Select / …）
  → 晚期内联 + SynthConstArray
  → 3× 循环轮（LICM / SCEV / GuardedAccum / RemoveEmptyLoop）
  → 最终 DCE + SimplifyCFG + InstSchedule
  → mlir_rv（Lower / InstCombine / RegAlloc）→ rv/ asm peephole
```

各 pass 可通过 `SYSY_CC_NO_*` / `SYSY_CC_ENABLE_*` 单独 bisect（见 §5.5）。

---

### 6.2 优化效果分析

#### 效果较好的优化

| 优化 | 对应 Pass | 作用 | 本仓库观测 |
|------|-----------|------|-----------|
| **常量传播 / 常量折叠** | `early-const-fold`、`regular-fold` | 编译期将已知常量替换变量、折叠算术，减少运行时计算 | 全管线多次运行；`EarlyConstFold` 在 Pureness 前后各一轮 |
| **函数内联** | `early-inline`、`inline`、`late-inline` | 将 callee 体展开到 caller，消除 call/ret 与参数传递开销 | 默认 threshold=200；`BitStubFold` 在内联前折叠 32 轮 bitwise 模拟器，避免热点膨胀 |
| **LICM（循环不变代码外提）** | `licm` | 将不依赖归纳变量的计算/可证明安全的 store 提到循环外 | 主循环轮 + 3× loop-round 重复运行；对 matmul 内层地址计算有帮助 |
| **寄存器分配（图着色）** | `rv-regalloc` | 将 SSA 虚拟寄存器映射到物理寄存器，溢出时才写栈 | `mlir_rv/RegAlloc`；可选 `SYSY_CC_MLIR_RV_FAST_RA` |
| **GVN（全局值编号）** | `gvn` | 识别等价表达式并 CSE，复用已有计算结果 | 管线内 **≥8 轮**；配合 `Pureness` 标记 impure call，避免误 CSE `get_random()` |
| **Mem2Reg** | `mem2reg` | alloca/load/store 提升为 SSA 寄存器，减少内存 traffic | Flatten 后 **必经**；后续 GVN/LICM/DSE 均依赖 |
| **DCE（死代码删除）** | `dce`、`aggressive-dce`、`loop-dce` | 删除不可达 BB、无用 op、无用函数 | `loop-dce` 在结构化阶段删纯循环体；`aggressive-dce` 在循环轮末清扫 |
| **CFG 简化** | `simplify-cfg` | 合并空块、消除冗余分支/跳转 | 杂项阶段 + 最终清理各一轮 |
| **循环展开** | `const-loop-unroll` | 对 trip count 为常数的循环完全展开 | 默认开启；适合小循环、DP 表初始化 |
| **尾调用优化** | `tco` | 尾递归改循环，消除栈帧增长 | pre-opt 阶段，对递归题有效 |
| **死存储/加载消除** | `dse`、`dle` | 删除冗余 store 与重复 load | 多轮运行；2026-05-30 修复 DSE 重复 erase 导致 segfault |
| **表达式重结合** | `reassociate` | 调整加法链形状，便于 GVN CSE | 在 experimental 脚本中；默认 eval **关闭**（shuffle/h-5 会退化） |
| **强度削弱** | `strength-reduction`（RV + asm） | 乘除/比较改 cheaper 指令 | RV 中端 + `rv/` asm peephole 双层 |

#### 效果一般的优化

| 优化 | 对应 Pass | 说明 |
|------|-----------|------|
| **InstCombine** | `rv-inst-combine` + `rv/` `instCombine` | RV 层指令级 peephole（如 `addi 0` 消除）；单条收益小，靠全管线累积 |
| **过程间分析（非完整 IPO-DCE）** | `call-graph` + `pureness` + `dce` | 标记 impure 函数、删未被调用的 dead function；**无**跨函数常量传播/全程序 DDE |
| **GCM（全局代码移动）** | `gcm` | 将指令调度到更早/更晚的合法位置；本仓库 RV 调度较简，收益有限 |
| **DAE（死参数消除）** | `dae` | 删未使用形参；SysY 小题调用模式下触发少 |
| **SCEV 归纳变量替换** | `scev` | 多项式 IV 替换；对简单 for 有效，复杂 nest 常 no-op |
| **Select 化** | `select` | if 改 cmov 风格；RISC-V 无原生 select，后端需再展开 |
| **指令调度** | `inst-schedule`、`rv-schedule` | 列表调度藏 load/mul 延迟；默认 schedule 关闭，需 `SYSY_CC_ENABLE_RV_SCHEDULE` |
| **MatTransposePair** | `mat-transpose-pair` | 识别 `b[i][j]=a[j][i]` swap；performance 集上常 `rewrites=0`（pass 前 IR 已变形） |
| **InlineStore** | `inline-store` | 默认 **关闭**；小 store 内联实验性 |
| **SynthConstArray** | `synth-const-array` | 合成常量数组初始化；触发面窄 |

#### 针对性原创优化（效果因题而异）

| Pass | 针对场景 | 平台/本地观测 |
|------|---------|--------------|
| **RowScratchMatmul** | 矩阵乘 `C[i][j]+=A[i][k]*B[k][j]` → 行缓冲 + helper | 平台 matmul1/2/3 **+6.36**；many_mat_cal asm DIFF；`guarded-k` 仍拒条件累加 |
| **LoopTiling** | 多层循环 strip-mine / 嵌套分块 | many_mat_cal/sl 受益；matmul2 **热点 k 环**默认不 tile（`ltInnerIsSimpleReduction` 过滤含 Mod 内层） |
| **GuardedAccum** | `if (cond) acc += …` 分支less 化 | matmul2/sl/shuffle2 有 lift；**回归** crypto-1、03_sort1、01_mm2 |
| **BitStubFold** | 内联前折叠 bitwise 模拟循环 | 防 crypto 类题 IR 膨胀 |
| **ColumnMajor / Fusion** | 列主序识别、相邻循环融合 | experimental；Fusion 对 shuffle 类题有害 |
| **KnapsackDp**（前端） | DP 表滚动优化 | 仅 dialect 前端 AST 改写，非 IR pass |

**2026-05-30 本地 kernel bisect**（相对 all-on 关 pass 更慢 → 净收益为正，但仍 ×2 于 abbf8a4 baseline）：

| 关 pass | kernel 合计 | 净收益 |
|---------|------------:|-------:|
| ALL_ON | 7933 ms | — |
| NO_GUARDED_ACCUM | 11068 ms | +3135 ms |
| NO_ROW_SCRATCH | 9136 ms | +1203 ms |
| NO_LOOP_TILING | 9867 ms | +1934 ms |

---

### 6.3 已删除 / 移出管线的 Pass

| 项 | 原因 |
|----|------|
| **Legacy AST O1 全套**（`loop_interchange/tiling/unroll`、`row_scratch_matmul` AST 版、`knapsack_dp` AST 版、`mm_hoist`、`land_lor_split`） | 官方 `-O1` 已不走 legacy；2026-05-30 删除 |
| **`hir/`、`cfg/`、`dialect_hir/`** | 废弃 HIR→CFG 双轨 lowering |
| **`pre-opt/Parallelize`、`Unswitch`、`Unroll`** | 未接入 dialect pipeline，源码已删 |
| **`ir_cleanup`、`ir_dom`、`ir_reassociate`** | orphan 文件，未进 Makefile |
| **`dialect_fallback`** | O1 不再 fallback 到 legacy |

---

### 6.4 存在问题 / 待修

| Pass / 项 | 问题 | 状态 |
|-----------|------|------|
| **RemoveEmptyLoop**（空循环消除） | CFG 层删除「无副作用」循环；对跨块 use、隐式副作用判断保守不足，存在误删风险 | **仍在 pipeline**（loop 轮末 2×）；建议 bisect 验证后考虑移除或加严条件 |
| **LoopDCE**（结构化无用循环消除） | 删 step=1 的纯循环体；与 `RemoveEmptyLoop` 职责重叠 | pre-opt 保留；与上项不同 IR 层 |
| **DSE** | 同一 store 重复入 remove 集 → 双 erase UAF | **已修复**（`unordered_set`） |
| **LoopTiling NESTED** | acc phi 跨 tile 未修全时 GVN crash | 默认关 `NESTED`；matmul2 k 分块仍 blocked |
| **GuardedAccum** | 误匹配 matmul-step / conv2d | 已加 reject；crypto/sort 仍回归 |
| **全量 kernel 合计** | all-on 7933 ms vs baseline 3877 ms（×2.05） | 需按题型选择性开 pass，非全开最优 |

---

### 6.5 基础设施改动（工程向）

- **Dialect pipeline** 分阶段 pass 注册 + env bisect（`scripts/bisect-kernel-passes.sh`）
- **Pureness + CallGraph**：impure 标记，支撑 GVN/DCE 安全 CSE
- **2026-05-30 代码清理**：删 ~6000 行废弃路径；`make check` / 官方路径可用

### 6.6 平台观测（2026-05-30，希冀）

- 总分 **+6.50**；matmul1/2/3 **+6.36**；crypto 不变；many_mat_cal-1 −0.08

---

## 7. 现阶段问题

| 问题 | 现状 | 影响 |
|------|------|------|
| **matmul2 热点未优化** | RSM `guarded-k` reject；GA `matmul-step` reject；LoopTiling 默认跳过含 Mod 内层 | 平台 matmul 仍落后榜首 ~3× |
| **Pass 回归** | GuardedAccum/LoopTiling 害 crypto-1、shuffle1、03_sort1 | 全量 kernel 合计 ×2 vs baseline |
| **MatTransposePair** | performance 集常 `rewrites=0`（IR 在 pass 前已变形） | transpose 题无明显 asm diff |
| **双路径维护成本** | legacy 档 D + dialect 两套优化 | 改 pass 需明确路径 |
| **本地 ≠ 官方** | libsysy / 机器 / 超时不同 | 以 AC + 趋势为准，不以绝对 ms 为准 |

---

## 8. 后续优化策略

按 [`OPT_ACTION.md`](OPT_ACTION.md) Phase 顺序，**先 asm A/B 再 QEMU**：

| Phase | 目标 | 关键动作 |
|-------|------|---------|
| **1 RowScratch** | many_mat_cal 全覆盖 | 逐条放宽 reject（一次一条） |
| **2 LoopTiling** | matmul2 k 环 tile | acc-aware NESTED + phi 对齐；或补 LoopInterchange |
| **3 GuardedAccum** | matmul2 branchless | 放宽 `matmul-step`，保留语义 |
| **4 Gate** | 60/60 + kernel ≤ baseline×1.05 | case 级 pass 禁用 / 修回归 |

**不做的事**（除非明确例外）：整 pipeline 重排、为涨分放宽 merge 条件、依赖 UB、匹配函数名。

---

## 9. 构建与常用命令

```sh
make                  # 生成 ./compiler
make clean && make -j4
make check            # 编译冒烟
make libsysy.a        # 本地运行时静态库

# 官方等价用法
./compiler -S -o out.s foo.sy       # O0
./compiler -S -O1 -o out.s foo.sy   # O1（dialect 主路径）
```

### 相关文档

- [`OPT_ACTION.md`](OPT_ACTION.md) — 优化 Phase 步骤与验收命令
- [`EVAL_BUGLOG.md`](EVAL_BUGLOG.md) — 历史 WA/CE 与 fix 记录
- [`scripts/opt-passes-on.sh`](scripts/opt-passes-on.sh) — 默认开启的 pass

---

## 10. 待补充的信息

1. **最近几次希冀提交的分数**（功能/性能/分题耗时 CSV 或截图）
2. **`performance/` 用例** 实际存放路径（仓库内 / Docker 挂载）
3. 若计划 **O0 也切 dialect**（可彻底删 `lexer/` legacy 与 `ir_*`），需单独立项

---

*最后更新：2026-05-30 — 删除 legacy O1 AST pass；O1 仅 dialect；O0 保留 legacy 最小路径。*
