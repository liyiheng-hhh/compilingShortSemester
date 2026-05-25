// compiler2026-x phase-2 (dialect_parse parser split)

#include "Parser.h"
#include "ASTNode.h"
#include "Lexer.h"
#include "Type.h"
#include "TypeContext.h"
#include <cstdlib>
#include <ostream>
#include <vector>
#include <sstream>

using namespace sys;
int ConstValue::size() {
  int total = 1;
  for (auto x : dims)
    total *= x;
  return total;
}

int ConstValue::stride() {
  int stride = 1;
  for (size_t i = 1; i < dims.size(); i++)
    stride *= dims[i];
  return stride;
}

std::ostream &operator<<(std::ostream &os, ConstValue value) {
  auto sz = value.size();
  auto vi = (int*) value.getRawRef();
  os << vi[0];
  for (int i = 1; i < sz; i++)
    os << ", " << vi[i];
  
  return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<int> vec) {
  if (vec.size() > 0)
    os << vec[0];
  for (int i = 1; i < vec.size(); i++)
    os << ", " << vec[i];
  return os;
}

ConstValue ConstValue::operator[](int i) {
  assert(dims.size() >= 1);

  std::vector<int> newDims;
  newDims.reserve(dims.size() - 1);

  for (size_t i = 1; i < dims.size(); i++) 
    newDims.push_back(dims[i]);
  
  return ConstValue(vi + i * stride(), newDims);
};

int *ConstValue::getRaw() {
  auto total = size();
  auto result = new int[total];
  memcpy(result, vi, total * sizeof(int));
  return result;
}

float *ConstValue::getRawFloat() {
  auto total = size();
  auto result = new float[total];
  memcpy(result, vi, total * sizeof(float));
  return result;
}

void ConstValue::release() {
  delete[] vi;
}

int ConstValue::getInt() {
  assert((!dims.size() || (dims[0] == 1 && dims.size() == 1)));
  if (isFloat)
    return *vf;
  return *vi;
}

float ConstValue::getFloat() {
  assert((!dims.size() || (dims[0] == 1 && dims.size() == 1)));
  if (isFloat)
    return *vf;
  return *vi;
}
