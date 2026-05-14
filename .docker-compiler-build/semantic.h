#pragma once

#include "ast.h"
#include "common.h"

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using std::make_unique;
using std::max;
using std::min;
using std::string;
using std::unique_ptr;
using std::vector;

class Semantic {
public:
  explicit Semantic(Program &program);

  void run();

  const vector<unique_ptr<Symbol>> &symbols() const { return symbols_; }

  const vector<Symbol *> &globalsInOrder() const { return globalsInOrder_; }

  const std::unordered_map<string, Function *> &functions() const {
    return functions_;
  }

private:
  Program &program_;
  vector<unique_ptr<Symbol>> symbols_;
  vector<unique_ptr<Function>> functionStorage_;
  vector<Symbol *> globalsInOrder_;
  std::unordered_map<string, Symbol *> globals_;
  std::unordered_map<string, Function *> functions_;
  vector<std::unordered_map<string, Symbol *>> scopes_;
  Function *currentFunction_ = nullptr;

  [[noreturn]] void fail(int line, const string &message) const;

  Symbol *newSymbol();

  Function *newFunction(const string &name, BaseType ret);

  void addRuntime(const string &name, BaseType ret, vector<ParamType> params,
                  string asmName = "", bool injectLine = false,
                  bool variadic = false);

  void addRuntimeFunctions();

  void predeclareUserFunctions();

  void enterScope();

  void leaveScope();

  Symbol *lookupVar(const string &name) const;

  void addLocal(Symbol *sym, int line);

  int allocFrame(int size, int align);

  void allocateSymbolStorage(Symbol *sym);

  Symbol *declareSymbol(const string &name, BaseType base, bool isConst,
                        bool isGlobal, bool isArray, bool isParam,
                        bool isParamArray, const vector<int> &dims, int line);

  int evalConstInt(Expr *expr);

  vector<int> evalDims(vector<ExprPtr> &dims);

  void visitGlobalDecl(DeclStmt &decl);

  ConstValue zeroConst(BaseType base);

  vector<ConstValue> flattenConstInit(InitVal *init, const vector<int> &dims,
                                      BaseType base);

  int fillConstAggregate(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<ConstValue> &values, BaseType base);

  size_t chooseInitChildDepth(const vector<int> &dims, size_t depth,
                              int flatIndex);

  void visitFunction(FuncDef &def);

  void visitBlock(BlockStmt &block, bool createScope);

  void visitStmt(Stmt *stmt);

  void visitLocalDecl(DeclStmt &decl);

  void visitInit(InitVal *init);

  void visitExpr(Expr *expr);

  void visitLVal(LValExpr *expr);

  int flattenIndex(const Symbol &sym, const vector<int> &idxValues);

  void visitCall(CallExpr *expr);

  void visitUnary(UnaryExpr *expr);

  void visitBinary(BinaryExpr *expr);

  ConstValue evalBinaryConst(const string &op, ConstValue lhs, ConstValue rhs,
                             BaseType resultType);
};
