# ToyC Compiler

本仓库用于短学期编译实践项目：实现 ToyC 语言到 RISC-V32 汇编的编译器。

## 技术选型

- 实现语言：C++20
- 构建系统：CMake
- 输入输出接口：从 `stdin` 读取 ToyC 源码，向 `stdout` 输出 RISC-V32 汇编
- 优化开关：接受可选参数 `-opt`

## 构建

```powershell
cmake -S . -B build
cmake --build build
```

## 运行

```powershell
.\build\toyc.exe < tests\smoke\return_zero.tc > out.s
```

在 Linux 或评测环境中，可执行文件路径可能为：

```bash
./build/toyc < tests/smoke/return_zero.tc > out.s
```

## 当前状态

`lyh` 分支已实现一个完整的 ToyC 到 RISC-V32 汇编编译器，覆盖：

- 手写词法分析和递归下降语法分析。
- AST、作用域、局部/全局变量、局部/全局常量和常量表达式求值。
- 表达式、赋值、声明、`if/else`、`while`、`break`、`continue`、`return`。
- int/void 函数、递归、多参数调用和全局变量读写。
- RISC-V32 汇编输出，包含 `.data`、`.text`、函数栈帧、短路逻辑和分支标签。
- `-opt` 下的常量表达式折叠。

本地已通过 CMake 构建，并对 `tests/` 下样例完成普通模式和 `-opt` 模式的汇编生成验证。

## 开发文档

- [项目初始化与分工计划](docs/项目初始化与分工计划.md)
- [完整实现方案](docs/完整实现方案.md)

## 目录规划

```text
.
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── 项目初始化与分工计划.md
│   └── 完整实现方案.md
├── src/
│   └── main.cpp
└── tests/
    ├── README.md
    ├── decl/
    ├── expr/
    ├── func/
    ├── global/
    ├── smoke/
    └── stmt/
```
