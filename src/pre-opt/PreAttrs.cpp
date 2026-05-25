#include "PreAttrs.h"
#include <sstream>

// compiler2026-x phase-D (trivial opt dedup)

using namespace sys;

namespace {

std::string attrJoinInts(const std::vector<int> &vals) {
  if (vals.empty())
    return "";
  std::stringstream ss;
  ss << vals[0];
  for (size_t i = 1; i < vals.size(); ++i)
    ss << ", " << vals[i];
  return ss.str();
}

} // namespace

std::string SubscriptAttr::toString() {
  return "<subscript = " + attrJoinInts(subscript) + ">";
}

std::string getValueNumber(Value value);

std::string BaseAttr::toString() {
  return "<base = " + getValueNumber(base->getResult()) + ">";
}

std::string ParallelizableAttr::toString() {
  if (!accum)
    return "<parallelizable>";
  return "<parallel accum = " + getValueNumber(accum->getResult()) + ">";
}
