// compiler2026-x phase-B (CDCL dump helpers)
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

void dumpAtomic(std::ostream &os, Atomic atom) {
  auto var = cdclVar(atom) + 1;
  if (cdclIsNeg(atom))
    os << "!" << var;
  else
    os << var;
}

void Clause::dump(std::ostream &os) const {
  os << "{ ";
  if (content.size() > 0)
    dumpAtomic(os, content[0]);
  for (int i = 1; i < content.size(); i++) {
    os << ", ";
    dumpAtomic(os, content[i]);
  }
  os << " } (watching: ";
  dumpAtomic(os, watch[0]);
  os << ", ";
  dumpAtomic(os, watch[1]);
  os << ")";
}

void Solver::dump(std::ostream &os) const {
  os << "varcnt = " << varcnt << "\n";
  for (auto x : clauses)
    os << x << "\n";
  for (int i = 0; i < varcnt; i++)
    os << i + 1 << " = " << (int) assignment[i] << "\n";
}

void dumpArray(const std::vector<Atomic> &content) {
  auto &os = std::cerr;
  os << "{ ";
  if (content.size() > 0)
    dumpAtomic(os, content[0]);
  for (int i = 1; i < content.size(); i++) {
    os << ", ";
    dumpAtomic(os, content[i]);
  }
  os << " }";
}

Solver::Boolean Solver::value(Atomic atom) {
  Variable var = cdclVar(atom);
  Boolean logic = assignment[var];
  return cdclIsNeg(atom) ? cdclLogicNot(logic) : logic;
}
