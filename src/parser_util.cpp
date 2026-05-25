#include "parser.h"

// compiler2026-x phase-3 (parser helpers)

using namespace std;

namespace {

bool parIsBTypeKind(TokenKind kind) {
  return kind == TokenKind::KwInt || kind == TokenKind::KwFloat;
}

bool parIsFuncHeaderStart(TokenKind kind) {
  return kind == TokenKind::KwVoid || parIsBTypeKind(kind);
}

}  // namespace

bool Parser::isBType(TokenKind kind) {
  return parIsBTypeKind(kind);
}

bool Parser::isFuncDefAhead() const {
  if (tok().kind == TokenKind::KwConst) {
    return false;
  }
  if (!parIsFuncHeaderStart(tok().kind)) {
    return false;
  }
  return tok(1).kind == TokenKind::Ident && tok(2).kind == TokenKind::LParen;
}
