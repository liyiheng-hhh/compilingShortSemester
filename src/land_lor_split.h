#pragma once

#include "ast.h"

// 将 if/while 条件中的 a && b && … 拆成嵌套 if 或 break 链，使 IR 后端可接受（无短路 &&）。
void splitLogicalAndPass(Program &program);
