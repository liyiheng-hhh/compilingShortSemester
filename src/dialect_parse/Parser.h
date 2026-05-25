// compiler2026-x phase-C (header layout)
#ifndef PARSER_H
#define PARSER_H

// compiler2026-x phase-A (dialect_parse parser API)
#include <cassert>
#include <iostream>
#include <map>
#include <vector>
#include "TypeContext.h"
#include "Lexer.h"
#include "CompileError.h"
#include "ASTNode.h"


namespace sys {

// Compile-time constant for array dimension folding (integers only per spec).
class ConstValue {
  union {
    int *vi;
    float *vf;
  };
  std::vector<int> dims;
public:
  bool isFloat;

  ConstValue() {}
  ConstValue(int *vi, const std::vector<int> &dims): vi(vi), dims(dims), isFloat(false) {}
  ConstValue(float *vf, const std::vector<int> &dims): vf(vf), dims(dims), isFloat(true) {}

  ConstValue operator[](int i);
  int getInt();
  float getFloat();
  const auto &getDims() { return dims; }

  int size();
  int stride();

  int *getRaw();
  float *getRawFloat();
  void *getRawRef() { return vi; }

  void release();
};

class Parser {
  using SymbolTable = std::map<std::string, ConstValue>;
  SymbolTable symbols;

  class SemanticScope {
    Parser &parser;
    SymbolTable symbols;
  public:
    SemanticScope(Parser &parser): parser(parser), symbols(parser.symbols) {};
    ~SemanticScope() { parser.symbols = symbols; }
  };

  std::vector<Token> tokens;
  size_t loc;
  TypeContext &ctx;

  std::string currentFunc;

  Token dpLastToken();
  Token dpPeekToken();
  Token dpConsumeToken();

  bool peek(Token::Type t);
  Token dpExpectToken(Token::Type t);

  void dpPrintContext();
  [[noreturn]] void dpFail(const std::string &msg);

  template<class... Rest>
  bool peek(Token::Type t, Rest... ts) {
    return peek(t) || peek(ts...);
  }

  template<class... T>
  bool test(T... ts) {
    if (peek(ts...)) {
      loc++;
      return true;
    }
    return false;
  }

  Type *dpParseSimpleType();
  ConstValue dpEarlyFold(ASTNode *node);

  ASTNode *dpParsePrimary();
  ASTNode *dpParseUnary();
  ASTNode *dpParseMul();
  ASTNode *dpParseAdd();
  ASTNode *dpParseRel();
  ASTNode *dpParseEq();
  ASTNode *dpParseLAnd();
  ASTNode *dpParseLOr();
  ASTNode *dpParseExpr();
  ASTNode *dpParseStmt();
  BlockNode *dpParseBlock();
  TransparentBlockNode *dpParseVarDecl(bool global);
  FnDeclNode *dpParseFnDecl();
  BlockNode *dpParseCompUnit();

  void *dpBuildArrayInit(const std::vector<int> &dims, bool expectFloat, bool doFold);

public:
  Parser(const std::string &input, TypeContext &ctx);
  ASTNode *parse();
};

}

#endif
