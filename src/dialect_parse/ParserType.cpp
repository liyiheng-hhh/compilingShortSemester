// compiler2026-x phase-A (dialect_parse parser split)

#include "Parser.h"
#include "ASTNode.h"
#include "Lexer.h"
#include "Type.h"
#include "TypeContext.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <vector>
#include <sstream>

using namespace sys;

Type *Parser::dpParseSimpleType() {
  switch (dpConsumeToken().type) {
  case Token::Void:
    return ctx.create<VoidType>();
  case Token::Int:
    return ctx.create<IntType>();
  case Token::Float:
    return ctx.create<FloatType>();
  default:
    dpFail("[dialect-parse] unknown type token");
  }
}

void *Parser::dpBuildArrayInit(const std::vector<int> &dims, bool expectFloat, bool doFold) {
  auto carry = [&](std::vector<int> &x) {
    for (int i = (int) x.size() - 1; i >= 1; i--) {
      if (x[i] >= dims[i]) {
        auto quot = x[i] / dims[i];
        x[i] %= dims[i];
        x[i - 1] += quot;
      }
    }
  };

  auto offset = [&](const std::vector<int> &x) {
    int total = 0, stride = 1;
    for (int i = (int) x.size() - 1; i >= 0; i--) {
      total += x[i] * stride;
      stride *= dims[i];
    }
    return total;
  };

  // Initialize with `dims.size()` zeroes.
  std::vector<int> place(dims.size(), 0);
  int size = 1;
  for (auto x : dims)
    size *= x;
  void *vi = !doFold
    ? (void*) new ASTNode*[size]
    : expectFloat ? (void*) new float[size] : new int[size];
  memset(vi, 0, size * (doFold ? expectFloat ? sizeof(float) : sizeof(int) : sizeof(ASTNode*)));

  // add 1 to `place[addAt]` when we meet the next `}`.
  int addAt = -1;
  do {
    if (test(Token::LBrace)) {
      addAt++;
      continue;
    }

    if (test(Token::RBrace)) {
      if (--addAt == -1)
        break;

      // Bump `place[addAt]`, and set everything after it to 0.
      place[addAt]++;
      for (int i = addAt + 1; i < dims.size(); i++)
        place[i] = 0;
      if (!peek(Token::RBrace))
        carry(place);
      
      // If this `}` isn't at the end, then a `,` or `}` must follow.
      if (addAt != -1 && !peek(Token::RBrace))
        dpExpectToken(Token::Comma);
      continue;
    }

    if (!doFold)
      ((ASTNode**) vi)[offset(place)] = dpParseExpr();
    else if (expectFloat)
      ((float*) vi)[offset(place)] = dpEarlyFold(dpParseExpr()).getFloat();
    else
      ((int*) vi)[offset(place)] = dpEarlyFold(dpParseExpr()).getInt();

    place[place.size() - 1]++;

    // Automatically carry.
    // But don't carry if the next token is `}`. See official functional test 05.
    if (!peek(Token::RBrace))
      carry(place);
    if (!test(Token::Comma) && !peek(Token::RBrace))
      dpExpectToken(Token::RBrace);
  } while (addAt != -1);

  return vi;
}
