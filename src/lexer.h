#pragma once

#include "common.h"
#include "token.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

class Lexer {
public:
  explicit Lexer(std::string source);

  std::vector<Token> run();

private:
  std::string src_;
  std::size_t pos_ = 0;
  int line_ = 1;
  int col_ = 1;

  char peek(std::size_t ahead = 0) const;
  char get();

  static bool isIdentStart(char c);
  static bool isIdentPart(char c);

  void skipSpaceAndComments();
  Token scanIdent();
  Token scanNumber();
  Token scanString();
  Token scanPunct();
};
