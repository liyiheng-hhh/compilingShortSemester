// 完整 SSA Mem2Reg 实现
// 基于 Cytron et al. 1991 "Efficiently Computing Static Single Assignment Form"
//
// 算法步骤：
// 1. 计算支配树（Dominator Tree）
// 2. 计算支配前沿（Dominance Frontier）
// 3. 放置 Phi 节点（在 DF 中）
// 4. 重命名变量（支配树遍历）

#include "ir_mem2reg.h"
#include "ir.h"

#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// ===== 支配树计算 =====
struct DominatorTree {
  int n;  // 基本块数量
  vector<vector<bool>> dom;      // dom[i][j] = true 表示 j 支配 i
  vector<int> idom;               // 直接支配者（immediate dominator）
  vector<vector<int>> children; // 支配树子节点
  vector<int> level;            // 支配树深度

  explicit DominatorTree(int numBlocks) : n(numBlocks) {
    dom.assign(n, vector<bool>(n, false));
    idom.assign(n, -1);
    children.assign(n, {});
    level.assign(n, 0);
  }

  // 迭代数据流算法计算支配者
  void compute(const vector<vector<int>> &preds) {
    if (n == 0) return;

    // 入口节点（0）只支配自己
    dom[0][0] = true;
    for (int i = 1; i < n; i++) {
      dom[0][i] = false;
    }

    // 其他节点初始被所有节点支配
    for (int i = 1; i < n; i++) {
      for (int j = 0; j < n; j++) {
        dom[i][j] = true;
      }
    }

    bool changed = true;
    while (changed) {
      changed = false;
      for (int i = 1; i < n; i++) {  // 从 1 开始，跳过入口
        vector<bool> newDom(n, false);
        newDom[i] = true;  // 每个节点支配自己

        // 交集：所有前驱的支配者
        if (!preds[i].empty()) {
          fill(newDom.begin(), newDom.end(), true);
          for (int p : preds[i]) {
            for (int j = 0; j < n; j++) {
              newDom[j] = newDom[j] && dom[p][j];
            }
          }
          newDom[i] = true;  // 确保自己支配自己
        }

        if (newDom != dom[i]) {
          dom[i] = move(newDom);
          changed = true;
        }
      }
    }

    // 计算直接支配者（idom）
    computeIdom();

    // 构建支配树
    buildDomTree();
  }

  // 计算直接支配者
  void computeIdom() {
    idom[0] = 0;  // 入口的 idom 是自己

    for (int i = 1; i < n; i++) {
      // idom(i) 是 i 的严格支配者中，不被其他严格支配者支配的那个
      for (int d = 0; d < n; d++) {
        if (d == i) continue;
        if (!dom[i][d]) continue;  // d 必须支配 i

        bool isIdom = true;
        for (int other = 0; other < n; other++) {
          if (other == i || other == d) continue;
          if (!dom[i][other]) continue;  // other 必须支配 i
          if (dom[other][d]) {  // other 支配 d，则 d 不是 idom
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
  }

  // 构建支配树
  void buildDomTree() {
    for (int i = 0; i < n; i++) {
      if (idom[i] >= 0 && idom[i] != i) {
        children[idom[i]].push_back(i);
      }
    }

    // 计算深度（BFS）
    queue<pair<int, int>> q;  // (节点, 深度)
    q.push({0, 0});
    vector<bool> visited(n, false);
    visited[0] = true;

    while (!q.empty()) {
      auto [u, d] = q.front();
      q.pop();
      level[u] = d;
      for (int v : children[u]) {
        if (!visited[v]) {
          visited[v] = true;
          q.push({v, d + 1});
        }
      }
    }
  }
};

// ===== 支配前沿计算 =====
static vector<unordered_set<int>> computeDominanceFrontier(
    const DominatorTree &dt,
    const vector<vector<int>> &preds) {
  int n = dt.n;
  vector<unordered_set<int>> df(n);

  for (int b = 0; b < n; b++) {
    if (preds[b].size() < 2) continue;  // 单前驱没有 DF

    for (int p : preds[b]) {
      int runner = p;
      while (runner != dt.idom[b] && runner >= 0) {
        df[runner].insert(b);
        runner = dt.idom[runner];
      }
    }
  }

  return df;
}

// ===== Phi 节点数据结构 =====
struct PhiNode {
  int blockIdx;        // 所在基本块
  Symbol *var;         // 变量
  int dstVreg;         // Phi 输出的 vreg
  vector<int> preds;   // 前驱块索引
  vector<int> vregs;   // 每个前驱对应的 vreg
};

// ===== Mem2Reg Pass 主类 =====
class Mem2RegPass {
public:
  IRFunction *fn;
  DominatorTree dt;
  vector<unordered_set<int>> df;

  // 变量信息
  struct Variable {
    Symbol *sym;
    unordered_set<int> defBlocks;  // 定义该变量的基本块
  };

  vector<Variable> variables;
  unordered_map<Symbol*, int> varIndex;

  // Phi 节点
  vector<PhiNode> phis;
  vector<vector<int>> blockPhis;  // 每个基本块的 Phi 索引

  // 重命名栈
  vector<vector<int>> nameStack;

  explicit Mem2RegPass(IRFunction *f) : fn(f), dt(0) {}

  bool run() {
    if (fn->blocks.empty()) {
      irRefreshCFG(*fn);
    }

    int n = fn->blocks.size();
    if (n == 0) return false;

    // 1. 构建前驱列表
    vector<vector<int>> preds(n);
    for (int b = 0; b < n; b++) {
      for (int succ : fn->blocks[b].succ) {
        if (succ >= 0 && succ < n) {
          preds[succ].push_back(b);
        }
      }
    }

    // 2. 计算支配树
    dt = DominatorTree(n);
    dt.compute(preds);

    // 3. 计算支配前沿
    df = computeDominanceFrontier(dt, preds);

    // 4. 收集可提升变量
    collectVariables();
    if (variables.empty()) return false;

    // 5. 放置 Phi 节点
    placePhiNodes(preds);

    // 6. 重命名变量
    renameVariables();

    // 7. 物化 Phi 指令（关键修复：让 Phi 真正出现在 IR 中）
    materializePhis();

    // 8. 清理剩余的 LoadLocal/StoreLocal
    cleanup();

    return true;
  }

  // 判断变量是否可提升
  static bool isPromotable(Symbol *sym) {
    if (!sym) return false;
    if (sym->isArray) return false;
    if (sym->isParam) return false;
    if (sym->isGlobal) return false;
    return true;
  }

  // 收集可提升变量
  void collectVariables() {
    varIndex.clear();
    variables.clear();

    for (int b = 0; b < fn->blocks.size(); b++) {
      const auto &blk = fn->blocks[b];
      for (int idx = blk.begin; idx < blk.end; idx++) {
        const auto &inst = fn->insts[idx];
        if (inst.op == IROp::StoreLocal && isPromotable(inst.sym)) {
          if (varIndex.find(inst.sym) == varIndex.end()) {
            int idx = variables.size();
            varIndex[inst.sym] = idx;
            variables.push_back({inst.sym, {}});
          }
          variables[varIndex[inst.sym]].defBlocks.insert(b);
        }
      }
    }
  }

  // 放置 Phi 节点（迭代算法）
  void placePhiNodes(const vector<vector<int>> &preds) {
    int n = fn->blocks.size();
    int nVars = variables.size();

    blockPhis.assign(n, {});
    vector<vector<bool>> hasPhi(n, vector<bool>(nVars, false));

    for (int v = 0; v < nVars; v++) {
      const auto &defBlocks = variables[v].defBlocks;
      if (defBlocks.empty()) continue;

      // 工作列表：需要处理的基本块
      vector<int> workList(defBlocks.begin(), defBlocks.end());
      size_t idx = 0;

      while (idx < workList.size()) {
        int b = workList[idx++];

        // 对 b 的支配前沿中的每个节点 y
        for (int y : df[b]) {
          if (!hasPhi[y][v]) {
            // 在 y 插入 Phi 节点
            hasPhi[y][v] = true;

            PhiNode phi;
            phi.blockIdx = y;
            phi.var = variables[v].sym;
            phi.dstVreg = fn->nextVreg++;

            // 收集前驱
            for (int p : preds[y]) {
              phi.preds.push_back(p);
              phi.vregs.push_back(-1);  // 稍后填充
            }

            int phiIdx = phis.size();
            phis.push_back(phi);
            blockPhis[y].push_back(phiIdx);

            // 如果 y 不是定义块，加入工作列表继续传播
            if (defBlocks.find(y) == defBlocks.end()) {
              workList.push_back(y);
            }
          }
        }
      }
    }
  }

  // 重命名变量（支配树遍历）
  void renameVariables() {
    int nVars = variables.size();
    nameStack.assign(nVars, {});

    renameBlock(0);
  }

  void renameBlock(int blockIdx) {
    int nVars = variables.size();

    // 记录进入时的栈高度
    vector<int> stackHeights(nVars);
    for (int v = 0; v < nVars; v++) {
      stackHeights[v] = nameStack[v].size();
    }

    // 处理此基本块的 Phi 定义
    for (int phiIdx : blockPhis[blockIdx]) {
      auto &phi = phis[phiIdx];
      int vidx = varIndex[phi.var];
      nameStack[vidx].push_back(phi.dstVreg);
    }

    // 遍历指令
    auto &blk = fn->blocks[blockIdx];
    for (int i = blk.begin; i < blk.end; i++) {
      auto &inst = fn->insts[i];

      // 替换 LoadLocal 为当前栈顶 vreg
      if (inst.op == IROp::LoadLocal && isPromotable(inst.sym)) {
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
      // 替换 StoreLocal 为 push 新 vreg
      else if (inst.op == IROp::StoreLocal && isPromotable(inst.sym)) {
        auto it = varIndex.find(inst.sym);
        if (it != varIndex.end()) {
          int vidx = it->second;
          // Store 的值成为新的当前定义
          // inst.dst 已经有存储的值
          nameStack[vidx].push_back(inst.dst);
          // 标记为删除（稍后处理）
          inst.op = IROp::Nop;
        }
      }
    }

    // 填充后继 Phi 的前驱 vreg
    for (int succ : fn->blocks[blockIdx].succ) {
      for (int phiIdx : blockPhis[succ]) {
        auto &phi = phis[phiIdx];
        int vidx = varIndex[phi.var];

        // 找到当前块在 phi.preds 中的位置
        for (size_t i = 0; i < phi.preds.size(); i++) {
          if (phi.preds[i] == blockIdx) {
            if (!nameStack[vidx].empty()) {
              phi.vregs[i] = nameStack[vidx].back();
            }
            break;
          }
        }
      }
    }

    // 递归处理支配树子节点
    for (int child : dt.children[blockIdx]) {
      renameBlock(child);
    }

    // 恢复栈高度
    for (int v = 0; v < nVars; v++) {
      while ((int)nameStack[v].size() > stackHeights[v]) {
        nameStack[v].pop_back();
      }
    }
  }

  // 物化 Phi 节点：在基本块开头插入真正的 IROp::Phi 指令
  void materializePhis() {
    if (phis.empty()) return;

    // 按 block 分组已有的 Phi
    vector<vector<int>> phisByBlock(fn->blocks.size());
    for (size_t i = 0; i < phis.size(); ++i) {
      phisByBlock[phis[i].blockIdx].push_back(static_cast<int>(i));
    }

    // 从后往前处理块，避免插入后索引偏移
    for (int b = static_cast<int>(fn->blocks.size()) - 1; b >= 0; --b) {
      const auto &phiIdxs = phisByBlock[b];
      if (phiIdxs.empty()) continue;

      auto &blk = fn->blocks[b];
      // 找到插入点：跳过开头的 Label
      int insertPos = blk.begin;
      if (insertPos < blk.end && fn->insts[insertPos].op == IROp::Label) {
        ++insertPos;
      }

      // 为每个 Phi 构造指令并插入
      for (int phiIdx : phiIdxs) {
        const auto &p = phis[phiIdx];
        IRInst phiInst;
        phiInst.op = IROp::Phi;
        phiInst.dst = p.dstVreg;
        phiInst.sym = p.var;  // 保留符号信息供调试/分配使用

        // 把前驱 vreg 存到 args 里（Phi 的操作数）
        phiInst.args.clear();
        for (int v : p.vregs) {
          phiInst.args.push_back(v);
        }

        fn->insts.insert(fn->insts.begin() + insertPos, phiInst);
        ++blk.end;  // 当前块范围扩大
        ++insertPos;

        // 更新后续所有块的 begin/end
        for (size_t bb = static_cast<size_t>(b) + 1; bb < fn->blocks.size(); ++bb) {
          fn->blocks[bb].begin++;
          fn->blocks[bb].end++;
        }
      }
    }
  }

  // 清理剩余的 LoadLocal/StoreLocal
  void cleanup() {
    for (auto &inst : fn->insts) {
      if ((inst.op == IROp::LoadLocal || inst.op == IROp::StoreLocal) &&
          isPromotable(inst.sym)) {
        inst.op = IROp::Nop;
      }
    }
  }
};

// 外部接口
bool irMem2Reg(IRFunction &fn) {
  Mem2RegPass pass(&fn);
  return pass.run();
}
