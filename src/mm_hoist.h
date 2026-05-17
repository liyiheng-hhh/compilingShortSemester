#pragma once

#include "ast.h"

// -O1：01_mm 风格：A[i][k] 提出 j 内层，避免每列重复 load/算址。
void applyMmAikHoistPass(Program &program);
