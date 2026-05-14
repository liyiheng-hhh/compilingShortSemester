#!/usr/bin/env bash
# 批量 SysY 功能回归：对每个 *.sy 调用 compiler 生成 .s，用 riscv64-linux-gnu-gcc
# 与 libsysy.a 静态链接（默认 -static -mcmodel=medany），在 qemu-riscv64-static 下运行，
# 与同名 .out（及可选 .ret）比对。
#
# 约定：
#   foo.sy    源
#   foo.out   期望 stdout（可选）
#   foo.in    stdin（可选）
#   foo.ret   期望退出码一行（可选）
#
#   cd 项目根目录 && make
#   export LIBSYSY=/path/to/libsysy.a
#   ./scripts/run_sy_tests.sh /path/to/testdir
#
# 环境变量：COMPILER RISCV_GCC QEMU LINK_FLAGS LIBSYSY USE_O1=1 CONTINUE_ON_FAIL=1
#           RUN_TIMEOUT=60s（可选，单个 qemu 运行超时；默认不限制）

set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

COMPILER="${COMPILER:-$ROOT/compiler}"
RISCV_GCC="${RISCV_GCC:-riscv64-linux-gnu-gcc}"
QEMU="${QEMU:-qemu-riscv64-static}"
LINK_FLAGS="${LINK_FLAGS:--static -mcmodel=medany}"

TESTDIR="${1:-}"
if [[ -z "$TESTDIR" || ! -d "$TESTDIR" ]]; then
  echo "usage: $0 <directory-containing-.sy-files>" >&2
  exit 2
fi

if [[ ! -f "$COMPILER" || ! -x "$COMPILER" ]]; then
  echo "error: compiler not executable: $COMPILER (run make first)" >&2
  exit 2
fi

if [[ -z "${LIBSYSY:-}" || ! -f "$LIBSYSY" ]]; then
  echo "error: set LIBSYSY to the path of libsysy.a" >&2
  exit 2
fi

command -v "$RISCV_GCC" >/dev/null 2>&1 || { echo "error: missing $RISCV_GCC" >&2; exit 2; }
command -v "$QEMU" >/dev/null 2>&1 || { echo "error: missing $QEMU" >&2; exit 2; }
if [[ -n "${RUN_TIMEOUT:-}" ]]; then
  command -v timeout >/dev/null 2>&1 || { echo "error: RUN_TIMEOUT needs GNU timeout" >&2; exit 2; }
fi

ac=0
wa=0
ce=0
re=0
nogolden=0

# 返回：0=AC 10=无.out仅冒烟 1=WA 2=CE 3=链接失败 4=运行崩溃
run_one() {
  local sy="$1"
  local base dir s elf act act_with_rc tmp stdin_file out rc_exp
  base=$(basename "$sy" .sy)
  dir=$(dirname "$sy")
  tmp=$(mktemp -d "${TMPDIR:-/tmp}/sytest.XXXXXX")
  s="$tmp/$base.s"
  elf="$tmp/$base.elf"
  act="$tmp/act.out"
  act_with_rc="$tmp/act_with_rc.out"

  local comp_args=( -S -o "$s" "$sy" )
  if [[ "${USE_O1:-0}" == "1" ]]; then
    comp_args+=( -O1 )
  fi

  if ! "$COMPILER" "${comp_args[@]}" 2>"$tmp/ce.log"; then
    echo "CE  $sy"
    sed 's/^/    /' "$tmp/ce.log" 2>/dev/null || true
    rm -rf "$tmp"
    return 2
  fi

  if ! $RISCV_GCC $LINK_FLAGS "$s" "$LIBSYSY" -o "$elf" 2>"$tmp/le.log"; then
    echo "RE(link)  $sy"
    sed 's/^/    /' "$tmp/le.log" 2>/dev/null || true
    rm -rf "$tmp"
    return 3
  fi

  stdin_file=/dev/null
  [[ -f "$dir/$base.in" ]] && stdin_file="$dir/$base.in"

  set +e
  if [[ -n "${RUN_TIMEOUT:-}" ]]; then
    timeout "$RUN_TIMEOUT" "$QEMU" "$elf" <"$stdin_file" >"$act" 2>"$tmp/qemu.err"
  else
    "$QEMU" "$elf" <"$stdin_file" >"$act" 2>"$tmp/qemu.err"
  fi
  local rc_act=$?
  set -e

  if [[ -n "${RUN_TIMEOUT:-}" && "$rc_act" -eq 124 ]]; then
    echo "RE(timeout ${RUN_TIMEOUT})  $sy"
    rm -rf "$tmp"
    return 4
  fi

  if [[ -s "$tmp/qemu.err" ]]; then
    echo "RE(run)  $sy  qemu stderr:" >&2
    sed 's/^/    /' "$tmp/qemu.err" >&2
  fi

  cp "$act" "$act_with_rc"
  if [[ -s "$act_with_rc" ]] && [[ "$(tail -c 1 "$act_with_rc")" != "" ]]; then
    printf '\n' >>"$act_with_rc"
  fi
  printf '%s\n' "$rc_act" >>"$act_with_rc"

  out="$dir/$base.out"
  rc_exp="$dir/$base.ret"

  if [[ ! -f "$out" ]]; then
    echo "OK(no .out)  $sy  exit=$rc_act"
    rm -rf "$tmp"
    return 10
  fi

  if ! cmp -s "$out" "$act" && ! cmp -s "$out" "$act_with_rc"; then
    echo "WA  $sy"
    echo "    --- expected"
    sed 's/^/    /' "$out" 2>/dev/null || true
    echo "    --- actual(stdout + exit)"
    sed 's/^/    /' "$act_with_rc" 2>/dev/null || true
    rm -rf "$tmp"
    return 1
  fi

  if [[ -f "$rc_exp" ]]; then
    local want got
    want=$(tr -d ' \t\r\n' <"$rc_exp")
    got=$rc_act
    if [[ "$want" != "$got" ]]; then
      echo "WA(ret)  $sy  want=$want got=$got"
      rm -rf "$tmp"
      return 1
    fi
  fi

  echo "AC  $sy"
  rm -rf "$tmp"
  return 0
}

count=0
while IFS= read -r -d '' sy; do
  count=$((count + 1))
done < <(find "$TESTDIR" -type f -name '*.sy' -print0)

if [[ "$count" -eq 0 ]]; then
  echo "no .sy files under $TESTDIR" >&2
  exit 1
fi

echo "=== SysY batch: $count file(s) under $TESTDIR ==="
echo "    COMPILER=$COMPILER USE_O1=${USE_O1:-0}"
echo "    LINK_FLAGS=$LINK_FLAGS"

fail=0
while IFS= read -r -d '' sy; do
  code=0
  run_one "$sy" || code=$?
  case $code in
    0) ac=$((ac + 1)) ;;
    10) nogolden=$((nogolden + 1)) ;;
    1) wa=$((wa + 1)); fail=$((fail + 1)) ;;
    2) ce=$((ce + 1)); fail=$((fail + 1)) ;;
    3|4) re=$((re + 1)); fail=$((fail + 1)) ;;
  esac
  if [[ "${CONTINUE_ON_FAIL:-1}" != "1" && "$fail" -gt 0 ]]; then
    break
  fi
done < <(find "$TESTDIR" -type f -name '*.sy' -print0)

echo "=== Summary ==="
echo "AC: $ac   WA: $wa   CE: $ce   RE: $re   OK(no .out): $nogolden"

if [[ "$fail" -eq 0 ]]; then
  exit 0
fi
exit 1
