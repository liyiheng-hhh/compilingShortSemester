#pragma once

#include "ast.h"
#include "common.h"
#include "token.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using std::make_unique;
using std::min;
using std::string;
using std::unique_ptr;
using std::vector;

class Parser {
public:
  explicit Parser(vector<Token> tokens);

  Program parseProgram();

private:
  vector<Token> tokens_;
  std::size_t pos_ = 0;

  const Token &tok(std::size_t ahead = 0) const;

  bool check(TokenKind kind) const;

  bool match(TokenKind kind);

  const Token &expect(TokenKind kind, const string &what);

  CompileError error(const string &message) const;

  static bool isBType(TokenKind kind);

  BaseType parseBType();

  BaseType parseFuncType();

  bool isFuncDefAhead() const;

  unique_ptr<DeclStmt> parseDecl();

  VarDef parseVarDef(bool requireInit);

  unique_ptr<InitVal> parseInitVal();

  unique_ptr<FuncDef> parseFuncDef();

  Param parseParam();

  unique_ptr<BlockStmt> parseBlock();

  StmtPtr parseStmt();

  // ToyC: Exp → LOrExpr（含 !）
  ExprPtr parseExp();

  // 保留 parseCond 供内部使用，与 parseExp 等价
  ExprPtr parseCond();

  unique_ptr<LValExpr> parseLVal();

  ExprPtr parsePrimary();

  ExprPtr parseUnary(bool allowBang);

  ExprPtr parseMul(bool allowBang);

  ExprPtr parseAdd(bool allowBang);

  ExprPtr parseRel();

  ExprPtr parseEq();

  ExprPtr parseLAnd();

  ExprPtr parseLOr();
};
