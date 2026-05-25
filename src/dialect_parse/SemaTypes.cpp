// compiler2026-x phase-2 (dialect_parse sema split)

#include "Sema.h"
#include "ASTNode.h"
#include "../utils/DynamicCast.h"
#include "Type.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>
#include <sstream>

using namespace sys;
PointerType *Sema::decay(ArrayType *arrTy) {
  std::vector<int> dims;
  for (int i = 1; i < arrTy->dims.size(); i++)
    dims.push_back(arrTy->dims[i]);
  if (!dims.size())
    return ctx.create<PointerType>(arrTy->base);
  return ctx.create<PointerType>(ctx.create<ArrayType>(arrTy->base, dims));
}

ArrayType *Sema::raise(PointerType *ptr) {
  std::vector<int> dims { 1 };
  Type *base;
  if (auto pointee = dyn_cast<ArrayType>(ptr->pointee)) {
    for (auto x : pointee->dims)
      dims.push_back(x);
    base = pointee->base;
  } else
    base = ptr->pointee;
  return ctx.create<ArrayType>(base, dims);
}

[[noreturn]] void Sema::fail(const std::string &msg) {
  throw CompileError("sema error: " + msg);
}

void Sema::declareSymbol(const std::string &name, Type *ty, bool isMutable) {
  if (scopeDecls.empty())
    scopeDecls.emplace_back();
  auto &cur = scopeDecls.back();
  if (cur.count(name))
    fail("duplicate declaration in same scope: " + name);
  cur.insert(name);
  symbols[name] = ty;
  mutableSymbols[name] = isMutable;
}

bool Sema::isMutableSymbol(const std::string &name) const {
  auto it = mutableSymbols.find(name);
  if (it == mutableSymbols.end())
    return true;
  return it->second;
}

