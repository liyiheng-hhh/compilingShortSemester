// compiler2026-x phase-2 (matcher split)

#include "Matcher.h"
#include "../codegen/Attrs.h"
#include <cstdlib>
#include <iostream>

using namespace sys;

#include "MatcherMacros.inc"

Op *Rule::mtBuildExpr(Expr *expr) {
  if (auto atom = dyn_cast<Atom>(expr)) {
    // This is an integer literal. Evaluate it.
    if (std::isdigit(atom->value[0]) || atom->value[0] == '-' || atom->value[0] == '\'') {
      int result = mtEvalExpr(expr);
      return builder.create<IntOp>({ new IntAttr(result) });
    }

    if (!binding.count(atom->value)) {
      std::cerr << "unbound variable: " << atom->value << "\n";
      assert(false);
    }
    return binding[atom->value];
  }

  auto list = dyn_cast<List>(expr);

  assert(list && !list->elements.empty());

  auto head = dyn_cast<Atom>(list->elements[0]);
  std::string_view opname = head->value;

  if (opname[0] == '!') {
    int result = mtEvalExpr(expr);
    if (opname == "!only-if" && !failed)
      return mtBuildExpr(list->elements[2]);

    return builder.create<IntOp>({ new IntAttr(result) });
  }

  if (opname[0] == '?') {
    float result = mtEvalFExpr(expr);

    return builder.create<FloatOp>({ new FloatAttr(result) });
  }

  BUILD_TERNARY("select", SelectOp);

  BUILD_BINARY("add", AddIOp);
  BUILD_BINARY("sub", SubIOp);
  BUILD_BINARY("mul", MulIOp);
  BUILD_BINARY("div", DivIOp);
  BUILD_BINARY("addl", AddLOp);
  BUILD_BINARY("mull", MulLOp);
  BUILD_BINARY("fadd", AddFOp);
  BUILD_BINARY("fsub", SubFOp);
  BUILD_BINARY("fmul", MulFOp);
  BUILD_BINARY("fdiv", DivFOp);
  BUILD_BINARY("mod", ModIOp);
  BUILD_BINARY("and", AndIOp);
  BUILD_BINARY("or", OrIOp);
  BUILD_BINARY("eq", EqOp);
  BUILD_BINARY("ne", NeOp);
  BUILD_BINARY("le", LeOp);
  BUILD_BINARY("lt", LtOp);

  BUILD_UNARY("minus", MinusOp);
  BUILD_UNARY("fminus", MinusFOp);
  BUILD_UNARY("not", NotOp);
  BUILD_UNARY("snz", SetNotZeroOp);

  if (opname == "gt") {
    Value a = mtBuildExpr(list->elements[1]);
    Value b = mtBuildExpr(list->elements[2]);
    return builder.create<LtOp>({ b, a });
  }

  if (opname == "ge") {
    Value a = mtBuildExpr(list->elements[1]);
    Value b = mtBuildExpr(list->elements[2]);
    return builder.create<LeOp>({ b, a });
  }

  std::cerr << "unknown opname: " << opname << "\n";
  assert(false);
  std::abort();
}
