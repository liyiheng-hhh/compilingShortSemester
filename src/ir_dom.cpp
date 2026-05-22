#include "ir_dom.h"

#include <algorithm>
#include <functional>
#include <queue>

using namespace std;

void IrDomTree::build(IRFunction &fn) {
  irRefreshCFG(fn);
  n = static_cast<int>(fn.blocks.size());
  idom.assign(n, -1);
  children.assign(n, {});
  if (n == 0) {
    return;
  }

  vector<vector<int>> preds(n), succ(n);
  for (int b = 0; b < n; b++) {
    for (int s : fn.blocks[static_cast<size_t>(b)].succ) {
      if (s >= 0 && s < n) {
        preds[s].push_back(b);
        succ[b].push_back(s);
      }
    }
  }

  idom[0] = 0;
  vector<int> rpo;
  rpo.reserve(n);
  vector<char> vis(n, 0);
  function<void(int)> dfs = [&](int u) {
    if (u < 0 || u >= n || vis[u]) return;
    vis[u] = 1;
    for (int v : succ[u]) {
      if (v >= 0 && v < n && !vis[v]) dfs(v);
    }
    rpo.push_back(u);
  };
  dfs(0);

  vector<int> order(n, -1);
  for (int i = 0; i < static_cast<int>(rpo.size()); i++) {
    int b = rpo[i];
    if (b >= 0 && b < n) order[b] = i;
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
    for (int ri = static_cast<int>(rpo.size()) - 1; ri >= 1; --ri) {
      int b = rpo[static_cast<size_t>(ri)];
      if (b == 0) continue;
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

  idom[0] = 0;
  for (int i = 1; i < n; i++) {
    if (idom[i] < 0) idom[i] = 0;
  }
  for (int i = 1; i < n; i++) {
    if (idom[i] == i && !preds[i].empty()) {
      idom[i] = preds[i][0];
    }
  }
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

  for (int i = 0; i < n; i++) {
    if (idom[i] >= 0 && idom[i] != i) {
      children[idom[i]].push_back(i);
    }
  }
}
