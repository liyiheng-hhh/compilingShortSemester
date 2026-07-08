# ToyC Test Samples

These samples are small regression inputs for the compiler. The expected result is the process exit code after assembling and running the generated RISC-V32 program.

| File | Expected exit code | Coverage |
| --- | ---: | --- |
| `smoke/return_zero.tc` | 0 | Minimal `main` |
| `smoke/return_expr.tc` | 14 | Constant expression and precedence |
| `expr/logic_short_circuit.tc` | 7 | Logical operators and short-circuit behavior |
| `expr/algebra_strength.tc` | 49 | Algebraic simplification and multiply strength reduction |
| `expr/nested_binary.tc` | 45 | Nested binary expressions and temporary registers |
| `expr/many_locals_nested.tc` | 124 | Many locals, spilled stack slots, nested expressions |
| `expr/repeated_subexpr.tc` | 81 | Repeated operand expression reuse |
| `stmt/control_flow.tc` | 12 | `while`, `if`, `break`, `continue` |
| `stmt/branch_conditions.tc` | 17 | Direct conditional branches and logical negation |
| `func/factorial.tc` | 120 | Function call and recursion |
| `func/inline_args_once.tc` | 6 | Inline call arguments are evaluated once |
| `func/inline_simple.tc` | 23 | Simple return-expression function inlining |
| `func/many_args.tc` | 55 | More than eight call arguments |
| `func/register_args.tc` | 55 | Register and stack argument passing |
| `func/tail_recursive.tc` | 120 | Tail-recursive self-call lowering |
| `func/void_side_effect.tc` | 9 | `void` function and global side effect |
| `global/globals.tc` | 18 | Global variables, global constants, assignment |
| `decl/scope_const.tc` | 21 | Local constants and nested scope shadowing |
