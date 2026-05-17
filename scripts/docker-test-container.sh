#!/usr/bin/env bash
# Reusable Docker test environment for SysY RISC-V regression.
#
#   SYSY_DOCKER_IMAGE=ubuntu:22.04 scripts/docker-test-container.sh init   # 默认已是 22.04；可改为 24.04 等
#   scripts/docker-test-container.sh test /work/performance
#   USE_O1=1 scripts/docker-test-container.sh test /work/performance
#   scripts/docker-test-container.sh perf performance O1
#   scripts/docker-test-container.sh gate 20
#   SY_TEST_DIRS="/work/a /work/b" scripts/docker-test-container.sh test ""
#   scripts/docker-test-container.sh shell
#   scripts/docker-test-container.sh exec bash -lc 'riscv64-linux-gnu-gcc --version'

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME="${SYSY_DOCKER_NAME:-sysy-riscv-test}"
IMAGE="${SYSY_DOCKER_IMAGE:-ubuntu:22.04}"
READY="/opt/sysy-test-env.ready"
APT_MIRROR_HTTP="${APT_MIRROR_HTTP:-http://mirrors.tuna.tsinghua.edu.cn}"
APT_RETRIES="${APT_RETRIES:-5}"

docker_exists() {
  docker inspect "$NAME" >/dev/null 2>&1
}

docker_running() {
  [[ "$(docker inspect -f '{{.State.Running}}' "$NAME" 2>/dev/null || true)" == "true" ]]
}

create_container() {
  if docker_exists; then
    return
  fi
  docker run -d \
    --name "$NAME" \
    -v "$ROOT:/work" \
    -v /private/tmp:/private/tmp \
    -w /work \
    "$IMAGE" \
    sleep infinity >/dev/null
}

start_container() {
  if ! docker_running; then
    docker start "$NAME" >/dev/null
  fi
}

container_toolchain_ok() {
  docker exec "$NAME" bash -lc \
    'command -v riscv64-linux-gnu-gcc >/dev/null 2>&1 && command -v qemu-riscv64-static >/dev/null 2>&1'
}

install_tools() {
  if docker exec "$NAME" test -f "$READY" 2>/dev/null && container_toolchain_ok; then
    echo "container already ready: $NAME"
    return
  fi
  if docker exec "$NAME" test -f "$READY" 2>/dev/null; then
    echo "warn: $READY exists but toolchain missing; reinstalling packages..." >&2
    docker exec "$NAME" rm -f "$READY" 2>/dev/null || true
  fi
  docker exec \
    -e INSTALL_ONLY=1 \
    -e APT_MIRROR_HTTP="$APT_MIRROR_HTTP" \
    -e APT_RETRIES="$APT_RETRIES" \
    "$NAME" \
    bash /work/scripts/e2e-docker.sh
  docker exec "$NAME" mkdir -p "$(dirname "$READY")"
  docker exec "$NAME" touch "$READY"
  echo "container ready: $NAME"
}

ensure() {
  create_container
  start_container
  install_tools
}

run_test() {
  local test_dir="${1:-}"
  shift || true
  ensure
  local exec_env=(
    -w /work
    -e SKIP_APT=1
    -e USE_O1="${USE_O1:-0}"
    -e LINK_FLAGS="${LINK_FLAGS:--static -mcmodel=medany}"
  )
  if [[ -n "$test_dir" ]]; then
    exec_env+=(-e "SY_TEST_DIR=$test_dir")
  fi
  if [[ -n "${SY_TEST_DIRS:-}" ]]; then
    exec_env+=(-e "SY_TEST_DIRS=$SY_TEST_DIRS")
  fi
  docker exec "${exec_env[@]}" "$NAME" bash /work/scripts/e2e-docker.sh "$@"
}

run_perf() {
  local suite="${1:-performance}"
  local opt="${2:-O1}"
  shift 2 || true
  ensure
  docker exec -w /work "$NAME" bash -lc \
    "make -s compiler libsysy.a && chmod +x scripts/*.sh && ./scripts/eval-runtime.sh '$suite' '$opt' $*"
}

run_gate() {
  local perf_timeout="${1:-20}"
  ensure
  docker exec -w /work "$NAME" bash -lc \
    "make -s compiler libsysy.a && chmod +x scripts/*.sh && ./scripts/eval-gate.sh '$perf_timeout'"
}

case "${1:-init}" in
  init|ensure)
    ensure
    ;;
  test)
    shift
    run_test "${1:-}"
    ;;
  perf)
    shift
    run_perf "${1:-performance}" "${2:-O1}" "${@:3}"
    ;;
  gate)
    shift
    run_gate "${1:-20}"
    ;;
  exec)
    shift
    ensure
    docker exec -w /work "$NAME" "$@"
    ;;
  shell)
    ensure
    docker exec -it -w /work "$NAME" bash
    ;;
  start)
    create_container
    start_container
    ;;
  stop)
    docker stop "$NAME" >/dev/null
    ;;
  status)
    if docker_exists; then
      docker inspect -f 'name={{.Name}} running={{.State.Running}} image={{.Config.Image}}' "$NAME"
    else
      echo "container missing: $NAME"
    fi
    ;;
  *)
    echo "usage: $0 {init|test [dir]|perf [suite] [O0|O1]|gate [perf_sec]|exec cmd...|shell|start|stop|status}" >&2
    exit 2
    ;;
esac
