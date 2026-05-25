// compiler2026-x phase-B (CDCL unit propagation)
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

// Unit propagation tries to build up edges in implied graph, and also detect conflicts.
void Solver::unitPropagation() {
  while (qhead < trail.size()) {
    Atomic atom = trail[qhead++];
    Atomic neg = cdclAtomNot(atom);

    // In unit propagation, the things we pushed onto queue must have been true.
    // When the atomic is (!v), then v must be false;
    // When the atomic is v, then v must be true.
    assignment[cdclVar(atom)] = cdclIsNeg(atom) ? False : True;

    // Therefore, `neg` is false. Check whether we have something watched on it.
    auto &watches = watched[neg];
    for (int i = 0; i < watches.size();) {
      auto clause = watches[i];

      // Find the other watched literal of the clause.
      int watchid = neg != clause->watch[0];
      Atomic other = clause->watch[!watchid];
      
      // The clause has been satisfied.
      if (other != -1 && value(other) == True) {
        i++;
        continue;
      }

      // Try to find another literal to watch in the clause.
      // When there's only 2 literals, no other things are possible.
      if (clause->content.size() > 2) {
        Atomic watch = -1;
        for (auto atomic : clause->content) {
          if (atomic == neg || atomic == other)
            continue;

          if (value(atomic) != False) {
            watch = atomic;
            break;
          }
        }

        if (watch != -1) {
          clause->watch[watchid] = watch;
          // Remove the current clause from watchers of `neg`.
          watches[i] = watches.back();
          watches.pop_back();
          // Add the current clause to watchers of `watch`.
          watched[watch].push_back(clause);
          // Don't increment `i` because the current element has changed to a new one.
          continue;
        }
      }

      // No other literals to watch. We can determine the final literal.
      
      // Assigned false. A conflict.
      if (other == -1 || value(other) == False) {
        // Increase activity.
        for (auto atom : clause->content) {
          Variable v = cdclVar(atom);
          activity[v] += inc;
          vheap.push({ activity[v], v });
        }

        // Conflict.
        unitConflict = true;
        conflictClause = clause;
        return;
      }

      // Trigger a new round of unit propagation.
      if (value(other) == Unassigned) {
        enqueue(other, clause);
        i++;
      }
    }
  }
}
void Solver::enqueue(Atomic atom, Clause* ante) {
  int var = cdclVar(atom);
  if (assignment[var] != 0)
    return;
  
  Boolean result = !cdclIsNeg(atom) ? True : False;
  phase[var] = result;
  assignment[var] = result;
  antecedent[var] = ante;
  decisionLevel[var] = dl;
  trail.push_back(atom);
}
