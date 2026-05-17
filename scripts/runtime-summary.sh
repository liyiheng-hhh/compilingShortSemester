#!/usr/bin/env bash
# 汇总 eval-runtime 生成的 CSV（单文件或整个目录）
#
#   ./scripts/runtime-summary.sh tests/.out/runtime
#   ./scripts/runtime-summary.sh tests/.out/runtime/sysy-performance-O1.csv
set -euo pipefail

TARGET="${1:-}"
if [[ -z "$TARGET" ]]; then
  echo "usage: $0 <csv-file|directory>" >&2
  exit 2
fi

summarize_one() {
  local path="$1"
  awk -F, '
    NR == 1 { next }
    {
      total++
      if ($7 == "1") pass++
      if ($5 == "timeout") timeout++
      if ($5 == "compile_fail") compile_fail++
      if ($5 == "link_fail") link_fail++
      if ($7 == "1" && $8 != "") {
        ms = $8 + 0
        sum += ms
        vals[++n] = ms
      }
    }
    END {
      printf "file=%s\n", FILENAME
      printf "suite=%s opt=%s label=%s\n", suite, opt, label
      printf "total=%d pass=%d pass_rate=%.1f%% timeout=%d compile_fail=%d link_fail=%d\n",
        total, pass, (total ? 100.0 * pass / total : 0),
        timeout + 0, compile_fail + 0, link_fail + 0
      if (n > 0) {
        asort(vals)
        mid = (n % 2 == 1) ? vals[(n + 1) / 2] : (vals[n / 2] + vals[n / 2 + 1]) / 2
        p90i = int(n * 0.9)
        if (p90i < 1) p90i = 1
        if (p90i > n) p90i = n
        printf "median_sum_ms=%.3f suite_median_ms=%.3f p90_ms=%.3f\n", sum, mid, vals[p90i]
      }
    }
  ' suite="$(basename "$path" .csv)" opt="" label="" path="$path" "$path"

  echo "slowest (pass=1):"
  awk -F, 'NR > 1 && $7 == "1" && $8 != "" { print $8, $2 }' "$path" | sort -rn | head -10

  echo "failed:"
  awk -F, 'NR > 1 && $7 != "1" { printf "  %s status=%s compare=%s\n", $2, $5, $6 }' "$path" | head -20
  echo ""
}

if [[ -f "$TARGET" ]]; then
  summarize_one "$TARGET"
  exit 0
fi

if [[ ! -d "$TARGET" ]]; then
  echo "error: not a file or directory: $TARGET" >&2
  exit 2
fi

shopt -s nullglob
csvs=("$TARGET"/*.csv)
if [[ ${#csvs[@]} -eq 0 ]]; then
  echo "no csv under $TARGET" >&2
  exit 1
fi

printf 'profile,suite,opt,total,pass,pass_rate,timeout,median_sum_ms,suite_median_ms,p90_ms,path\n'
for f in "${csvs[@]}"; do
  awk -F, -v file="$f" '
    NR == 1 { next }
    {
      total++
      if ($7 == "1") pass++
      if ($5 == "timeout") timeout++
      if ($7 == "1" && $8 != "") { sum += $8; vals[++n] = $8 + 0 }
    }
    END {
      if (n > 0) {
        asort(vals)
        mid = (n % 2 == 1) ? vals[(n+1)/2] : (vals[n/2]+vals[n/2+1])/2
        p90i = int(n * 0.9); if (p90i < 1) p90i = 1; if (p90i > n) p90i = n
        p90 = vals[p90i]
      } else { mid = 0; p90 = 0 }
      pr = total ? 100.0 * pass / total : 0
      printf "%s,%s,%s,%d,%d,%.1f,%d,%.3f,%.3f,%.3f,%s\n",
        $4, $1, $3, total, pass, pr, timeout+0, sum, mid, p90, file
    }
  ' "$f"
done

echo ""
echo "per-file detail:"
for f in "${csvs[@]}"; do
  echo "======== $f ========"
  summarize_one "$f"
done
