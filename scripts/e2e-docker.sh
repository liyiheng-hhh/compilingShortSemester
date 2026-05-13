#!/usr/bin/env bash
# 在 Linux 容器内：安装 RISC-V 交叉工具链与 qemu-user，拉取 SysY 运行时，
# 编译本仓库 compiler，smoke 端到端；可选跑批量官方样例目录。
#
# 用法：
#   docker run --rm -v "$PWD:/work" ubuntu:24.04 bash /work/scripts/e2e-docker.sh
# 挂载官方 .sy 目录并比对 golden（需每个用例旁有同名 .out）：
#   docker run --rm -v "$PWD:/work" -e SY_TEST_DIR=/work/path/to/sy_tests ubuntu:24.04 bash /work/scripts/e2e-docker.sh
# 多个目录（空格分隔）：
#   -e SY_TEST_DIRS="/work/examples/golden_smoke /work/examples/golden_o1_const"
#
# 可选：LINK_FLAGS（默认 -static -mcmodel=medany）、APT_MIRROR_HTTP、USE_O1=1（传给批量脚本）

set -euo pipefail
export DEBIAN_FRONTEND=noninteractive

# 将 apt 换为清华源。说明：纯镜像默认没有完整 CA，若直接用 https 源会导致
#「Certificate verification failed」且无法安装 ca-certificates。因此第一步使用
# 清华的 **HTTP** 源完成 update + 安装 ca-certificates；之后再可选改用 HTTPS。
replace_apt_mirror_tsinghua() {
  local base="${APT_MIRROR_HTTP:-http://mirrors.tuna.tsinghua.edu.cn}"
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

replace_apt_mirror_tsinghua
apt-get update -qq
# 先装 CA，便于后续若改用 https 源或访问 https://github.com
apt-get install -y -qq ca-certificates
apt-get install -y -qq git make g++ gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu qemu-user-static

RT=/tmp/sysy-runtime-lib
echo "=== Clone SysY runtime ==="
rm -rf "$RT"
git clone --depth 1 https://github.com/pku-minic/sysy-runtime-lib.git "$RT"

echo "=== Build libsysy.a (riscv64-linux-gnu) ==="
mkdir -p "$RT/build/obj"
riscv64-linux-gnu-gcc -Wall -O3 -c "$RT/src/sysy.c" -o "$RT/build/obj/sysy.o" -I"$RT/src"
riscv64-linux-gnu-ar rc "$RT/build/libsysy.a" "$RT/build/obj/sysy.o"
riscv64-linux-gnu-ranlib "$RT/build/libsysy.a"

# 与赛方 RISC-V 说明一致的大代码模型（可与官方评测链接选项对齐）
export LINK_FLAGS="${LINK_FLAGS:--static -mcmodel=medany}"

echo "=== Build compiler (host g++) ==="
cd /work
make clean && make

echo "=== smoke.sy -> smoke.s ==="
./compiler -S -o /tmp/smoke.s examples/smoke.sy

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
