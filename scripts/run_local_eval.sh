#!/usr/bin/env bash
# 生成分支/除法属性用例，并对 local_eval_cases 跑 O0、O1 批测（需 LIBSYSY）。
#
#   export LIBSYSY=/path/to/libsysy.a
#   ./scripts/run_local_eval.sh
#
# Docker 内请在仓库根（挂载为 /work）执行 `make`，使 ./compiler 与容器 glibc 一致；否则可 export COMPILER=...

set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

COMPILER="${COMPILER:-}"
if [[ -z "$COMPILER" || ! -x "$COMPILER" ]]; then
  if [[ -x "$ROOT/compiler" ]]; then
    COMPILER="$ROOT/compiler"
  elif [[ -x "/tmp/compiler-build/compiler" ]]; then
    COMPILER="/tmp/compiler-build/compiler"
  else
    COMPILER="$ROOT/compiler"
  fi
fi
export COMPILER

if [[ -z "${LIBSYSY:-}" || ! -f "$LIBSYSY" ]]; then
  echo "error: export LIBSYSY=/path/to/libsysy.a" >&2
  exit 2
fi

PY=""
if command -v python3 >/dev/null 2>&1; then
  PY=python3
elif command -v python >/dev/null 2>&1; then
  PY=python
fi
if [[ -n "$PY" ]]; then
  "$PY" tools/gen_divmod_property_sy.py local_eval_cases/_gen_divmod.sy
  "$PY" - "$ROOT/local_eval_cases/_gen_divmod.out" <<'PY'
import sys, pathlib
path = pathlib.Path(sys.argv[1])

def int32(x):
    x &= 0xFFFFFFFF
    if x >= 0x80000000:
        x -= 0x100000000
    return x

def trunc_div(n, d):
    n, d = int32(n), int32(d)
    if d == 0:
        return 0
    neg = (n < 0) ^ (d < 0)
    an, ad = abs(n), abs(d)
    q = an // ad
    q = q if not neg else -q
    return int32(q)

def trunc_mod(n, d):
    return int32(n - trunc_div(n, d) * int32(d))

ns = [0, 1, -1, 42, -42, 100, -100, 2147483647, -2147483647, -2147483648, -2147483647 - 1]
ds = [1, 2, -2, 3, 7, -7, 13, -13, 1024, -1024, 4, -4]
lines = []
for n in ns:
    for d in ds:
        lines.append(f"{n} {d} {trunc_div(n, d)} {trunc_mod(n, d)}\n")
path.write_text("".join(lines), encoding="utf-8")
PY
else
  echo "warn: no python3 in PATH; skip regenerating _gen_divmod.sy / .out (use committed files)" >&2
fi

echo "=== local_eval_cases USE_O1=0 ==="
USE_O1=0 LIBSYSY="$LIBSYSY" ./scripts/run_sy_tests.sh "$ROOT/local_eval_cases"
echo "=== local_eval_cases USE_O1=1 ==="
USE_O1=1 LIBSYSY="$LIBSYSY" ./scripts/run_sy_tests.sh "$ROOT/local_eval_cases"

echo "=== O0 vs O1 stdout diff (per .sy except _gen_divmod) ==="
for sy in "$ROOT/local_eval_cases"/*.sy; do
  [[ "$(basename "$sy")" == _gen_divmod.sy ]] && continue
  echo "-- $(basename "$sy") --"
  COMPILER="${COMPILER:-$ROOT/compiler}" LIBSYSY="$LIBSYSY" \
    "$ROOT/scripts/cmp_o0_o1.sh" "$sy" || exit 1
done
echo "OK: all cmp_o0_o1 checks passed"
