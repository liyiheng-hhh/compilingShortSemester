# 路径 A：compiler2026-x 壳 + MLIR RV 管线

在**保留本仓库前端与赛题入口**的前提下，迁入方言 IR（ModuleOp）与 RV 后端 pass，通过桥接层接到现有 `IRFunction`。

## 原则

- **保留**：`lexer` / `parser` / `semantic`、`src/main.cpp`、`Makefile` 的 `SRCS` 白名单、`EVAL_BUGLOG.md`
- **迁入**：`codegen/`、`opt/`（按需）、`mlir_rv/`；后续 `cfg/` + 完整 `hir/`（阶段 4）
- **命名**：环境变量 `SYSY_RV_*`；目录 `mlir_rv/`（避免与文本级 `src/rv/` 冲突）
- **提交节奏**：每阶段可独立评测，避免单次巨型 diff

## 阶段

| 阶段 | 目标 | 验收 |
|------|------|------|
| **P0** | `ir_to_module` 不崩溃 + smoke | `examples/smoke.sy -O1` + `SYSY_CC_ENABLE_MLIR_RV=1` |
| **P1** | 子集 12 例正确性 | `scripts/compare-rv-ir-subset.sh` ≥ baseline 水平 |
| **P2** | 调度/强度削减开关 | `SYSY_RV_ENABLE_SCHEDULE=1`，h-5-01 性能不退化 |
| **P3** | 补全 `ir_to_module`（Call>8、非常量除法回退） | performance 大类无 compile_fail |
| **P4** | 可选 HIR→CFG 桥（减维护成本） | 与参考管线语义对齐，逐步删 `ir_to_module` |

## 当前文件地图

```
src/codegen/       OpBase, Ops, Attrs, Builder
src/opt/           Pass, GVN（后续按 P2 加 Mem2Reg 等）
src/mlir_rv/       Lower, Schedule, RegAlloc, Dump, …
src/ir_to_module.* IRFunction → ModuleOp
src/rv_mlir_pipeline.*  pass 管线入口
src/rv/            文本 asm 后处理（与 MLIR 无关，保留）
```

## 环境变量（对外）

| 变量 | 默认 | 含义 |
|------|------|------|
| `SYSY_CC_ENABLE_MLIR_RV=1` | 0 | 启用 MLIR RV 后端 |
| `SYSY_RV_ENABLE_SCHEDULE=1` | 1 | 基本块内列表调度 |
| `SYSY_RV_ENABLE_INST_COMBINE=1` | 1 | RV inst combine |
| `SYSY_CC_MLIR_RV_FAST_RA=1` | 0 | RegAlloc 快速模式 |

## P0 工作项（进行中）

1. `ir_to_module`：栈布局、AliasAttr、跨块常量
2. 大函数 `mlirRvHeuristicEligible` 回退
3. `scripts/compare-rv-ir-subset.sh` 回归

维护：与 `DIALECT_RV_BACKEND.md` 同步。
