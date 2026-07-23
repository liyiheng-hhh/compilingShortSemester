#pragma once

#include "common.hpp"
#include <memory>
#include <string>
#include <vector>

namespace toyc {

enum class ValueType {
    Int,
    Void,
};

enum class ExprKind {
    Number,
    Variable,
    Unary,
    Binary,
    Call,
};

enum class CtfeOp {
    None,
    Positive,
    Negative,
    LogicalNot,
    Add,
    Subtract,
    Multiply,
    Divide,
    Remainder,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    Equal,
    NotEqual,
    LogicalAnd,
    LogicalOr,
};

struct Function;

struct Expr {
    ExprKind kind = ExprKind::Number;
    SourceLocation loc;
    std::int64_t number = 0;
    std::string name;
    std::string op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
    std::vector<std::unique_ptr<Expr>> args;
    int ctfeSlot = -1;
    bool ctfeGlobal = false;
    Function* ctfeCallee = nullptr;
    CtfeOp ctfeOp = CtfeOp::None;
};

struct Decl {
    bool isConst = false;
    std::string name;
    std::unique_ptr<Expr> init;
    SourceLocation loc;
    int ctfeSlot = -1;
    bool ctfeGlobal = false;
};

enum class StmtKind {
    Block,
    Empty,
    ExprStmt,
    Assign,
    DeclStmt,
    If,
    While,
    Break,
    Continue,
    Return,
};

struct Stmt {
    StmtKind kind = StmtKind::Empty;
    SourceLocation loc;
    std::vector<std::unique_ptr<Stmt>> statements;
    std::unique_ptr<Expr> expr;
    std::string name;
    std::unique_ptr<Decl> decl;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
    int ctfeSlot = -1;
    bool ctfeGlobal = false;
};

struct Function {
    ValueType returnType = ValueType::Int;
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<Stmt> body;
    SourceLocation loc;
    std::size_t ctfeLocalCount = 0;
    bool ctfeMemoizable = false;
};

struct CompUnit {
    std::vector<std::unique_ptr<Decl>> globals;
    std::vector<std::unique_ptr<Function>> functions;
};

} // namespace toyc
