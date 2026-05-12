#include "lexer.h"

using namespace std;
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

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
    return c == '_' || isalpha(static_cast<unsigned char>(c));
  }

bool Lexer::isIdentPart(char c){
    return isIdentStart(c) || isdigit(static_cast<unsigned char>(c));
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

Token Lexer::scanIdent(){
    Token tok;
    tok.line = line_;
    tok.col = col_;
    while (isIdentPart(peek())) {
      tok.text.push_back(get());
    }
    static const unordered_map<string, TokenKind> keywords = {
        {"const", TokenKind::KwConst},       {"int", TokenKind::KwInt},
        {"float", TokenKind::KwFloat},       {"void", TokenKind::KwVoid},
        {"if", TokenKind::KwIf},             {"else", TokenKind::KwElse},
        {"while", TokenKind::KwWhile},       {"break", TokenKind::KwBreak},
        {"continue", TokenKind::KwContinue}, {"return", TokenKind::KwReturn}};
    auto it = keywords.find(tok.text);
    tok.kind = it == keywords.end() ? TokenKind::Ident : it->second;
    return tok;
  }

Token Lexer::scanNumber(){
    Token tok;
    tok.line = line_;
    tok.col = col_;
    bool isFloat = false;
    bool isHex = peek() == '0' && (peek(1) == 'x' || peek(1) == 'X');

    if (isHex) {
      tok.text.push_back(get());
      tok.text.push_back(get());
      while (isxdigit(static_cast<unsigned char>(peek()))) {
        tok.text.push_back(get());
      }
      if (peek() == '.') {
        isFloat = true;
        tok.text.push_back(get());
        while (isxdigit(static_cast<unsigned char>(peek()))) {
          tok.text.push_back(get());
        }
      }
      if (peek() == 'p' || peek() == 'P') {
        isFloat = true;
        tok.text.push_back(get());
        if (peek() == '+' || peek() == '-') {
          tok.text.push_back(get());
        }
        while (isdigit(static_cast<unsigned char>(peek()))) {
          tok.text.push_back(get());
        }
      }
    } else {
      while (isdigit(static_cast<unsigned char>(peek()))) {
        tok.text.push_back(get());
      }
      if (peek() == '.') {
        isFloat = true;
        tok.text.push_back(get());
        while (isdigit(static_cast<unsigned char>(peek()))) {
          tok.text.push_back(get());
        }
      }
      if (peek() == 'e' || peek() == 'E') {
        isFloat = true;
        tok.text.push_back(get());
        if (peek() == '+' || peek() == '-') {
          tok.text.push_back(get());
        }
        while (isdigit(static_cast<unsigned char>(peek()))) {
          tok.text.push_back(get());
        }
      }
    }

    if (peek() == 'f' || peek() == 'F' || peek() == 'l' || peek() == 'L') {
      isFloat = true;
      get();
    }

    if (isFloat) {
      tok.kind = TokenKind::FloatConst;
      tok.floatVal = strtof(tok.text.c_str(), nullptr);
    } else {
      tok.kind = TokenKind::IntConst;
      long long v = strtoll(tok.text.c_str(), nullptr, 0);
      tok.intVal = static_cast<int32_t>(v);
    }
    return tok;
  }

Token Lexer::scanString(){
    Token tok;
    tok.kind = TokenKind::String;
    tok.line = line_;
    tok.col = col_;
    get();
    while (peek() != '"') {
      if (peek() == '\0') {
        throw CompileError("unterminated string literal");
      }
      if (peek() != '\\') {
        tok.text.push_back(get());
        continue;
      }
      get();
      char c = get();
      switch (c) {
      case 'n':
        tok.text.push_back('\n');
        break;
      case 't':
        tok.text.push_back('\t');
        break;
      case 'r':
        tok.text.push_back('\r');
        break;
      case '\\':
        tok.text.push_back('\\');
        break;
      case '"':
        tok.text.push_back('"');
        break;
      case '0':
        tok.text.push_back('\0');
        break;
      default:
        tok.text.push_back(c);
        break;
      }
    }
    get();
    return tok;
  }

Token Lexer::scanPunct(){
    Token tok;
    tok.line = line_;
    tok.col = col_;
    char c = get();
    tok.text.push_back(c);
    auto one = [&](TokenKind kind) {
      tok.kind = kind;
      return tok;
    };
    switch (c) {
    case '+':
      return one(TokenKind::Plus);
    case '-':
      return one(TokenKind::Minus);
    case '*':
      return one(TokenKind::Star);
    case '/':
      return one(TokenKind::Slash);
    case '%':
      return one(TokenKind::Percent);
    case '(':
      return one(TokenKind::LParen);
    case ')':
      return one(TokenKind::RParen);
    case '{':
      return one(TokenKind::LBrace);
    case '}':
      return one(TokenKind::RBrace);
    case '[':
      return one(TokenKind::LBracket);
    case ']':
      return one(TokenKind::RBracket);
    case ';':
      return one(TokenKind::Semicolon);
    case ',':
      return one(TokenKind::Comma);
    case '!':
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::Neq);
      }
      return one(TokenKind::Bang);
    case '&':
      if (peek() == '&') {
        tok.text.push_back(get());
        return one(TokenKind::AndAnd);
      }
      break;
    case '|':
      if (peek() == '|') {
        tok.text.push_back(get());
        return one(TokenKind::OrOr);
      }
      break;
    case '=':
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::EqEq);
      }
      return one(TokenKind::Assign);
    case '<':
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::Le);
      }
      return one(TokenKind::Lt);
    case '>':
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::Ge);
      }
      return one(TokenKind::Gt);
    default:
      break;
    }
    string msg = "unexpected character '";
    msg.push_back(c);
    msg += "'";
    throw CompileError(msg);
  }
