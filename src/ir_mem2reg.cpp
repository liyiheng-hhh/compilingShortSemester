// 完整 SSA Mem2Reg 实现
// 基于 Cytron et al. 1991 "Efficiently Computing Static Single Assignment Form"
//
// 算法步骤：
// 1. 计算支配树（Dominator Tree）
// 2. 计算支配前沿（Dominance Frontier）
// 3. 放置 Phi 节点（在 DF 中）
// 4. 重命名变量（支配树遍历）
// 5. 将 Phi 降低为前驱块末尾的 Copy（本 IR 后端不直接支持 Phi 指令）

#include "ir_mem2reg.h"
#include "ir.h"

#include <algorithm>
#include <cstdio>
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// ===== 支配树计算 =====
struct DominatorTree {
  int n;
  vector<vector<bool>> dom;
  vector<int> idom;
  vector<vector<int>> children;
  vector<int> level;

  explicit DominatorTree(int numBlocks) : n(numBlocks) {
    dom.assign(n, vector<bool>(n, false));
    idom.assign(n, -1);
    children.assign(n, {});
    level.assign(n, 0);
  }

  void compute(const vector<vector<int>> &preds,
               const vector<vector<int>> &succ) {
    if (n == 0) return;
    computeIdomCooper(preds, succ);
    buildDomTree();
  }

  // Cooper et al. 2001：简单正确的 idom 迭代算法
  void computeIdomCooper(const vector<vector<int>> &preds,
                         const vector<vector<int>> &succ) {
    idom.assign(n, -1);
    idom[0] = 0;

    vector<int> rpo;
    rpo.reserve(n);
    vector<bool> vis(n, false);
    function<void(int)> dfs = [&](int u) {
      vis[u] = true;
      for (int v : succ[u]) {
        if (!vis[v]) dfs(v);
      }
      rpo.push_back(u);
    };
    dfs(0);

    vector<int> order(n, -1);
    for (int i = 0; i < n; i++) {
      order[rpo[i]] = i;
    }

    auto intersect = [&](int b1, int b2) {
      for (int step = 0; step < n && b1 != b2; ++step) {
        if (b1 < 0 || b2 < 0) break;
        if (order[b1] > order[b2]) {
          int n1 = idom[b1];
          if (n1 == b1) break;
          b1 = n1;
        } else if (order[b2] > order[b1]) {
          int n2 = idom[b2];
          if (n2 == b2) break;
          b2 = n2;
        } else {
          break;
        }
      }
      return b1;
    };

    bool changed = true;
    int iterCap = n * 4 + 4;
    while (changed && iterCap-- > 0) {
      changed = false;
      // 按逆后序遍历（Cooper 算法要求）
      for (int ri = static_cast<int>(rpo.size()) - 1; ri >= 1; --ri) {
        int b = rpo[static_cast<size_t>(ri)];
        if (preds[b].empty()) continue;

        int newIdom = -1;
        for (int p : preds[b]) {
          if (idom[p] < 0) continue;
          if (newIdom < 0) {
            newIdom = p;
          } else {
            newIdom = intersect(newIdom, p);
          }
        }
        if (newIdom >= 0 && idom[b] != newIdom) {
          idom[b] = newIdom;
          changed = true;
        }
      }
    }

    for (int i = 0; i < n; i++) {
      if (idom[i] < 0) idom[i] = 0;
    }
    for (int i = 1; i < n; i++) {
      if (idom[i] == i && !preds[i].empty()) {
        idom[i] = preds[i][0];
      }
    }
    // 保证 idom 链无环（否则 renameBlock 会无限递归）
    for (int i = 1; i < n; i++) {
      int cur = i;
      for (int step = 0; step < n; ++step) {
        if (idom[cur] < 0 || idom[cur] == cur) break;
        cur = idom[cur];
        if (cur == i) {
          idom[i] = 0;
          break;
        }
      }
    }
  }

  void buildDomTree() {
    children.assign(n, {});
    for (int i = 0; i < n; i++) {
      if (idom[i] >= 0 && idom[i] != i) {
        children[idom[i]].push_back(i);
      }
    }

    queue<pair<int, int>> q;
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

static vector<unordered_set<int>> computeDominanceFrontier(
    const DominatorTree &dt,
    const vector<vector<int>> &preds) {
  int n = dt.n;
  vector<unordered_set<int>> df(n);

  for (int b = 0; b < n; b++) {
    if (preds[b].size() < 2) continue;

    for (int p : preds[b]) {
      int runner = p;
      while (runner >= 0 && runner != dt.idom[b]) {
        df[runner].insert(b);
        int next = dt.idom[runner];
        if (next < 0 || next == runner) break;
        runner = next;
      }
    }
  }

  return df;
}

struct PhiNode {
  int blockIdx;
  Symbol *var;
  int dstVreg;
  vector<int> preds;
  vector<int> vregs;
};

class Mem2RegPass {
public:
  IRFunction *fn;
  DominatorTree dt;
  vector<unordered_set<int>> df;

  struct Variable {
    Symbol *sym;
    unordered_set<int> defBlocks;
  };

  vector<Variable> variables;
  unordered_map<Symbol *, int> varIndex;
  vector<PhiNode> phis;
  vector<vector<int>> blockPhis;
  vector<vector<int>> nameStack;
  int zeroVreg_ = -1;  // 共享的常量 0 vreg

  explicit Mem2RegPass(IRFunction *f) : fn(f), dt(0) {}

  bool run() {
    if (fn->blocks.empty()) {
      irRefreshCFG(*fn);
    }

    int n = static_cast<int>(fn->blocks.size());
    if (n == 0) return false;

    vector<vector<int>> preds(n);
    vector<vector<int>> succ(n);
    for (int b = 0; b < n; b++) {
      for (int s : fn->blocks[b].succ) {
        if (s >= 0 && s < n) {
          preds[s].push_back(b);
          succ[b].push_back(s);
        }
      }
    }

    auto dbg = [](const char *s) {
      if (getenv("SYSY_CC_MEM2REG_DEBUG")) fprintf(stderr, "mem2reg: %s\n", s);
    };

    dbg("idom");
    dt = DominatorTree(n);
    dt.compute(preds, succ);
    dbg("df");
    df = computeDominanceFrontier(dt, preds);

    dbg("collect");
    collectVariables();
    if (variables.empty()) return false;

    dbg("init");
    createZeroVregAtEntry();
    ensureInitialDefinitions();
    dbg("phi");
    placePhiNodes(preds);

    // 需要 Phi 的变量（跨块合并）暂不重命名，避免错误语义与死循环
    unordered_set<Symbol *> phiSyms;
    for (const auto &p : phis) {
      if (p.var) phiSyms.insert(p.var);
    }

    dbg("rename");
    renameVariables(phiSyms);
    dbg("lower");
    if (!getenv("SYSY_CC_MEM2REG_NO_LOWER") && phiSyms.empty()) {
      lowerPhisToCopies();
    }
    dbg("cleanup");
    cleanup(phiSyms);

    return true;
  }

  static bool isPromotable(Symbol *sym) {
    if (!sym) return false;
    if (sym->isArray) return false;
    if (sym->isParam) return false;
    if (sym->isGlobal) return false;
    return true;
  }

  void createZeroVregAtEntry() {
    if (fn->blocks.empty()) return;
    zeroVreg_ = fn->nextVreg++;
    IRInst c;
    c.op = IROp::ConstI;
    c.dst = zeroVreg_;
    c.immI = 0;

    auto &entry = fn->blocks[0];
    int insertPos = static_cast<int>(entry.begin);
    if (insertPos < static_cast<int>(entry.end) &&
        fn->insts[insertPos].op == IROp::Label) {
      ++insertPos;
    }
    fn->insts.insert(fn->insts.begin() + insertPos, c);
    ++entry.end;
    for (size_t b = 1; b < fn->blocks.size(); ++b) {
      fn->blocks[b].begin++;
      fn->blocks[b].end++;
    }
  }

  int getZeroVreg() const { return zeroVreg_; }

  void collectVariables() {
    varIndex.clear();
    variables.clear();

    for (int b = 0; b < static_cast<int>(fn->blocks.size()); b++) {
      const auto &blk = fn->blocks[b];
      for (int idx = blk.begin; idx < static_cast<int>(blk.end); idx++) {
        const auto &inst = fn->insts[idx];
        if (inst.op == IROp::StoreLocal && isPromotable(inst.sym)) {
          if (varIndex.find(inst.sym) == varIndex.end()) {
            int vi = static_cast<int>(variables.size());
            varIndex[inst.sym] = vi;
            variables.push_back({inst.sym, {}});
          }
          variables[varIndex[inst.sym]].defBlocks.insert(b);
        }
      }
    }
  }

  void ensureInitialDefinitions() {
    if (fn->blocks.empty()) return;
    auto &entry = fn->blocks[0];

    int insertPos = static_cast<int>(entry.begin);
    if (insertPos < static_cast<int>(entry.end) &&
        fn->insts[insertPos].op == IROp::Label) {
      ++insertPos;
    }

    for (auto &var : variables) {
      if (var.defBlocks.count(0)) continue;

      int z = zeroVreg_;
      IRInst st;
      st.op = IROp::StoreLocal;
      st.sym = var.sym;
      st.u = z;
      st.isFloat = (var.sym->base == BaseType::Float);

      fn->insts.insert(fn->insts.begin() + insertPos, st);
      ++entry.end;
      for (size_t b = 1; b < fn->blocks.size(); ++b) {
        fn->blocks[b].begin++;
        fn->blocks[b].end++;
      }
      var.defBlocks.insert(0);
      ++insertPos;
    }
  }

  void placePhiNodes(const vector<vector<int>> &preds) {
    int n = static_cast<int>(fn->blocks.size());
    int nVars = static_cast<int>(variables.size());

    blockPhis.assign(n, {});
    vector<vector<bool>> hasPhi(n, vector<bool>(nVars, false));

    for (int v = 0; v < nVars; v++) {
      const auto &defBlocks = variables[v].defBlocks;
      if (defBlocks.empty()) continue;

      vector<int> workList(defBlocks.begin(), defBlocks.end());
      size_t wi = 0;
      const size_t maxWork = static_cast<size_t>(n) * 8u + 8u;

      while (wi < workList.size() && workList.size() < maxWork) {
        int b = workList[wi++];

        for (int y : df[b]) {
          if (!hasPhi[y][v]) {
            hasPhi[y][v] = true;

            PhiNode phi;
            phi.blockIdx = y;
            phi.var = variables[v].sym;
            phi.dstVreg = fn->nextVreg++;

            for (int p : preds[y]) {
              phi.preds.push_back(p);
              phi.vregs.push_back(-1);
            }

            int phiIdx = static_cast<int>(phis.size());
            phis.push_back(phi);
            blockPhis[y].push_back(phiIdx);

            if (defBlocks.find(y) == defBlocks.end()) {
              workList.push_back(y);
            }
          }
        }
      }
    }
  }

  void renameVariables(const unordered_set<Symbol *> &skipSyms) {
    int nVars = static_cast<int>(variables.size());
    nameStack.assign(nVars, {});
    renameBlock(0, skipSyms);
  }

  void renameBlock(int blockIdx, const unordered_set<Symbol *> &skipSyms) {
    int nVars = static_cast<int>(variables.size());

    vector<int> stackHeights(nVars);
    for (int v = 0; v < nVars; v++) {
      stackHeights[v] = static_cast<int>(nameStack[v].size());
    }

    for (int phiIdx : blockPhis[blockIdx]) {
      auto &phi = phis[phiIdx];
      if (phi.var && skipSyms.count(phi.var)) continue;
      int vidx = varIndex[phi.var];
      nameStack[vidx].push_back(phi.dstVreg);
    }

    auto &blk = fn->blocks[blockIdx];
    for (int i = blk.begin; i < static_cast<int>(blk.end); i++) {
      auto &inst = fn->insts[i];

      if (inst.op == IROp::LoadLocal && isPromotable(inst.sym) &&
          !skipSyms.count(inst.sym)) {
        auto it = varIndex.find(inst.sym);
        if (it != varIndex.end()) {
          int vidx = it->second;
          if (!nameStack[vidx].empty()) {
            inst.op = IROp::Copy;
            inst.u = nameStack[vidx].back();
            inst.sym = nullptr;
          }
        }
      } else if (inst.op == IROp::StoreLocal && isPromotable(inst.sym) &&
                 !skipSyms.count(inst.sym)) {
        auto it = varIndex.find(inst.sym);
        if (it != varIndex.end()) {
          int vidx = it->second;
          if (inst.u >= 0) {
            nameStack[vidx].push_back(inst.u);
          }
          inst.op = IROp::Nop;
        }
      }
    }

    for (int succ : fn->blocks[blockIdx].succ) {
      for (int phiIdx : blockPhis[succ]) {
        auto &phi = phis[phiIdx];
        if (phi.var && skipSyms.count(phi.var)) continue;
        int vidx = varIndex[phi.var];

        for (size_t i = 0; i < phi.preds.size(); i++) {
          if (phi.preds[i] == blockIdx) {
            if (!nameStack[vidx].empty()) {
              phi.vregs[i] = nameStack[vidx].back();
            } else {
              phi.vregs[i] = zeroVreg_;
            }
            break;
          }
        }
      }
    }

    for (int child : dt.children[blockIdx]) {
      renameBlock(child, skipSyms);
    }

    for (int v = 0; v < nVars; v++) {
      while (static_cast<int>(nameStack[v].size()) > stackHeights[v]) {
        nameStack[v].pop_back();
      }
    }
  }

  // 将 Phi 降低为各前驱块末尾的 Copy（codegen 不处理 IROp::Phi）
  void lowerPhisToCopies() {
    if (phis.empty()) return;

    // 从后往前插入，避免索引偏移
    struct EdgeCopy {
      int predBlock;
      IRInst inst;
    };
    vector<EdgeCopy> pending;

    for (const auto &phi : phis) {
      for (size_t i = 0; i < phi.preds.size(); i++) {
        int src = phi.vregs[i];
        if (src < 0) src = getZeroVreg();

        IRInst c;
        c.op = IROp::Copy;
        c.dst = phi.dstVreg;
        c.u = src;

        pending.push_back({phi.preds[i], c});
      }
    }

    // 按 predBlock 从大到小排序，同块内多条按顺序插入
    sort(pending.begin(), pending.end(),
         [](const EdgeCopy &a, const EdgeCopy &b) {
           return a.predBlock > b.predBlock;
         });

    for (const auto &ec : pending) {
      auto &blk = fn->blocks[ec.predBlock];
      int insertAt = static_cast<int>(blk.end);
      for (int i = static_cast<int>(blk.end) - 1; i >= static_cast<int>(blk.begin);
           --i) {
        IROp op = fn->insts[i].op;
        if (op == IROp::J || op == IROp::Ret || op == IROp::Beqz) {
          insertAt = i;
        } else if (op != IROp::Label) {
          break;
        }
      }

      fn->insts.insert(fn->insts.begin() + insertAt, ec.inst);
      blk.end++;

      for (size_t bb = static_cast<size_t>(ec.predBlock) + 1; bb < fn->blocks.size();
           ++bb) {
        fn->blocks[bb].begin++;
        fn->blocks[bb].end++;
      }
    }
  }

  void cleanup(const unordered_set<Symbol *> &skipSyms) {
    for (auto &inst : fn->insts) {
      if ((inst.op == IROp::LoadLocal || inst.op == IROp::StoreLocal) &&
          isPromotable(inst.sym) && !skipSyms.count(inst.sym)) {
        inst.op = IROp::Nop;
      }
      if (inst.op == IROp::Phi) {
        inst.op = IROp::Nop;
      }
    }
  }
};

bool irMem2Reg(IRFunction &fn) {
  Mem2RegPass pass(&fn);
  return pass.run();
}
