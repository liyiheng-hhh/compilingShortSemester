// compiler2026-x phase-B (CDCL conflict analysis / decisions)
#include "CDCL.h"
#include <algorithm>
#include <set>
#include <cassert>
#include <unordered_set>

using namespace smt;

#define cdclVar(atom) ((atom) >> 1)
#define cdclLogicNot(logic) (-(logic))
#define cdclAtomNot(atom) ((atom) ^ 1)
#define cdclIsNeg(atom) ((atom) & 1)

std::pair<int, std::vector<Atomic>> Solver::analyzeConflict() {
  if (!dl)
    return { -1, {} };

  const auto &conflict = conflictClause->content;
  std::unordered_set<Atomic> learnt(conflict.begin(), conflict.end());
  // Atomics of the conflicting clause at current decision level.
  std::unordered_set<Atomic> atomics;
  for (auto atom : conflictClause->content) {
    if (decisionLevel[cdclVar(atom)] == dl)
      atomics.insert(atom);
  }

  while (atomics.size() != 1) {
    Atomic implied = -1;
    for (auto atom : atomics) {
      if (antecedent[cdclVar(atom)]) {
        implied = atom;
        break;
      }
    }

    assert(implied != -1);

    auto ante = antecedent[cdclVar(implied)];
    // Increase activity of antecedent.
    for (auto atom : ante->content) {
      Variable v = cdclVar(atom);
      activity[v] += inc;
      vheap.push({ activity[v], v });
    }
    
    for (auto atom : ante->content)
      learnt.insert(atom);
    
    learnt.erase(implied);
    learnt.erase(cdclAtomNot(implied));

    atomics.clear();
    for (auto atom : learnt) {
      if (decisionLevel[cdclVar(atom)] == dl)
        atomics.insert(atom);
    }
  }

  inc /= decay;
  // Scale activity to prevent overflow.
  if (inc >= 1e100) {
    inc *= 1e-100;
    for (int i = 0; i < varcnt; i++)
      activity[i] *= 1e-100;
  }
  
  // Backtrack to the second largest decision level.
  // If that doesn't exist, backtrack to dl = 0.
  std::vector<Atomic> result(learnt.begin(), learnt.end());

  if (result.size() <= 1)
    return { 0, result };
  else {
    std::set<int, std::greater<int>> dls;
    for (auto x : result)
      dls.insert(decisionLevel[cdclVar(x)]);
    return { *++dls.begin(), result };
  }
}
void Solver::backtrack(int level) {
  // Remove all assignments done above the backtrack level.
  for (int i = 0; i < varcnt; i++) {
    if (decisionLevel[i] > level) {
      assignment[i] = Unassigned;
      antecedent[i] = nullptr;
      decisionLevel[i] = 0;
    }
  }
}

bool Solver::allAssigned() {
  for (auto x : assignment) {
    if (x == Unassigned)
      return false;
  }
  return true;
}

std::pair<Variable, Solver::Boolean> Solver::pickPivot() {
  // If the heap is too large, rebuild it.
  if (vheap.size() > varcnt * 8) {
    decltype(vheap) newHeap;
    for (Variable v = 0; v < varcnt; v++) {
      if (assignment[v] == Unassigned)
        newHeap.push({ activity[v], v });
    }
    vheap = std::move(newHeap);
  }

  while (!vheap.empty()) {
    auto [score, v] = vheap.top();
    vheap.pop();

    // Stale.
    if (score < activity[v])
      continue;

    if (assignment[v] == Unassigned)
      return { v, phase[v] == Unassigned ? True : phase[v] };
  }

  // Pick the first unassigned variable if heap is exhausted.
  for (Variable v = 0; v < varcnt; v++) {
    if (assignment[v] == Unassigned)
      return { v, phase[v] == Unassigned ? True : phase[v] };
  }
  assert(false);
}
