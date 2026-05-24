#!/usr/bin/env bash
# Bisect dialect O1 pipeline by stopping after each pass and running under qemu.
# Faster than IR interpreter for large cases (e.g. crypto-1).
#
# Usage:
#   scripts/pass-bisect-qemu.sh performance/crypto-1
#   scripts/pass-bisect-qemu.sh performance/crypto-1 flatten-cfg gvn inline mem2reg
#
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CASE="${1:?usage: pass-bisect-qemu.sh CASE [pass ...]}"
shift || true
BASE="${CASE%.sy}"
SY="$ROOT/${BASE}.sy"
OUT="$ROOT/${BASE}.out"
IN="$ROOT/${BASE}.in"
LIB="${LIBSYSY:-$ROOT/libsysy.a}"
GCC="${RISCV_GCC:-riscv64-linux-gnu-gcc}"
QEMU="${QEMU:-qemu-riscv64-static}"
LINK_FLAGS="${LINK_FLAGS:--static -mcmodel=medany}"
TIMEOUT="${PASS_BISECT_TIMEOUT:-60}"

command -v "$GCC" >/dev/null || { echo "missing $GCC" >&2; exit 2; }
command -v "$QEMU" >/dev/null || QEMU=qemu-riscv64

EXP=$(head -n -1 "$OUT")
EXE=$(tail -1 "$OUT")

if [[ $# -gt 0 ]]; then
  PASSES=("$@")
else
  PASSES=(
    flatten-cfg gvn dce inline mem2reg alias dse dle
    licm late-inline aggressive-dce inst-schedule
  )
fi

cd "$ROOT"
make -q compiler 2>/dev/null || make compiler

run_stop() {
  local p="$1"
  if ! SYSY_CC_STOP_AFTER_PASS="$p" ./compiler -S -O1 -o /tmp/pb.s "$SY" 2>/tmp/pb.err; then
    echo "$p: COMPILE_FAIL $(head -1 /tmp/pb.err 2>/dev/null || true)"
    return
  fi
  if ! $GCC $LINK_FLAGS /tmp/pb.s "$LIB" -o /tmp/pb -lm 2>/tmp/pb.link; then
    echo "$p: LINK_FAIL"
    return
  fi
  local act ec got
  act=$(timeout "$TIMEOUT" "$QEMU" /tmp/pb < "$IN" 2>/dev/null; echo $?)
  got=$(echo "$act" | head -n -1)
  ec=$(echo "$act" | tail -1)
  if [[ "$got" == "$EXP" && "$ec" == "$EXE" ]]; then
    echo "$p: OK"
  else
    echo "$p: MISMATCH"
    echo "  expected: $EXP (exit $EXE)"
    echo "  actual:   $got (exit $ec)"
  fi
}

echo "case: $BASE"
echo "expected exit: $EXE"
echo

for p in "${PASSES[@]}"; do
  run_stop "$p"
done
