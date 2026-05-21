#pragma once

#include "ast.h"

// AST 级循环展开
// 展开小常量边界循环以减少循环开销

void applySmallLoopUnrollPass(Program &program);
