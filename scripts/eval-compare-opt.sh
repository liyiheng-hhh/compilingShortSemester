#!/usr/bin/env bash
# O0 vs O1 对比（对应 Sisyphus 的 eval-o2-aggressive 思路；本项目无 O2）
#
#   ./scripts/eval-compare-opt.sh performance 20
#   AGGR_MEDIAN_TOL_RATIO=0.02 ./scripts/eval-compare-opt.sh functional 10
set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 ]]; then
  echo "usage: $0 <suite> [perf_timeout_sec]" >&2
  exit 1
fi

SUITE="$1"
PERF_TIMEOUT_SEC="${2:-20}"
AGGR_MEDIAN_TOL_RATIO="${AGGR_MEDIAN_TOL_RATIO:-0.01}"
AGGR_MEDIAN_TOL_MS="${AGGR_MEDIAN_TOL_MS:-0.05}"

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EVAL_RUNTIME="$ROOT/scripts/eval-runtime.sh"
OUT_DIR="$ROOT/tests/.out/runtime/compare"
mkdir -p "$OUT_DIR"

TS="$(date +%Y%m%d-%H%M%S)"
O0_CSV="$OUT_DIR/${SUITE}-O0-${TS}.csv"
O1_CSV="$OUT_DIR/${SUITE}-O1-${TS}.csv"
REGRESSED="$OUT_DIR/${SUITE}-regressed-${TS}.csv"
IMPROVED="$OUT_DIR/${SUITE}-improved-${TS}.csv"
REPORT="$OUT_DIR/${SUITE}-report-${TS}.txt"

echo "[compare] suite=$SUITE O0 then O1 (perf_timeout=${PERF_TIMEOUT_SEC}s)"

RUNTIME_SOFT_PERF=1 RUNTIME_PERF_TIMEOUT_SEC="$PERF_TIMEOUT_SEC" \
  RUNTIME_CSV="$O0_CSV" "$EVAL_RUNTIME" "$SUITE" O0

RUNTIME_SOFT_PERF=1 RUNTIME_PERF_TIMEOUT_SEC="$PERF_TIMEOUT_SEC" \
  RUNTIME_CSV="$O1_CSV" "$EVAL_RUNTIME" "$SUITE" O1

calc_pass_rate() {
  awk -F, 'NR > 1 { t++; if ($7 == "1") p++ } END { if (t==0) print 0; else printf "%.4f", p/t }' "$1"
}

calc_quantile_ms() {
  local csv="$1" q="$2"
  awk -F, 'NR > 1 && $7 == "1" && $8 != "" { print $8 }' "$csv" | sort -n | awk -v q="$q" '
    { a[++n] = $1 }
    END {
      if (n == 0) { print "0"; exit }
      idx = int((n - 1) * q + 0.999999) + 1
      if (idx < 1) idx = 1
      if (idx > n) idx = n
      print a[idx]
    }'
}

O0_PASS="$(calc_pass_rate "$O0_CSV")"
O1_PASS="$(calc_pass_rate "$O1_CSV")"
O0_MED="$(calc_quantile_ms "$O0_CSV" 0.5)"
O1_MED="$(calc_quantile_ms "$O1_CSV" 0.5)"
O0_P90="$(calc_quantile_ms "$O0_CSV" 0.9)"
O1_P90="$(calc_quantile_ms "$O1_CSV" 0.9)"

ratio="N/A"
if awk -v o0="$O0_MED" 'BEGIN { exit !(o0 > 0) }'; then
  ratio="$(awk -v o1="$O1_MED" -v o0="$O0_MED" 'BEGIN { printf "%.4f", o1 / o0 }')"
fi

printf 'case_id,o0_pass,o1_pass,o0_median_ms,o1_median_ms,delta_ms,ratio,verdict\n' >"$REGRESSED"
printf 'case_id,o0_pass,o1_pass,o0_median_ms,o1_median_ms,delta_ms,ratio,verdict\n' >"$IMPROVED"

regressed=0
improved=0

while IFS=, read -r _ c0 _ _ _ _ p0 m0 _; do
  [[ "$c0" == "case_id" ]] && continue
  line_o1=$(awk -F, -v id="$c0" 'NR > 1 && $2 == id { print; exit }' "$O1_CSV") || true
  [[ -z "$line_o1" ]] && continue
  IFS=, read -r _ _ _ _ _ _ p1 m1 _ <<<"$line_o1"

  [[ "$p0" != "1" || "$p1" != "1" || -z "$m0" || -z "$m1" ]] && continue
  delta="$(awk -v a="$m1" -v b="$m0" 'BEGIN { printf "%.3f", a - b }')"
  r="$(awk -v a="$m1" -v b="$m0" 'BEGIN { if (b <= 0) print ""; else printf "%.4f", a / b }')"

  tol_ok=0
  if awk -v d="$delta" -v t="$AGGR_MEDIAN_TOL_MS" 'BEGIN { exit !(d <= t) }'; then
    tol_ok=1
  fi
  if [[ "$tol_ok" -eq 0 && -n "$r" ]]; then
    maxr="$(awk -v t="$AGGR_MEDIAN_TOL_RATIO" 'BEGIN { printf "%.6f", 1 + t }')"
    awk -v r="$r" -v m="$maxr" 'BEGIN { exit !(r <= m) }' && tol_ok=1
  fi

  row="$c0,$p0,$p1,$m0,$m1,$delta,$r,"
  if [[ "$tol_ok" -eq 0 ]] && awk -v d="$delta" 'BEGIN { exit !(d > 0) }'; then
    regressed=$((regressed + 1))
    echo "${row}regressed" >>"$REGRESSED"
  elif awk -v d="$delta" 'BEGIN { exit !(d < 0) }'; then
    improved=$((improved + 1))
    echo "${row}improved" >>"$IMPROVED"
  fi
done <"$O0_CSV"

{
  echo "suite=$SUITE ts=$TS"
  echo "O0: csv=$O0_CSV pass_rate=$O0_PASS median_ms=$O0_MED p90_ms=$O0_P90"
  echo "O1: csv=$O1_CSV pass_rate=$O1_PASS median_ms=$O1_MED p90_ms=$O1_P90"
  echo "median_ratio(O1/O0)=$ratio"
  echo "regressed=$regressed improved=$improved"
  echo "tolerance: ratio<=${AGGR_MEDIAN_TOL_RATIO} or delta_ms<=${AGGR_MEDIAN_TOL_MS}"
  echo ""
  echo "top regressed:"
  awk -F, 'NR > 1 { print $6, $1 }' "$REGRESSED" | sort -rn | head -15
  echo ""
  echo "top improved:"
  awk -F, 'NR > 1 { print $6, $1 }' "$IMPROVED" | sort -n | head -15
} | tee "$REPORT"

echo "report=$REPORT"
