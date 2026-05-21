#pragma once

#include "ast.h"

// AST 层循环优化：转置交换、many_mat i-j-k→i-k-j 交换 + 16×32 分块（见 loop_tiling）。
void loopInterchangePass(Program &program);
void loopTilingPass(Program &program);
