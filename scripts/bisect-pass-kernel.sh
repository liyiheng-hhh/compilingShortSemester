#!/usr/bin/env bash
# Pass 二分：逐个关闭 pass，记录 platform kernel 合计
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
source "$ROOT/scripts/opt-passes-on.sh"
export RUNTIME_PERF_TIMEOUT_SEC=120
export LIBSYSY="${LIBSYSY:-$ROOT/libsysy.a}"

mkdir -p tests/.out/runtime/bisect

kernel_sum() {
  python3 - "$1" << 'PY'
import csv, sys
s = 0.0
with open(sys.argv[1]) as f:
    for row in csv.DictReader(f):
        if row.get("pass") != "1": continue
        k = row.get("median_kernel_s", "").strip()
        if k: s += float(k) * 1000
print(f"{s:.3f}")
PY
}

run_one() {
  local name="$1"
  shift
  unset SYSY_CC_NO_GUARDED_ACCUM SYSY_CC_NO_ROW_SCRATCH_MATMUL SYSY_CC_NO_LOOP_TILING 2>/dev/null || true
  for v in "$@"; do export "${v}=1"; done
  local csv="tests/.out/runtime/bisect/${name}.csv"
  export RUNTIME_CSV="$csv"
  echo "=== bisect: $name flags=${*:-none} ==="
  if ! make runtime-eval SUITE=performance OPT=O1 >"tests/.out/runtime/bisect/${name}.log" 2>&1; then
    echo "FAIL $name (see log)"
    return 1
  fi
  local ks
  ks="$(kernel_sum "$csv")"
  echo "RESULT $name kernel_sum_ms=$ks"
  echo "$name,$ks" >> tests/.out/runtime/bisect/summary.csv
}

: > tests/.out/runtime/bisect/summary.csv
echo "name,kernel_sum_ms" >> tests/.out/runtime/bisect/summary.csv

run_one "all-on"
run_one "no-ga" SYSY_CC_NO_GUARDED_ACCUM
run_one "no-rsm" SYSY_CC_NO_ROW_SCRATCH_MATMUL
run_one "no-tile" SYSY_CC_NO_LOOP_TILING
run_one "no-ga-rsm" SYSY_CC_NO_GUARDED_ACCUM SYSY_CC_NO_ROW_SCRATCH_MATMUL
run_one "no-ga-tile" SYSY_CC_NO_GUARDED_ACCUM SYSY_CC_NO_LOOP_TILING
run_one "no-rsm-tile" SYSY_CC_NO_ROW_SCRATCH_MATMUL SYSY_CC_NO_LOOP_TILING
run_one "no-all-three" SYSY_CC_NO_GUARDED_ACCUM SYSY_CC_NO_ROW_SCRATCH_MATMUL SYSY_CC_NO_LOOP_TILING

echo ""
echo "=== bisect summary (baseline ref 3876.724) ==="
if command -v column >/dev/null 2>&1; then
  column -t -s, tests/.out/runtime/bisect/summary.csv
else
  cat tests/.out/runtime/bisect/summary.csv
fi
