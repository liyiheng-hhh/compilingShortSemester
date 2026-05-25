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

Token Parser::dpLastToken() {
  if (loc - 1 >= tokens.size())
    return Token::End;
  return tokens[loc - 1];
}

Token Parser::dpPeekToken() {
  if (loc >= tokens.size())
    return Token::End;
  return tokens[loc];
}

Token Parser::dpConsumeToken() {
  if (loc >= tokens.size())
    return Token::End;
  return tokens[loc++];
}

bool Parser::peek(Token::Type t) {
  return dpPeekToken().type == t;
}

Token Parser::dpExpectToken(Token::Type t) {
  if (!test(t)) {
    std::ostringstream os;
    os << "[dialect-parse] expected " << t << ", but got " << dpPeekToken().type;
    dpFail(os.str());
  }
  return dpLastToken();
}

[[noreturn]] void Parser::dpFail(const std::string &msg) {
  std::cerr << msg << "\n";
  dpPrintContext();
  throw CompileError(msg);
}

void Parser::dpPrintContext() {
  std::cerr << "surrounding:\n";
  for (size_t i = std::max(0ul, loc - 5); i < std::min(tokens.size(), loc + 6); i++) {
    std::cerr << tokens[i].type;
    if (tokens[i].type == Token::LInt) {
      std::cerr << " <int = " << tokens[i].vi << ">";
    }
    if (tokens[i].type == Token::LFloat) {
      std::cerr << " <float = " << tokens[i].vf << "f>";
    }
    if (tokens[i].type == Token::Ident) {
      std::cerr << " <name = " << tokens[i].vs << ">";
    }
    std::cerr << (i == loc ? " (here)" : "") << "\n";
  }
}
