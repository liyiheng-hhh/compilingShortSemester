#pragma once

#include "ast.h"

// AST 层循环优化：转置交换 + 16×16 分块（矩形二重循环、j-k 矩阵乘内层）。
void loopInterchangePass(Program &program);
void loopTilingPass(Program &program);
