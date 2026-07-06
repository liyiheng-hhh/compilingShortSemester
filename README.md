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
.\build\Debug\toyc.exe < tests\smoke\return_zero.tc > out.s
```

在 Linux 或评测环境中，可执行文件路径可能为：

```bash
./build/toyc < tests/smoke/return_zero.tc > out.s
```

## 当前状态

当前提交是项目初始化骨架：已建立 C++20/CMake 工程、评测接口入口、冒烟测试样例和项目分工计划。后续需要按计划接入词法分析、语法分析、语义分析、IR、RISC-V32 代码生成、优化和实践报告。

## 目录规划

```text
.
├── CMakeLists.txt
├── README.md
├── docs/
│   └── 项目初始化与分工计划.md
├── src/
│   └── main.cpp
└── tests/
    └── smoke/
        └── return_zero.tc
```

