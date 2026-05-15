# Compiler2026 RISC-V Baseline

这是 2026 编译系统设计赛（编译系统实现赛 · 初赛 · **RISC-V 赛道**）的 SysY2022 编译器基线工程，源码位于本目录，可提交到希冀平台 GitLab 供官方拉取评测。

## 评测与仓库约定（请务必核对）

- **拉取分支**：官方评测默认拉取 **`master`** 分支；若需指定其他分支，在提交面板的仓库 HTTPS 地址后**空格**再写分支名（见提交页帮助）。
- **可执行文件名**：生成的编译器可执行文件必须命名为 **`compiler`**（本仓库 `make` 产物即为此名）。
- **实现语言**：C++，需符合 **C++17**；项目中须有且仅有 **一个** `main`（本实现入口为 **`main.cpp`**）。
- **评测编译命令**：官方使用 `clang++` **递归扫描**仓库内 `.h/.hpp/.c/.cc/.cpp/...` 并链接为 `compiler`。
  - **切勿**在仓库中提交 **第二套** 与根目录重复的 `.cpp` 副本（例如历史遗留的 `.docker-compiler-build/` 镜像内拷贝），否则会出现 **`_main` / duplicate symbol** 等链接错误，评测直接 **CE**。
  - 推荐仅通过本仓库根目录 **`Makefile`** 所列 **`SRCS`** 参与构建；Docker 内编译请使用 **`BUILD_DIR=/tmp/compiler-build`**（或任意**未跟踪**目录），勿把构建副本提交进 Git。
- **功能测试命令**：`compiler -S -o testcase.s testcase.sy`（路径在评测机上为绝对路径）。
- **性能测试命令**：`compiler -S -o testcase.s testcase.sy -O1`

生成汇编为 **64 位 RISC-V**，需能与官方 SysY 运行时一并汇编、链接并在指定 RISC-V Linux 环境运行；地址空间上需满足 **`-mcmodel=medany`**（或等价）的大地址模型约定。

## 构建

```sh
make
```

也可以直接使用评测环境的 C++17 编译器（与 `Makefile` 中 **`SRCS`** 一致的多文件编译）：

```sh
clang++ -std=c++17 -O2 -c main.cpp -o main.o
clang++ -std=c++17 -O2 -c common.cpp -o common.o
clang++ -std=c++17 -O2 -c lexer.cpp -o lexer.o
clang++ -std=c++17 -O2 -c parser.cpp -o parser.o
clang++ -std=c++17 -O2 -c semantic.cpp -o semantic.o
clang++ -std=c++17 -O2 -c codegen.cpp -o codegen.o
clang++ -std=c++17 -O2 -c ir_build.cpp -o ir_build.o
clang++ -std=c++17 -O2 -c ir_opt.cpp -o ir_opt.o
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
```

## 源码布局（模块化）

| 文件 | 职责 |
|------|------|
| `main.cpp` | 命令行、`compileFile` 流水线入口 |
| `common.*` | `CompileError`、文件读写、`product`、`floatBits`、汇编字符串转义 |
| `token.h` | 词法记号 |
| `ast.h` | AST / 符号 / 类型 |
| `lexer.*` | 词法分析 |
| `parser.*` | 递归下降语法分析 |
| `semantic.*` | 语义分析与常量折叠 |
| `codegen.*` | RISC-V64 汇编生成（含 `-O1` 下 IR 后端发射） |
| `ir.h` | 中端 IR（含 `Label`/`J`/`Beqz` 控制流、`IRBlock` 视野与 CFG 刷新） |
| `ir_build.cpp` | 将满足 `irFunctionEligible` 的函数体降为 IR，并刷新基本块划分 |
| `ir_opt.cpp` | 常量折叠、CSE、Copy 合并、不变量外提、块级活跃信息辅助的 slot 等 |

从旧版 **单文件** 重建模块时：先从 Git 历史恢复当时的 `compiler.cpp`，再运行 `python3 tools/split_compiler.py`（脚本依赖该文件作为输入）。

## 实现范围与后续优化方向（概要）

**当前能力**：直接生成 RISC-V64；`-O1` 下对满足 `irFunctionEligible`（无局部数组、`&&`/`||` 等约束）的函数优先走 **IR → 优化 → 槽位分配 → 发射**；**允许含函数调用的函数**进入 IR（与寄存器/活跃区间协同演进中）；仍为 **扁平 `insts` 发射**，`IRBlock` 为分析与作用域辅助。

**与性能测试强相关的后续路线**（按投入/收益权衡推进）：

1. **IR 与控制流**：由单块 IR 扩展为带 **Label / 条件分支 / 循环** 的非 SSA CFG，再上做跨块的 **常量传播、DCE、CSE** 与 **load/store forwarding**。
2. **循环专项**：不变量外提、归纳变量与 **数组基址 + 指针步进**（减少每轮 `i * stride`）。
3. **寄存器分配**：在基本块或全局上做活跃变量分析，将常用 vreg 固定在 **t/a/ft**，跨调用再 spill。
4. **除法/取模**：对 32 位有符号除常数实现 **magic multiply**（处理负除数、`INT_MIN`、`-1` 等边界），与现有 2 的幂路径互补。
5. **内联**：在无副作用前提下放宽小函数内联（参数个数、多语句、float 返回等）。
6. **语义兜底**：调用实参个数/类型、const 左值赋值等在进入激进优化前严格报错。

命令行会正确识别 **`-O1`**；语义阶段含基础常量折叠，**`-O1`** 将选项传入 `CodeGen` 与 IR 流水线。
