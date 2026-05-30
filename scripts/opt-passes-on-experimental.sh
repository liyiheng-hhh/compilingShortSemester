#!/usr/bin/env bash
# 在 opt-passes-on 基础上再打开 Reassociate / Fusion；跑 performance 前务必 eval-vs-baseline。
# shellcheck source=opt-passes-on.sh
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/opt-passes-on.sh"
unset SYSY_CC_NO_REASSOCIATE 2>/dev/null || true
unset SYSY_CC_NO_PREOPT_FUSION 2>/dev/null || true
