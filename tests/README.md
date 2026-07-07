# ToyC Test Samples

These samples are small regression inputs for the compiler. The expected result is the process exit code after assembling and running the generated RISC-V32 program.

| File | Expected exit code | Coverage |
| --- | ---: | --- |
| `smoke/return_zero.tc` | 0 | Minimal `main` |
| `smoke/return_expr.tc` | 14 | Constant expression and precedence |
| `expr/logic_short_circuit.tc` | 7 | Logical operators and short-circuit behavior |
| `stmt/control_flow.tc` | 12 | `while`, `if`, `break`, `continue` |
| `func/factorial.tc` | 120 | Function call and recursion |
| `func/many_args.tc` | 55 | More than eight call arguments |
| `func/void_side_effect.tc` | 9 | `void` function and global side effect |
| `global/globals.tc` | 18 | Global variables, global constants, assignment |
| `decl/scope_const.tc` | 21 | Local constants and nested scope shadowing |
