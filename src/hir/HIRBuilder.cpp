#include "HIRBuilder.h"

#include "../ast.h"

#include <memory>

namespace sys::hir {

static std::unique_ptr<Op> buildExpr(const Expr *e);
static std::unique_ptr<Op> buildStmt(const Stmt *s);
static std::unique_ptr<Op> buildBlock(const BlockStmt *blk);

static std::unique_ptr<Op> buildExpr(const Expr *e) {
  if (!e) return nullptr;

  switch (e->kind) {
    case ExprKind::Number: {
      auto *num = static_cast<const NumberExpr *>(e);
      auto op = std::make_unique<Op>(OpKind::ConstInt, const_cast<Expr *>(e));
      op->hasIntValue = !num->isFloat;
      op->intValue = num->isFloat ? static_cast<long long>(num->floatVal) : num->intVal;
      op->hasFloatValue = num->isFloat;
      op->floatValue = num->floatVal;
      op->type = num->isFloat ? TypeKind::Float : TypeKind::Int;
      return op;
    }
    case ExprKind::LVal: {
      auto *lv = static_cast<const LValExpr *>(e);
      auto op = std::make_unique<Op>(OpKind::Load, const_cast<Expr *>(e));
      op->symbol = lv->name;
      op->type = mapType(lv->type);
      return op;
    }
    case ExprKind::Binary: {
      auto *bin = static_cast<const BinaryExpr *>(e);
      auto op = std::make_unique<Op>(OpKind::Arith, const_cast<Expr *>(e));
      op->symbol = bin->op;
      if (auto lhs = buildExpr(bin->lhs.get())) op->append(std::move(lhs));
      if (auto rhs = buildExpr(bin->rhs.get())) op->append(std::move(rhs));
      return op;
    }
    case ExprKind::Unary: {
      auto *un = static_cast<const UnaryExpr *>(e);
      auto op = std::make_unique<Op>(OpKind::Arith, const_cast<Expr *>(e));
      op->symbol = un->op;
      if (auto sub = buildExpr(un->expr.get())) op->append(std::move(sub));
      return op;
    }
    case ExprKind::Call: {
      auto *call = static_cast<const CallExpr *>(e);
      auto op = std::make_unique<Op>(OpKind::Call, const_cast<Expr *>(e));
      op->symbol = call->name;
      for (const auto &arg : call->args) {
        if (auto a = buildExpr(arg.get())) op->append(std::move(a));
      }
      return op;
    }
    default:
      return std::make_unique<Op>(OpKind::Unknown, const_cast<Expr *>(e));
  }
}

static std::unique_ptr<Op> buildStmt(const Stmt *s) {
  if (!s) return nullptr;

  switch (s->kind) {
    case StmtKind::Block: {
      return buildBlock(static_cast<const BlockStmt *>(s));
    }
    case StmtKind::While: {
      auto *w = static_cast<const WhileStmt *>(s);
      auto op = std::make_unique<Op>(OpKind::While, const_cast<Stmt *>(s));
      if (auto cond = buildExpr(w->cond.get())) op->append(std::move(cond));
      if (auto body = buildStmt(w->body.get())) op->append(std::move(body));
      return op;
    }
    case StmtKind::If: {
      auto *ifs = static_cast<const IfStmt *>(s);
      auto op = std::make_unique<Op>(OpKind::If, const_cast<Stmt *>(s));
      if (auto cond = buildExpr(ifs->cond.get())) op->append(std::move(cond));
      if (ifs->thenStmt) {
        if (auto thenOp = buildStmt(ifs->thenStmt.get())) op->append(std::move(thenOp));
      }
      if (ifs->elseStmt) {
        if (auto elseOp = buildStmt(ifs->elseStmt.get())) op->append(std::move(elseOp));
      }
      return op;
    }
    case StmtKind::Assign: {
      auto *as = static_cast<const AssignStmt *>(s);
      auto op = std::make_unique<Op>(OpKind::Store, const_cast<Stmt *>(s));
      op->symbol = as->lhs ? as->lhs->name : "";
      if (as->lhs) {
        for (const auto &ix : as->lhs->indices) {
          if (auto iop = buildExpr(ix.get())) op->append(std::move(iop));
        }
      }
      if (auto rhs = buildExpr(as->rhs.get())) op->append(std::move(rhs));
      return op;
    }
    case StmtKind::Expr: {
      auto *es = static_cast<const ExprStmt *>(s);
      return buildExpr(es->expr.get());
    }
    case StmtKind::Return: {
      // For now treat return as a simple terminator
      return std::make_unique<Op>(OpKind::Return, const_cast<Stmt *>(s));
    }
    case StmtKind::Break:
      return std::make_unique<Op>(OpKind::Break, const_cast<Stmt *>(s));
    case StmtKind::Continue:
      return std::make_unique<Op>(OpKind::Continue, const_cast<Stmt *>(s));
    default:
      return std::make_unique<Op>(OpKind::Unknown, const_cast<Stmt *>(s));
  }
}

static std::unique_ptr<Op> buildBlock(const BlockStmt *blk) {
  auto blockOp = std::make_unique<Op>(OpKind::Block, const_cast<BlockStmt *>(blk));
  for (const auto &item : blk->items) {
    if (auto child = buildStmt(item.get())) {
      blockOp->append(std::move(child));
    }
  }
  return blockOp;
}

static std::unique_ptr<Op> buildFunction(FuncDef *fn) {
  if (!fn) return nullptr;

  auto funcOp = std::make_unique<Op>(OpKind::Func, fn);
  funcOp->symbol = fn->name;

  if (fn->body) {
    if (auto bodyOp = buildStmt(fn->body.get())) {
      funcOp->append(std::move(bodyOp));
    }
  }
  return funcOp;
}

std::unique_ptr<Module> buildHIR(const Program &program) {
  auto mod = std::make_unique<Module>();
  mod->originAst = const_cast<Program *>(&program);

  auto root = std::make_unique<Op>(OpKind::Module, const_cast<Program *>(&program));

  for (const auto &item : program.items) {
    if (item.func) {
      if (auto f = buildFunction(item.func.get())) {
        root->append(std::move(f));
      }
    }
  }

  mod->root = std::move(root);
  return mod;
}

} // namespace sys::hir