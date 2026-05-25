#include "lexer.h"

// compiler2026-x phase-3 (lexer: driver)

using namespace std;

Lexer::Lexer(string source) : src_(std::move(source)) {}

vector<Token> Lexer::run() {
  vector<Token> tokens;
  while (true) {
  skipSpaceAndComments();
  Token tok;
  tok.line = line_;
  tok.col = col_;
  char c = peek();
  if (c == '\0') {
  tok.kind = TokenKind::End;
  tok.text = "";
  tokens.push_back(tok);
  return tokens;
  }
  if (isIdentStart(c)) {
  tokens.push_back(scanIdent());
  continue;
  }
  if (isdigit(static_cast<unsigned char>(c)) ||
  (c == '.' && isdigit(static_cast<unsigned char>(peek(1))))) {
  tokens.push_back(scanNumber());
  continue;
  }
  if (c == '"') {
  tokens.push_back(scanString());
  continue;
  }
  tokens.push_back(scanPunct());
  }
  }
