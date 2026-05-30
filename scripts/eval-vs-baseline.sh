#!/usr/bin/env bash
# 两份 CSV A/B 对比（改 pass 前后、或与本机 baseline 编译器对比）
# 默认用 median_kernel_s（平台 kernel ms），不用 median_ms（QEMU wall，h-5/shuffle 会失真）。
#
#   ./scripts/eval-vs-baseline.sh baseline.csv candidate.csv
#   ./scripts/eval-vs-baseline.sh --run performance O1 /path/to/other/compiler
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if [[ "${1:-}" == "--run" ]]; then
  shift
  SUITE="${1:?suite}"
  OPT="${2:?O0|O1}"
  BASELINE_COMPILER="${3:?path/to/baseline/compiler}"
  shift 3
  TS="$(date +%Y%m%d-%H%M%S)"
  OUT="$ROOT/tests/.out/runtime/ab"
  mkdir -p "$OUT"
  A_CSV="$OUT/baseline-${TS}.csv"
  B_CSV="$OUT/candidate-${TS}.csv"
  RUNTIME_SOFT_PERF=1 RUNTIME_CSV="$A_CSV" COMPILER="$BASELINE_COMPILER" \
    "$ROOT/scripts/eval-runtime.sh" "$SUITE" "$OPT"
  RUNTIME_SOFT_PERF=1 RUNTIME_CSV="$B_CSV" \
    "$ROOT/scripts/eval-runtime.sh" "$SUITE" "$OPT"
  set -- "$A_CSV" "$B_CSV"
fi

if [[ $# -ne 2 ]]; then
  echo "usage: $0 <baseline.csv> <candidate.csv>" >&2
  echo "   or: $0 --run <suite> <opt> <baseline_compiler>" >&2
  exit 1
fi

A_CSV="$1"
B_CSV="$2"
[[ -f "$A_CSV" && -f "$B_CSV" ]] || { echo "error: missing csv" >&2; exit 2; }

STAGE_A_RATIO="${VS_BASELINE_STAGE_A_RATIO:-1.15}"
STAGE_B_RATIO="${VS_BASELINE_STAGE_B_RATIO:-1.05}"

python3 - "$A_CSV" "$B_CSV" "$STAGE_A_RATIO" "$STAGE_B_RATIO" << 'PY'
import csv, sys

a_csv, b_csv, stage_a, stage_b = sys.argv[1:5]
stage_a, stage_b = float(stage_a), float(stage_b)

def load(path):
    out = {}
    with open(path, newline="") as f:
        for row in csv.DictReader(f):
            if row.get("pass") != "1":
                continue
            cid = row["case_id"]
            ks = row.get("median_kernel_s", "").strip()
            if ks:
                out[cid] = float(ks) * 1000.0
            else:
                out[cid] = float(row["median_ms"])
    return out

base = load(a_csv)
cand = load(b_csv)
common = sorted(set(base) & set(cand))
ratios = []
regressed = improved = 0
base_sum = cand_sum = 0.0

print(f"{'case_id':<40} {'base_ms':>10} {'cand_ms':>10} {'ratio':>10} verdict")
print(f"{'':40} {'(kernel)':>10} {'(kernel)':>10}")
for cid in common:
    b, c = base[cid], cand[cid]
    base_sum += b
    cand_sum += c
    ratio = c / b if b > 0 else 0.0
    ratios.append(ratio)
    if ratio > 1.001:
        regressed += 1
        verdict = "slower"
    elif ratio < 0.999:
        improved += 1
        verdict = "faster"
    else:
        verdict = "ok"
    print(f"{cid:<40} {b:10.3f} {c:10.3f} {ratio:10.4f} {verdict}")

ratios.sort()
n = len(ratios)
if n:
    median = ratios[n // 2] if n % 2 else (ratios[n // 2 - 1] + ratios[n // 2]) / 2
else:
    median = float("nan")

print()
print(f"common_pass_cases={n} regressed={regressed} improved={improved}")
print(f"kernel_sum base={base_sum:.3f} cand={cand_sum:.3f} sum_ratio={cand_sum/base_sum:.4f}")
print(f"median_ratio(candidate/baseline)={median:.4f}  metric=platform_kernel")
print(f"gate stageA: median<={stage_a}  stageB: median<={stage_b}")
if median <= stage_b:
    gate = "stageB"
elif median <= stage_a:
    gate = "stageA"
else:
    gate = "fail"
print(f"gate={gate}")
PY
