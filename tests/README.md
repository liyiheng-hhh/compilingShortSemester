# ToyC 测试目录规范

## 目录结构

```
tests/
├── smoke/          # 冒烟测试：最小可运行样例，验证构建和基础流程
├── expr/           # 表达式测试：字面量、算术、关系、逻辑、短路求值
├── decl/           # 声明测试：变量、常量、作用域、符号表
├── stmt/           # 语句测试：赋值、if/else、while、break/continue、return
├── func/           # 函数测试：定义、调用、参数、递归、栈帧
├── global/         # 全局测试：全局变量、全局常量、编译期求值
├── negative/       # 负面测试：语法错误、语义错误、边界检查
├── regress/        # 回归测试：已修复 bug 的复现用例
└── opt/            # 优化测试：开启 -opt 前后的对比用例
```

## 测试文件命名约定

| 文件类型 | 扩展名 | 说明 |
|---------|--------|------|
| 测试源码 | `.tc` | ToyC 源程序，由编译器从 stdin 读取 |
| 期望结果 | `.expected` | 期望的退出码和可选的输出对比内容 |
| 参考输出 | `.reference.s` | 参考汇编输出（可选，用于人工比对） |

### `.expected` 文件格式

```
EXIT_CODE: <整数>
STDOUT_MATCH: <正则表达式或空>
STDERR_MATCH: <正则表达式或空>
```

示例：

```
EXIT_CODE: 0
```

```
EXIT_CODE: 0
STDOUT_MATCH: ^    li a0, 42$
```

```
EXIT_CODE: 1
STDERR_MATCH: undefined symbol 'x'
```

## 测试运行

```bash
# 运行全部测试
python run_tests.py

# 运行指定目录的测试
python run_tests.py smoke/

# 运行指定测试文件
python run_tests.py smoke/return_zero.tc

# 开启优化模式运行测试
python run_tests.py --opt
```

## 添加新测试

1. 在对应功能目录下创建 `.tc` 文件
2. 创建同名 `.expected` 文件，指定期望退出码
3. （可选）创建 `.reference.s` 文件作为参考汇编

## 测试分类说明

### smoke/
- 验证编译器能正常构建和运行
- 覆盖最小闭环：`int main() { return 常量; }`
- 用于快速判断构建状态

### expr/
- 表达式优先级与结合性
- 算术运算：`+`, `-`, `*`, `/`, `%`
- 关系运算：`<`, `>`, `<=`, `>=`, `==`, `!=`
- 逻辑运算：`&&`, `||`, `!`
- 短路求值验证

### decl/
- 局部变量声明与初始化
- 局部常量声明与初始化
- 嵌套作用域与变量屏蔽
- 声明后使用检查

### stmt/
- 语句块 `{ ... }`
- 空语句 `;`
- 表达式语句
- 赋值语句
- `if` / `if-else`
- `while` 循环
- `break` / `continue`
- `return` 语句

### func/
- 函数定义与调用
- 参数传递（多参数）
- 返回值处理
- void 函数
- 递归调用

### global/
- 全局变量声明与初始化
- 全局常量声明与初始化
- 编译期常量求值
- 全局作用域访问

### negative/
- 语法错误：缺少分号、括号不匹配、非法 token
- 语义错误：未定义变量、类型不匹配、常量赋值
- 边界检查：除零、数组越界（如支持）

### regress/
- 修复的 bug 必须添加回归用例
- 命名格式：`issue_<编号>_<描述>.tc`
- 记录修复的 commit hash 和问题描述

### opt/
- 常量折叠前后对比
- 死代码删除验证
- 公共子表达式消除
- 寄存器复用效果

## 注意事项

- 所有测试用例应保持独立，不依赖其他测试文件
- 避免在测试中使用硬编码的文件路径
- 负面测试需明确期望的错误类型和退出码
- 回归测试需在注释中说明修复的问题
## 当前可执行回归

完整回归由 `tests/run_regressions.py` 内置的 RISC-V32 执行器完成，不依赖外部 QEMU、汇编器或链接器。以下每个样例都会分别在普通模式和 `-opt` 模式运行并断言最终退出码：

| 文件 | 期望退出码 | 覆盖内容 |
| --- | ---: | --- |
| `smoke/return_zero.tc` | 0 | 最小 main |
| `smoke/return_one.tc` | 1 | 基础常量返回 |
| `smoke/return_42.tc` | 42 | 基础常量返回 |
| `smoke/return_expr.tc` | 14 | 常量表达式与优先级 |
| `expr/logic_short_circuit.tc` | 7 | 逻辑运算与短路求值 |
| `expr/algebra_strength.tc` | 49 | 代数化简与乘法强度削弱 |
| `expr/readonly_propagation.tc` | 42 | 只读常量、复制传播与纯死表达式 |
| `expr/nested_binary.tc` | 45 | 嵌套二元表达式与临时寄存器 |
| `expr/many_locals_nested.tc` | 124 | 多局部变量、栈槽与嵌套表达式 |
| `expr/repeated_subexpr.tc` | 81 | 重复子表达式复用 |
| `expr/sparse_multiply.tc` | 56 | 稀疏常量乘法的移位加法 |
| `expr/target_reg_assign.tc` | 26 | 目标寄存器赋值 |
| `stmt/control_flow.tc` | 12 | while、if、break、continue |
| `stmt/branch_conditions.tc` | 17 | 直接条件分支与逻辑取反 |
| `stmt/dead_unreachable.tc` | 7 | 纯死语句与不可达代码 |
| `stmt/loop_bound_cache.tc` | 86 | 循环不变量边界缓存 |
| `stmt/loop_invariant_multiply.tc` | 53 | 嵌套循环乘法提升与修改集保护 |
| `stmt/induction_multiply.tc` | 132 | 单位步长归纳乘积强度削弱与可变步长保护 |
| `stmt/affine_loop_ctfe.tc` | 20 | 仿射计数循环快速幂与顺序递推语义 |
| `stmt/affine_loop_declarations.tc` | 20 | 仿射循环内局部声明、常量槽位与临时量依赖 |
| `stmt/loop_register_priority.tc` | 214 | 循环热点变量优先寄存器分配与动态指令上限 |
| `func/factorial.tc` | 120 | 函数调用与递归 |
| `func/inline_args_once.tc` | 6 | 内联实参只求值一次 |
| `func/inline_block.tc` | 19 | 带局部声明的函数内联 |
| `func/inline_branch_return.tc` | 12 | 双返回分支叶函数内联与实参单次求值 |
| `func/inline_simple.tc` | 23 | 简单返回表达式内联 |
| `func/leaf_no_frame.tc` | 15 | 无栈帧叶函数 |
| `func/many_args.tc` | 55 | 超过八个实参 |
| `func/register_args.tc` | 55 | 寄存器与栈传参 |
| `func/tail_recursive.tc` | 120 | 尾递归转跳转 |
| `func/void_side_effect.tc` | 9 | void 函数与全局副作用 |
| `global/globals.tc` | 18 | 全局变量、常量与赋值 |
| `decl/scope_const.tc` | 21 | 局部常量和嵌套遮蔽 |
| `opt/whole_program_eval.tc` | 133 | 全程序求值、全局副作用、循环和尾递归 |
| `opt/ctfe_slot_binding.tc` | 37 | CTFE 槽位绑定、遮蔽和循环内重复声明 |
| `opt/ctfe_memoized_recursion.tc` | 112 | 纯递归记忆化与全局副作用隔离 |

构建后运行：

```bash
ctest --test-dir build --output-on-failure
```

根目录的 `run_tests.py` 继续保留，用于按目录筛选、检查编译器诊断和维护 `.expected` 文件；最终运行结果以 CTest 的完整退出码回归为准。
