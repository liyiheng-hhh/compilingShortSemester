#include "Lexer.h"

// compiler2026-x phase-3 (dialect lexer keywords)

#include <map>
#include <string>

namespace sys {

const std::map<std::string, Token::Type> &dplxKeywordMap() {
  static const std::map<std::string, Token::Type> table = {
      {"if", Token::If},
      {"else", Token::Else},
      {"while", Token::While},
      {"for", Token::For},
      {"return", Token::Return},
      {"int", Token::Int},
      {"float", Token::Float},
      {"void", Token::Void},
      {"const", Token::Const},
      {"break", Token::Break},
      {"continue", Token::Continue},
  };
  return table;
}

}  // namespace sys
