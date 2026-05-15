#!/usr/bin/env bash
# 单条性能 .sy：-O1 汇编行数 + 可选 qemu + /usr/bin/time 计时（对齐 README Profiling）
#
#   make
#   export LIBSYSY=/path/to/libsysy.a
#   chmod +x scripts/profile_sy.sh   # 首次
#   ./scripts/profile_sy.sh performance/matmul1.sy
#
#   LINK_FLAGS="-static -mcmodel=medany" QEMU=qemu-riscv64 ./scripts/profile_sy.sh performance/conv2d-1.sy
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
COMPILER="${COMPILER:-$ROOT/compiler}"
SY="${1:-}"
if [[ -z "$SY" || ! -f "$SY" ]]; then
  echo "usage: $0 path/to/foo.sy [extra compiler flags...]" >&2
  exit 2
fi
shift || true
EXTRA_CC=("$@")

if [[ ! -x "$COMPILER" ]]; then
  echo "error: run make first; missing $COMPILER" >&2
  exit 2
fi

RISCV_GCC="${RISCV_GCC:-riscv64-linux-gnu-gcc}"
QEMU="${QEMU:-qemu-riscv64-static}"
command -v "$RISCV_GCC" >/dev/null 2>&1 || { echo "error: missing $RISCV_GCC" >&2; exit 2; }
if ! command -v "$QEMU" >/dev/null 2>&1; then
  QEMU=qemu-riscv64
fi
command -v "$QEMU" >/dev/null 2>&1 || { echo "error: missing qemu-riscv64 (or qemu-riscv64-static)" >&2; exit 2; }

LINK_FLAGS="${LINK_FLAGS:--static -mcmodel=medany}"
tmp=$(mktemp -d "${TMPDIR:-/tmp}/profilesy.XXXXXX")
trap 'rm -rf "$tmp"' EXIT

base=$(basename "$SY" .sy)
s="$tmp/$base.s"
elf="$tmp/$base.bin"

echo "compile -O1: $SY ${EXTRA_CC[*]:+${EXTRA_CC[*]}}"
"$COMPILER" -S -O1 -o "$s" "$SY" "${EXTRA_CC[@]}"
lines=$(wc -l <"$s" | tr -d ' ')
echo "asm_lines=$lines"

if [[ -n "${LIBSYSY:-}" && -f "$LIBSYSY" ]]; then
  $RISCV_GCC $LINK_FLAGS "$s" "$LIBSYSY" -o "$elf" -lm || {
    echo "error: link failed" >&2
    exit 1
  }
  stdin=/dev/null
  idir=$(dirname "$SY")
  if [[ -f "$idir/$base.in" ]]; then
    stdin="$idir/$base.in"
    echo "stdin=$stdin"
  fi
  echo "qemu + time:"
  if command -v /usr/bin/time >/dev/null 2>&1; then
    /usr/bin/time -p "$QEMU" "$elf" <"$stdin" >/dev/null
  elif command -v time >/dev/null 2>&1; then
    time -p "$QEMU" "$elf" <"$stdin" >/dev/null
  else
    echo "hint: install GNU time (/usr/bin/time) for elapsed wall-clock" >&2
    "$QEMU" "$elf" <"$stdin" >/dev/null
  fi
else
  echo "skip_link: set LIBSYSY to libsysy.a for timed qemu run"
fi
