#!/usr/bin/env bash
# 宿主机：确保常驻容器就绪，在容器内跑 local_eval（自动编 libsysy.a，无需宿主机 export LIBSYSY）。
#   ./scripts/docker_local_eval.sh
#   make docker-local-eval
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
exec "$ROOT/scripts/docker-test-container.sh" exec bash /work/scripts/docker_local_eval_in_container.sh
