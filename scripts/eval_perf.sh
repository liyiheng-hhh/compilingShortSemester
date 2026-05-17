#!/usr/bin/env bash
# 批量运行时评测核心：warmup + RUNS 次取中位数 + 正确性
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=runtime_common.sh
source "$ROOT/scripts/runtime_common.sh"

emit_csv_row() {
  {
    printf '%s,%s,%s,%s,%s,%s,%s,%s,%s' \
      "$SUITE_NAME" "$base" "$OPT_NAME" "$LABEL" "$status" "$compare" "$pass" \
      "$median_ms" "$warmup_ms"
    for ((ri = 0; ri < RUNS; ++ri)); do
      printf ',%s' "${run_ms[$ri]:-}"
    done
    printf ',%s,%s,%s,%s\n' "$asm_lines" "$compile_ok" "$link_ok" "$log"
  } >>"$PERF_CSV"
}

TESTDIR="${1:-}"
if [[ -z "$TESTDIR" || ! -d "$TESTDIR" ]]; then
  echo "usage: $0 <dir-with-.sy>" >&2
  exit 2
fi

COMPILER="${COMPILER:-$ROOT/compiler}"
LIBSYSY="${LIBSYSY:-$ROOT/libsysy.a}"
RISCV_GCC="${RISCV_GCC:-riscv64-linux-gnu-gcc}"
QEMU="${QEMU:-qemu-riscv64-static}"
LINK_FLAGS="${LINK_FLAGS:--static -mcmodel=medany}"
RUNS="${RUNS:-3}"
WARMUP="${WARMUP:-1}"
TIMEOUT_SEC="${TIMEOUT_SEC:-20}"
LABEL="${PERF_LABEL:-${RUNTIME_LABEL:-sysy}}"
SUITE_NAME="${RUNTIME_SUITE:-$(basename "$TESTDIR")}"
OPT_NAME="${RUNTIME_OPT:-$([[ "${USE_O1:-1}" == "1" ]] && echo O1 || echo O0)}"
SUITE_KIND="${RUNTIME_SUITE_KIND:-perf}"
SOFT="${RUNTIME_SOFT_PERF:-0}"

OPT_FLAGS="${OPT_FLAGS:-}"
if [[ -z "$OPT_FLAGS" && "${USE_O1:-1}" == "1" ]]; then
  OPT_FLAGS="-O1"
fi

PERF_CSV="${PERF_CSV:-$ROOT/tests/.out/perf/${LABEL}-$(basename "$TESTDIR")-${OPT_NAME}.csv}"
CASE_FILTER="${CASE_FILTER:-${RUNTIME_CASE_FILTER:-}}"
CASE_LIMIT="${CASE_LIMIT:-${RUNTIME_CASE_LIMIT:-0}}"

[[ -x "$COMPILER" ]] || { echo "error: missing compiler: $COMPILER" >&2; exit 2; }
[[ -f "$LIBSYSY" ]] || { echo "error: missing LIBSYSY=$LIBSYSY (make libsysy.a)" >&2; exit 2; }
command -v "$RISCV_GCC" >/dev/null 2>&1 || { echo "error: $RISCV_GCC" >&2; exit 2; }
if ! command -v "$QEMU" >/dev/null 2>&1; then
  if command -v qemu-riscv64 >/dev/null 2>&1; then
    QEMU=qemu-riscv64
  else
    echo "error: need qemu-riscv64-static or qemu-riscv64" >&2
    exit 2
  fi
fi
command -v timeout >/dev/null 2>&1 || { echo "error: GNU timeout required" >&2; exit 2; }

mkdir -p "$(dirname "$PERF_CSV")"
tmp_root="${TMPDIR:-/tmp}/evalperf.$$"
mkdir -p "$tmp_root"
trap 'rm -rf "$tmp_root"' EXIT

printf 'suite,case_id,opt,label,status,compare,pass,median_ms,warmup_ms' >"$PERF_CSV"
for ((r = 1; r <= RUNS; ++r)); do
  printf ',run%d_ms' "$r" >>"$PERF_CSV"
done
printf ',asm_lines,compile_ok,link_ok,log\n' >>"$PERF_CSV"

total=0
pass_count=0
hard_fail=0

while IFS= read -r -d '' sy; do
  base=$(basename "$sy" .sy)
  if [[ -n "$CASE_FILTER" && "$base" != *"$CASE_FILTER"* ]]; then
    continue
  fi
  if [[ "$CASE_LIMIT" -gt 0 && "$total" -ge "$CASE_LIMIT" ]]; then
    break
  fi
  total=$((total + 1))
  dir=$(dirname "$sy")
  out_golden="$dir/$base.out"
  in_file="$dir/$base.in"
  [[ -f "$in_file" ]] || in_file=/dev/null

  work="$tmp_root/$base"
  mkdir -p "$work"
  asm="$work/$base.s"
  elf="$work/$base.elf"
  log="$work/run.log"

  status="ok"
  pass=0
  compare="skip"
  compile_ok=0
  link_ok=0
  asm_lines=""
  warmup_ms=""
  run_ms=()
  run_rcs=()
  median_ms=""

  : >"$log"
  echo "[suite] $SUITE_NAME [opt] $OPT_NAME [case] $base" >>"$log"

  set +e
  # shellcheck disable=SC2086
  "$COMPILER" -S -o "$asm" $OPT_FLAGS "$sy" >>"$log" 2>&1
  compile_rc=$?
  set -e
  if [[ $compile_rc -ne 0 || ! -f "$asm" ]]; then
    status="compile_fail"
    hard_fail=$((hard_fail + 1))
    emit_csv_row
    echo "[$base] status=$status"
    continue
  fi
  compile_ok=1
  asm_lines=$(wc -l <"$asm" | tr -d ' ')

  set +e
  # shellcheck disable=SC2086
  $RISCV_GCC $LINK_FLAGS "$asm" "$LIBSYSY" -o "$elf" -lm >>"$log" 2>&1
  link_rc=$?
  set -e
  if [[ $link_rc -ne 0 ]]; then
    status="link_fail"
    hard_fail=$((hard_fail + 1))
    emit_csv_row
    echo "[$base] status=$status"
    continue
  fi
  link_ok=1

  if [[ "$WARMUP" -gt 0 ]]; then
    runtime_run_timed "$elf" "$in_file" "$work/warm.out" "$work/warm.err" "$TIMEOUT_SEC" "$QEMU"
    warmup_ms="$(runtime_ns_to_ms "$RUNTIME_RUN_NS")"
    echo "[warmup] rc=$RUNTIME_RUN_RC ms=$warmup_ms" >>"$log"
  fi

  ns_vals=()
  for ((r = 1; r <= RUNS; ++r)); do
    runtime_run_timed "$elf" "$in_file" "$work/run${r}.out" "$work/run${r}.err" \
      "$TIMEOUT_SEC" "$QEMU"
    ms="$(runtime_ns_to_ms "$RUNTIME_RUN_NS")"
    run_ms+=("$ms")
    run_rcs+=("$RUNTIME_RUN_RC")
    ns_vals+=("$RUNTIME_RUN_NS")
    echo "[run$r] rc=$RUNTIME_RUN_RC ms=$ms" >>"$log"
    if [[ $RUNTIME_RUN_RC -eq 124 ]]; then
      status="timeout"
    elif [[ $RUNTIME_RUN_RC -ne 0 ]]; then
      status="runtime_fail"
    fi
  done

  if [[ -f "$out_golden" ]]; then
    compare="mismatch"
    for ((r = 1; r <= RUNS; ++r)); do
      act="$work/run${r}.stripped"
      tr -d '\r' <"$work/run${r}.out" >"$act"
      if cmp -s "$out_golden" "$act"; then
        compare="ok"
        break
      fi
      # 与 run_sy_tests.sh 一致：官方 .out 常为「stdout + 退出码一行」
      withrc="$work/run${r}.withrc"
      cp "$act" "$withrc"
      # 与 run_sy_tests.sh 相同：$(tail -c 1) 会去掉换行符，故用「是否为空」判断末尾有无 \n
      if [[ -s "$withrc" ]] && [[ -n "$(tail -c 1 "$withrc")" ]]; then
        printf '\n' >>"$withrc"
      fi
      printf '%s\n' "${run_rcs[$((r - 1))]}" >>"$withrc"
      if cmp -s "$out_golden" "$withrc"; then
        compare="ok"
        break
      fi
    done
  else
    compare="no_golden"
  fi

  if [[ "$status" == "ok" && ( "$compare" == "ok" || "$compare" == "no_golden" ) ]]; then
    pass=1
    pass_count=$((pass_count + 1))
    med_ns="$(runtime_median_ns "${ns_vals[@]}")"
    median_ms="$(runtime_ns_to_ms "$med_ns")"
  elif [[ "$SUITE_KIND" == "functional" ]]; then
    hard_fail=$((hard_fail + 1))
  fi

  emit_csv_row
  echo "[$base] status=$status pass=$pass median_ms=${median_ms:-N/A} asm_lines=$asm_lines"
done < <(find "$TESTDIR" -maxdepth 1 -type f -name '*.sy' -print0 | sort -z)

echo ""
echo "csv=$PERF_CSV"
echo "suite=$SUITE_NAME opt=$OPT_NAME total=$total pass=$pass_count fail=$((total - pass_count))"

if [[ "$total" -eq 0 ]]; then
  echo "warn: no .sy files under $TESTDIR" >&2
  exit 1
fi

if [[ "$SOFT" != "1" && "$hard_fail" -gt 0 ]]; then
  echo "hard_fail=$hard_fail" >&2
  exit 1
fi

exit 0
