#pragma once

namespace sys {

class ASTNode;
class TypeContext;

// Replace knapsack_naive(N,W) in main with bottom-up DP (same transform as legacy knapsack_dp).
bool applyKnapsackDpDialect(ASTNode *root, TypeContext &ctx);

}  // namespace sys
