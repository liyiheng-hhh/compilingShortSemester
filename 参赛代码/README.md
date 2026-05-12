# Compiler2026 RISC-V Baseline

这是 2026 编译系统设计赛（编译系统实现赛 · 初赛 · **RISC-V 赛道**）的 SysY2022 编译器基线工程，源码位于本目录，可提交到希冀平台 GitLab 供官方拉取评测。

## 评测与仓库约定（请务必核对）

- **拉取分支**：官方评测默认拉取 **`master`** 分支；若需指定其他分支，在提交面板的仓库 HTTPS 地址后**空格**再写分支名（见提交页帮助）。
- **可执行文件名**：生成的编译器可执行文件必须命名为 **`compiler`**（本仓库 `make` 产物即为此名）。
- **实现语言**：C++，需符合 **C++17**；项目中须有且仅有 **一个** `main`（本实现为单文件 `compiler.cpp`）。
- **评测编译命令**：官方使用 `clang++` 扫描 `.h/.hpp/.c/.cc/.cpp/...` 等后缀并链接为 `compiler`，请避免在同仓库混入多个带 `main` 的测试程序。
- **功能测试命令**：`compiler -S -o testcase.s testcase.sy`（路径在评测机上为绝对路径）。
- **性能测试命令**：`compiler -S -o testcase.s testcase.sy -O1`

生成汇编为 **64 位 RISC-V**，需能与官方 SysY 运行时一并汇编、链接并在指定 RISC-V Linux 环境运行；地址空间上需满足 **`-mcmodel=medany`**（或等价）的大地址模型约定。

## 构建

```sh
make
```

也可以直接使用评测环境的 C++17 编译器：

```sh
clang++ -std=c++17 -O2 compiler.cpp -o compiler
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

会在 `examples/` 下生成 `smoke.s`、`smoke_O1.s`（已被 `.gitignore` 忽略）。需在具备 RISC-V 工具链与 SysY 运行时的环境中再将 `.s` 汇编链接执行以做端到端验证。

## 实现范围说明

当前版本为**功能基线**：直接生成 RISC-V64 汇编，包含词法分析、递归下降语法分析、语义分析、作用域与常量折叠、多维数组与初始化、函数调用、`int`/`float` 转换、短路求值与 SysY 库调用等。

命令行会正确识别 **`-O1`**；语义阶段已包含基础常量折叠，后续可在保留该开关的前提下接入 **IR 与更多 `-O1` 优化**，并把选项传入代码生成或中端流水线。
