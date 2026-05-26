# shellcheck shell=bash
# 运行时评测公共函数（被 eval-runtime.sh / eval_perf.sh 等 source）

runtime_root_dir() {
  cd "$(dirname "${BASH_SOURCE[1]:-${BASH_SOURCE[0]}}")/.." && pwd
}

runtime_ns_to_ms() {
  awk -v ns="$1" 'BEGIN { printf "%.3f", ns / 1000000.0 }'
}

runtime_normalize_out() {
  awk '{ sub(/[ \t\r]+$/, "", $0); print }' "$1"
}

# 去掉性能用例自报的计时代码行，避免与官方 .out 比对时误报 WA。
# 匹配行首 TOTAL: 或 Timer@（与常见 SysY 性能输出约定一致）。
runtime_filter_timing_lines() {
  local src="$1" dst="$2"
  if [[ ! -s "$src" ]]; then
    : >"$dst"
    return 0
  fi
  grep -Ev '^(TOTAL:|Timer@)' "$src" >"$dst" || true
}

# 将程序 stdout 规范化后与 golden 比对；$3 为可写临时文件路径。
# 返回 0 表示匹配。
runtime_golden_matches() {
  local golden="$1" raw_out="$2" scratch="$3"
  runtime_filter_timing_lines "$raw_out" "$scratch"
  cmp -s "$golden" "$scratch"
}

runtime_median_ns() {
  printf '%s\n' "$@" | sort -n | awk '
    { a[NR]=$1 }
    END {
      if (NR == 0) { print ""; exit }
      if (NR % 2 == 1) print a[(NR+1)/2]
      else print (a[NR/2] + a[NR/2+1]) / 2
    }'
}

runtime_run_timed() {
  local exe="$1" stdin_file="$2" stdout_file="$3" stderr_file="$4"
  local timeout_sec="$5" qemu="$6"
  local start end rc
  start="$(date +%s%N)"
  set +e
  if [[ -f "$stdin_file" ]]; then
    timeout "$timeout_sec" "$qemu" "$exe" <"$stdin_file" >"$stdout_file" 2>"${stderr_file}"
  else
    timeout "$timeout_sec" "$qemu" "$exe" >"$stdout_file" 2>"${stderr_file}"
  fi
  rc=$?
  set -e
  end="$(date +%s%N)"
  RUNTIME_RUN_RC=$rc
  RUNTIME_RUN_NS=$((end - start))
}

# 解析 suite 名 → 测试目录；输出变量 RUNTIME_SUITE_DIR RUNTIME_SUITE_KIND
# kind: functional | perf | custom
runtime_resolve_suite() {
  local root="$1" suite="$2"
  RUNTIME_SUITE_DIR=""
  RUNTIME_SUITE_KIND="custom"

  case "$suite" in
  functional)
    RUNTIME_SUITE_KIND="functional"
    RUNTIME_SUITE_DIR="${RUNTIME_FUNCTIONAL_DIR:-$root/2026初赛RISCV赛道功能用例/functional}"
    ;;
  performance|perf)
    RUNTIME_SUITE_KIND="perf"
    RUNTIME_SUITE_DIR="${RUNTIME_PERF_DIR:-$root/performance}"
    ;;
  *)
    if [[ -d "$suite" ]]; then
      RUNTIME_SUITE_DIR="$suite"
      if [[ "$suite" == *functional* || "$suite" == *功能* ]]; then
        RUNTIME_SUITE_KIND="functional"
      else
        RUNTIME_SUITE_KIND="perf"
      fi
    elif [[ -d "$root/$suite" ]]; then
      RUNTIME_SUITE_DIR="$root/$suite"
      RUNTIME_SUITE_KIND="perf"
    else
      echo "error: unknown suite '$suite' (not a directory)" >&2
      return 1
    fi
    ;;
  esac

  if [[ ! -d "$RUNTIME_SUITE_DIR" ]]; then
    echo "error: suite dir missing: $RUNTIME_SUITE_DIR" >&2
    echo "hint: set RUNTIME_FUNCTIONAL_DIR or RUNTIME_PERF_DIR" >&2
    return 1
  fi
}

runtime_opt_to_flags() {
  local opt="$1"
  case "$opt" in
  O0) RUNTIME_OPT_FLAGS="" ;;
  O1) RUNTIME_OPT_FLAGS="-O1" ;;
  *)
    echo "error: opt must be O0|O1" >&2
    return 1
    ;;
  esac
}

runtime_default_csv() {
  local root="$1" label="$2" suite="$3" opt="$4"
  local safe_suite
  safe_suite="${suite//\//__}"
  printf '%s/tests/.out/runtime/%s-%s-%s.csv' "$root" "$label" "$safe_suite" "$opt"
}
