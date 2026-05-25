#include "lexer.h"

// compiler2026-x phase-3 (lexer: position + skip)

using namespace std;

namespace {

bool lxIsIdentStartChar(char c) {
  return c == '_' || isalpha(static_cast<unsigned char>(c));
}

bool lxIsIdentPartChar(char c) {
  return lxIsIdentStartChar(c) || isdigit(static_cast<unsigned char>(c));
}

}  // namespace

char Lexer::peek(size_t ahead) const {
    size_t p = pos_ + ahead;
    return p < src_.size() ? src_[p] : '\0';
  }

char Lexer::get(){
    char c = peek();
    if (c == '\0') {
      return c;
    }
    ++pos_;
    if (c == '\n') {
      ++line_;
      col_ = 1;
    } else {
      ++col_;
    }
    return c;
  }

bool Lexer::isIdentStart(char c){
    return lxIsIdentStartChar(c);
  }

bool Lexer::isIdentPart(char c){
    return lxIsIdentPartChar(c);
  }

void Lexer::skipSpaceAndComments(){
    while (true) {
      while (isspace(static_cast<unsigned char>(peek()))) {
        get();
      }
      if (peek() == '/' && peek(1) == '/') {
        while (peek() != '\0' && peek() != '\n') {
          get();
        }
        continue;
      }
      if (peek() == '/' && peek(1) == '*') {
        get();
        get();
        while (!(peek() == '*' && peek(1) == '/')) {
          if (peek() == '\0') {
            throw CompileError("unterminated block comment");
          }
          get();
        }
        get();
        get();
        continue;
      }
      break;
    }
  }
