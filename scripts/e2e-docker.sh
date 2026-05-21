#!/usr/bin/env bash
# 在 Linux 容器内：安装 RISC-V 交叉工具链与 qemu-user，**现场编译** libsysy.a，
# 编译本仓库 compiler，smoke 端到端；可选跑批量官方样例目录。
#
# libsysy.a 来源（本脚本不下载预编译 .a，也不使用 libsysy.so）：
#   - 默认：/work/runtime/sysy_runtime.c → sysy.o → ar 成 /tmp/sysy-runtime-lib/build/libsysy.a
#   - 备选：无本地 C 源时 git clone pku-minic/sysy-runtime-lib（默认分支最新，无固定 tag）
#   - 批测：export LIBSYSY="$RT/build/libsysy.a" 交给 scripts/run_sy_tests.sh
#   - 链接：LINK_FLAGS 默认 -static -mcmodel=medany
#
# 用法：
#   docker run --rm -v "$PWD:/work" ubuntu:22.04 bash /work/scripts/e2e-docker.sh
# 挂载官方 .sy 目录并比对 golden（需每个用例旁有同名 .out）：
#   docker run --rm -v "$PWD:/work" -e SY_TEST_DIR=/work/path/to/sy_tests ubuntu:22.04 bash /work/scripts/e2e-docker.sh
# 多个目录（空格分隔）：
#   -e SY_TEST_DIRS="/work/examples/golden_smoke /work/examples/golden_o1_const"
#
# 可选：LINK_FLAGS（默认 -static -mcmodel=medany）、APT_MIRROR_HTTP、USE_O1=1（传给批量脚本）
#      SKIP_APT=1（仅当容器内已有 riscv64-linux-gnu-gcc 与 qemu-riscv64-static 时跳过 apt；缺则自动安装）
#      INSTALL_ONLY=1（只完成 apt 工具链安装，供长期容器初始化使用）

set -euo pipefail
export DEBIAN_FRONTEND=noninteractive

# 将 apt 换为清华源。说明：纯镜像默认没有完整 CA，若直接用 https 源会导致
#「Certificate verification failed」且无法安装 ca-certificates。因此第一步使用
# 清华的 **HTTP** 源完成 update + 安装 ca-certificates；之后再可选改用 HTTPS。
replace_apt_mirror_tsinghua() {
  local base="${APT_MIRROR_HTTP:-http://mirrors.tuna.tsinghua.edu.cn}"
  [[ "$base" == "default" ]] && return 0
  local f
  for f in /etc/apt/sources.list.d/ubuntu.sources /etc/apt/sources.list; do
    [[ -f "$f" ]] || continue
    sed -i.bak \
      -e "s|http://archive.ubuntu.com/ubuntu|${base}/ubuntu|g" \
      -e "s|https://archive.ubuntu.com/ubuntu|${base}/ubuntu|g" \
      -e "s|http://security.ubuntu.com/ubuntu|${base}/ubuntu|g" \
      -e "s|https://security.ubuntu.com/ubuntu|${base}/ubuntu|g" \
      -e "s|http://ports.ubuntu.com/ubuntu-ports|${base}/ubuntu-ports|g" \
      -e "s|https://ports.ubuntu.com/ubuntu-ports|${base}/ubuntu-ports|g" \
      "$f"
  done
}

LOCAL_SYSY=/work/runtime/sysy_runtime.c

# SKIP_APT=1 仅在容器里已装好交叉 gcc 与 qemu-user-static 时生效；否则仍跑 apt（避免「已 ready」但命令不存在）
toolchain_present() {
  command -v riscv64-linux-gnu-gcc >/dev/null 2>&1 && command -v qemu-riscv64-static >/dev/null 2>&1
}

if [[ "${SKIP_APT:-0}" == "1" ]] && toolchain_present; then
  echo "=== Skip apt setup (SKIP_APT=1, toolchain present) ==="
else
  if [[ "${SKIP_APT:-0}" == "1" ]]; then
    echo "=== SKIP_APT=1 but riscv64-linux-gnu-gcc or qemu-riscv64-static missing; running apt install ===" >&2
  fi
  replace_apt_mirror_tsinghua
  apt-get -o Acquire::Retries="${APT_RETRIES:-5}" update -qq
  APT_PKGS=(make g++ gcc-riscv64-linux-gnu libc6-dev-riscv64-cross binutils-riscv64-linux-gnu qemu-user-static)
  if [[ ! -f "$LOCAL_SYSY" ]]; then
    APT_PKGS+=(ca-certificates git)
  fi
  apt-get -o Acquire::Retries="${APT_RETRIES:-5}" install -y -qq --no-install-recommends "${APT_PKGS[@]}"
fi

if [[ "${INSTALL_ONLY:-0}" == "1" ]]; then
  echo "=== Install-only setup done ==="
  exit 0
fi

RT=/tmp/sysy-runtime-lib
echo "=== Build libsysy.a (riscv64-linux-gnu) ==="
rm -rf "$RT"
mkdir -p "$RT/build/obj"
if [[ -f "$LOCAL_SYSY" ]]; then
  echo "using local runtime: $LOCAL_SYSY"
  riscv64-linux-gnu-gcc -Wall -O3 -c "$LOCAL_SYSY" -o "$RT/build/obj/sysy.o"
else
  echo "local runtime missing; cloning official runtime"
  git clone --depth 1 https://github.com/pku-minic/sysy-runtime-lib.git "$RT/src"
  riscv64-linux-gnu-gcc -Wall -O3 -c "$RT/src/src/sysy.c" -o "$RT/build/obj/sysy.o" -I"$RT/src/src"
fi
riscv64-linux-gnu-ar rc "$RT/build/libsysy.a" "$RT/build/obj/sysy.o"
riscv64-linux-gnu-ranlib "$RT/build/libsysy.a"

# 与赛方 RISC-V 说明一致的大代码模型（可与官方评测链接选项对齐）
export LINK_FLAGS="${LINK_FLAGS:--static -mcmodel=medany}"

echo "=== Build compiler (host g++) ==="
BUILD_DIR="${BUILD_DIR:-/tmp/compiler-build}"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/src"
cp /work/Makefile "$BUILD_DIR/"
cp /work/src/*.cpp /work/src/*.h "$BUILD_DIR/src/"
# 确保目录存在（防御性），并禁用并行编译避免竞争
mkdir -p "$BUILD_DIR/src"
make -C "$BUILD_DIR" -j1 clean
make -C "$BUILD_DIR" -j1
export COMPILER="$BUILD_DIR/compiler"

echo "=== smoke.sy -> smoke.s ==="
"$COMPILER" -S -o /tmp/smoke.s /work/examples/smoke.sy

echo "=== Link (LINK_FLAGS=$LINK_FLAGS) ==="
riscv64-linux-gnu-gcc $LINK_FLAGS /tmp/smoke.s "$RT/build/libsysy.a" -o /tmp/smoke.elf

echo "=== qemu-riscv64 (stdout bytes + exit) ==="
qemu-riscv64-static /tmp/smoke.elf | tee /tmp/smoke.out
echo "exit_code=$?"
echo "hex:"
od -An -tx1 /tmp/smoke.out

export LIBSYSY="$RT/build/libsysy.a"
if [[ -n "${SY_TEST_DIR:-}" && -d "${SY_TEST_DIR}" ]]; then
  echo ""
  echo "=== Batch functional tests: SY_TEST_DIR=$SY_TEST_DIR ==="
  bash /work/scripts/run_sy_tests.sh "$SY_TEST_DIR"
fi
if [[ -n "${SY_TEST_DIRS:-}" ]]; then
  echo ""
  echo "=== Batch (multiple dirs SY_TEST_DIRS) ==="
  for d in $SY_TEST_DIRS; do
    [[ -d "$d" ]] || { echo "skip (not a dir): $d"; continue; }
    echo "--- $d ---"
    bash /work/scripts/run_sy_tests.sh "$d"
  done
fi
