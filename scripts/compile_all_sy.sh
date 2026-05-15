#!/usr/bin/env bash
# 递归将目录下所有 *.sy 编译为同名 .s（用于无 qemu 时的全量语法/后端冒烟）
# 用法（在项目根）:
#   ./scripts/compile_all_sy.sh "examples path/to/functional"
#   OLEVEL=-O1 ./scripts/compile_all_sy.sh "path/to/performance"
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
COMPILER="${COMPILER:-$ROOT/compiler}"
if [[ ! -x "$COMPILER" ]]; then
  echo "error: run make first, missing $COMPILER" >&2
  exit 2
fi

DIRS="${1:-examples}"
OLEVEL="${OLEVEL:-}" # 空为默认（O0），或 -O1

failed=0
total=0
for d in $DIRS; do
  [[ -d "$d" ]] || { echo "skip (not a dir): $d" >&2; continue; }
  while IFS= read -r -d '' f; do
    total=$((total + 1))
    s="${f%.sy}.s"
    if [[ -n "$OLEVEL" ]]; then
      if ! "$COMPILER" -S $OLEVEL -o "$s" "$f" 2>/tmp/ce_compile_all.log; then
        echo "CE  $f" >&2
        sed 's/^/    /' /tmp/ce_compile_all.log >&2 || true
        failed=$((failed + 1))
      fi
    else
      if ! "$COMPILER" -S -o "$s" "$f" 2>/tmp/ce_compile_all.log; then
        echo "CE  $f" >&2
        sed 's/^/    /' /tmp/ce_compile_all.log >&2 || true
        failed=$((failed + 1))
      fi
    fi
  done < <(find "$d" -name '*.sy' -print0)
done

echo "compile-all: total=$total failed=$failed"
[[ "$failed" -eq 0 ]]
