// compiler2026-x phase-E (driver)

#include "CodeGen.h"
#include "../utils/DynamicCast.h"
#include "Attrs.h"
#include "OpBase.h"
#include "Ops.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace sys;

namespace {

bool cgAstIsFloat(Type *ty) {
  return ty && isa<FloatType>(ty);
}

bool cgValueIsFloat(Value v) {
  if (!v.defining)
    return false;

  auto ty = v.defining->getResultType();
  return ty == Value::f32 || ty == Value::f128;
}

bool cgPreferFloat(Type *astTy, Value v) {
  return cgAstIsFloat(astTy) || cgValueIsFloat(v);
}

} // namespace
CodeGen::CodeGen(ASTNode *node): module(new ModuleOp()) {
  module->createFirstBlock();
  builder.setToRegionStart(module->getRegion());
  cgcEmit(node);
}

int CodeGen::getSize(Type *ty) {
  // Some C-oriented public suites may reach codegen with missing AST type tags.
  // Use int-width fallback to keep compilation safe and deterministic.
  if (!ty)
    return 4;

  if (isa<IntType>(ty) || isa<FloatType>(ty))
    return 4;
  if (auto arrTy = dyn_cast<ArrayType>(ty))
    return getSize(arrTy->base) * arrTy->getSize();
  
  return 8;
}
