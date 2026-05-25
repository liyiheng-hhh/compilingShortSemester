// compiler2026-x phase-2 (semantic split)
// compiler2026-x phase-3 (semantic const helpers)

#include "semantic.h"

using namespace std;

namespace {

size_t semInitSpan(const vector<int> &dims, size_t depth) {
  return depth >= dims.size() ? 1 : product(dims, depth);
}

size_t semPickInitChildDepth(const vector<int> &dims, size_t depth, int flatIndex) {
  size_t childDepth = min(depth + 1, dims.size());
  while (childDepth < dims.size()) {
    int childSize = product(dims, childDepth);
    if (childSize == 0 || flatIndex % childSize == 0) {
      break;
    }
    ++childDepth;
  }
  return childDepth;
}

}  // namespace

ConstValue Semantic::zeroConst(BaseType base) {
    ConstValue v;
    v.type = base;
    v.i = 0;
    v.f = 0.0f;
    return v;
  }

vector<ConstValue> Semantic::flattenConstInit(InitVal *init, const vector<int> &dims,
                                      BaseType base) {
    vector<ConstValue> values(product(dims), zeroConst(base));
    if (!init) {
      return values;
    }
    fillConstAggregate(init, dims, 0, 0, values, base);
    return values;
  }

int Semantic::fillConstAggregate(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<ConstValue> &values, BaseType base) {
    if (!init->isList) {
      if (start >= static_cast<int>(values.size())) {
        fail(init->expr ? init->expr->line : 0, "too many initializer elements");
      }
      visitExpr(init->expr.get());
      if (!init->expr->isConst) {
        fail(init->expr->line, "initializer is not constant");
      }
      values[start] = castConst(init->expr->constVal, base);
      return start + 1;
    }

    int subSize = semInitSpan(dims, depth);
    if (init->list.empty()) {
      return start + subSize;
    }
    int idx = start;
    for (auto &child : init->list) {
      if (child->isList) {
        size_t childDepth = semPickInitChildDepth(dims, depth, idx);
        idx = fillConstAggregate(child.get(), dims, childDepth, idx, values, base);
      } else {
        idx = fillConstAggregate(child.get(), dims, dims.size(), idx, values, base);
      }
      if (idx > start + subSize) {
        fail(child->expr ? child->expr->line : 0, "too many initializer elements");
      }
    }
    return start + subSize;
  }

size_t Semantic::chooseInitChildDepth(const vector<int> &dims, size_t depth, int flatIndex) {
    return semPickInitChildDepth(dims, depth, flatIndex);
  }
