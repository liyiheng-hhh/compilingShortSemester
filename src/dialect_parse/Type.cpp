#include <sstream>

#include "Type.h"

using namespace sys;

namespace {

std::string typJoinTypeStrings(const std::vector<Type*> &types) {
  std::stringstream ss;
  for (auto x : types)
    ss << x->toString() << ", ";
  auto str = ss.str();
  if (str.size() > 2) {
    str.pop_back();
    str.pop_back();
  }
  return str;
}

} // namespace

std::string FunctionType::toString() const {
  return "(" + typJoinTypeStrings(params) + ") -> " + ret->toString();
}

std::string ArrayType::toString() const {
  std::stringstream ss(base->toString());
  for (auto dim : dims)
    ss << "[" << dim << "]";
  return ss.str();
}

int ArrayType::getSize() const {
  int size = 1;
  for (auto dim : dims)
    size *= dim;
  return size;
}
