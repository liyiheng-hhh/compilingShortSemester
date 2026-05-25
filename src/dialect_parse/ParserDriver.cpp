// compiler2026-x phase-A (dialect_parse parser split)

#include "Parser.h"
#include "ASTNode.h"
#include "Lexer.h"
#include "Type.h"
#include "TypeContext.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <sstream>
#include <vector>

using namespace sys;

Parser::Parser(const std::string &input, TypeContext &ctx): loc(0), ctx(ctx) {
  Lexer lex(input);

  while (lex.hasMore())
    tokens.push_back(lex.dplxNextToken());
}

ASTNode *Parser::parse() {
  ASTNode *unit = nullptr;
  unit = dpParseCompUnit();

  // Release memory.
  for (auto tok : tokens) {
    if (tok.type == Token::Ident)
      delete[] tok.vs;
  }

  return unit;
}
