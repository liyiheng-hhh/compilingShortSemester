// compiler2026-x phase-B (CDCL driver)
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

Clause *Solver::addClause(const std::vector<Atomic> &clause) {
  if (clause.empty()) {
    addedConflict = true;
    return nullptr;
  }

  std::vector<Atomic> processed;
  bool sat = false;

  // To initialize watchers, we need to find 2 atomics that haven't been assigned false.
  Atomic nonfalse[2];
  int top = 0;

  // Remove already known values.
  for (Atomic atom : clause) {
    Boolean val = value(atom);
    if (val == True) {
      sat = true;
      break;
    }
    if (val != False) {
      processed.push_back(atom);
      if (top < 2)
        nonfalse[top++] = atom;
    }
  }
  if (sat)
    return nullptr;

  if (processed.empty()) {
    addedConflict = true;
    return nullptr;
  }

  if (top == 1)
    nonfalse[1] = -1;

  Clause *added = new Clause { .content = processed, .watch = { nonfalse[0], nonfalse[1] } };
  clauses.push_back(added);

  // Set watchers.
  watched[nonfalse[0]].push_back(added);
  if (nonfalse[1] != -1)
    watched[nonfalse[1]].push_back(added);
  return added;
}

Clause *Solver::addLearnt(std::vector<Atomic> clause) {
  std::sort(clause.begin(), clause.end(), [&](Atomic a, Atomic b) {
    return decisionLevel[cdclVar(a)] > decisionLevel[cdclVar(b)];
  });
  
  if (clause.size() == 1) {
    Clause *added = new Clause { .content = clause, .watch = { clause[0], -1 } };
    clauses.push_back(added);

    watched[clause[0]].push_back(added);
    return added;
  }

  Clause *added = new Clause { .content = clause, .watch = { clause[0], clause[1] } };
  clauses.push_back(added);

  watched[clause[0]].push_back(added);
  watched[clause[1]].push_back(added);
  return added;
}

void Solver::init(int varcnt) {
  // These are indexed by variable.
  assignment.resize(varcnt);
  antecedent.resize(varcnt);
  decisionLevel.resize(varcnt);
  // These are index by atomics.
  watched.resize(varcnt * 2 + 1);
  activity.resize(varcnt);
  phase.resize(varcnt);

  unitConflict = false;

  this->varcnt = varcnt;
}

Atomic Solver::findUnit(const std::vector<Atomic> &unit) {
  int unassigned = 0;
  for (auto x : unit) {
    auto var = cdclVar(x);
    if (assignment[var] == Unassigned)
      return x;
  }
  assert(false);
}

// False for unsatisfiable.
bool Solver::solve(std::vector<signed char> &assignments) {
  assert(varcnt != -1);
  qhead = 0;
  dl = 0;
  conflict = 0;

  if (addedConflict) {
    addedConflict = false;
    return false;
  }
  unitPropagation();
  if (unitConflict) {
    unitConflict = false;
    return false;
  }

  while (!allAssigned()) {
    auto [var, val] = pickPivot();
    dl++;
    Atomic atom = val == True ? (var << 1) : ((var << 1) + 1);
    enqueue(atom, nullptr);

    for (;;) {
      unitPropagation();

      if (!unitConflict)
        break;
      
      unitConflict = false;
      auto [btlvl, learnt] = analyzeConflict();
      if (btlvl < 0 || learnt.empty())
        return false;

      Clause *cl = addLearnt(learnt);
      backtrack(btlvl);
      dl = btlvl;

      trail.clear();
      Atomic atom = findUnit(learnt);
      // The variable must be true. Do a unit propagation from here.
      qhead = 0;
      enqueue(atom, cl);
    }
  }

  // Clear clauses.
  for (auto clause : clauses)
    delete clause;
  clauses.clear();

  assignments.resize(varcnt);
  for (int i = 0; i < varcnt; i++)
    assignments[i] = (assignment[i] == True);
  
  return true;
}
