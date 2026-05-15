# Compiler2026 RISC-V Baseline

这是 2026 编译系统设计赛（编译系统实现赛 · 初赛 · **RISC-V 赛道**）的 SysY2022 编译器基线工程，源码位于本目录，可提交到希冀平台 GitLab 供官方拉取评测。

## 评测与仓库约定（请务必核对）

- **拉取分支**：官方评测默认拉取 **`master`** 分支；若需指定其他分支，在提交面板的仓库 HTTPS 地址后**空格**再写分支名（见提交页帮助）。
- **可执行文件名**：生成的编译器可执行文件必须命名为 **`compiler`**（本仓库 `make` 产物即为此名）。
- **实现语言**：C++，需符合 **C++17**；项目中须有且仅有 **一个** `main`（本实现入口为 **`src/main.cpp`**）。
- **评测编译命令**：官方使用 `clang++` **递归扫描**仓库内 `.h/.hpp/.c/.cc/.cpp/...` 并链接为 `compiler`。
  - **切勿**在仓库中提交 **第二套** 与 `src/` 重复的 `.cpp` 副本（例如历史遗留的 `.docker-compiler-build/` 镜像内拷贝），否则会出现 **`_main` / duplicate symbol** 等链接错误，评测直接 **CE**。
  - 推荐仅通过本仓库根目录 **`Makefile`** 所列 **`SRCS`** 参与构建；Docker 内编译请使用 **`BUILD_DIR=/tmp/compiler-build`**（或任意**未跟踪**目录），勿把构建副本提交进 Git。
- **功能测试命令**：`compiler -S -o testcase.s testcase.sy`（路径在评测机上为绝对路径）。
- **性能测试命令**：`compiler -S -o testcase.s testcase.sy -O1`

生成汇编为 **64 位 RISC-V**，需能与官方 SysY 运行时一并汇编、链接并在指定 RISC-V Linux 环境运行；地址空间上需满足 **`-mcmodel=medany`**（或等价）的大地址模型约定。

## 构建

```sh
make
```

也可以直接使用评测环境的 C++17 编译器（与 `Makefile` 中 **`SRCS`** 一致的多文件编译；源文件在 **`src/`**）：

```sh
clang++ -std=c++17 -O2 -c src/main.cpp -o main.o
clang++ -std=c++17 -O2 -c src/common.cpp -o common.o
clang++ -std=c++17 -O2 -c src/lexer.cpp -o lexer.o
clang++ -std=c++17 -O2 -c src/parser.cpp -o parser.o
clang++ -std=c++17 -O2 -c src/semantic.cpp -o semantic.o
clang++ -std=c++17 -O2 -c src/codegen.cpp -o codegen.o
clang++ -std=c++17 -O2 -c src/ir_build.cpp -o ir_build.o
clang++ -std=c++17 -O2 -c src/ir_opt.cpp -o ir_opt.o
clang++ -std=c++17 -O2 -o compiler main.o common.o lexer.o parser.o semantic.o codegen.o ir_build.o ir_opt.o
```

## 使用

```sh
./compiler -S -o testcase.s testcase.sy
./compiler -S -o testcase.s testcase.sy -O1
```

## 本地冒烟（可选）

```sh
make check
```

会在 `examples/` 下生成 `smoke.s`、`smoke_O1.s`（已被 `.gitignore` 忽略），并 **`-O1` 编译** `examples/golden_magic_div/boundary.sy` 做除常数/取模边界 CE 检查。需在具备 RISC-V 工具链与 SysY 运行时的环境中再将 `.s` 汇编链接执行以做端到端验证。

## 批量编译 / 回归 / 体积对比

| 目标 | 说明 |
|------|------|
| `make compile-all` | 递归编译 `SY_DIRS`（默认 `examples`）下全部 `*.sy` → 同名 `.s`，**不需要** qemu |
| `make compile-all-o1` | 同上且加 **`-O1`** |
| `make sytest` | 需 `LIBSYSY`、`TESTDIR`；沿用 `scripts/run_sy_tests.sh` |
| `make sytest-o0` / `make sytest-o1` | 固定 `USE_O1=0/1` 的 qemu 比对 |
| `make size-report` | 对每个用例输出 O0/O1 汇编行数及差值（默认扫描 `SY_DIRS`） |

**Docker 常驻容器**（`scripts/docker-test-container.sh`）：先 `make docker-init`，再对容器内路径跑批测（仓库根挂载为 **`/work`**）。本地若把官方树放在仓库根下，默认路径为下表 `DOCKER_*`；否则自行改 `DOCKER_FUNC` / `DOCKER_PERF` 或直接用 `test` 子命令传目录。

| 目标 | 说明 |
|------|------|
| `make docker-init` | 创建/启动容器并安装 RISC-V 工具链与 qemu（同 `e2e-docker.sh`） |
| `make docker-test-functional` | `SY_TEST_DIR=$(DOCKER_FUNC)`，`USE_O1` 默认 0，可覆盖 |
| `make docker-test-performance` | `SY_TEST_DIR=$(DOCKER_PERF)`，`USE_O1` 默认 1，可覆盖 |
| `make docker-test-dirs` | 需设 **`SY_TEST_DIRS="dir1 dir2..."`**（容器内路径），走 `e2e-docker.sh` 多目录逻辑 |

日常基线示例：

```sh
make check
USE_O1=1 ./scripts/docker-test-container.sh test /work/performance
./scripts/docker-test-container.sh test "/work/2026初赛RISCV赛道功能用例/functional"
make docker-test-functional
```

## `libsysy.a`：从哪里来、什么「版本」、怎么「下载」

逻辑集中在 **`scripts/e2e-docker.sh`**（Docker 批测由 **`scripts/docker-test-container.sh`** 调用）。本仓库**不会**从网盘或赛方直接下载现成的 **`libsysy.a`**，也**没有** `libsysy.so` / `-shared` 动态库流程。

### 1. 存放与构建位置（容器内）

| 项 | 值 |
|----|-----|
| 工作目录 | `RT=/tmp/sysy-runtime-lib` |
| 产物 | `$RT/build/libsysy.a`（如 `/tmp/sysy-runtime-lib/build/libsysy.a`） |
| 交给批测 | `export LIBSYSY="$RT/build/libsysy.a"` → `scripts/run_sy_tests.sh` |

每次完整跑 `e2e-docker.sh`（非 `INSTALL_ONLY=1`）会先 **`rm -rf "$RT"`** 再重编，因此 **libsysy.a 是当次构建产物**，不是长期缓存的官方二进制。

### 2. 运行时 C 源：两条路径（二选一）

**路径 A（当前仓库默认）** — 若挂载进容器存在：

`/work/runtime/sysy_runtime.c`

则用本地 C 源交叉编译为 `sysy.o`，**不 clone**。「版本」即 **Git 里这份 `runtime/sysy_runtime.c` 的内容**；是否与本届希冀官方运行时逐字节一致，需你们自行对照大赛资料或官方库维护。

**路径 B（仅当本地没有 `sysy_runtime.c` 时）** — 从网络拉**源码仓库**（不是下载 `.a`）：

`git clone --depth 1 https://github.com/pku-minic/sysy-runtime-lib.git`

编译 `$RT/src/src/sysy.c`。脚本**未固定** branch/tag/commit，`--depth 1` 表示默认分支**当时最新**提交，**不可复现**到固定版本（若要固定，需自行改脚本加 `-b` 或 `checkout`）。此路径下 `apt install` 会额外装 **`ca-certificates`**、**`git`**。

### 3. 从 `.o` 到 `libsysy.a`

`riscv64-linux-gnu-gcc -c` → `riscv64-linux-gnu-ar rc` → `riscv64-linux-gnu-ranlib`。工具链来自 Ubuntu 包 **`gcc-riscv64-linux-gnu`**、**`binutils-riscv64-linux-gnu`** 等（同脚本 `apt-get install`）。

### 4. Docker 如何串起来

| 阶段 | 行为 |
|------|------|
| `make docker-init` | `docker-test-container.sh` → `e2e-docker.sh` 且 **`INSTALL_ONLY=1`**：只做 apt/工具链，**不**编 libsysy |
| `make docker-test-functional` 等 | 再次跑 `e2e-docker.sh`（通常 **`SKIP_APT=1`**，缺工具链会自动补 apt），**重建** libsysy.a 并批测 |

### 5. 与「官方 libsysy.a」、本机 `make sytest` 的关系

- **希冀评测**：组委会环境使用**官方提供的静态运行时**（习惯称 `libsysy.a`）；与本文 Docker 里**现场 ar 出来的**库在实现细节上可能略有差别，本地对拍以 **`.out` 一致** 为准。
- **本机不用 Docker**：`make sytest` / `scripts/run_sy_tests.sh` 要求你自行设置 **`LIBSYSY=/path/to/libsysy.a`**（可来自大赛资料包、或自己用官方/ minic 源码交叉编译），脚本**不会**替你下载。
- **链接模型**：默认 **`LINK_FLAGS=-static -mcmodel=medany`**，与赛方 RISC-V 大地址模型说明对齐。

## 仓库目录（概要）

| 路径 | 内容 |
|------|------|
| `src/` | 编译器 C++ 源码（唯一 `main` 在 `src/main.cpp`） |
| `scripts/` | 批测、Docker 内 `e2e-docker`、`run_sy_tests` 等 |
| `runtime/` | Docker 内用于生成 `libsysy.a` 的 `sysy_runtime.c` |
| `examples/` | 冒烟与小型 golden |
| `tools/` | 辅助脚本（如 `split_compiler.py`） |
| `performance/` | 性能用例目录（默认 `.gitignore`；放仓库根便于 `DOCKER_PERF`） |

## 源码布局（模块化）

| 文件 | 职责 |
|------|------|
| `src/main.cpp` | 命令行、`compileFile` 流水线入口 |
| `src/common.*` | `CompileError`、文件读写、`product`、`floatBits`、汇编字符串转义 |
| `src/token.h` | 词法记号 |
| `src/ast.h` | AST / 符号 / 类型 |
| `src/lexer.*` | 词法分析 |
| `src/parser.*` | 递归下降语法分析 |
| `src/semantic.*` | 语义分析与常量折叠 |
| `src/codegen.*` | RISC-V64 汇编生成（含 `-O1` 下 IR 后端发射） |
| `src/ir.h` | 中端 IR（含 `Label`/`J`/`Beqz` 控制流、`IRBlock` 视野与 CFG 刷新） |
| `src/ir_build.cpp` | 将满足 `irFunctionEligible` 的函数体降为 IR，并刷新基本块划分 |
| `src/ir_opt.cpp` | 常量折叠、CSE、Copy 合并、不变量外提、块级活跃信息辅助的 slot 等 |

从旧版 **单文件** 重建模块时：先从 Git 历史恢复当时的 `compiler.cpp` 于仓库根，再运行 `python3 tools/split_compiler.py`（输出写入 **`src/`**）。

## 实现范围与后续优化方向（概要）

**当前能力**：直接生成 RISC-V64；`-O1` 下对满足 `irFunctionEligible`（无局部数组、`&&`/`||` 等约束）的函数优先走 **IR → 优化 → 槽位分配 → 发射**；**允许含函数调用的函数**进入 IR（与寄存器/活跃区间协同演进中）；仍为 **扁平 `insts` 发射**，`IRBlock` 为分析与作用域辅助。

**与性能测试强相关的后续路线**（按投入/收益权衡推进）：

1. **IR 与控制流**：由单块 IR 扩展为带 **Label / 条件分支 / 循环** 的非 SSA CFG，再上做跨块的 **常量传播、DCE、CSE** 与 **load/store forwarding**。
2. **循环专项**：不变量外提、归纳变量与 **数组基址 + 指针步进**（减少每轮 `i * stride`）。
3. **寄存器分配**：在基本块或全局上做活跃变量分析，将常用 vreg 固定在 **t/a/ft**，跨调用再 spill。
4. **除法/取模**：对 32 位有符号除常数实现 **magic multiply**（处理负除数、`INT_MIN`、`-1` 等边界），与现有 2 的幂路径互补。
5. **内联**：在无副作用前提下放宽小函数内联（参数个数、多语句、float 返回等）。
6. **语义兜底**：调用实参个数/类型、const 左值赋值等在进入激进优化前严格报错。

### 性能榜单对照：慢用例 → 瓶颈假设 → 推荐落地顺序

下面与你贴的榜单（`conv2d-1` ~244s、`many_mat_cal-*` ~150s、`transpose2` ~119s、`matmul2` ~59s 等）对齐，便于按**投入产出**排期（均需 **`compiler … -O1`** + 对拍正确后再看计时）。

| 热点类型 | 代表用例 | 典型瓶颈 | 优先策略 |
|----------|----------|----------|----------|
| 多维矩阵 / 卷积 | `conv2d-*`, `matmul*`, `01_mm*`, `many_mat_cal*` | 内层 `idx`/下标乘法重复、`%`/除法在循环内、访存顺序差 | **跨循环 LICM**（把 `r*N+c`、`base` 地址不变量提出）；**归纳变量**（指针步进替代每轮 `i*stride`）；可选 **分块/交换循环**（需正确性证明） |
| 转置 / 访存模式 | `transpose*`, `matmul2` 中 `b[i][j]=a[j][i]` | cache miss、行优先 vs 列优先 | **循环交换 + 分块**（tiling）；编译器侧可做 **末维连续访问优先** 的启发式 |
| 背包 / DP | `knapsack_naive*` | 内层循环访存 + 分支 | **边界分支外提**、减少 redundant load、IR **全局数组 load 外提**（已有雏形，可扩展到 `LoadMem` 的不变基址） |
| Huffman / 排序 / 复杂控制流 | `huffman*`, `03_sort*` | 分支预测、指针追逐 | **内联**小函数、**块布局**（hot edge fall-through）；大树堆操作可考虑 **迭代化**（源码级） |
| CRC / 位运算 | `crc*` | 内层查表或逐位 | **查表不变量外提**、** strength reduce**（对固定宽度的移位/掩码） |
| FFT / 浮点 | `fft*` | `float` 路径、调用 | 保证 **float 寄存器**尽量驻留；减少 `fcvt`；小函数 **内联** |

### 分阶段实现步骤（建议在仓库内的落点）

**阶段 A — 低风险、见效快（1～3 天量级）**

1. **IR：固定点多轮块优化（已实现）**：`irOptimizeBlock` 外层按 **`irInstructionFingerprint`** 迭代（最多 16 轮）；每轮仍含 hoist、单遍扫描 + CSE/常量折叠、DCE（实现于 `irOptimizeBlockOneRound`）。**`codegen.cpp` 中只对 IR 函数调用一次 `irOptimizeBlock`**。
2. **窥孔 / 强度削弱（部分落地）**：IR 内需已含 **2 的幂乘法→`Sll`**、**/±1、`0 / c`、`0 % c`** 常量折叠、`Rem`/`Div`/`Mul`/`Add`/`Sub`/`Neg`/`F*` 等对 `codegen.cpp`/`emitIr*` 的补充请继续按热点加；汇编侧 **magic 有符号除常数** 仍在发射阶段。
3. **Profiling（已实现脚本与 Make 入口）**：`make perf-profile PERF_SY=performance/matmul1.sy`（可选 `LIBSYSY=…` 做 qemu 限时）；批量对比仍用 **`make size-report SY_DIRS=performance`**；希冀以官方 `starttime/stoptime` 为准。

**阶段 B — 中端（1～2 周）**

4. **放宽 `irFunctionEligible`**：允许**小**局部数组或带 `LeaLocal` 的栈槽，使含 `temp`/`buf` 的核心循环仍走 IR（`ir_build.cpp` + `ir.h` 扩展）。
5. **跨基本块 LICM**：在已有 `irRefreshCFG` / 块划分之上，对「单入口循环」把不变 `LoadGlobal` / `LeaGlobal+offset` / 纯算术提到循环头（扩展 `ir_opt.cpp` 里 hoist 逻辑）。
6. **简单 mem2reg 或 SSA 构造**：减少跨迭代的虚假依赖，为后续 CSE 服务。

**阶段 C — 进攻榜单顶部（持续）**

7. **仿射访问分析**：识别 `a[i][k]`、`b[k][j]` 的仿射关系，做 **tile** 或 **unroll**（需上限防止代码爆炸）。
8. **协作式优化**：与赛题允许的 **手写 intrinsic** 无关时，仅靠语言子集则依赖上述通用循环与访存优化。

命令行会正确识别 **`-O1`**；语义阶段含基础常量折叠，**`-O1`** 将选项传入 `CodeGen` 与 IR 流水线。
