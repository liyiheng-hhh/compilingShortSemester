#!/usr/bin/env bash
# 热点子集评测：对 performance 目录按名称过滤分批跑 eval-runtime，便于改 pass 后定点验证
#
#   ./scripts/eval-hotspots.sh O1 20
#   ./scripts/eval-hotspots.sh O1 20 matmul
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EVAL_RUNTIME="$ROOT/scripts/eval-runtime.sh"
OPT="${1:-O1}"
PERF_TIMEOUT_SEC="${2:-20}"
EXTRA_FILTER="${3:-}"

if [[ "$OPT" != "O0" && "$OPT" != "O1" ]]; then
  echo "usage: $0 <O0|O1> [perf_timeout_sec] [extra_case_filter]" >&2
  exit 1
fi

OUT_DIR="${ROOT}/tests/.out/runtime/hotspots"
mkdir -p "$OUT_DIR"
TS="$(date +%Y%m%d-%H%M%S)"
SUMMARY="$OUT_DIR/summary-${OPT}-${TS}.txt"
: >"$SUMMARY"

run_group() {
  local name="$1"
  local filter="$2"
  if [[ -n "$EXTRA_FILTER" ]]; then
    filter="${filter}${EXTRA_FILTER}"
  fi
  local csv="$OUT_DIR/performance-${OPT}-${name}-${TS}.csv"

  echo "[hotspot] filter='$filter' -> $csv"
  RUNTIME_SOFT_PERF=1 \
    RUNTIME_PERF_TIMEOUT_SEC="$PERF_TIMEOUT_SEC" \
    RUNTIME_CASE_FILTER="$filter" \
    RUNTIME_CSV="$csv" \
    "$EVAL_RUNTIME" performance "$OPT" >>"$SUMMARY" 2>&1 || true

  {
    echo "=== $name (filter=$filter) ==="
    "$ROOT/scripts/runtime-summary.sh" "$csv" 2>/dev/null || cat "$csv"
    echo ""
  } | tee -a "$SUMMARY"
}

# 常见性能热点族（可按你本地 performance/ 用例名调整）
run_group "matmul" "matmul"
run_group "conv2d" "conv2d"
run_group "transpose" "transpose"
run_group "fft" "fft"
run_group "sort" "sort"
run_group "crypto" "crypto"
run_group "crc" "crc"
run_group "huffman" "huffman"
run_group "knapsack" "knapsack"

echo "summary: $SUMMARY"
