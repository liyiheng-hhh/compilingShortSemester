#!/usr/bin/env bash
# 两份 CSV A/B 对比（改 pass 前后、或与本机 baseline 编译器对比）
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

declare -A a_ms a_pass
while IFS=, read -r _ cid _ _ _ _ pass med _; do
  [[ "$cid" == "case_id" ]] && continue
  a_pass["$cid"]="$pass"
  a_ms["$cid"]="$med"
done <"$A_CSV"

ratios=()
regressed=0
improved=0
common=0

printf '%-40s %10s %10s %10s %s\n' "case_id" "base_ms" "cand_ms" "ratio" "verdict"
while IFS=, read -r _ cid _ _ _ _ pass med _; do
  [[ "$cid" == "case_id" ]] && continue
  [[ -z "${a_ms[$cid]+x}" ]] && continue
  [[ "${a_pass[$cid]}" != "1" || "$pass" != "1" ]] && continue
  [[ -z "${a_ms[$cid]}" || -z "$med" ]] && continue
  common=$((common + 1))
  ratio="$(awk -v c="$med" -v b="${a_ms[$cid]}" 'BEGIN { printf "%.4f", c / b }')"
  ratios+=("$ratio")
  verdict="ok"
  if awk -v r="$ratio" 'BEGIN { exit !(r > 1.001) }'; then
    regressed=$((regressed + 1))
    verdict="slower"
  elif awk -v r="$ratio" 'BEGIN { exit !(r < 0.999) }'; then
    improved=$((improved + 1))
    verdict="faster"
  fi
  printf '%-40s %10s %10s %10s %s\n' "$cid" "${a_ms[$cid]}" "$med" "$ratio" "$verdict"
done <"$B_CSV"

median_ratio="N/A"
if [[ ${#ratios[@]} -gt 0 ]]; then
  median_ratio="$(printf '%s\n' "${ratios[@]}" | sort -n | awk '
    { a[NR]=$1 }
    END {
      if (NR % 2 == 1) printf "%.4f", a[(NR+1)/2]
      else printf "%.4f", (a[NR/2] + a[NR/2+1]) / 2
    }')"
fi

echo ""
echo "common_pass_cases=$common regressed=$regressed improved=$improved"
echo "median_ratio(candidate/baseline)=$median_ratio"
echo "gate stageA: median<=$STAGE_A_RATIO  stageB: median<=$STAGE_B_RATIO"

gate="fail"
if [[ "$median_ratio" != "N/A" ]]; then
  if awk -v m="$median_ratio" -v t="$STAGE_A_RATIO" 'BEGIN { exit !(m <= t) }'; then
    gate="stageA"
  elif awk -v m="$median_ratio" -v t="$STAGE_B_RATIO" 'BEGIN { exit !(m <= t) }'; then
    gate="stageB"
  fi
fi
echo "gate=$gate"
