#!/usr/bin/env bash
# 统一运行时评测入口（仿照竞赛本地 eval-runtime 工作流，实现为本仓库自有脚本）
#
#   ./scripts/eval-runtime.sh <suite> <opt>
#
# suite:
#   functional          功能集（默认 10s 超时，失败则脚本 exit 1）
#   performance | perf  性能集（默认 20s，可用 RUNTIME_SOFT_PERF=1 软失败）
#   /path/to/cases      任意含 .sy 的目录
#
# opt: O0 | O1
#
# 示例：
#   make && make libsysy.a
#   ./scripts/eval-runtime.sh performance O1
#   RUNTIME_SOFT_PERF=1 ./scripts/eval-runtime.sh performance O1
#   ./scripts/runtime-summary.sh tests/.out/runtime
#   ./scripts/eval-compare-opt.sh performance 20
#   ./scripts/eval-vs-baseline.sh tests/.out/runtime/sysy-performance-O0.csv tests/.out/runtime/sysy-performance-O1.csv
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=opt-passes-on.sh
source "$ROOT/scripts/opt-passes-on.sh"
# shellcheck source=runtime_common.sh
source "$ROOT/scripts/runtime_common.sh"

if [[ $# -ne 2 ]]; then
  echo "usage: $0 <suite> <opt>" >&2
  echo "  suite: functional | performance | /path/to/dir" >&2
  echo "  opt:   O0 | O1" >&2
  exit 1
fi

SUITE="$1"
OPT="$2"

runtime_resolve_suite "$ROOT" "$SUITE"
runtime_opt_to_flags "$OPT"

LABEL="${RUNTIME_LABEL:-sysy}"
RUNTIME_ROOT="${RUNTIME_ROOT:-$ROOT/tests/.out/runtime}"
mkdir -p "$RUNTIME_ROOT"

if [[ -n "${RUNTIME_CSV:-}" ]]; then
  CSV_OUT="$RUNTIME_CSV"
else
  CSV_OUT="$(runtime_default_csv "$ROOT" "$LABEL" "$SUITE" "$OPT")"
fi

if [[ "$RUNTIME_SUITE_KIND" == "functional" ]]; then
  TIMEOUT_SEC="${RUNTIME_TIMEOUT_SEC:-10}"
  SOFT="${RUNTIME_SOFT_PERF:-0}"
else
  TIMEOUT_SEC="${RUNTIME_PERF_TIMEOUT_SEC:-20}"
  SOFT="${RUNTIME_SOFT_PERF:-1}"
fi

export RUNTIME_SUITE="$SUITE"
export RUNTIME_SUITE_KIND="$RUNTIME_SUITE_KIND"
export RUNTIME_OPT="$OPT"
export RUNTIME_LABEL="$LABEL"
export RUNTIME_CSV="$CSV_OUT"
export RUNTIME_SOFT_PERF="$SOFT"

exec env \
  PERF_LABEL="$LABEL" \
  PERF_CSV="$CSV_OUT" \
  USE_O1="$([[ "$OPT" == O1 ]] && echo 1 || echo 0)" \
  OPT_FLAGS="$RUNTIME_OPT_FLAGS" \
  TIMEOUT_SEC="$TIMEOUT_SEC" \
  RUNS="${RUNS:-3}" \
  WARMUP="${WARMUP:-1}" \
  CASE_FILTER="${RUNTIME_CASE_FILTER:-}" \
  CASE_LIMIT="${RUNTIME_CASE_LIMIT:-0}" \
  COMPILER="${COMPILER:-$ROOT/compiler}" \
  LIBSYSY="${LIBSYSY:-$ROOT/libsysy.a}" \
  "$ROOT/scripts/eval_perf.sh" "$RUNTIME_SUITE_DIR"
