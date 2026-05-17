#!/usr/bin/env bash
# 兼容入口 → runtime-summary.sh
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
exec "$ROOT/scripts/runtime-summary.sh" "${1:?usage: $0 path/to.csv}"
