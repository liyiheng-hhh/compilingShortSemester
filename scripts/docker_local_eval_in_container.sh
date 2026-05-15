#!/usr/bin/env bash
# 在 Docker 容器内由 docker_local_eval.sh 调用：编 /work/compiler、按需编 libsysy.a，再跑 run_local_eval。
set -euo pipefail
cd /work

command -v riscv64-linux-gnu-gcc >/dev/null 2>&1 || {
  echo "error: riscv64-linux-gnu-gcc missing in container (run: make docker-init)" >&2
  exit 2
}
command -v riscv64-linux-gnu-ar >/dev/null 2>&1 || {
  echo "error: riscv64-linux-gnu-ar missing in container" >&2
  exit 2
}

make -j4

RT=/tmp/sysy-runtime-lib
LIB="$RT/build/libsysy.a"
if [[ ! -f "$LIB" ]]; then
  echo "=== docker-local-eval: build libsysy.a -> $LIB ==="
  mkdir -p "$RT/build/obj"
  if [[ -f /work/runtime/sysy_runtime.c ]]; then
    riscv64-linux-gnu-gcc -Wall -O3 -c /work/runtime/sysy_runtime.c -o "$RT/build/obj/sysy.o"
  else
    echo "error: missing /work/runtime/sysy_runtime.c" >&2
    exit 2
  fi
  riscv64-linux-gnu-ar rc "$LIB" "$RT/build/obj/sysy.o"
  riscv64-linux-gnu-ranlib "$LIB"
fi

export LIBSYSY="$LIB"
exec bash /work/scripts/run_local_eval.sh
