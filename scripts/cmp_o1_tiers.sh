#!/usr/bin/env bash
# 对同一 .sy：O0 与 SYSY_O1_TIER=A|B|C|D 的 -O1 分别 qemu，两两比对 stdout/退出码。
#
#   export LIBSYSY=/path/to/libsysy.a
#   ./scripts/cmp_o1_tiers.sh performance/matmul1.sy
#   ./scripts/cmp_o1_tiers.sh performance/*.sy   # 批量（大题很慢）

set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
COMPILER="${COMPILER:-$ROOT/compiler}"
RISCV_GCC="${RISCV_GCC:-riscv64-linux-gnu-gcc}"
QEMU="${QEMU:-qemu-riscv64}"
LINK_FLAGS="${LINK_FLAGS:--static -mcmodel=medany}"
TIERS=(A B C D)

if [[ $# -lt 1 ]]; then
  echo "usage: $0 file.sy [file2.sy ...]" >&2
  exit 2
fi
if [[ ! -x "$COMPILER" ]]; then
  echo "error: run make first: $COMPILER" >&2
  exit 2
fi
if [[ -z "${LIBSYSY:-}" || ! -f "$LIBSYSY" ]]; then
  echo "error: export LIBSYSY=/path/to/libsysy.a" >&2
  exit 2
fi
command -v "$RISCV_GCC" >/dev/null 2>&1 || { echo "error: missing $RISCV_GCC" >&2; exit 2; }
command -v "$QEMU" >/dev/null 2>&1 || { echo "error: missing $QEMU" >&2; exit 2; }

run_one_sy() {
  local SY="$1"
  SY="$(realpath "$SY")"
  local dir base stdin_file golden_out TMP
  dir=$(dirname "$SY")
  base=$(basename "$SY" .sy)
  TMP=$(mktemp -d "${TMPDIR:-/tmp}/tiers.XXXXXX")
  stdin_file=/dev/null
  [[ -f "$dir/$base.in" ]] && stdin_file="$dir/$base.in"
  golden_out="$dir/$base.out"

  run_tag() {
    local tag="$1"
    shift
    local s="$TMP/$tag.s" elf="$TMP/$tag.elf" out="$TMP/$tag.stdout" rc="$TMP/$tag.rc"
    local -a comp=( "$COMPILER" -S -o "$s" "$SY" )
    [[ "$tag" != "O0" ]] && comp+=( -O1 )
    if ! env "$@" "${comp[@]}" 2>"$TMP/$tag.ce.log"; then
      echo "  $tag CE" >&2
      head -3 "$TMP/$tag.ce.log" >&2
      return 2
    fi
    if ! $RISCV_GCC $LINK_FLAGS "$s" "$LIBSYSY" -o "$elf" 2>"$TMP/$tag.le.log"; then
      echo "  $tag LE" >&2
      return 3
    fi
    set +e
    if [[ -n "${RUN_TIMEOUT:-}" ]]; then
      timeout "$RUN_TIMEOUT" "$QEMU" "$elf" <"$stdin_file" >"$out" 2>"$TMP/$tag.qemu.err"
    else
      "$QEMU" "$elf" <"$stdin_file" >"$out" 2>"$TMP/$tag.qemu.err"
    fi
    local ec=$?
    set -e
    if [[ "$ec" -eq 124 ]]; then
      echo "  $tag TLE" >&2
      return 4
    fi
    if [[ "$ec" -ne 0 ]]; then
      echo "  $tag RE exit=$ec" >&2
      return 4
    fi
    echo "$ec" >"$rc"
    return 0
  }

  echo "=== $base ==="
  run_tag O0 || { rm -rf "$TMP"; return 1; }
  for t in "${TIERS[@]}"; do
    run_tag "O1_$t" SYSY_O1_TIER="$t" || { rm -rf "$TMP"; return 1; }
  done

  tr -d '\r' <"$TMP/O0.stdout" >"$TMP/O0.norm"
  local ref_rc ref_out
  ref_rc=$(cat "$TMP/O0.rc")
  ref_out="$TMP/O0.norm"

  if [[ -f "$golden_out" ]]; then
    if ! diff -q "$golden_out" "$ref_out" >/dev/null; then
      echo "  WARN: O0 stdout != $base.out (reference will still be O0)"
    else
      echo "  O0 matches golden .out"
    fi
  fi

  local fail=0
  local first_bad=""

  for t in "${TIERS[@]}"; do
    local tag="O1_$t"
    local rc
    rc=$(cat "$TMP/$tag.rc")
    tr -d '\r' <"$TMP/$tag.stdout" >"$TMP/$tag.norm"
    if [[ "$rc" != "$ref_rc" ]]; then
      echo "  WA tier $t: exit O0=$ref_rc vs $t=$rc"
      fail=1
      [[ -z "$first_bad" ]] && first_bad="$t"
      continue
    fi
    if ! diff -q "$ref_out" "$TMP/$tag.norm" >/dev/null; then
      echo "  WA tier $t: stdout differs from O0 (exit=$ref_rc)"
      fail=1
      [[ -z "$first_bad" ]] && first_bad="$t"
    else
      echo "  OK tier $t (vs O0)"
    fi
  done

  if [[ "$fail" -eq 0 ]]; then
    echo "  >> ALL tiers A-D match O0"
  else
    local prev="O0"
    for t in "${TIERS[@]}"; do
      [[ "$t" == "$first_bad" ]] && break
      prev="$t"
    done
    echo "  >> first failing tier: $first_bad ; suggest default <= $prev"
  fi
  rm -rf "$TMP"
  return "$fail"
}

fail_count=0
for sy in "$@"; do
  if [[ ! -f "$sy" ]]; then
    echo "skip missing: $sy" >&2
    continue
  fi
  if ! run_one_sy "$sy"; then
    ((fail_count++)) || true
  fi
done

if [[ "$fail_count" -gt 0 ]]; then
  echo "FAILED $fail_count case(s)" >&2
  exit 1
fi
echo "All cases OK"
