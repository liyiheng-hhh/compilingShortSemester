#!/usr/bin/env bash
# 对同一 .sy 分别用 O0 / -O1 编译 → 链接 → qemu，比较 stdout 与退出码。
# 用于无隐藏点时判断「问题是否出在优化路径」。
#
#   export LIBSYSY=/path/to/libsysy.a
#   ./scripts/cmp_o0_o1.sh path/to/foo.sy
#
# 环境变量：COMPILER RISCV_GCC QEMU LINK_FLAGS LIBSYSY RUN_TIMEOUT

set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER="${COMPILER:-}"
if [[ -z "$COMPILER" || ! -x "$COMPILER" ]]; then
  if [[ -x "$ROOT/compiler" ]]; then
    COMPILER="$ROOT/compiler"
  elif [[ -x "/tmp/compiler-build/compiler" ]]; then
    COMPILER="/tmp/compiler-build/compiler"
  else
    COMPILER="$ROOT/compiler"
  fi
fi
export COMPILER
cd "$ROOT"

RISCV_GCC="${RISCV_GCC:-riscv64-linux-gnu-gcc}"
QEMU="${QEMU:-qemu-riscv64-static}"
LINK_FLAGS="${LINK_FLAGS:--static -mcmodel=medany}"

if [[ $# -lt 1 || ! -f "${1:-}" ]]; then
  echo "usage: $0 <file.sy>" >&2
  exit 2
fi
if [[ ! -x "$COMPILER" ]]; then
  echo "error: run 'make' first: $COMPILER" >&2
  exit 2
fi
if [[ -z "${LIBSYSY:-}" || ! -f "$LIBSYSY" ]]; then
  echo "error: export LIBSYSY=/path/to/libsysy.a" >&2
  exit 2
fi
command -v "$RISCV_GCC" >/dev/null 2>&1 || { echo "error: missing $RISCV_GCC" >&2; exit 2; }
command -v "$QEMU" >/dev/null 2>&1 || { echo "error: missing $QEMU" >&2; exit 2; }

SY="$(realpath "$1")"
TMP="$(mktemp -d "${TMPDIR:-/tmp}/cmpo0o1.XXXXXX")"
cleanup() { rm -rf "$TMP"; }
trap cleanup EXIT

run_level() {
  local olevel="$1" tag="$2"
  local s="$TMP/$tag.s" elf="$TMP/$tag.elf" out="$TMP/$tag.stdout" rc="$TMP/$tag.rc"
  local comp_args=( -S -o "$s" "$SY" )
  [[ "$olevel" == "1" ]] && comp_args+=( -O1 )
  if ! "$COMPILER" "${comp_args[@]}" 2>"$TMP/$tag.ce.log"; then
    echo "CE ($tag) compile failed:" >&2
    sed 's/^/  /' "$TMP/$tag.ce.log" >&2
    return 2
  fi
  if ! $RISCV_GCC $LINK_FLAGS "$s" "$LIBSYSY" -o "$elf" 2>"$TMP/$tag.le.log"; then
    echo "LE ($tag) link failed:" >&2
    sed 's/^/  /' "$TMP/$tag.le.log" >&2
    return 3
  fi
  set +e
  if [[ -n "${RUN_TIMEOUT:-}" ]]; then
    timeout "$RUN_TIMEOUT" "$QEMU" "$elf" >"$out" 2>"$TMP/$tag.qemu.err"
  else
    "$QEMU" "$elf" >"$out" 2>"$TMP/$tag.qemu.err"
  fi
  local ec=$?
  set -e
  if [[ "$ec" -eq 124 ]]; then
    echo "RE ($tag) qemu timeout" >&2
    return 4
  fi
  if [[ "$ec" -ne 0 ]]; then
    echo "RE ($tag) qemu exit=$ec" >&2
    cat "$TMP/$tag.qemu.err" >&2 || true
    return 4
  fi
  echo "$ec" >"$rc"
}

echo "=== O0 ==="
run_level 0 O0
echo "=== O1 ==="
run_level 1 O1

rc0=$(cat "$TMP/O0.rc")
rc1=$(cat "$TMP/O1.rc")
if [[ "$rc0" != "$rc1" ]]; then
  echo "MISMATCH: exit code O0=$rc0 O1=$rc1" >&2
  exit 1
fi
if ! diff -q "$TMP/O0.stdout" "$TMP/O1.stdout" >/dev/null; then
  echo "MISMATCH: stdout differs (exit both=$rc0)" >&2
  echo "--- diff -u O0 O1 ---" >&2
  diff -u "$TMP/O0.stdout" "$TMP/O1.stdout" >&2 || true
  exit 1
fi
echo "OK: O0 and O1 agree (exit=$rc0, stdout bytes $(wc -c <"$TMP/O0.stdout"))"
