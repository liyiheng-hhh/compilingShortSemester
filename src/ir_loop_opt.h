#pragma once

#include "ir.h"
#include "opt_config.h"

// 通用循环/标量 IR 优化（非 benchmark 指纹）
void irOptimizeLoopsAndScalars(IRFunction &fn, const O1Profile &prof);
