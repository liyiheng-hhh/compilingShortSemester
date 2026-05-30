#!/usr/bin/env bash
# 默认只开启已验收的 MatKernelOpt pass；勿一次性打开 Reassociate/Fusion（shuffle/h-5 会严重退化）。
#
#   source ./scripts/opt-passes-on.sh
#   source ./scripts/opt-passes-on-experimental.sh   # 含 Reassociate / Fusion，需单独 eval
#
# eval-runtime.sh / cmp_o1_tiers.sh 已自动 source 本文件。
unset SYSY_CC_NO_GUARDED_ACCUM 2>/dev/null || true
unset SYSY_CC_NO_MAT_TRANSPOSE_PAIR 2>/dev/null || true
