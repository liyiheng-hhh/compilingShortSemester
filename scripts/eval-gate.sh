#!/usr/bin/env bash
# 竞赛式门禁：functional 硬门 + performance 软门 + 汇总
#
#   ./scripts/eval-gate.sh 20
set -euo pipefail

PERF_TIMEOUT_SEC="${1:-20}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EVAL_RUNTIME="$ROOT/scripts/eval-runtime.sh"
RUNTIME_ROOT="$ROOT/tests/.out/runtime"
mkdir -p "$RUNTIME_ROOT"

TS="$(date +%Y%m%d-%H%M%S)"
FUNC_CSV="$RUNTIME_ROOT/gate-functional-O1-${TS}.csv"
PERF_CSV="$RUNTIME_ROOT/gate-performance-O1-${TS}.csv"
GATE_LOG="$RUNTIME_ROOT/gate-${TS}.log"

{
  echo "=== functional O1 (hard, timeout=${RUNTIME_TIMEOUT_SEC:-10}s) ==="
  RUNTIME_SOFT_PERF=0 RUNTIME_TIMEOUT_SEC="${RUNTIME_TIMEOUT_SEC:-10}" \
    RUNTIME_CSV="$FUNC_CSV" "$EVAL_RUNTIME" functional O1
  echo ""
  echo "=== performance O1 (soft, timeout=${PERF_TIMEOUT_SEC}s) ==="
  RUNTIME_SOFT_PERF=1 RUNTIME_PERF_TIMEOUT_SEC="$PERF_TIMEOUT_SEC" \
    RUNTIME_CSV="$PERF_CSV" "$EVAL_RUNTIME" performance O1 || true
  echo ""
  echo "=== summary ==="
  "$ROOT/scripts/runtime-summary.sh" "$FUNC_CSV"
  echo ""
  "$ROOT/scripts/runtime-summary.sh" "$PERF_CSV"
} 2>&1 | tee "$GATE_LOG"

echo ""
echo "gate_log=$GATE_LOG"
echo "functional_csv=$FUNC_CSV"
echo "performance_csv=$PERF_CSV"
