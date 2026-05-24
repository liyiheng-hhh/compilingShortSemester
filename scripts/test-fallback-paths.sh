#!/usr/bin/env bash
# Compare dialect vs legacy on fallback-related tests (run inside docker after make + libsysy).
set -uo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

export LIBSYSY="${LIBSYSY:-/tmp/sysy-runtime-lib/build/libsysy.a}"
export USE_O1=1
export CONTINUE_ON_FAIL=1
export RUN_TIMEOUT="${RUN_TIMEOUT:-120s}"

CASEDIR="$ROOT/tests/.tmp-fallback-cases"
rm -rf "$CASEDIR" && mkdir -p "$CASEDIR"

for sy in \
  performance/huffman-01.sy performance/crc1.sy performance/shuffle0.sy \
  performance/knapsack_naive-1.sy examples/golden_knapsack/knapsack_mini.sy \
  performance/h-10-01.sy performance/conv2d-1.sy; do
  [[ -f "$sy" ]] || continue
  base=$(basename "$sy" .sy)
  dir=$(dirname "$sy")
  cp "$sy" "$CASEDIR/$base.sy"
  cp "$dir/$base.out" "$CASEDIR/$base.out" 2>/dev/null || true
  cp "$dir/$base.in" "$CASEDIR/$base.in" 2>/dev/null || true
  cp "$dir/$base.ret" "$CASEDIR/$base.ret" 2>/dev/null || true
done

run_batch() {
  local label="$1"
  shift
  echo "========== $label =========="
  env "$@" ./scripts/run_sy_tests.sh "$CASEDIR"
  echo
}

run_batch "default (fallback rules)"
run_batch "dialect forced" SYSY_CC_FORCE_DIALECT_PIPELINE=1
run_batch "legacy forced" SYSY_CC_NO_DIALECT_PIPELINE=1
