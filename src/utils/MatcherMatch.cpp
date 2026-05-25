// compiler2026-x phase-2 (matcher split)

#include "Matcher.h"
#include "../codegen/Attrs.h"
#include <cstdlib>
#include <iostream>

using namespace sys;

#include "MatcherMacros.inc"

bool Rule::matchExpr(Expr *expr, Op* op) {
  if (auto* atom = dyn_cast<Atom>(expr)) {
    std::string_view var = atom->value;

    // This is a float literal.
    if (var[0] == '*') {
      if (!isa<FloatOp>(op))
        return false;

      if (std::isdigit(var[1]) || var[1] == '-') {
        std::string str(var.substr(1));
        if (std::stof(str) != F(op))
          return false;
      }

      if (binding.count(var))
        return F(binding[var]) == F(op);

      binding[var] = op;
      return true;
    }

    // A normal binding.
    if (var[0] != '\'' && !(std::isdigit(var[0]) || var[0] == '-')) {
      if (binding.count(var))
        return binding[var] == op;

      binding[var] = op;
      return true;
    }

    // This denotes a int-constant.
    if (!isa<IntOp>(op)) {
      return false;
    }

    // This is a int literal.
    if (std::isdigit(var[0]) || var[0] == '-') {
      std::string str(var);
      if (std::stoi(str) != V(op))
        return false;
    }

    if (binding.count(var))
      return V(binding[var]) == V(op);

    binding[var] = op;
    return true;
  }

  List *list = dyn_cast<List>(expr);
  if (!list)
    return false;

  assert(!list->elements.empty());
  Atom *head = dyn_cast<Atom>(list->elements[0]);
  if (!head)
    return false;

  std::string_view opname = head->value;

  MATCH_TERNARY("select", SelectOp);

  MATCH_BINARY("eq", EqOp);
  MATCH_BINARY("ne", NeOp);
  MATCH_BINARY("le", LeOp);
  MATCH_BINARY("lt", LtOp);
  MATCH_BINARY("feq", EqFOp);
  MATCH_BINARY("fne", NeFOp);
  MATCH_BINARY("fle", LeFOp);
  MATCH_BINARY("flt", LtFOp);
  MATCH_BINARY("add", AddIOp);
  MATCH_BINARY("sub", SubIOp);
  MATCH_BINARY("mul", MulIOp);
  MATCH_BINARY("div", DivIOp);
  MATCH_BINARY("mod", ModIOp);
  MATCH_BINARY("and", AndIOp);
  MATCH_BINARY("or", OrIOp);
  MATCH_BINARY("xor", XorIOp);
  MATCH_BINARY("addl", AddLOp);
  MATCH_BINARY("subl", SubLOp);
  MATCH_BINARY("mull", MulLOp);
  MATCH_BINARY("divl", DivLOp);
  MATCH_BINARY("fadd", AddFOp);
  MATCH_BINARY("fsub", SubFOp);
  MATCH_BINARY("fmul", MulFOp);
  MATCH_BINARY("fdiv", DivFOp);
  MATCH_BINARY("store", StoreOp);
  MATCH_BINARY("lshift", LShiftOp);
  MATCH_BINARY("rshift", RShiftOp);

  MATCH_UNARY("not", NotOp);
  MATCH_UNARY("snz", SetNotZeroOp);
  MATCH_UNARY("minus", MinusOp);
  MATCH_UNARY("fminus", MinusFOp);
  MATCH_UNARY("br", BranchOp);
  MATCH_UNARY("f2i", F2IOp);
  MATCH_UNARY("i2f", I2FOp);
  MATCH_UNARY("load", LoadOp);

  return false;
}

bool Rule::match(Op *op, const std::map<std::string, Op*> &external) {
  loc = 0;
  failed = false;
  binding.clear();
  externalStrs.clear();
  
  for (auto [k, v] : external) {
    externalStrs.push_back(k);
    binding[externalStrs.back()] = v;
  }

  return matchExpr(pattern, op);
}

Op *Rule::extract(const std::string &name) {
  if (!binding.count(name)) {
    std::cerr << "querying unknown name: " << name << "\n";
    dump();
    assert(false);
  }
  return binding[name];
}

bool Rule::rewrite(Op *op) {
  loc = 0;
  failed = false;
  binding.clear();
  
  auto list = dyn_cast<List>(pattern);
  assert(dyn_cast<Atom>(list->elements[0])->value == "change");
  auto matcher = list->elements[1];
  auto rewriter = list->elements[2];

  if (!matchExpr(matcher, op))
    return false;

  builder.setBeforeOp(op);
  Op *opnew = buildExpr(rewriter);
  if (!opnew || failed)
    return false;

  op->replaceAllUsesWith(opnew);
  op->erase();
  return true;
}
