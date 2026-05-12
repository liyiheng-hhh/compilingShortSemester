#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

enum class BaseType { Void, Int, Float };

struct Type {
  BaseType base = BaseType::Int;
  bool isPointer = false;
  vector<int> dims;

  static Type scalar(BaseType b) {
    Type t;
    t.base = b;
    return t;
  }

  static Type pointer(BaseType b, vector<int> remainingDims = {}) {
    Type t;
    t.base = b;
    t.isPointer = true;
    t.dims = std::move(remainingDims);
    return t;
  }

  bool isFloatScalar() const { return !isPointer && base == BaseType::Float; }
  bool isIntScalar() const { return !isPointer && base == BaseType::Int; }
};

struct ConstValue {
  BaseType type = BaseType::Int;
  int32_t i = 0;
  float f = 0.0f;
};

inline int32_t constAsInt(ConstValue v) {
  if (v.type == BaseType::Float) {
    return static_cast<int32_t>(v.f);
  }
  return v.i;
}

inline float constAsFloat(ConstValue v) {
  if (v.type == BaseType::Float) {
    return v.f;
  }
  return static_cast<float>(v.i);
}

inline ConstValue castConst(ConstValue v, BaseType target) {
  ConstValue out;
  out.type = target;
  if (target == BaseType::Float) {
    out.f = constAsFloat(v);
  } else {
    out.i = constAsInt(v);
  }
  return out;
}

struct Symbol;
struct Function;

enum class ExprKind { Number, String, LVal, Call, Unary, Binary };

struct Expr {
  explicit Expr(ExprKind k, int lineNo) : kind(k), line(lineNo) {}
  virtual ~Expr() = default;
  ExprKind kind;
  int line = 1;
  Type type;
  bool isConst = false;
  ConstValue constVal;
};

using ExprPtr = unique_ptr<Expr>;

struct NumberExpr : Expr {
  NumberExpr(int line, int32_t value) : Expr(ExprKind::Number, line) {
    isFloat = false;
    intVal = value;
  }
  NumberExpr(int line, float value) : Expr(ExprKind::Number, line) {
    isFloat = true;
    floatVal = value;
  }
  bool isFloat = false;
  int32_t intVal = 0;
  float floatVal = 0.0f;
};

struct StringExpr : Expr {
  StringExpr(int line, string value)
      : Expr(ExprKind::String, line), value(std::move(value)) {}
  string value;
  string label;
};

struct LValExpr : Expr {
  LValExpr(int line, string n) : Expr(ExprKind::LVal, line), name(std::move(n)) {}
  string name;
  vector<ExprPtr> indices;
  Symbol *symbol = nullptr;
};

struct CallExpr : Expr {
  CallExpr(int line, string n) : Expr(ExprKind::Call, line), name(std::move(n)) {}
  string name;
  vector<ExprPtr> args;
  Function *function = nullptr;
};

struct UnaryExpr : Expr {
  UnaryExpr(int line, string op, ExprPtr expr)
      : Expr(ExprKind::Unary, line), op(std::move(op)), expr(std::move(expr)) {}
  string op;
  ExprPtr expr;
};

struct BinaryExpr : Expr {
  BinaryExpr(int line, string op, ExprPtr lhs, ExprPtr rhs)
      : Expr(ExprKind::Binary, line), op(std::move(op)), lhs(std::move(lhs)),
        rhs(std::move(rhs)) {}
  string op;
  ExprPtr lhs;
  ExprPtr rhs;
};

struct InitVal {
  bool isList = false;
  ExprPtr expr;
  vector<unique_ptr<InitVal>> list;
};

struct VarDef {
  string name;
  vector<ExprPtr> dims;
  unique_ptr<InitVal> init;
  Symbol *symbol = nullptr;
  int line = 1;
};

enum class StmtKind {
  Decl,
  Block,
  Assign,
  Expr,
  If,
  While,
  Break,
  Continue,
  Return
};

struct Stmt {
  explicit Stmt(StmtKind k, int lineNo) : kind(k), line(lineNo) {}
  virtual ~Stmt() = default;
  StmtKind kind;
  int line = 1;
};

using StmtPtr = unique_ptr<Stmt>;

struct DeclStmt : Stmt {
  DeclStmt(int line, bool c, BaseType b)
      : Stmt(StmtKind::Decl, line), isConst(c), base(b) {}
  bool isConst = false;
  BaseType base = BaseType::Int;
  vector<VarDef> defs;
};

struct BlockStmt : Stmt {
  explicit BlockStmt(int line) : Stmt(StmtKind::Block, line) {}
  vector<StmtPtr> items;
};

struct AssignStmt : Stmt {
  AssignStmt(int line, unique_ptr<LValExpr> lhs, ExprPtr rhs)
      : Stmt(StmtKind::Assign, line), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  unique_ptr<LValExpr> lhs;
  ExprPtr rhs;
};

struct ExprStmt : Stmt {
  ExprStmt(int line, ExprPtr expr) : Stmt(StmtKind::Expr, line), expr(std::move(expr)) {}
  ExprPtr expr;
};

struct IfStmt : Stmt {
  IfStmt(int line, ExprPtr cond, StmtPtr thenStmt, StmtPtr elseStmt)
      : Stmt(StmtKind::If, line), cond(std::move(cond)),
        thenStmt(std::move(thenStmt)), elseStmt(std::move(elseStmt)) {}
  ExprPtr cond;
  StmtPtr thenStmt;
  StmtPtr elseStmt;
};

struct WhileStmt : Stmt {
  WhileStmt(int line, ExprPtr cond, StmtPtr body)
      : Stmt(StmtKind::While, line), cond(std::move(cond)), body(std::move(body)) {}
  ExprPtr cond;
  StmtPtr body;
};

struct BreakStmt : Stmt {
  explicit BreakStmt(int line) : Stmt(StmtKind::Break, line) {}
};

struct ContinueStmt : Stmt {
  explicit ContinueStmt(int line) : Stmt(StmtKind::Continue, line) {}
};

struct ReturnStmt : Stmt {
  ReturnStmt(int line, ExprPtr expr) : Stmt(StmtKind::Return, line), expr(std::move(expr)) {}
  ExprPtr expr;
};

struct Param {
  string name;
  BaseType base = BaseType::Int;
  bool isArray = false;
  vector<ExprPtr> tailDims;
  vector<int> dims;
  Symbol *symbol = nullptr;
  int line = 1;
};

struct FuncDef {
  string name;
  BaseType ret = BaseType::Int;
  vector<Param> params;
  unique_ptr<BlockStmt> body;
  Function *function = nullptr;
  int line = 1;
};

struct TopItem {
  unique_ptr<DeclStmt> decl;
  unique_ptr<FuncDef> func;
};

struct Program {
  vector<TopItem> items;
};

struct Symbol {
  string name;
  string label;
  BaseType base = BaseType::Int;
  bool isConst = false;
  bool isGlobal = false;
  bool isArray = false;
  bool isParam = false;
  bool isParamArray = false;
  bool needsStorage = true;
  vector<int> dims;
  int offset = 0;
  bool hasConstValue = false;
  ConstValue constValue;
  vector<ConstValue> initValues;
};

struct ParamType {
  BaseType base = BaseType::Int;
  bool isArray = false;
  vector<int> dims;
};

struct Function {
  string name;
  string asmName;
  BaseType ret = BaseType::Int;
  vector<ParamType> params;
  bool runtime = false;
  bool injectLineArgument = false;
  bool variadic = false;
  FuncDef *def = nullptr;
  int frameUsed = 16;
  int frameSize = 16;
  string returnLabel;
};
