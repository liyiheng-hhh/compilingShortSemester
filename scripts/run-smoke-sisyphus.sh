#!/usr/bin/env bash
# Sisyphus 风格的快速冒烟测试
# 直接在 Docker 中编译、链接、运行
#
# 用法：
#   ./scripts/run-smoke-sisyphus.sh [test.sy]
#   ./scripts/run-smoke-sisyphus.sh performance/01_mm1.sy

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
COMPILER="${ROOT}/compiler"
RUNTIME_C="${ROOT}/runtime/sysy_runtime.c"

default_tests=(
  "${ROOT}/performance/01_mm1.sy"
  "${ROOT}/performance/01_mm2.sy"
  "${ROOT}/performance/01_mm3.sy"
)

# 如果没有参数，使用默认测试
if [ $# -eq 0 ]; then
  tests=("${default_tests[@]}")
else
  tests=("$@")
fi

# 检查 Docker 可用
if ! command -v docker &>/dev/null; then
  echo "error: docker not found" >&2
  exit 1
fi

# Docker 镜像
IMAGE="ubuntu:22.04"
CONTAINER_NAME="sisyphus-test-$(date +%s)"

echo "=== Sisyphus Smoke Test ==="
echo "Compiler: ${COMPILER}"
echo "Runtime:  ${RUNTIME_C}"
echo ""

# 确保 compiler 已构建
if [ ! -f "${COMPILER}" ]; then
  echo "Building compiler..."
  make -C "${ROOT}" compiler
fi

# 确保 runtime 存在
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

  # 编译
  if ! "${COMPILER}" -S -O1 -o "/tmp/${base}.s" "${test_file}" 2>/dev/null; then
    echo "COMPILE_FAIL"
    failed=$((failed + 1))
    continue
  fi

  # 在 Docker 中链接和运行
  result=$(docker run --rm \
    -v "${ROOT}:${ROOT}" \
    -v "/tmp:/tmp" \
    -w "/tmp" \
    "${IMAGE}" \
    bash -c "
      set -e
      # 安装工具链
      export DEBIAN_FRONTEND=noninteractive
      apt-get update -qq >/dev/null 2>&1
      apt-get install -y -qq gcc-riscv64-linux-gnu qemu-user-static libc6-dev-riscv64-cross >/dev/null 2>&1

      # 链接（Sisyphus 风格：直接链接 .c）
      riscv64-linux-gnu-gcc -static -mcmodel=medany \
        '/tmp/${base}.s' '${RUNTIME_C}' -lm -o '/tmp/${base}' 2>/dev/null

      # 运行
      if [ -f '${in_file}' ]; then
        timeout 30 qemu-riscv64-static '/tmp/${base}' <'${in_file}' 2>/dev/null
      else
        timeout 30 qemu-riscv64-static '/tmp/${base}' 2>/dev/null
      fi
    " 2>/dev/null || true)

  # 验证输出
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
