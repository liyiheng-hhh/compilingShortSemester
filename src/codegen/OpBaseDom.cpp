// compiler2026-x phase-E (dominance / post-dom analysis)

#include "OpBase.h"
#include "Attrs.h"
#include "Ops.h"

#include <map>
#include <unordered_map>
#include <vector>

using namespace sys;

namespace {

// DFN is the number of each node in DFS order.
using DFN = std::unordered_map<BasicBlock*, int>;
using BBMap = std::unordered_map<BasicBlock*, BasicBlock*>;

using Vertex = std::vector<BasicBlock*>;

// Semidominator of `u` is the node `v` with the smallest DFN,
// such that `v` dominates `u` on every path not going through its parent in the DFS tree.
using SDom = BBMap;
using Parent = BBMap;
using UnionFind = BBMap;
// Best ancestor found so far.
using Best = BBMap;

int num = 0;
int pnum = 0;

// Dominators.
DFN dfn;
SDom sdom;
Vertex vertex;
Parent parents;
UnionFind uf;
Best best;
// Post-dominators. Just a copy-paste.
DFN pdfn;
SDom psdom;
Vertex pvertex;
Parent pparents;
UnionFind puf;
Best pbest;


void obUpdateDFN(BasicBlock *current) {
  dfn[current] = num++;
  vertex.push_back(current);
  for (auto v : current->succs) {
    if (!dfn.count(v)) {
      parents[v] = current;
      obUpdateDFN(v);
    }
  }
}

void obUpdatePDFN(BasicBlock *current) {
  pdfn[current] = pnum++;
  pvertex.push_back(current);
  for (auto v : current->preds) {
    if (!pdfn.count(v)) {
      pparents[v] = current;
      obUpdatePDFN(v);
    }
  }
}

BasicBlock* obDomFind(BasicBlock *v) {
  if (uf[v] != v) {
    BasicBlock* u = obDomFind(uf[v]);
    if (dfn[sdom[best[uf[v]]]] < dfn[sdom[best[v]]])
      best[v] = best[uf[v]];
    uf[v] = u;
  }
  return uf[v];
}

BasicBlock* obPdomFind(BasicBlock* v) {
  if (puf[v] != v) {
    BasicBlock* u = obPdomFind(puf[v]);
    if (pdfn[psdom[pbest[puf[v]]]] < pdfn[psdom[pbest[v]]])
      pbest[v] = pbest[puf[v]];
    puf[v] = u;
  }
  return puf[v];
}


// Links `w` to `v` (setting the father of `w` to `v`).
void obDomLink(BasicBlock *v, BasicBlock *w) {
  uf[w] = v;
}

void obPdomPlink(BasicBlock *v, BasicBlock *w) {
  puf[w] = v;
}

}

// Use the Langauer-Tarjan approach.
// https://www.cs.princeton.edu/courses/archive/fall03/cs528/handouts/a%20fast%20algorithm%20for%20finding.pdf
// Loop unrolling might update dominators very frequently, and it's quite time consuming.
void Region::updateDoms() {
  updatePreds();
  // Clear existing data.
  for (auto bb : bbs) {
    bb->doms.clear();
    bb->idom = nullptr;
  }

  // Clear global data as well.
  dfn.clear();
  vertex.clear();
  parents.clear();
  sdom.clear();
  uf.clear();
  best.clear();
  num = 1;

  // For each `u` as key, it contains all blocks that it semi-dominates.
  // 'b' for bucket.
  std::map<BasicBlock*, std::vector<BasicBlock*>> bsdom;

  auto entry = getFirstBlock();
  obUpdateDFN(entry);

  for (auto bb : bbs) {
    sdom[bb] = bb;
    uf[bb] = bb;
    best[bb] = bb;
  }

  // Deal with every block in reverse dfn order.
  for (auto it = vertex.rbegin(); it != vertex.rend(); it++) {
    auto bb = *it;
    for (auto v : bb->preds) {
      // Unreachable. Skip it.
      if (!dfn.count(v))
        continue;
      BasicBlock *u;
      if (dfn[v] < dfn[bb])
        u = v;
      else {
        obDomFind(v);
        u = best[v];
      }
      if (dfn[sdom[u]] < dfn[sdom[bb]])
        sdom[bb] = sdom[u];
    }

    bsdom[sdom[bb]].push_back(bb);
    auto parent = parents.count(bb) ? parents[bb] : nullptr;
    if (!parent)
      continue;
    obDomLink(parent, bb);

    for (auto v : bsdom[parent]) {
      obDomFind(v);
      v->idom = sdom[best[v]] == sdom[v] ? parent : best[v];
    }
  }

  // Find idom, but ignore the entry block (which has no idom).
  for (int i = 1; i < vertex.size(); ++i) {
    auto bb = vertex[i];
    assert(bb->idom);
    if (bb->idom != sdom[bb])
      bb->idom = bb->idom->idom;
  }
}

void Region::updateDomFront() {
  updateDoms();
  for (auto bb : bbs)
    bb->domFront.clear();

  // Update dominance frontier.
  // See https://en.wikipedia.org/wiki/Static_single-assignment_form#Computing_minimal_SSA_using_dominance_frontiers
  // For each block, if it has at least 2 preds, then it must be at dominance frontier of all its `preds`,
  // till its `idom`.
  for (auto bb : bbs) {
    if (!dfn.count(bb))
      continue;
    if (!bb->idom)
      continue;
    if (bb->preds.size() < 2)
      continue;

    for (auto pred : bb->preds) {
      if (!pred || !dfn.count(pred))
        continue;
      auto runner = pred;
      int guard = (int) bbs.size() + 2;
      while (runner && runner != bb->idom && guard-- > 0) {
        runner->domFront.insert(bb);
        if (runner->idom == runner)
          break;
        runner = runner->idom;
      }
    }
  }
}

// A dual of updateDoms().
void Region::updatePDoms() {
  updatePreds();

  std::vector<BasicBlock*> exits;
  for (auto bb : bbs) {
    if (!bb || bb->getOpCount() == 0)
      continue;
    auto *last = bb->getLastOp();
    if (last && isa<ReturnOp>(last))
      exits.push_back(bb);
  }

  if (exits.size() != 1) {
    for (auto bb : bbs) {
      bb->pdoms.clear();
      bb->ipdom = nullptr;
    }
    return;
  }

  auto exit = exits[0];

  for (auto bb : bbs) {
    bb->pdoms.clear();
    bb->ipdom = nullptr;
  }

  pdfn.clear();
  pvertex.clear();
  pparents.clear();
  psdom.clear();
  puf.clear();
  pbest.clear();
  pnum = 1;

  std::map<BasicBlock*, std::vector<BasicBlock*>> pbsdom;

  obUpdatePDFN(exit);

  for (auto bb : bbs) {
    psdom[bb] = bb;
    puf[bb] = bb;
    pbest[bb] = bb;
  }

  for (auto it = pvertex.rbegin(); it != pvertex.rend(); ++it) {
    auto bb = *it;
    for (auto v : bb->succs) {
      if (!pdfn.count(v))
        continue;
      BasicBlock *u;
      if (pdfn[v] < pdfn[bb])
        u = v;
      else {
        obPdomFind(v);
        u = pbest[v];
      }
      if (pdfn[psdom[u]] < pdfn[psdom[bb]])
        psdom[bb] = psdom[u];
    }

    pbsdom[psdom[bb]].push_back(bb);
    obPdomPlink(pparents[bb], bb);

    for (auto *v : pbsdom[pparents[bb]]) {
      obPdomFind(v);
      v->ipdom = (psdom[pbest[v]] == psdom[v]) ? pparents[bb] : pbest[v];
    }
  }

  for (size_t i = 1; i < pvertex.size(); ++i) {
    auto bb = pvertex[i];
    assert(bb->ipdom);
    if (bb->ipdom != psdom[bb])
      bb->ipdom = bb->ipdom->ipdom;
  }
}
