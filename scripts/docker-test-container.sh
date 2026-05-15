#!/usr/bin/env bash
# Reusable Docker test environment for SysY RISC-V regression.
#
# Usage:
#   scripts/docker-test-container.sh init
#   scripts/docker-test-container.sh test /work/performance
#   USE_O1=1 scripts/docker-test-container.sh test /work/performance
#   SY_TEST_DIRS="/work/a /work/b" scripts/docker-test-container.sh test ""
#   scripts/docker-test-container.sh shell
#   scripts/docker-test-container.sh exec bash -lc 'riscv64-linux-gnu-gcc --version'

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME="${SYSY_DOCKER_NAME:-sysy-riscv-test}"
IMAGE="${SYSY_DOCKER_IMAGE:-ubuntu:24.04}"
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

install_tools() {
  if docker exec "$NAME" test -f "$READY"; then
    echo "container already ready: $NAME"
    return
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

case "${1:-init}" in
  init|ensure)
    ensure
    ;;
  test)
    shift
    run_test "${1:-}"
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
    echo "usage: $0 {init|test [dir]|exec cmd...|shell|start|stop|status}" >&2
    exit 2
    ;;
esac
