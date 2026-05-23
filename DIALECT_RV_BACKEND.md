# 方言 RV 后端（ModuleOp / Pass / Dump）

**决策**：在保留本仓库 `lexer` / `parser` / `semantic` / `CodeGen` 入口的前提下，采用 **ModuleOp → rv::Lower → Schedule → RegAlloc → Dump** 管线；废弃 `src/rv_ir/` 的 `IRInst → RvBuilder` 实验路径。

## 架构

| 旧路径 | 当前路径 |
|--------|----------|
| `IRInst` + 字符串发射 | `ir_to_module` → `mlir_rv` pass → `Dump` |
| `src/rv/` 文本 peephole | 保留（`-O1` 默认可开，与方言后端独立） |

## 目录

```
src/dialect_parse/   参考前端（sys::Parser + Sema）
src/dialect_hir/     DHIR（dhir::Builder）
src/cfg/             HIRToCFG → CFGToLegacy → ModuleOp
src/dialect_pipeline.*   切片入口 + 最小 PassManager
src/codegen/         OpBase, Ops, Attrs, Builder
src/opt/             PassManager, Mem2Reg, RegularFold, DCE, SimplifyCFG, …
src/mlir_rv/         Lower, Schedule, RegAlloc, Dump, InstCombine, …
src/ir_to_module.*   IRFunction → ModuleOp 桥（legacy O1 仍可用）
src/rv_mlir_pipeline.*   mlir_rv 管线入口
src/rv/              汇编文本后处理
```

## 管线

**切片 1（方言前端，与 legacy 并行）**

```
SysY → dialect_parse → dhir → cfg::lowerFromHIR → cfg::lowerToLegacyIR
     → PassManager(Mem2Reg, RegularFold, DCE, SimplifyCFG, DCE)
     → mlir_rv(Lower … Dump)
```

**桥接（Codegen / ir_to_module）**

```
irToModuleOp → rv::Lower → rv::InstCombine? → rv::RvDCE → GVN → rv::Schedule? → rv::RegAlloc → rv::Dump
```

## 环境变量（对外仅 `SYSY_*`）

| 变量 | 默认 | 含义 |
|------|------|------|
| `SYSY_CC_ENABLE_DIALECT_PIPELINE` | **默认开** | 设为 `0` 关闭；设为 `1` 显式开启（与默认等价） |
| `SYSY_CC_NO_DIALECT_PIPELINE=1` | — | 强制关闭方言切片（跑 legacy O1 对照用） |
| `SYSY_CC_ENABLE_MLIR_RV=1` | 0 | Codegen 内 `ir_to_module` → mlir_rv |
| `SYSY_CC_ENABLE_RV_IR_BACKEND=1` | 0 | 同上（兼容旧名） |
| `SYSY_RV_ENABLE_SCHEDULE=1` | 1 | 基本块内列表调度 |
| `SYSY_RV_ENABLE_INST_COMBINE=1` | 1 | RV 指令合并 |
| `SYSY_CC_MLIR_RV_FAST_RA=1` | 0 | RegAlloc 快速模式 |
| `SYSY_MLIR_RV_MAX_INSTS` / `MAX_VREGS` / `MAX_FRAME` | 1200/512/4096 | 大函数回退启发式 |

## 阶段（PATH A）

| 阶段 | 目标 |
|------|------|
| P0 | smoke + 不崩溃 |
| P1 | performance 子集正确性 |
| P2 | Schedule / AliasAttr 闭环 |
| P3 | Call>8、浮点回退策略 |
| P4 | HIR→CFG 进同一 `compiler`（**切片 1 已接入**；仍保留 `ir_to_module` 桥） |

## 本地对照

只读上游快照可放在仓库外或 `.gitignore` 的 `vendor/` 目录，**不**作为提交物、**不**在运行时 `exec` 另一套 `compiler`。提分逻辑以本仓库 `src/mlir_rv` + 桥接为准。

更新：2026-05-23
