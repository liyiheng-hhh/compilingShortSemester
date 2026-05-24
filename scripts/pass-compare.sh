#!/usr/bin/env bash
# Run dialect mid-end with IR interpreter compare after each pass (post-flatten).
# For large/slow cases (crypto-1), use scripts/pass-bisect-qemu.sh instead.
#
# Usage:
#   scripts/pass-compare.sh examples/smoke
#   SYSY_CC_COMPARE_SPARSE=1 scripts/pass-compare.sh examples/smoke
#   SYSY_CC_COMPARE_ONLY_PASS=gvn scripts/pass-compare.sh performance/crypto-1
#
# Env:
#   SYSY_CC_COMPARE_WITH      .out file (stdout + exit code line)
#   SYSY_CC_SIMULATE_INPUT    .in file
#   SYSY_CC_COMPARE_SPARSE    compare only major passes (faster)
#   SYSY_CC_COMPARE_ONLY_PASS compare after one pass only
#   SYSY_CC_STOP_AFTER_PASS   stop pipeline early
#   SYSY_CC_DUMP_PIPELINE     list passes
#   SYSY_CC_VERIFY_PASSES     run Verify after mem2reg

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CASE="${1:?usage: pass-compare.sh CASE (e.g. performance/crypto-1)}"
BASE="${CASE%.sy}"

export SYSY_CC_COMPARE_WITH="${SYSY_CC_COMPARE_WITH:-$ROOT/${BASE}.out}"
export SYSY_CC_SIMULATE_INPUT="${SYSY_CC_SIMULATE_INPUT:-$ROOT/${BASE}.in}"

cd "$ROOT"
make -q compiler 2>/dev/null || make compiler

echo "compare: $SYSY_CC_COMPARE_WITH"
echo "input:   $SYSY_CC_SIMULATE_INPUT"
echo

./compiler -S -O1 -o "/tmp/${BASE##*/}.s" "${BASE}.sy"
