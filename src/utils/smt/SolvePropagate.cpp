// compiler2026-x phase-B (SMT clause encodings)
#include "SMT.h"
#include <cassert>

using namespace smt;

void BvSolver::addAnd(Variable out, Variable a, Variable b) {
  reserve({ ctx.neg(a), ctx.neg(b), ctx.pos(out) });
  reserve({ ctx.pos(a), ctx.neg(out) });
  reserve({ ctx.pos(b), ctx.neg(out) });
}

void BvSolver::addOr(Variable out, Variable a, Variable b) {
  reserve({ ctx.pos(a), ctx.pos(b), ctx.neg(out) });
  reserve({ ctx.neg(a), ctx.pos(out) });
  reserve({ ctx.neg(b), ctx.pos(out) });
}

void BvSolver::addXor(Variable out, Variable a, Variable b) {
  reserve({ ctx.neg(a), ctx.neg(b), ctx.neg(out) });
  reserve({ ctx.pos(a), ctx.pos(b), ctx.neg(out) });
  reserve({ ctx.pos(a), ctx.neg(b), ctx.pos(out) });
  reserve({ ctx.neg(a), ctx.pos(b), ctx.pos(out) });
}

void BvSolver::addNot(Variable out, Variable a) {
  reserve({ ctx.neg(a), ctx.neg(out) });
  reserve({ ctx.pos(a), ctx.pos(out) });
}

// Equivalent to (a & !b).
// Just change the polarity of `b` in addAnd.
void BvSolver::addAndNot(Variable out, Variable a, Variable b) {
  reserve({ ctx.neg(a), ctx.pos(b), ctx.pos(out) });
  reserve({ ctx.pos(a), ctx.neg(out) });
  reserve({ ctx.neg(b), ctx.neg(out) });
}

void BvSolver::addXnor(Variable out, Variable a, Variable b) {
  reserve({ ctx.neg(out), ctx.pos(a), ctx.neg(b) });
  reserve({ ctx.neg(out), ctx.neg(a), ctx.pos(b) });
  reserve({ ctx.pos(out), ctx.pos(a), ctx.pos(b) });
  reserve({ ctx.pos(out), ctx.neg(a), ctx.neg(b) });
}
