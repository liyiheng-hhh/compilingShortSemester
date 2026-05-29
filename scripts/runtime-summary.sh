#!/usr/bin/env bash
# 汇总 performance CSV — 精简输出
#
#   make runtime-eval SUITE=performance OPT=O1
#   make runtime-summary
#   RUNTIME_CSV=path/to.csv make runtime-summary
set -euo pipefail

TARGET="${1:-tests/.out/runtime/sysy-performance-O1.csv}"

if [[ ! -f "$TARGET" ]]; then
  echo "error: CSV 不存在: $TARGET" >&2
  echo "请先: make runtime-eval SUITE=performance OPT=O1" >&2
  exit 2
fi

export SUMMARY_CSV="$TARGET"
python3 - <<'PY'
import csv
import os
import sys

path = os.environ["SUMMARY_CSV"]
rows = []
with open(path, newline="") as f:
    reader = csv.DictReader(f)
    fields = reader.fieldnames or []
    for row in reader:
        rows.append(row)

if not rows:
    print(f"error: CSV 为空: {path}", file=sys.stderr)
    sys.exit(1)

def fnum(key, row, default=None):
    v = (row.get(key) or "").strip()
    if not v:
        return default
    try:
        return float(v)
    except ValueError:
        return default

total = len(rows)
passed = sum(1 for r in rows if (r.get("pass") or "").strip() == "1")
failed = [r for r in rows if (r.get("pass") or "").strip() != "1"]

has_kernel = "median_kernel_s" in fields
has_wall = "median_ms" in fields

wall_sum = 0.0
wall_n = 0
kernel_sum = 0.0
kernel_n = 0
ranked = []

for r in rows:
    if (r.get("pass") or "").strip() != "1":
        continue
    ks = fnum("median_kernel_s", r)
    wm = fnum("median_ms", r)
    if ks is not None:
        kernel_sum += ks
        kernel_n += 1
    if wm is not None:
        wall_sum += wm
        wall_ms = wm
        wall_n += 1
    else:
        wall_ms = None
    ranked.append({
        "case": r.get("case_id", "?"),
        "kernel_s": ks,
        "wall_ms": wall_ms,
    })

# 最慢 5 题：按平台口径 kernel_s 排序；无 kernel 时退化为 wall_ms
def sort_key(item):
    if item["kernel_s"] is not None:
        return (0, -item["kernel_s"])
    if item["wall_ms"] is not None:
        return (1, -item["wall_ms"])
    return (2, 0)

ranked.sort(key=sort_key)
top5 = ranked[:5]

print(f"文件: {path}")
print()

# ── 1. 正确性 ──
print(f"[正确性] {passed}/{total} 通过", end="")
if total != 60:
    print(f"  ⚠ 期望 60 题，当前仅 {total} 题（若曾用 RUNTIME_CASE_FILTER 单测，请重跑完整 eval）")
else:
    print()
print()

# ── 2. 失败用例 ──
print("[失败用例]")
if not failed:
    print("  无")
else:
    for r in failed:
        case = r.get("case_id", "?")
        status = r.get("status", "?")
        compare = r.get("compare", "?")
        print(f"  {case}  status={status}  compare={compare}")
print()

# ── 3. 总耗时 ──
print("[总耗时]")
if has_kernel and kernel_n > 0:
    kernel_sum_ms = kernel_sum * 1000.0
    print(f"  平台口径 (kernel 之和): {kernel_sum_ms:.3f} ms   ({kernel_n} 题，对标赛方 Time(s) 相加)")
else:
    print("  平台口径: N/A（CSV 无 median_kernel_s，请重新 make runtime-eval）")
if has_wall and wall_n > 0:
    print(f"  本地口径 (wall 之和):   {wall_sum:.3f} ms  ({wall_n} 题，QEMU 墙钟，含读入 .in)")
else:
    print("  本地口径: N/A")
print()

print("[最慢 5 题]（按平台 kernel 降序）")
print(f"  {'用例':<28} {'平台 kernel':>14} {'本地 wall':>14}")
print(f"  {'-'*28} {'-'*14} {'-'*14}")
for item in top5:
    if item["kernel_s"] is not None:
        ks = f"{item['kernel_s'] * 1000:.3f} ms"
    else:
        ks = "N/A"
    wm = f"{item['wall_ms']:.3f} ms" if item["wall_ms"] is not None else "N/A"
    print(f"  {item['case']:<28} {ks:>14} {wm:>14}")
print()
print("说明: 平台 kernel ≈ 赛方单题 Time(s)；本地 wall 仅 QEMU 参考。")
PY
