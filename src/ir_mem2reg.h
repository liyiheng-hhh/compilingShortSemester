#pragma once

#include "ir.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

// SSA Mem2Reg：将局部标量变量提升为虚拟寄存器
// 基于 Cytron et al. 1991 "Efficiently Computing Static Single Assignment Form"

struct PhiNode {
  int varIdx = -1;
  int dstVreg = -1;
  Symbol *sym = nullptr;
  std::vector<int> preds;  // 前驱基本块
  std::vector<int> vregs;  // 每个前驱对应的 vreg
};

struct Mem2RegPass {
  IRFunction *fn = nullptr;

  // 支配信息
  std::vector<std::vector<bool>> dom;  // 支配矩阵
  std::vector<std::vector<int>> preds;  // 前驱列表
  std::vector<int> idom;  // 直接支配者
  std::vector<std::vector<int>> domTreeChildren;
  std::vector<std::unordered_set<int>> df;  // 支配前沿

  // 变量信息
  struct Variable {
    Symbol *sym = nullptr;
    std::unordered_set<int> defBlocks;
  };

  std::unordered_map<Symbol*, int> varIndex;
  std::vector<Variable> variables;

  // Phi 节点
  std::vector<PhiNode> phiNodes;
  std::vector<std::vector<int>> blockPhis;  // 每个基本块的 Phi 索引
  std::vector<std::vector<bool>> placedPhi;

  // 重命名栈
  std::vector<std::vector<int>> nameStack;

  explicit Mem2RegPass(IRFunction *f) : fn(f) {}

  bool run();

private:
  void computeDominators();
  void buildDomTree();
  void computeDominanceFrontier();

  void collectPromotableVariables();
  bool isPromotable(Symbol *sym);

  void placePhiNodes();
  void renameVariables();
  void renameBlock(int blockIdx);
  void generatePhiInstructions();
};

// 外部入口
bool irMem2Reg(IRFunction &fn);
