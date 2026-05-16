#!/usr/bin/env bash
# 对同一 .sy 在不同分层 -O1 下编译，打印汇编 MD5（需已 make）。
#
# 默认提交：未设 SYSY_O1_TIER 时为 **D**（见 opt_config.h SYSY_O1_DEFAULT_TIER）。
# 本地试档：SYSY_O1_TIER=B|C|D 或 SYSY_O1_FULL=1 编的 compiler。
#
#   ./scripts/bisect_sysy_o1_env.sh performance/matmul1.sy

set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
COMPILER="$ROOT/compiler"
SY="${1:?usage: $0 path/to/file.sy}"

if [[ ! -x "$COMPILER" ]]; then
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
echo "tier	env	..."
run_mode "A_default" 

run_mode "B_ir_light" SYSY_O1_TIER=B

run_mode "C_simple_licm" SYSY_O1_TIER=C

run_mode "D_full" SYSY_O1_TIER=D

run_mode "D_no_cfg_licm" SYSY_O1_TIER=D SYSY_CC_NO_CFG_LICM=1

run_mode "D_no_ast_ix" SYSY_O1_TIER=D SYSY_CC_NO_AST_LOOP_INTERCHANGE=1
