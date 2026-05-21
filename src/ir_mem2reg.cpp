#include "ir_mem2reg.h"

#include <algorithm>
#include <queue>
#include <stack>

// 计算支配者（迭代数据流算法）
void Mem2RegPass::computeDominators() {
  int n = static_cast<int>(fn->blocks.size());
  if (n == 0) return;

  // 入口节点支配自己
  dom.assign(n, std::vector<bool>(n, false));
  dom[0][0] = true;
  for (int i = 1; i < n; ++i) {
    std::fill(dom[i].begin(), dom[i].end(), true);
  }

  // 构建前驱列表
  preds.assign(n, {});
  for (int b = 0; b < n; ++b) {
    for (int s : fn->blocks[b].succ) {
      if (s >= 0 && s < n) {
        preds[s].push_back(b);
      }
    }
  }

  bool changed = true;
  while (changed) {
    changed = false;
    for (int i = 1; i < n; ++i) {
      std::vector<bool> newDom(n, false);
      newDom[i] = true;

      const auto &p = preds[i];
      if (!p.empty()) {
        std::fill(newDom.begin(), newDom.end(), true);
        for (int pred : p) {
          for (int j = 0; j < n; ++j) {
            newDom[j] = newDom[j] && dom[pred][j];
          }
        }
        newDom[i] = true;
      }

      if (newDom != dom[i]) {
        dom[i] = std::move(newDom);
        changed = true;
      }
    }
  }
}

// 构建支配树
void Mem2RegPass::buildDomTree() {
  int n = static_cast<int>(fn->blocks.size());
  idom.assign(n, -1);
  domTreeChildren.assign(n, {});

  if (n == 0) return;

  idom[0] = 0;

  for (int i = 1; i < n; ++i) {
    for (int d = 0; d < n; ++d) {
      if (d == i) continue;
      if (!dom[i][d]) continue;

      bool isIdom = true;
      for (int other = 0; other < n; ++other) {
        if (other == i || other == d) continue;
        if (!dom[i][other]) continue;
        if (dom[other][d]) {
          isIdom = false;
          break;
        }
      }

      if (isIdom) {
        idom[i] = d;
        break;
      }
    }
  }

  for (int i = 0; i < n; ++i) {
    if (idom[i] >= 0 && idom[i] != i) {
      domTreeChildren[idom[i]].push_back(i);
    }
  }
}

// 计算支配前沿
void Mem2RegPass::computeDominanceFrontier() {
  int n = static_cast<int>(fn->blocks.size());
  df.assign(n, {});

  for (int b = 0; b < n; ++b) {
    if (preds[b].size() < 2) continue;

    for (int p : preds[b]) {
      int runner = p;
      while (runner != idom[b] && runner >= 0) {
        df[runner].insert(b);
        runner = idom[runner];
      }
    }
  }
}

bool Mem2RegPass::isPromotable(Symbol *sym) {
  if (!sym) return false;
  if (sym->isArray) return false;
  if (sym->isParam) return false;
  if (sym->isGlobal) return false;
  return true;
}

void Mem2RegPass::collectPromotableVariables() {
  varIndex.clear();
  variables.clear();

  for (size_t b = 0; b < fn->blocks.size(); ++b) {
    const auto &blk = fn->blocks[b];
    for (size_t idx = blk.begin; idx < blk.end; ++idx) {
      const auto &inst = fn->insts[idx];
      if (inst.op == IROp::LoadLocal || inst.op == IROp::StoreLocal) {
        if (isPromotable(inst.sym)) {
          if (varIndex.find(inst.sym) == varIndex.end()) {
            int idx = static_cast<int>(variables.size());
            varIndex[inst.sym] = idx;
            variables.push_back({inst.sym, {}});
          }
          int vidx = varIndex[inst.sym];
          if (inst.op == IROp::StoreLocal) {
            variables[vidx].defBlocks.insert(static_cast<int>(b));
          }
        }
      }
    }
  }
}

void Mem2RegPass::placePhiNodes() {
  int nBlocks = static_cast<int>(fn->blocks.size());
  int nVars = static_cast<int>(variables.size());

  blockPhis.assign(nBlocks, {});
  placedPhi.assign(nBlocks, std::vector<bool>(nVars, false));

  for (int v = 0; v < nVars; ++v) {
    const auto &defBlocks = variables[v].defBlocks;
    if (defBlocks.empty()) continue;

    std::vector<int> workList(defBlocks.begin(), defBlocks.end());
    size_t idx = 0;

    while (idx < workList.size()) {
      int b = workList[idx++];
      for (int y : df[b]) {
        if (!placedPhi[y][v]) {
          placedPhi[y][v] = true;

          PhiNode phi;
          phi.varIdx = v;
          phi.dstVreg = fn->nextVreg++;
          phi.sym = variables[v].sym;

          for (int p : preds[y]) {
            phi.preds.push_back(p);
            phi.vregs.push_back(-1);
          }

          int phiIdx = static_cast<int>(phiNodes.size());
          phiNodes.push_back(phi);
          blockPhis[y].push_back(phiIdx);

          if (defBlocks.find(y) == defBlocks.end()) {
            workList.push_back(y);
          }
        }
      }
    }
  }
}

void Mem2RegPass::renameVariables() {
  int nVars = static_cast<int>(variables.size());
  nameStack.assign(nVars, {});

  renameBlock(0);
}

void Mem2RegPass::renameBlock(int blockIdx) {
  int nVars = static_cast<int>(variables.size());
  std::vector<int> stackHeights(nVars);
  for (int v = 0; v < nVars; ++v) {
    stackHeights[v] = static_cast<int>(nameStack[v].size());
  }

  // 处理此块的 Phi 输出
  for (int phiIdx : blockPhis[blockIdx]) {
    auto &phi = phiNodes[phiIdx];
    nameStack[phi.varIdx].push_back(phi.dstVreg);
  }

  // 遍历指令
  auto &blk = fn->blocks[blockIdx];
  for (size_t i = blk.begin; i < blk.end; ++i) {
    auto &inst = fn->insts[i];

    if (inst.op == IROp::LoadLocal) {
      auto it = varIndex.find(inst.sym);
      if (it != varIndex.end()) {
        int vidx = it->second;
        if (!nameStack[vidx].empty()) {
          inst.op = IROp::Copy;
          inst.u = nameStack[vidx].back();
          inst.sym = nullptr;
        }
      }
    }
    else if (inst.op == IROp::StoreLocal) {
      auto it = varIndex.find(inst.sym);
      if (it != varIndex.end()) {
        int vidx = it->second;
        int newVreg = fn->nextVreg++;
        nameStack[vidx].push_back(newVreg);
        inst.op = IROp::Copy;
        inst.dst = newVreg;
        inst.sym = nullptr;
      }
    }
  }

  // 填充后继 Phi 的前驱 vreg
  for (int succ : fn->blocks[blockIdx].succ) {
    for (int phiIdx : blockPhis[succ]) {
      auto &phi = phiNodes[phiIdx];
      for (size_t i = 0; i < phi.preds.size(); ++i) {
        if (phi.preds[i] == blockIdx) {
          if (!nameStack[phi.varIdx].empty()) {
            phi.vregs[i] = nameStack[phi.varIdx].back();
          }
          break;
        }
      }
    }
  }

  // 递归处理支配树子节点
  for (int child : domTreeChildren[blockIdx]) {
    renameBlock(child);
  }

  // 恢复栈
  for (int v = 0; v < nVars; ++v) {
    while (static_cast<int>(nameStack[v].size()) > stackHeights[v]) {
      nameStack[v].pop_back();
    }
  }
}

void Mem2RegPass::generatePhiInstructions() {
  // 将 Phi 节点转换为 IR 指令
  // 简化处理：在基本块开头插入 Phi 作为特殊指令
  for (size_t phiIdx = 0; phiIdx < phiNodes.size(); ++phiIdx) {
    auto &phi = phiNodes[phiIdx];
    int blockIdx = -1;

    // 找到 Phi 所在的基本块
    for (int b = 0; b < static_cast<int>(blockPhis.size()); ++b) {
      for (int pIdx : blockPhis[b]) {
        if (static_cast<size_t>(pIdx) == phiIdx) {
          blockIdx = b;
          break;
        }
      }
      if (blockIdx >= 0) break;
    }

    if (blockIdx < 0) continue;

    // 创建 Phi 指令（简化：使用 Copy + 条件选择）
    // 实际 SSA Phi 会在寄存器分配阶段处理
    // 这里我们只是记录 Phi 关系
  }
}

bool Mem2RegPass::run() {
  if (fn->blocks.empty()) {
    irRefreshCFG(*fn);
  }

  int nBlocks = static_cast<int>(fn->blocks.size());
  if (nBlocks == 0) return false;

  computeDominators();
  buildDomTree();
  computeDominanceFrontier();
  collectPromotableVariables();

  if (variables.empty()) return false;

  placePhiNodes();
  renameVariables();
  generatePhiInstructions();

  // 标记已处理的 LoadLocal/StoreLocal 为 Nop
  for (auto &inst : fn->insts) {
    if ((inst.op == IROp::LoadLocal || inst.op == IROp::StoreLocal) &&
        isPromotable(inst.sym)) {
      inst.op = IROp::Nop;
    }
  }

  return true;
}

bool irMem2Reg(IRFunction &fn) {
  Mem2RegPass pass(&fn);
  return pass.run();
}
