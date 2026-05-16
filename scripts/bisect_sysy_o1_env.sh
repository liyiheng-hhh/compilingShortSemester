#!/usr/bin/env bash
# 对同一 .sy 用不同 SYSY_CC_* 环境变量跑 -O1，打印是否编过 + 汇编 MD5，便于二分 WA 是否来自
# AST 循环交换 / 单块 While LICM / CFG LICM / 整块 IR 中端优化。
#
# 用法（在仓库根，已 make 出 ./compiler）:
#   ./scripts/bisect_sysy_o1_env.sh path/to/fail.sy
#
# 若需对拍运行结果，自行对每种模式生成的 /tmp/sysy_bisect_*.s 链接运行（本脚本只比编译产物）。

set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
COMPILER="$ROOT/compiler"
SY="${1:?usage: $0 path/to/file.sy}"
shift || true

if [[ ! -f "$COMPILER" ]]; then
  echo "missing $COMPILER — run: make" >&2
  exit 1
fi

run_mode() {
  local name=$1
  shift
  local out="/tmp/sysy_bisect_${name}.s"
  rm -f "$out"
  set +e
  env "$@" "$COMPILER" -S -O1 -o "$out" "$SY" 2>/tmp/sysy_bisect_err.txt
  local st=$?
  set -e
  if [[ $st -ne 0 ]]; then
    echo "$name	CE_or_fail	exit=$st	$(head -1 /tmp/sysy_bisect_err.txt | tr '\n' ' ')"
    return
  fi
  echo -n "$name	OK	bytes=$(wc -c <"$out")	md5="
  md5sum "$out" | awk '{print $1}'
}

echo "SY=$SY"
echo "mode	status	..."
run_mode "00_baseline"

run_mode "01_no_ast_loop_ix" SYSY_CC_NO_AST_LOOP_INTERCHANGE=1

run_mode "02_no_cfg_licm" SYSY_CC_NO_CFG_LICM=1

run_mode "03_no_simple_while_licm" SYSY_CC_NO_SIMPLE_WHILE_LICM=1

run_mode "04_no_all_licm" \
  SYSY_CC_NO_SIMPLE_WHILE_LICM=1 \
  SYSY_CC_NO_CFG_LICM=1

run_mode "05_no_ir_opt" SYSY_CC_NO_IR_OPT=1
