// compiler2026-x phase-A (dialect_parse sema driver)

#include "Sema.h"
#include "ASTNode.h"
#include "Type.h"

using namespace sys;


Sema::Sema(ASTNode *node, TypeContext &ctx): ctx(ctx) {
  auto intTy = ctx.create<IntType>();
  auto floatTy = ctx.create<FloatType>();
  auto voidTy = ctx.create<VoidType>();
  auto intPtrTy = ctx.create<ArrayType>(intTy, std::vector { 1 });
  auto floatPtrTy = ctx.create<ArrayType>(floatTy, std::vector { 1 });

  using Args = std::vector<Type*>;
  Args empty;
  
  // Internal library.
  symbols = {
    { "getint", ctx.create<FunctionType>(intTy, empty) },
    { "getch", ctx.create<FunctionType>(intTy, empty) },
    { "getfloat", ctx.create<FunctionType>(floatTy, empty) },
    { "getarray", ctx.create<FunctionType>(intTy, Args { intPtrTy }) },
    { "getfarray", ctx.create<FunctionType>(intTy, Args { floatPtrTy } ) },
    { "putint", ctx.create<FunctionType>(voidTy, Args { intTy }) },
    { "putch", ctx.create<FunctionType>(voidTy, Args { intTy }) },
    { "putfloat", ctx.create<FunctionType>(voidTy, Args { floatTy }) },
    { "putarray", ctx.create<FunctionType>(voidTy, Args { intTy, intPtrTy }) },
    { "putfarray", ctx.create<FunctionType>(voidTy, Args { intTy, floatPtrTy }) },
    { "_sysy_starttime", ctx.create<FunctionType>(voidTy, Args { intTy }) },
    { "_sysy_stoptime", ctx.create<FunctionType>(voidTy, Args { intTy }) },
  };
  for (const auto &it : symbols) {
    mutableSymbols[it.first] = true;
  }

  infer(node);
}
