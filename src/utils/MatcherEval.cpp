// compiler2026-x phase-2 (matcher split)

#include "Matcher.h"
#include "../codegen/Attrs.h"
#include <cstdlib>
#include <iostream>

using namespace sys;

#include "MatcherMacros.inc"

int Rule::mtEvalExpr(Expr *expr) {
  if (auto atom = dyn_cast<Atom>(expr)) {
    if (std::isdigit(atom->value[0]) || atom->value[0] == '-') {
      std::string str(atom->value);
      return std::stoi(str);
    }

    if (atom->value[0] == '\'') {
      auto lint = binding[atom->value];
      return V(lint);
    }
  }

  auto list = dyn_cast<List>(expr);

  assert(list && !list->elements.empty());

  auto head = dyn_cast<Atom>(list->elements[0]);
  std::string_view opname = head->value;

  EVAL_BINARY("add", +);
  EVAL_BINARY("sub", -);
  EVAL_BINARY("mul", *);
  EVAL_BINARY("div", /);
  EVAL_BINARY("mod", %);
  EVAL_BINARY("gt", >);
  EVAL_BINARY("lt", <);
  EVAL_BINARY("ge", >=);
  EVAL_BINARY("le", <=);
  EVAL_BINARY("eq", ==);
  EVAL_BINARY("ne", !=);
  EVAL_BINARY("and", &);
  EVAL_BINARY("or", |);
  EVAL_BINARY("lsh", <<);
  EVAL_BINARY("rsh", >>);

  EVAL_BINARY_F_OPERAND("feq", ==);
  EVAL_BINARY_F_OPERAND("fne", !=);
  EVAL_BINARY_F_OPERAND("fle", <=);
  EVAL_BINARY_F_OPERAND("fge", >=);
  EVAL_BINARY_F_OPERAND("flt", <);
  EVAL_BINARY_F_OPERAND("fgt", >);

  EVAL_UNARY("not", !);

  EVAL_UNARY_F_OPERAND("cvt", (int));

  if (opname == "!only-if") {
    int a = mtEvalExpr(list->elements[1]);
    if (!a)
      failed = true;
    return 0;
  }

  std::cerr << "unknown opname: " << opname << "\n";
  assert(false);
  std::abort();
}

float Rule::mtEvalFExpr(Expr *expr) {
  if (auto atom = dyn_cast<Atom>(expr)) {
    if (std::isdigit(atom->value[1]) || atom->value[1] == '-') {
      std::string str(atom->value.substr(1));
      return std::stof(str);
    }

    if (atom->value[0] == '*') {
      auto lint = binding[atom->value];
      return F(lint);
    }
  }

  auto list = dyn_cast<List>(expr);

  assert(list && !list->elements.empty());

  auto head = dyn_cast<Atom>(list->elements[0]);
  std::string_view opname = head->value;

  EVAL_BINARY_F("add", +);
  EVAL_BINARY_F("sub", -);
  EVAL_BINARY_F("mul", *);
  EVAL_BINARY_F("div", /);
  
  EVAL_UNARY_I_OPERAND("cvt", (float));

  std::cerr << "unknown opname: " << opname << "\n";
  assert(false);
  std::abort();
}
