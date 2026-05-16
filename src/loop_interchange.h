#pragma once

#include "ast.h"

// 语义保持的 AST 层优化：针对行优先下「外 i 内 j」且核心为 b[i][j]=a[j][i] 的转置式访存，
// 交换为「外 j 内 i」，使内层沿 a 的行（末维）连续访问。
// 分块（tiling）需常量界与更复杂依赖分析，暂未接入；后续可在识别到常界时在此模块扩展。
void loopInterchangePass(Program &program);
