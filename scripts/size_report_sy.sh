#!/usr/bin/env bash
# 对每个 *.sy 分别生成 O0 / O1 汇编到临时文件，输出 wc -l 及差值（量化优化体积）
# 用法:
#   ./scripts/size_report_sy.sh "examples path/to/functional"
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
COMPILER="${COMPILER:-$ROOT/compiler}"
if [[ ! -x "$COMPILER" ]]; then
  echo "error: run make first" >&2
  exit 2
fi
DIRS="${1:-examples}"
tmp=$(mktemp -d "${TMPDIR:-/tmp}/sizereport.XXXXXX")
trap 'rm -rf "$tmp"' EXIT

echo "# file	o0_lines	o1_lines	delta"
for d in $DIRS; do
  [[ -d "$d" ]] || continue
  while IFS= read -r -d '' f; do
    b=$(basename "$f" .sy)
    s0="$tmp/${b}_O0.s"
    s1="$tmp/${b}_O1.s"
    "$COMPILER" -S -o "$s0" "$f"
    "$COMPILER" -S -O1 -o "$s1" "$f"
    n0=$(wc -l <"$s0" | tr -d ' ')
    n1=$(wc -l <"$s1" | tr -d ' ')
    echo -e "$f\t$n0\t$n1\t$((n0 - n1))"
  done < <(find "$d" -name '*.sy' -print0)
done | sort -t$'\t' -k4 -nr
