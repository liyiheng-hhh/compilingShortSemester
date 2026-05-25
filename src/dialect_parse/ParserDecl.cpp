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

ASTNode *Parser::dpParseStmt() {
  // Consume all empty statements.
  if (test(Token::Semicolon))
    return new EmptyNode();

  if (peek(Token::LBrace))
    return dpParseBlock();

  if (test(Token::Return)) {
    if (test(Token::Semicolon))
      return new ReturnNode(currentFunc, nullptr);
    auto ret = new ReturnNode(currentFunc, dpParseExpr());
    dpExpectToken(Token::Semicolon);
    return ret;
  }

  if (test(Token::If)) {
    dpExpectToken(Token::LPar);
    auto cond = dpParseExpr();
    dpExpectToken(Token::RPar);
    auto ifso = dpParseStmt();
    ASTNode *ifnot = nullptr;
    if (test(Token::Else))
      ifnot = dpParseStmt();
    return new IfNode(cond, ifso, ifnot);
  }

  if (test(Token::While)) {
    dpExpectToken(Token::LPar);
    auto cond = dpParseExpr();
    dpExpectToken(Token::RPar);
    auto body = dpParseStmt();
    return new WhileNode(cond, body);
  }

  if (test(Token::Break)) {
    dpExpectToken(Token::Semicolon);
    return new BreakNode();
  }

  if (test(Token::Continue)) {
    dpExpectToken(Token::Semicolon);
    return new ContinueNode();
  }

  if (peek(Token::Const, Token::Int, Token::Float))
    return dpParseVarDecl(false);

  auto n = dpParseExpr();
  if (test(Token::Assign)) {
    if (!isa<VarRefNode>(n)) {
      dpFail("[dialect-parse] expected assignable lvalue");
    }
    auto value = dpParseExpr();
    dpExpectToken(Token::Semicolon);
    return new AssignNode(n, value);
  }

  dpExpectToken(Token::Semicolon);
  return n;
}

BlockNode *Parser::dpParseBlock() {
  SemanticScope scope(*this);

  dpExpectToken(Token::LBrace);
  std::vector<ASTNode *> nodes;
  
  while (!test(Token::RBrace))
    nodes.push_back(dpParseStmt());

  return new BlockNode(nodes);
}

TransparentBlockNode *Parser::dpParseVarDecl(bool global) {
  bool mut = !test(Token::Const);
  auto base = dpParseSimpleType();
  std::vector<VarDeclNode*> decls;

  do {
    Type *ty = base;
    std::string name = dpExpectToken(Token::Ident).vs;
    std::vector<int> dims;

    while (test(Token::LBrak)) {
      dims.push_back(dpEarlyFold(dpParseExpr()).getInt());
      dpExpectToken(Token::RBrak);
    }

    if (dims.size() != 0)
      // TODO: do folding immediately
      ty = new ArrayType(ty, dims);

    ASTNode *init = nullptr;
    if (test(Token::Assign)) {
      bool isFloat = isa<FloatType>(base);
      if (isa<ArrayType>(ty)) {
        auto arrayInit = dpBuildArrayInit(dims, isFloat, global);
        init = !global
          ? (ASTNode*) new LocalArrayNode((ASTNode **) arrayInit)
          : isFloat
            ? new ConstArrayNode((float*) arrayInit)
            : new ConstArrayNode((int*) arrayInit);
        
        // We can never infer the real type of ConstArrayNode in Sema.
        // Must place it here.
        init->type = ty;
      } else {
        init = dpParseExpr();
        if (global)
          init = !isFloat
            ? (ASTNode*) new IntNode(dpEarlyFold(init).getInt())
            : new FloatNode(dpEarlyFold(init).getFloat());
      }
    }
    // No initialization; all must be zero.
    if (!init && global) {
      if (isa<ArrayType>(ty)) {
        if (isa<FloatType>(base))
          init = new ConstArrayNode((float*) nullptr);
        else
          init = new ConstArrayNode((int*) nullptr);
        init->type = ty;
      } else init = new IntNode(0);
    }

    auto decl = new VarDeclNode(name, init, mut, global);
    decl->type = ty;
    decls.push_back(decl);

    // Record in symbol table.
    if (!mut) {
      if (!init)
        dpFail("[dialect-parse] const declaration requires initializer");
      symbols[name] = dpEarlyFold(init);
    }

    if (!test(Token::Comma) && !peek(Token::Semicolon))
      dpExpectToken(Token::Comma);
  } while (!test(Token::Semicolon));

  return new TransparentBlockNode(decls);
}

FnDeclNode *Parser::dpParseFnDecl() {
  Type *ret = dpParseSimpleType();

  auto name = dpExpectToken(Token::Ident).vs;
  currentFunc = name;

  std::vector<std::string> args;
  std::vector<Type*> params;

  dpExpectToken(Token::LPar);
  while (!test(Token::RPar)) {
    auto ty = dpParseSimpleType();
    args.push_back(dpExpectToken(Token::Ident).vs);
    std::vector<int> dims;

    bool isPointer = false;
    if (test(Token::LBrak)) {
      isPointer = true;
      dpExpectToken(Token::RBrak);
    }

    while (test(Token::LBrak)) {
      dims.push_back(dpEarlyFold(dpParseExpr()).getInt());
      dpExpectToken(Token::RBrak);
    }
    
    if (dims.size() != 0)
      ty = new ArrayType(ty, dims);

    if (isPointer)
      ty = new PointerType(ty);

    params.push_back(ty);

    if (!test(Token::Comma) && !peek(Token::RPar))
      dpExpectToken(Token::Comma);
  }

  auto decl = new FnDeclNode(name, args, dpParseBlock());
  decl->type = new FunctionType(ret, params);
  return decl;
}

BlockNode *Parser::dpParseCompUnit() {
  std::vector<ASTNode*> nodes;
  while (!test(Token::End)) {
    if (peek(Token::Const)) {
      nodes.push_back(dpParseVarDecl(true));
      continue;
    }

    // For functions, it would be:
    //   Type ident `(`
    // while for variables it's `=`.
    // Moreover, the Type is only a single token,
    // so we lookahead for 2 tokens.
    if (loc + 2 < tokens.size() && tokens[loc + 2].type == Token::LPar) {
      nodes.push_back(dpParseFnDecl());
      continue;
    }

    nodes.push_back(dpParseVarDecl(true));
  }

  return new BlockNode(nodes);
}
