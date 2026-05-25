// compiler2026-x phase-2 (parser split)
// compiler2026-x phase-3 (parser expr helpers)

#include "parser.h"

using namespace std;

namespace {

bool parIsUnaryOpToken(TokenKind kind, bool allowBang) {
  return kind == TokenKind::Plus || kind == TokenKind::Minus ||
         (allowBang && kind == TokenKind::Bang);
}

}  // namespace

ExprPtr Parser::parseExp() {
  // SysY 2022: Exp → AddExp；语义：单目不出现 '!'
  return parseAdd(false);
}

ExprPtr Parser::parseCond() {
  // SysY 2022: Cond → LOrExp
  return parseLOr();
}

unique_ptr<LValExpr> Parser::parseLVal() {
    const Token &name = expect(TokenKind::Ident, "identifier");
    auto expr = make_unique<LValExpr>(name.line, name.text);
    while (match(TokenKind::LBracket)) {
      expr->indices.push_back(parseExp());
      expect(TokenKind::RBracket, "']'");
    }
    return expr;
  }

ExprPtr Parser::parsePrimary() {
    if (match(TokenKind::LParen)) {
      auto expr = parseExp();
      expect(TokenKind::RParen, "')'");
      return expr;
    }
    if (check(TokenKind::IntConst)) {
      const Token &t = tokens_[pos_++];
      return make_unique<NumberExpr>(t.line, t.intVal);
    }
    if (check(TokenKind::FloatConst)) {
      const Token &t = tokens_[pos_++];
      return make_unique<NumberExpr>(t.line, t.floatVal);
    }
    if (check(TokenKind::String)) {
      const Token &t = tokens_[pos_++];
      return make_unique<StringExpr>(t.line, t.text);
    }
    if (check(TokenKind::Ident)) {
      if (tok(1).kind == TokenKind::LParen) {
        const Token &name = tokens_[pos_++];
        auto call = make_unique<CallExpr>(name.line, name.text);
        expect(TokenKind::LParen, "'('");
        if (!check(TokenKind::RParen)) {
          while (true) {
            call->args.push_back(parseExp());
            if (!match(TokenKind::Comma)) {
              break;
            }
          }
        }
        expect(TokenKind::RParen, "')'");
        return call;
      }
      return parseLVal();
    }
    throw error("expected expression");
  }

ExprPtr Parser::parseUnary(bool allowBang) {
    if (parIsUnaryOpToken(tok().kind, allowBang)) {
      const Token &opTok = tokens_[pos_++];
      return make_unique<UnaryExpr>(opTok.line, opTok.text,
                                    parseUnary(allowBang));
    }
    return parsePrimary();
  }

ExprPtr Parser::parseMul(bool allowBang) {
    auto lhs = parseUnary(allowBang);
    while (check(TokenKind::Star) || check(TokenKind::Slash) ||
           check(TokenKind::Percent)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseUnary(allowBang);
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

ExprPtr Parser::parseAdd(bool allowBang) {
    auto lhs = parseMul(allowBang);
    while (check(TokenKind::Plus) || check(TokenKind::Minus)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseMul(allowBang);
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

ExprPtr Parser::parseRel() {
    // RelExp → AddExp | RelExp relop AddExp；AddExp 在条件中允许 UnaryOp '!'
    auto lhs = parseAdd(true);
    while (check(TokenKind::Lt) || check(TokenKind::Gt) || check(TokenKind::Le) ||
           check(TokenKind::Ge)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseAdd(true);
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

ExprPtr Parser::parseEq() {
    auto lhs = parseRel();
    while (check(TokenKind::EqEq) || check(TokenKind::Neq)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseRel();
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

ExprPtr Parser::parseLAnd() {
    auto lhs = parseEq();
    while (check(TokenKind::AndAnd)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseEq();
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

ExprPtr Parser::parseLOr() {
    auto lhs = parseLAnd();
    while (check(TokenKind::OrOr)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseLAnd();
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }
