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

ASTNode *Parser::dpParsePrimary() {
  if (peek(Token::LInt))
    return new IntNode(dpConsumeToken().vi);

  if (peek(Token::LFloat))
    return new FloatNode(dpConsumeToken().vf);
  
  if (test(Token::LPar)) {
    auto n = dpParseExpr();
    dpExpectToken(Token::RPar);
    return n;
  }

  if (peek(Token::Ident)) {
    // Function call.
    auto vs = dpConsumeToken().vs;

    if (test(Token::LPar)) {
      std::vector<ASTNode*> args;
      while (!test(Token::RPar)) {
        args.push_back(dpParseExpr());
        if (!test(Token::Comma) && !peek(Token::RPar))
          dpExpectToken(Token::RPar);
      }

      // Take special care for _sysy_{start,stop}time.
      // Their line numbers are encoded in their names.
      std::string name = vs;
      if (name.rfind("_sysy_starttime_", 0) != std::string::npos) {
        name = "_sysy_starttime";
        args.push_back(new IntNode(strtol(vs + 16, NULL, 10)));
      }
      if (name.rfind("_sysy_stoptime_", 0) != std::string::npos) {
        name = "_sysy_stoptime";
        args.push_back(new IntNode(strtol(vs + 15, NULL, 10)));
      }
      return new CallNode(name, args);
    }

    if (test(Token::LBrak)) {
      // Read from array.

      std::vector<ASTNode*> indices;
      do {
        indices.push_back(dpParseExpr());
        dpExpectToken(Token::RBrak);
      } while (test(Token::LBrak));

      // This is actually an assign node.
      if (test(Token::Assign)) {
        auto value = dpParseExpr();
        // Don't expect semicolon here. It'll be expected in dpParseStmt().
        return new ArrayAssignNode(vs, indices, value);
      }

      return new ArrayAccessNode(vs, indices);
    }

    return new VarRefNode(vs);
  }

  std::ostringstream os;
  os << "[dialect-parse] unexpected token " << dpPeekToken().type;
  dpFail(os.str());
}

ASTNode *Parser::dpParseUnary() {
  if (test(Token::Minus))
    return new UnaryNode(UnaryNode::Minus, dpParseUnary());

  if (test(Token::Plus))
    return dpParseUnary();

  if (test(Token::Not))
    return new UnaryNode(UnaryNode::Not, dpParseUnary());

  return dpParsePrimary();
}

ASTNode *Parser::dpParseMul() {
  auto n = dpParseUnary();
  while (peek(Token::Mul, Token::Div, Token::Mod)) {
    switch (dpConsumeToken().type) {
    case Token::Mul:
      n = new BinaryNode(BinaryNode::Mul, n, dpParseUnary());
      break;
    case Token::Div:
      n = new BinaryNode(BinaryNode::Div, n, dpParseUnary());
      break;
    case Token::Mod:
      n = new BinaryNode(BinaryNode::Mod, n, dpParseUnary());
      break;
    default:
      assert(false);
    }
  }
  return n;
}

ASTNode *Parser::dpParseAdd() {
  auto n = dpParseMul();
  while (peek(Token::Plus, Token::Minus)) {
    switch (dpConsumeToken().type) {
    case Token::Plus:
      n = new BinaryNode(BinaryNode::Add, n, dpParseMul());
      break;
    case Token::Minus:
      n = new BinaryNode(BinaryNode::Sub, n, dpParseMul());
      break;
    default:
      assert(false);
    }
  }
  return n;
}

ASTNode *Parser::dpParseRel() {
  auto n = dpParseAdd();
  while (peek(Token::Lt, Token::Gt, Token::Ge, Token::Le)) {
    switch (dpConsumeToken().type) {
    case Token::Lt:
      n = new BinaryNode(BinaryNode::Lt, n, dpParseAdd());
      break;
    case Token::Le:
      n = new BinaryNode(BinaryNode::Le, n, dpParseAdd());
      break;
    case Token::Gt:
      n = new BinaryNode(BinaryNode::Lt, dpParseAdd(), n);
      break;
    case Token::Ge:
      n = new BinaryNode(BinaryNode::Le, dpParseAdd(), n);
      break;
    default:
      assert(false);
    }
  }
  return n;
}

ASTNode *Parser::dpParseEq() {
  auto n = dpParseRel();
  while (peek(Token::Eq, Token::Ne)) {
    switch (dpConsumeToken().type) {
    case Token::Eq:
      n = new BinaryNode(BinaryNode::Eq, n, dpParseRel());
      break;
    case Token::Ne:
      n = new BinaryNode(BinaryNode::Ne, n, dpParseRel());
      break;
    default:
      assert(false);
    }
  }
  return n;
}

ASTNode *Parser::dpParseLAnd() {
  auto n = dpParseEq();
  while (test(Token::And))
    n = new BinaryNode(BinaryNode::And, n, dpParseEq());
  
  return n;
}

ASTNode *Parser::dpParseLOr() {
  auto n = dpParseLAnd();
  while (test(Token::Or))
    n = new BinaryNode(BinaryNode::Or, n, dpParseLAnd());
  
  return n;
}

ASTNode *Parser::dpParseExpr() {
  return dpParseLOr();
}
