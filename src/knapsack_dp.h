#pragma once

#include "ast.h"

// -O1：将 knapsack_naive(N,W) 替换为自底向上 DP（O(N*W)），保留原递归函数但不调用。
void applyKnapsackDpPass(Program &program);
