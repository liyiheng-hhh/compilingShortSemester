#!/usr/bin/env bash
# 对同一 .sy 在不同 `SYSY_CC_*` 下跑 `-S -O1`，打印 CE/汇编 MD5。
#
# **当前默认提交**：未设 `SYSY_CC_FORCE_AGGRESSIVE_O1` / 未编译 `-DSYSY_O1_FULL=1` 时，
# `-O1` ≈ 不换 IR、不开 Codegen 特技，与 **不加 -O1** 行为一致；本脚本第 0 行即该模式。
#
# 要打满性能再试子 pass，先加一行 `01_aggressive`（`FORCE=1`），再对 `NO_*` 二分。
#
# 用法（仓库根、`make` 后）:
#   ./scripts/bisect_sysy_o1_env.sh path/to/fail.sy

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
run_mode "00_safe_default_O1"

run_mode "01_force_aggressive_full" SYSY_CC_FORCE_AGGRESSIVE_O1=1

run_mode "02_aggressive_no_ast_ix" \
  SYSY_CC_FORCE_AGGRESSIVE_O1=1 \
  SYSY_CC_NO_AST_LOOP_INTERCHANGE=1

run_mode "03_aggressive_no_cfg_licm" \
  SYSY_CC_FORCE_AGGRESSIVE_O1=1 \
  SYSY_CC_NO_CFG_LICM=1

run_mode "04_aggressive_no_simple_licm" \
  SYSY_CC_FORCE_AGGRESSIVE_O1=1 \
  SYSY_CC_NO_SIMPLE_WHILE_LICM=1

run_mode "05_aggressive_no_licm_pair" \
  SYSY_CC_FORCE_AGGRESSIVE_O1=1 \
  SYSY_CC_NO_SIMPLE_WHILE_LICM=1 \
  SYSY_CC_NO_CFG_LICM=1

run_mode "06_aggressive_no_ir_mid" \
  SYSY_CC_FORCE_AGGRESSIVE_O1=1 \
  SYSY_CC_NO_IR_OPT=1
