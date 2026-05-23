#!/usr/bin/env bash
# 逐函数对比 baseline vs RV IR 汇编（定位 Call/栈传参/除法等差异）
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
COMPILER="${COMPILER:-$ROOT/compiler}"
OPT_FLAGS="${OPT_FLAGS:--O1}"
SY="${1:-$ROOT/performance/crc1.sy}"
FUNCS="${2:-}"

tmp="${TMPDIR:-/tmp}/rv-func-diff.$$"
mkdir -p "$tmp"
trap 'rm -rf "$tmp"' EXIT

base="$tmp/base.s"
rv="$tmp/rv.s"
"$COMPILER" -S $OPT_FLAGS -o "$base" "$SY"
env SYSY_CC_ENABLE_RV_IR_BACKEND=1 "$COMPILER" -S $OPT_FLAGS -o "$rv" "$SY"

extract_fn() {
  local fn="$1" path="$2" out="$3"
  python3 - "$fn" "$path" "$out" <<'PY'
import re, sys
fn, path, out = sys.argv[1:4]
lines = open(path).read().splitlines()
i = next((i for i, l in enumerate(lines) if re.match(rf'^{re.escape(fn)}:\s*$', l)), None)
if i is None:
    open(out, 'w').write(f'# function {fn} not found\n')
    sys.exit(0)
body = []
for l in lines[i:]:
    body.append(l)
    if l.startswith('.size\t') and len(body) > 3:
        break
open(out, 'w').write('\n'.join(body) + '\n')
PY
}

if [[ -z "$FUNCS" ]]; then
  FUNCS="$(python3 - "$base" <<'PY'
import re, sys
for l in open(sys.argv[1]):
    m = re.match(r'^([A-Za-z_][A-Za-z0-9_]*):\s*$', l)
    if m and not m.group(1).startswith('.L'):
        print(m.group(1))
PY
)"
fi

printf 'sy=%s\n' "$SY"
for fn in $FUNCS; do
  extract_fn "$fn" "$base" "$tmp/${fn}.base"
  extract_fn "$fn" "$rv" "$tmp/${fn}.rv"
  if diff -q "$tmp/${fn}.base" "$tmp/${fn}.rv" >/dev/null 2>&1; then
    printf '  %-24s same\n' "$fn"
  else
    printf '  %-24s DIFF (%d / %d lines)\n' "$fn" \
      "$(wc -l <"$tmp/${fn}.base")" "$(wc -l <"$tmp/${fn}.rv")"
    diff -u "$tmp/${fn}.base" "$tmp/${fn}.rv" | head -80 || true
    printf '  ---\n'
  fi
done
