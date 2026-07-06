#include "lexer.h"

// compiler2026-x phase-3 (lexer: scan tokens)

using namespace std;
#include <cctype>
#include <cstdlib>
#include <unordered_map>

namespace {

const unordered_map<string, TokenKind> &lxKeywordTable() {
  static const unordered_map<string, TokenKind> table = {
      {"const", TokenKind::KwConst},       {"int", TokenKind::KwInt},
      {"float", TokenKind::KwFloat},       {"void", TokenKind::KwVoid},
      {"if", TokenKind::KwIf},             {"else", TokenKind::KwElse},
      {"while", TokenKind::KwWhile},       {"break", TokenKind::KwBreak},
      {"continue", TokenKind::KwContinue}, {"return", TokenKind::KwReturn}};
  return table;
}

TokenKind lxLookupKeyword(const string &text) {
  auto it = lxKeywordTable().find(text);
  return it == lxKeywordTable().end() ? TokenKind::Ident : it->second;
}

}  // namespace

Token Lexer::scanIdent(){
    Token tok;
    tok.line = line_;
    tok.col = col_;
    while (isIdentPart(peek())) {
      tok.text.push_back(get());
    }
    tok.kind = lxLookupKeyword(tok.text);
    return tok;
  }

Token Lexer::scanNumber(){
    Token tok;
    tok.line = line_;
    tok.col = col_;
    bool negative = false;
    if (peek() == '-') {
      negative = true;
      get();
    }
    if (peek() == '0') {
      if (negative) {
        tok.text.push_back('-');
      }
      tok.text.push_back(get());
      if (isdigit(static_cast<unsigned char>(peek()))) {
        throw CompileError("line " + to_string(tok.line) + ":" +
                           to_string(tok.col) + ": invalid integer literal");
      }
    } else if (peek() >= '1' && peek() <= '9') {
      if (negative) {
        tok.text.push_back('-');
      }
      while (isdigit(static_cast<unsigned char>(peek()))) {
        tok.text.push_back(get());
      }
    } else {
      throw CompileError("line " + to_string(tok.line) + ":" +
                         to_string(tok.col) + ": invalid integer literal");
    }
    tok.kind = TokenKind::IntConst;
    long long v = strtoll(tok.text.c_str(), nullptr, 10);
    tok.intVal = static_cast<int32_t>(v);
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
      throw CompileError("line " + to_string(tok.line) + ":" + to_string(tok.col) +
                         ": bitwise '&' is not in ToyC (use '&&' for logical and)");
    case '|':
      if (peek() == '|') {
        tok.text.push_back(get());
        return one(TokenKind::OrOr);
      }
      throw CompileError("line " + to_string(tok.line) + ":" + to_string(tok.col) +
                         ": bitwise '|' is not in ToyC (use '||' for logical or)");
    case '^':
      throw CompileError("line " + to_string(tok.line) + ":" + to_string(tok.col) +
                         ": '^' is not in ToyC");
    case '=':
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::EqEq);
      }
      return one(TokenKind::Assign);
    case '<':
      if (peek() == '<') {
        throw CompileError("line " + to_string(tok.line) + ":" + to_string(tok.col) +
                             ": '<<' is not in ToyC");
      }
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::Le);
      }
      return one(TokenKind::Lt);
    case '>':
      if (peek() == '>') {
        throw CompileError("line " + to_string(tok.line) + ":" + to_string(tok.col) +
                             ": '>>' is not in ToyC");
      }
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
