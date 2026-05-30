#!/usr/bin/env bash
# 逐个关闭 pass，跑 performance 全集，记录 platform kernel 合计。
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
# shellcheck source=opt-passes-on.sh
source "$ROOT/scripts/opt-passes-on.sh"
export LIBSYSY="$ROOT/libsysy.a"
export RUNTIME_PERF_TIMEOUT_SEC="${RUNTIME_PERF_TIMEOUT_SEC:-120}"

OUT="$ROOT/tests/.out/bisect-kernel"
mkdir -p "$OUT"
RESULT="$OUT/summary.txt"
: >"$RESULT"

run_one() {
  local label="$1"
  shift
  local csv="$OUT/${label}.csv"
  echo "=== [$label] $(date -Iseconds) env: $* ===" | tee -a "$RESULT"
  env "$@" RUNTIME_CSV="$csv" make runtime-eval SUITE=performance OPT=O1 >>"$OUT/${label}.log" 2>&1
  python3 - "$csv" "$label" >>"$RESULT" << 'PY'
import csv, sys
path, label = sys.argv[1], sys.argv[2]
rows = [r for r in csv.DictReader(open(path)) if r.get("pass") == "1" and r.get("median_kernel_s")]
if len(rows) < 60:
    print(f"{label}: INCOMPLETE cases={len(rows)}/60")
    sys.exit(0)
total = sum(float(r["median_kernel_s"]) * 1000 for r in rows)
print(f"{label}: kernel_sum_ms={total:.3f} cases=60 csv={path}")
PY
}

make -j4 >>"$OUT/build.log" 2>&1
make libsysy.a >>"$OUT/build.log" 2>&1

run_one "all_on"
run_one "no_guarded" SYSY_CC_NO_GUARDED_ACCUM=1
run_one "no_rsm" SYSY_CC_NO_ROW_SCRATCH_MATMUL=1
run_one "no_tiling" SYSY_CC_NO_LOOP_TILING=1
run_one "no_guard_rsm_tile" \
  SYSY_CC_NO_GUARDED_ACCUM=1 \
  SYSY_CC_NO_ROW_SCRATCH_MATMUL=1 \
  SYSY_CC_NO_LOOP_TILING=1

echo "=== done $(date -Iseconds) ===" | tee -a "$RESULT"
cat "$RESULT"
