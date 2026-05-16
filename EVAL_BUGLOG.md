# 评测 / 对拍 Bug 记录

每次官方评测、希冀、本地 Docker 批测或 FPGA 对拍之后，把**新发现的问题**追加到下方「记录表」一节（复制一行模板，填日期与结论即可）。

## 使用说明

- **何时写**：一次评测跑完后，只要有 **CE / RE / WA / TLE / MLE** 或本地与 golden 不一致，就记一条。
- **写什么**：用例名、现象、是否已修、根因一句话、相关文件/PR（若有）。
- **已修复**：把「状态」改为 `fixed`，并补一行「修复说明」或链接 commit。

---

## 记录表

| 日期 | 用例 / 场景 | 状态 | 现象摘要 | 根因 / 备注 |
|------|----------------|------|----------|-------------|
| 2026-05-15 | `golden_magic_div/boundary.sy` + `-O1` | fixed | 第三行输出应为 `-2`，实为 `12` | 有符号除常数 magic 与 libdivide 不一致：移位后应为 `q += (q < 0)`，误写成 `q += (q >> 31)`；见 `codegen.cpp` `emitSigned32DivByConstMagicPayload` |
| 2026-05-15 | 性能榜大量 `matmul*` / `conv2d*` 等 | mitigated | 日志 `Killed`（非语法 CE） | 评测机 OOM：`irHoistLoopInvariantCFG` 支配矩阵 `O(nb²)`；已对 `nb²` 超预算跳过 CFG LICM，大题仍若 `Killed` 需继续压内存或稀疏化 |
| 2026-05-15 | `codegen.cpp` `leaf` 未声明 | fixed | Docker 编编译器失败 | `emitFunction` 补充 `leaf` 与 `funcDefAstContainsCall` / `irContainsCall` |
| 2026-05-15 | `irHoistLoopInvariantCFG` / `irHoistPureInvariantSimpleWhile` / `irHoistInvariantLoadGlobalSimpleWhile` | fixed | `-O1` 编译小段循环时栈溢出 / SIGSEGV | 外提后曾尾递归自调，可无限递归；改为有限轮迭代 +「一轮无外提则返回」 |
| 2026-05-15 | `crc*` / `03_sort*` 等（含 `<<`/`>>` 的 SysY） | fixed（常见 WA） | 移位被误编译为加法 | `lexer`/`parser`/`semantic`/`ir_build`/`ir_opt`/`codegen` 补全 `<<`/`>>`；`ir_build` 对未知二元运算符改为报错 |

---

## 模板（复制到表格上方新的一行）

```markdown
| YYYY-MM-DD | 用例名或脚本 | open / fixed | 一句话现象 | 根因或待查方向 |
```

---

## WA / 哈希不一致 / 结果为 `None` —— 能否直接修？

**能修，但必须能本地复现。** 当前仓库里**没有**希冀/赛方下发的 `h-4-01`、`h-8-02`、`h-9-01` 等 `.sy` 源文件（仅有 `examples/` 下少量 golden），因此无法凭空对着「哈希名」改编译器逻辑。

请你任选一种方式提供复现材料（任选其一即可）：

1. 把对应 **`xxx.sy`**（及若有 **`xxx.in`**）放进本仓库任意路径（例如 `local_eval_cases/h-4-01.sy`），在聊天里说明路径；或  
2. 把 **`./compiler -S -O1 -o out.s 该文件.sy` 的 `out.s`**、以及 **`qemu` 实际 stdout / 退出码`** 贴出来；或  
3. 贴评测机返回的 **完整编译/运行日志**（含 `None` 前后几行）。

拿到其中一种后，可以按下面顺序查（也可交给 Cursor 按文件跑）：

### O1 子系统二分：`SYSY_CC_*` 环境变量（不改变默认提交行为）

在未改命令行（仍是 `compiler ... -O1`）的前提下，通过环境变量**单独关掉**某类优化（本地或自管评测机上设置即可；官方希冀未设置则与默认一致）。

| 变量 | 为真时效果（`1` 或非空、`0` 为关） |
|------|-------------------------------------|
| `SYSY_CC_NO_AST_LOOP_INTERCHANGE` | 关闭 AST **`loopInterchangePass`（双 `while` 转置交换）** |
| `SYSY_CC_NO_CFG_LICM` | 关闭 **`irHoistLoopInvariantCFG`（CFG 跨块循环不变量外提）** |
| `SYSY_CC_NO_SIMPLE_WHILE_LICM` | 关闭 **`irHoistInvariantLoadGlobalSimpleWhile` + `irHoistPureInvariantSimpleWhile`（单块 `while` 外提）** |
| 同时设 `NO_SIMPLE_WHILE_LICM` 与 `NO_CFG_LICM` | 关闭上述 **全部三类 LICM** |
| `SYSY_CC_NO_IR_OPT` | **跳过整个 `irOptimizeBlock`**（仅 `irRefreshCFG`），保留 IR 路径与槽分配；用于判断 WA 是否来自任意 IR 中端轮 |
| `SYSY_CC_FORCE_AST_LOOP_INTERCHANGE` | 在 **保守默认**（未 `-DSYSY_O1_FULL`）下强行打开 AST 转置交换（本地要打榜用） |
| `SYSY_CC_FORCE_CFG_LICM` | 在保守默认下强行打开 **CFG LICM** |

默认提交构建（未定 `SYSY_O1_FULL`）为 **稳健 O1**：不传环境变量时不做 AST 交换与 CFG LICM，`make` 中加 `CXXFLAGS_EXTRA=-DSYSY_O1_FULL=1` 可恢复全开。

一键对同一文件试多种组合（打 MD5，不跑 qemu）：

```bash
./scripts/bisect_sysy_o1_env.sh path/to/fail.sy
```

若某模式下 **MD5 与 baseline 不同** 且 **qemu 输出由 WA 变 AC**，即可把锅缩小到对应 pass。

| 步骤 | 命令 / 动作 |
|------|----------------|
| 确认是 O0 还是 O1 | 对同一 `.sy` 分别 `-O0`（或不加）与 `-O1` 生成 `.s`，看 WA 是否仅 O1 出现 |
| 最小化 | 若可删减源码仍 WA，保留最小片段便于 bisect |
| 对拍 | 与 `clang` 交叉编译 SysY 到可执行（若你有环境）或与官方 `.out` 逐行 diff |

**`None` 常见含义**：未生成输出文件、评测脚本未捕获 stdout、或 **FPGA/仿真阶段崩溃**（与「编译器逻辑 WA」不同）。需要完整日志才能区分。

---

## 状态说明

| 状态 | 含义 |
|------|------|
| `open` | 已确认，尚未合入修复 |
| `fixed` | 已在仓库中修复 |
| `mitigated` | 部分缓解或依赖环境（如加 ulimit、跳过某优化） |
| `wontfix` | 判定为评测脚本/数据问题，不修编译器 |

---

## 拿不到隐藏点时的办法（比赛常见）

没有 `h-4-01.sy` 时，靠**工程方法**缩小「必错空间」，而不是猜哈希。

### 1. O0 对拍 O1（最快定位「是不是优化惹的祸」）

同一 `.sy`：

- **O0 AC、O1 WA** → 高度怀疑 **IR / `ir_opt` / `emitIr*` / 激进 codegen**；可在本地临时关掉某类优化（或 `#if 0` 某 pass）做二分。
- **O0 也 WA** → **语义 / 前端 / 未优化路径**，与隐藏点是否公开无关，用任意能复现的小程序修。

### 2. 用公开或自造「同结构」用例代替隐藏点

按榜单**题型**自建最小 `.sy`（矩阵乘、转置、CRC、shuffle、多分支等），不必与官方同名：

- 对照 README 里「热点类型 → 典型瓶颈」表，**自己写 20～50 行**能触发同类 IR 的程序；
- 与 **O0 或 `riscv64 gcc` 参考语义**（若你有）对拍输出；
- **`-O1` 与不加优化** 对比，专门轰击你最担心的 pass。

### 3. 属性 / 随机小测（不依赖具体隐藏文件）

例如对 **有符号除常数 / 取模**：

- 随机 `n ∈ [-2^31, 2^31-1]`、随机小集合里的 `d`，检查  
  `n == (n/d)*d + (n%d)`（在 SysY 32 位语义下）  
  或与你信任的 **窄范围枚举** 对拍。

对 **浮点 / IO**：用小脚本批量编译运行，比单次提交便宜。

### 4. 本地模拟评测机「编译被杀」

优先使用仓库脚本（在受限地址空间下执行**带路径的** `./compiler`）：

```bash
./scripts/compile_memlimit.sh 512M ./compiler -S -O1 -o /tmp/x.s 你的大用例.sy
```

或直接（语义因系统而异）：

```bash
ulimit -Sv 524288   # 按需调小虚拟内存上限
./compiler -S -O1 -o /tmp/x.s 你的大用例.sy
```

配合 `/usr/bin/time -v` 看 **Max RSS**，专门压 **CFG / 支配 / 大函数**，避免只会在线上 `Killed`。

### 5. 提交策略上的「二分」

若平台允许多次提交：用 **feature flag** 或 **临时关掉某优化** 打一枪，看 WA/RE 是否从「哈希 A」变成「哈希 B」或变 AC——用提交结果当**远程二分信号**（成本高，但无源码时唯一信号）。

### 6. 仍无法定位时

- 查赛方 **公开样例包 / 往届公开题**（仅合法公开资料）；
- 论坛/答疑里是否提供 **错误类型统计**（仅 WA 还是 RE）；
- 在 `EVAL_BUGLOG.md` 里记「现象 + 已尝试的 pass 开关」，避免重复踩坑。

