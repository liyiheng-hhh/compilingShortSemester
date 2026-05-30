#!/usr/bin/env bash
# Phase 0.2: 批量 asm A/B + pass stats（OPT_ACTION.md）
#
#   BASE=/tmp/compiler2026-baseline/compiler ./scripts/asm-ab-perf.sh
#   BASE=... CAND=... ./scripts/asm-ab-perf.sh performance/matmul2.sy
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=opt-passes-on.sh
source "$ROOT/scripts/opt-passes-on.sh"
BASE="${BASE:-/tmp/compiler2026-baseline/compiler}"
CAND="${CAND:-$ROOT/compiler}"
PERF="${PERF:-$ROOT/performance}"

CASES=(
  matmul1 matmul2 matmul3
  01_mm1 01_mm2 01_mm3
  many_mat_cal-1 many_mat_cal-2 many_mat_cal-3
  transpose0 transpose1 transpose2
  shuffle0 shuffle1 shuffle2
  sl1 sl2 sl3
  conv2d-1 conv2d-2 conv2d-3
)

if [[ $# -gt 0 ]]; then
  CASES=("$@")
fi

if [[ ! -x "$BASE" ]]; then
  echo "error: baseline compiler missing: $BASE" >&2
  echo "  run Phase 0.1: git worktree add /tmp/compiler2026-baseline abbf8a4 --detach && cd /tmp/compiler2026-baseline && make -j4 && make libsysy.a" >&2
  exit 2
fi
if [[ ! -x "$CAND" ]]; then
  echo "error: candidate compiler missing: $CAND (run make)" >&2
  exit 2
fi

tmpdir="$(mktemp -d "${TMPDIR:-/tmp}/asm-ab.XXXXXX")"
trap 'rm -rf "$tmpdir"' EXIT

printf '%-22s %8s %8s %6s %s\n' "case" "base_ln" "cand_ln" "asm" "pass_stats"
for base in "${CASES[@]}"; do
  sy="$PERF/$base.sy"
  if [[ ! -f "$sy" ]]; then
    printf '%-22s %8s\n' "$base" "MISSING"
    continue
  fi
  bs="$tmpdir/$base.base.s"
  cs="$tmpdir/$base.cand.s"
  "$BASE" -S -O1 -o "$bs" "$sy" 2>/dev/null || { printf '%-22s %8s\n' "$base" "BASE_CE"; continue; }
  "$CAND" -S -O1 -o "$cs" "$sy" 2>/dev/null || { printf '%-22s %8s\n' "$base" "CAND_CE"; continue; }
  bln=$(wc -l <"$bs")
  cln=$(wc -l <"$cs")
  if diff -q "$bs" "$cs" >/dev/null 2>&1; then
    asm="SAME"
  else
    asm="DIFF"
  fi
  stats="$(
    SYSY_CC_PASS_STATS=1 "$CAND" -S -O1 -o /dev/null "$sy" 2>&1 \
      | grep -iE 'replaced|tiled|lifted|rewrite' \
      | grep -v ': 0' \
      | tr '\n' '; ' \
      | sed 's/; $//' || true
  )"
  [[ -z "$stats" ]] && stats="-"
  printf '%-22s %8s %8s %6s %s\n' "$base" "$bln" "$cln" "$asm" "$stats"
done
