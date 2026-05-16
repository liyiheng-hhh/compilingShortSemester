#pragma once

#include <cstdint>
#include <string>

enum class TokenKind {
  End,
  Ident,
  IntConst,
  FloatConst,
  String,
  KwConst,
  KwInt,
  KwFloat,
  KwVoid,
  KwIf,
  KwElse,
  KwWhile,
  KwBreak,
  KwContinue,
  KwReturn,
  Plus,
  Minus,
  Star,
  Slash,
  Percent,
  Bang,
  AndAnd,
  OrOr,
  EqEq,
  Neq,
  Lt,
  Gt,
  Le,
  Ge,
  Assign,
  LParen,
  RParen,
  LBrace,
  RBrace,
  LBracket,
  RBracket,
  Semicolon,
  Comma
};

struct Token {
  TokenKind kind = TokenKind::End;
  std::string text;
  int line = 1;
  int col = 1;
  int32_t intVal = 0;
  float floatVal = 0.0f;
};
