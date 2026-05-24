#!/usr/bin/env bash
# 快速冒烟测试：在 Docker 中编译、链接、运行
#
# 用法：
#   ./scripts/run-smoke-docker.sh [test.sy]
#   ./scripts/run-smoke-docker.sh performance/01_mm1.sy

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
COMPILER="${ROOT}/compiler"
RUNTIME_C="${ROOT}/runtime/sysy_runtime.c"

default_tests=(
  "${ROOT}/performance/01_mm1.sy"
  "${ROOT}/performance/01_mm2.sy"
  "${ROOT}/performance/01_mm3.sy"
)

if [ $# -eq 0 ]; then
  tests=("${default_tests[@]}")
else
  tests=("$@")
fi

if ! command -v docker &>/dev/null; then
  echo "error: docker not found" >&2
  exit 1
fi

IMAGE="ubuntu:22.04"
CONTAINER_NAME="sysy-smoke-$(date +%s)"

echo "=== Smoke Test (Docker) ==="
echo "Compiler: ${COMPILER}"
echo "Runtime:  ${RUNTIME_C}"
echo ""

if [ ! -f "${COMPILER}" ]; then
  echo "Building compiler..."
  make -C "${ROOT}" compiler
fi

if [ ! -f "${RUNTIME_C}" ]; then
  echo "error: runtime not found: ${RUNTIME_C}" >&2
  exit 1
fi

passed=0
failed=0

for test_file in "${tests[@]}"; do
  if [ ! -f "${test_file}" ]; then
    echo "skip: ${test_file} not found"
    continue
  fi

  base=$(basename "${test_file}" .sy)
  in_file="${test_file%.sy}.in"
  out_file="${test_file%.sy}.out"

  echo -n "Testing ${base}... "

  if ! "${COMPILER}" -S -O1 -o "/tmp/${base}.s" "${test_file}" 2>/dev/null; then
    echo "COMPILE_FAIL"
    failed=$((failed + 1))
    continue
  fi

  result=$(docker run --rm \
    -v "${ROOT}:${ROOT}" \
    -v "/tmp:/tmp" \
    -w "/tmp" \
    "${IMAGE}" \
    bash -c "
      set -e
      export DEBIAN_FRONTEND=noninteractive
      apt-get update -qq >/dev/null 2>&1
      apt-get install -y -qq gcc-riscv64-linux-gnu qemu-user-static libc6-dev-riscv64-cross >/dev/null 2>&1

      riscv64-linux-gnu-gcc -static -mcmodel=medany \
        '/tmp/${base}.s' '${RUNTIME_C}' -lm -o '/tmp/${base}' 2>/dev/null

      if [ -f '${in_file}' ]; then
        timeout 30 qemu-riscv64-static '/tmp/${base}' <'${in_file}' 2>/dev/null
      else
        timeout 30 qemu-riscv64-static '/tmp/${base}' 2>/dev/null
      fi
    " 2>/dev/null || true)

  if [ -f "${out_file}" ]; then
    expected=$(head -1 "${out_file}")
    if [ "${result}" = "${expected}" ]; then
      echo "AC"
      passed=$((passed + 1))
    else
      echo "WA (got: ${result}, expected: ${expected})"
      failed=$((failed + 1))
    fi
  else
    echo "OK (no .out to compare)"
    passed=$((passed + 1))
  fi
done

echo ""
echo "=== Summary ==="
echo "Passed: ${passed}"
echo "Failed: ${failed}"

if [ ${failed} -eq 0 ]; then
  echo "All tests passed!"
  exit 0
else
  exit 1
fi
