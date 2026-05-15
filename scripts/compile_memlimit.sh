#!/usr/bin/env bash
# 在虚拟地址上限下运行 compiler，模拟评测机「编译阶段 Killed / OOM」。
#
#   ./scripts/compile_memlimit.sh 512M ./compiler -S -O1 -o /tmp/x.s path/to/large.sy
#   ./scripts/compile_memlimit.sh 256000 ./compiler -S -o /tmp/x.s foo.sy   # 约 256000 KiB（依系统 ulimit 语义）
#
# 优先使用 prlimit（精确）；否则尝试 ulimit -Sv（部分环境为软限制）。

set -euo pipefail
if [[ $# -lt 3 ]]; then
  echo "usage: $0 <LIMIT> <compiler> [compiler-options...]" >&2
  echo "  example: $0 512M ./compiler -S -O1 -o /tmp/x.s path/to/large.sy" >&2
  echo "  LIMIT examples: 512M, 1G, 524288 (KiB for ulimit -Sv on some systems)" >&2
  exit 2
fi
LIMIT="$1"
shift

if command -v prlimit >/dev/null 2>&1; then
  BYTES=""
  if [[ "$LIMIT" =~ ^[0-9]+[MmGg]$ ]]; then
    num="${LIMIT%[MmGg]}"
    suf="${LIMIT: -1}"
    case "$suf" in
      M|m) BYTES=$((num * 1024 * 1024)) ;;
      G|g) BYTES=$((num * 1024 * 1024 * 1024)) ;;
    esac
  elif [[ "$LIMIT" =~ ^[0-9]+$ ]]; then
    BYTES="$LIMIT"
  fi
  if [[ -n "$BYTES" ]]; then
    exec prlimit --as="$BYTES" "$@"
  fi
fi

# fallback: ulimit -Sv 单位为 KiB（常见）；若 LIMIT 带 M/G 则换算
if [[ "$LIMIT" =~ ^[0-9]+[MmGg]$ ]]; then
  num="${LIMIT%[MmGg]}"
  suf="${LIMIT: -1}"
  kb=$num
  case "$suf" in
    M|m) kb=$((num * 1024)) ;;
    G|g) kb=$((num * 1024 * 1024)) ;;
  esac
  ulimit -Sv "$kb"
elif [[ "$LIMIT" =~ ^[0-9]+$ ]]; then
  ulimit -Sv "$LIMIT"
else
  echo "error: bad LIMIT: $LIMIT" >&2
  exit 2
fi
exec "$@"
