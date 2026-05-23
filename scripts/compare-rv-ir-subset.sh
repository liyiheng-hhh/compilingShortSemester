#!/usr/bin/env bash
# 对比 performance 子集正确性：
#   默认：baseline vs SYSY_CC_ENABLE_MLIR_RV=1（Codegen 内 ir_to_module 桥）
#   dialect：baseline vs SYSY_CC_ENABLE_DIALECT_PIPELINE=1（dialect_parse→HIR→CFG→mlir_rv）
# 用法：./scripts/compare-rv-ir-subset.sh [default|dialect|all]
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=runtime_common.sh
source "$ROOT/scripts/runtime_common.sh"

COMPILER="${COMPILER:-$ROOT/compiler}"
LIBSYSY="${LIBSYSY:-$ROOT/libsysy.a}"
RISCV_GCC="${RISCV_GCC:-riscv64-linux-gnu-gcc}"
QEMU="${QEMU:-qemu-riscv64}"
TIMEOUT_SEC="${TIMEOUT_SEC:-30}"
OPT_FLAGS="-O1"

VARIANT="${1:-${COMPARE_VARIANT:-default}}"
case "$VARIANT" in
  default | "") VARIANT=default ;;
  dialect) VARIANT=dialect ;;
  all) VARIANT=all ;;
  *)
    echo "usage: $0 [default|dialect|all]" >&2
    exit 2
    ;;
esac

# 代表性子集：小/中/大、sort/matmul/shuffle/h-5/conv/crypto
CASES=(
  crc1 h-1-01 optimization_scheduling1 knapsack_naive-1
  matmul1 01_mm1 03_sort1
  conv2d-1 crypto-1 h-5-01 shuffle1 huffman-01
)

PERF_DIR="${PERF_DIR:-$ROOT/performance}"
tmp="${TMPDIR:-/tmp}/rv-ir-cmp.$$"
mkdir -p "$tmp"
trap 'rm -rf "$tmp"' EXIT

run_one() {
  local mode="$1" base="$2"
  local sy="$PERF_DIR/$base.sy"
  local golden="$PERF_DIR/$base.out"
  local in_file="$PERF_DIR/$base.in"
  [[ -f "$in_file" ]] || in_file=/dev/null

  local work="$tmp/$mode/$base"
  mkdir -p "$work"
  local asm="$work/$base.s" elf="$work/$base.elf"

  if [[ "$mode" == "rv_ir" ]]; then
    env SYSY_CC_ENABLE_MLIR_RV=1 SYSY_CC_NO_DIALECT_PIPELINE=1 \
      "$COMPILER" -S -o "$asm" $OPT_FLAGS "$sy" >/dev/null 2>&1 || {
      echo "compile_fail"; return
    }
  elif [[ "$mode" == "dialect" ]]; then
    env SYSY_CC_ENABLE_DIALECT_PIPELINE=1 SYSY_CC_ENABLE_MLIR_RV=0 \
      "$COMPILER" -S -o "$asm" $OPT_FLAGS "$sy" >/dev/null 2>&1 || {
      echo "compile_fail"; return
    }
  else
    env SYSY_CC_NO_DIALECT_PIPELINE=1 "$COMPILER" -S -o "$asm" $OPT_FLAGS "$sy" >/dev/null 2>&1 || {
      echo "compile_fail"; return
    }
  fi

  $RISCV_GCC -static -mcmodel=medany "$asm" "$LIBSYSY" -o "$elf" -lm >/dev/null 2>&1 || {
    echo "link_fail"; return
  }

  set +e
  timeout "$TIMEOUT_SEC" "$QEMU" "$elf" <"$in_file" >"$work/out.txt" 2>"$work/err.txt"
  local rc=$?
  set -e

  if [[ $rc -eq 124 ]]; then
    echo "timeout"
    return
  fi
  if [[ $rc -ne 0 ]]; then
    echo "runtime_fail:$rc"
    return
  fi

  if [[ ! -f "$golden" ]]; then
    echo "ok_no_golden"
    return
  fi

  tr -d '\r' <"$work/out.txt" >"$work/out.stripped"
  if cmp -s "$golden" "$work/out.stripped"; then
    echo "ok"
    return
  fi
  local withrc="$work/out.withrc"
  cp "$work/out.stripped" "$withrc"
  if [[ -s "$withrc" ]] && [[ -n "$(tail -c 1 "$withrc")" ]]; then
    printf '\n' >>"$withrc"
  fi
  printf '%s\n' "$rc" >>"$withrc"
  if cmp -s "$golden" "$withrc"; then
    echo "ok"
    return
  fi
  echo "mismatch"
}

[[ -x "$COMPILER" ]] || { echo "error: build compiler first (make)" >&2; exit 2; }
[[ -f "$LIBSYSY" ]] || { echo "error: make libsysy.a" >&2; exit 2; }

run_compare_pair() {
  local alt_mode="$1" alt_label="$2"
  printf '%-28s %12s %12s %s\n' "case" "baseline" "$alt_label" "note"
  printf '%s\n' "----------------------------------------------------------------------------"

  local baseline_ok=0 alt_ok=0 both_ok=0 mismatch=0
  for base in "${CASES[@]}"; do
    local b r note
    b="$(run_one baseline "$base")"
    r="$(run_one "$alt_mode" "$base")"
    note=""
    if [[ "$b" == "ok" && "$r" == "ok" ]]; then
      both_ok=$((both_ok + 1))
      note="match"
    elif [[ "$b" == "ok" && "$r" != "ok" ]]; then
      mismatch=$((mismatch + 1))
      note="${alt_label^^}_REGRESSION"
    elif [[ "$b" != "ok" && "$r" == "ok" ]]; then
      note="${alt_label}_only_ok"
    else
      note="both_bad"
    fi
    [[ "$b" == "ok" ]] && baseline_ok=$((baseline_ok + 1))
    [[ "$r" == "ok" ]] && alt_ok=$((alt_ok + 1))
    printf '%-28s %12s %12s %s\n' "$base" "$b" "$r" "$note"
  done

  printf '%s\n' "----------------------------------------------------------------------------"
  printf 'variant=%s subset=%d baseline_ok=%d %s_ok=%d both_ok=%d regressions=%d\n' \
    "$VARIANT" "${#CASES[@]}" "$baseline_ok" "$alt_label" "$alt_ok" "$both_ok" "$mismatch"
  return "$([[ $mismatch -eq 0 ]] && echo 0 || echo 1)"
}

run_compare_all() {
  printf '%-28s %12s %12s %12s\n' "case" "baseline" "rv_ir" "dialect"
  printf '%s\n' "--------------------------------------------------------------------------------"

  local baseline_ok=0 rv_ok=0 dialect_ok=0 triple_ok=0
  for base in "${CASES[@]}"; do
    local b r d
    b="$(run_one baseline "$base")"
    r="$(run_one rv_ir "$base")"
    d="$(run_one dialect "$base")"
    [[ "$b" == "ok" ]] && baseline_ok=$((baseline_ok + 1))
    [[ "$r" == "ok" ]] && rv_ok=$((rv_ok + 1))
    [[ "$d" == "ok" ]] && dialect_ok=$((dialect_ok + 1))
    [[ "$b" == "ok" && "$r" == "ok" && "$d" == "ok" ]] && triple_ok=$((triple_ok + 1))
    printf '%-28s %12s %12s %12s\n' "$base" "$b" "$r" "$d"
  done

  printf '%s\n' "--------------------------------------------------------------------------------"
  printf 'variant=all subset=%d baseline_ok=%d rv_ir_ok=%d dialect_ok=%d triple_ok=%d\n' \
    "${#CASES[@]}" "$baseline_ok" "$rv_ok" "$dialect_ok" "$triple_ok"
  return 0
}

case "$VARIANT" in
  dialect) run_compare_pair dialect dialect ;;
  all) run_compare_all ;;
  *) run_compare_pair rv_ir rv_ir ;;
esac
exit $?
