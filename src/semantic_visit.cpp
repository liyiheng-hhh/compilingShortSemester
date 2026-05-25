// compiler2026-x phase-2 (semantic split)

#include "semantic.h"

using namespace std;

void Semantic::visitFunction(FuncDef &def) {
    currentFunction_ = def.function;
    currentFunction_->frameUsed = 16;
    enterScope();
    for (size_t i = 0; i < def.params.size(); ++i) {
      Param &p = def.params[i];
      vector<int> dims = evalDims(p.tailDims);
      p.dims = dims;
      currentFunction_->params[i].dims = dims;
      Symbol *sym = declareSymbol(p.name, p.base, false, false, p.isArray,
                                  true, p.isArray, dims, p.line);
      p.symbol = sym;
    }
    visitBlock(*def.body, true);
    leaveScope();
    currentFunction_->frameSize = alignTo(currentFunction_->frameUsed, 16);
    currentFunction_ = nullptr;
  }

void Semantic::visitBlock(BlockStmt &block, bool createScope) {
    if (createScope) {
      enterScope();
    }
    for (auto &item : block.items) {
      visitStmt(item.get());
    }
    if (createScope) {
      leaveScope();
    }
  }

void Semantic::visitStmt(Stmt *stmt) {
    switch (stmt->kind) {
    case StmtKind::Decl:
      visitLocalDecl(*static_cast<DeclStmt *>(stmt));
      break;
    case StmtKind::Block:
      visitBlock(*static_cast<BlockStmt *>(stmt), true);
      break;
    case StmtKind::Assign: {
      auto *s = static_cast<AssignStmt *>(stmt);
      visitExpr(s->lhs.get());
      if (s->lhs->symbol && s->lhs->symbol->isConst) {
        fail(s->line, "assignment to const " + s->lhs->name);
      }
      visitExpr(s->rhs.get());
      break;
    }
    case StmtKind::Expr: {
      auto *s = static_cast<ExprStmt *>(stmt);
      if (s->expr) {
        visitExpr(s->expr.get());
      }
      break;
    }
    case StmtKind::If: {
      auto *s = static_cast<IfStmt *>(stmt);
      visitExpr(s->cond.get());
      visitStmt(s->thenStmt.get());
      if (s->elseStmt) {
        visitStmt(s->elseStmt.get());
      }
      break;
    }
    case StmtKind::While: {
      auto *s = static_cast<WhileStmt *>(stmt);
      visitExpr(s->cond.get());
      visitStmt(s->body.get());
      break;
    }
    case StmtKind::Return: {
      auto *s = static_cast<ReturnStmt *>(stmt);
      if (s->expr) {
        visitExpr(s->expr.get());
      }
      break;
    }
    case StmtKind::Break:
    case StmtKind::Continue:
      break;
    }
  }

void Semantic::visitLocalDecl(DeclStmt &decl) {
    for (VarDef &def : decl.defs) {
      vector<int> dims = evalDims(def.dims);
      bool isArray = !dims.empty();
      Symbol *sym = declareSymbol(def.name, decl.base, decl.isConst, false,
                                  isArray, false, false, dims, def.line);
      def.symbol = sym;
      if (def.init) {
        visitInit(def.init.get());
      }
      if (decl.isConst && !isArray) {
        if (!def.init || def.init->isList || !def.init->expr ||
            !def.init->expr->isConst) {
          fail(def.line, "const scalar requires constant initializer");
        }
        sym->hasConstValue = true;
        sym->constValue = castConst(def.init->expr->constVal, decl.base);
      }
      if (decl.isConst && isArray) {
        if (!def.init) {
          fail(def.line, "const array requires initializer");
        }
        sym->initValues = flattenConstInit(def.init.get(), dims, decl.base);
      }
    }
  }

void Semantic::visitInit(InitVal *init) {
    if (init->isList) {
      for (auto &child : init->list) {
        visitInit(child.get());
      }
    } else if (init->expr) {
      visitExpr(init->expr.get());
    }
  }

void Semantic::visitExpr(Expr *expr) {
    switch (expr->kind) {
    case ExprKind::Number: {
      auto *n = static_cast<NumberExpr *>(expr);
      expr->isConst = true;
      if (n->isFloat) {
        expr->type = Type::scalar(BaseType::Float);
        expr->constVal.type = BaseType::Float;
        expr->constVal.f = n->floatVal;
      } else {
        expr->type = Type::scalar(BaseType::Int);
        expr->constVal.type = BaseType::Int;
        expr->constVal.i = n->intVal;
      }
      break;
    }
    case ExprKind::String: {
      expr->type = Type::pointer(BaseType::Int);
      expr->isConst = false;
      break;
    }
    case ExprKind::LVal:
      visitLVal(static_cast<LValExpr *>(expr));
      break;
    case ExprKind::Call:
      visitCall(static_cast<CallExpr *>(expr));
      break;
    case ExprKind::Unary:
      visitUnary(static_cast<UnaryExpr *>(expr));
      break;
    case ExprKind::Binary:
      visitBinary(static_cast<BinaryExpr *>(expr));
      break;
    }
  }

void Semantic::visitLVal(LValExpr *expr) {
    Symbol *sym = lookupVar(expr->name);
    if (!sym) {
      fail(expr->line, "undefined variable " + expr->name);
    }
    expr->symbol = sym;
    for (auto &idx : expr->indices) {
      visitExpr(idx.get());
    }
    int rank = sym->isArray ? static_cast<int>(sym->dims.size()) + (sym->isParamArray ? 1 : 0) : 0;
    if (static_cast<int>(expr->indices.size()) < rank) {
      vector<int> remaining;
      if (sym->isParamArray) {
        int consumed = static_cast<int>(expr->indices.size());
        for (int i = max(0, consumed - 1); i < static_cast<int>(sym->dims.size()); ++i) {
          remaining.push_back(sym->dims[i]);
        }
      } else {
        for (size_t i = expr->indices.size(); i < sym->dims.size(); ++i) {
          remaining.push_back(sym->dims[i]);
        }
      }
      expr->type = Type::pointer(sym->base, remaining);
    } else {
      expr->type = Type::scalar(sym->base);
    }

    if (sym->hasConstValue && expr->indices.empty()) {
      expr->isConst = true;
      expr->constVal = sym->constValue;
    } else if (sym->isConst && sym->isArray && !expr->type.isPointer &&
               !sym->initValues.empty()) {
      bool allConst = true;
      vector<int> idxValues;
      for (auto &idx : expr->indices) {
        if (!idx->isConst) {
          allConst = false;
          break;
        }
        idxValues.push_back(constAsInt(idx->constVal));
      }
      if (allConst) {
        int flat = flattenIndex(*sym, idxValues);
        if (flat >= 0 && flat < static_cast<int>(sym->initValues.size())) {
          expr->isConst = true;
          expr->constVal = sym->initValues[flat];
        }
      }
    }
  }

int Semantic::flattenIndex(const Symbol &sym, const vector<int> &idxValues) {
    int flat = 0;
    if (sym.isParamArray) {
      vector<int> fullDims = sym.dims;
      for (size_t i = 0; i < idxValues.size(); ++i) {
        int stride = i == 0 ? product(fullDims, 0)
                            : (i - 1 < fullDims.size() ? product(fullDims, i) : 1);
        flat += idxValues[i] * stride;
      }
      return flat;
    }
    for (size_t i = 0; i < idxValues.size(); ++i) {
      int stride = i + 1 < sym.dims.size() ? product(sym.dims, i + 1) : 1;
      flat += idxValues[i] * stride;
    }
    return flat;
  }

void Semantic::checkCallArgs(CallExpr *expr) {
    Function *fn = expr->function;
    const size_t na = expr->args.size();

    if (fn->variadic) {
      if (na < fn->params.size()) {
        fail(expr->line, "too few arguments to " + expr->name);
      }
      for (size_t i = 0; i < fn->params.size(); ++i) {
        Expr *arg = expr->args[i].get();
        const ParamType &pt = fn->params[i];
        if (pt.isArray) {
          if (!arg->type.isPointer || arg->type.base != pt.base) {
            fail(arg->line, "argument type mismatch in call to " + expr->name);
          }
        } else if (pt.base == BaseType::Int) {
          if (!arg->type.isIntScalar() && !arg->type.isFloatScalar()) {
            fail(arg->line, "argument type mismatch in call to " + expr->name);
          }
        } else {
          if (!arg->type.isFloatScalar() && !arg->type.isIntScalar()) {
            fail(arg->line, "argument type mismatch in call to " + expr->name);
          }
        }
      }
      return;
    }

    if (fn->injectLineArgument && fn->runtime) {
      if (fn->params.empty()) {
        fail(expr->line, "internal: injectLine function has no params");
      }
      if (na + 1 != fn->params.size()) {
        fail(expr->line, "wrong number of arguments to " + expr->name);
      }
      return;
    }

    if (!fn->def) {
      if (na != fn->params.size()) {
        fail(expr->line, "wrong number of arguments to " + expr->name);
      }
      for (size_t i = 0; i < na; ++i) {
        Expr *arg = expr->args[i].get();
        const ParamType &pt = fn->params[i];
        if (pt.isArray) {
          if (!arg->type.isPointer || arg->type.base != pt.base) {
            fail(arg->line, "argument type mismatch in call to " + expr->name);
          }
        } else if (pt.base == BaseType::Int) {
          if (!arg->type.isIntScalar() && !arg->type.isFloatScalar()) {
            fail(arg->line, "argument type mismatch in call to " + expr->name);
          }
        } else {
          if (!arg->type.isFloatScalar() && !arg->type.isIntScalar()) {
            fail(arg->line, "argument type mismatch in call to " + expr->name);
          }
        }
      }
      return;
    }

    FuncDef *def = fn->def;
    if (na != def->params.size()) {
      fail(expr->line, "wrong number of arguments to " + expr->name);
    }
    for (size_t i = 0; i < na; ++i) {
      Expr *arg = expr->args[i].get();
      const Param &p = def->params[i];
      if (p.isArray) {
        if (!arg->type.isPointer || arg->type.base != p.base) {
          fail(arg->line, "argument type mismatch in call to " + expr->name);
        }
      } else if (p.base == BaseType::Int) {
        if (!arg->type.isIntScalar() && !arg->type.isFloatScalar()) {
          fail(arg->line, "argument type mismatch in call to " + expr->name);
        }
      } else {
        if (!arg->type.isFloatScalar() && !arg->type.isIntScalar()) {
          fail(arg->line, "argument type mismatch in call to " + expr->name);
        }
      }
    }
  }

void Semantic::visitCall(CallExpr *expr) {
    auto it = functions_.find(expr->name);
    if (it == functions_.end()) {
      fail(expr->line, "undefined function " + expr->name);
    }
    expr->function = it->second;
    for (auto &arg : expr->args) {
      visitExpr(arg.get());
    }
    expr->type = Type::scalar(expr->function->ret);
    checkCallArgs(expr);
  }

void Semantic::visitUnary(UnaryExpr *expr) {
    visitExpr(expr->expr.get());
    expr->isConst = expr->expr->isConst;
    if (expr->op == "!") {
      expr->type = Type::scalar(BaseType::Int);
      if (expr->isConst) {
        expr->constVal.type = BaseType::Int;
        if (expr->expr->constVal.type == BaseType::Float) {
          expr->constVal.i = constAsFloat(expr->expr->constVal) == 0.0f;
        } else {
          expr->constVal.i = constAsInt(expr->expr->constVal) == 0;
        }
      }
      return;
    }
    expr->type = expr->expr->type;
    if (expr->isConst) {
      if (expr->op == "+") {
        expr->constVal = expr->expr->constVal;
      } else if (expr->op == "-") {
        expr->constVal = expr->expr->constVal;
        if (expr->constVal.type == BaseType::Float) {
          expr->constVal.f = -expr->constVal.f;
        } else {
          expr->constVal.i = -expr->constVal.i;
        }
      }
    }
  }

void Semantic::visitBinary(BinaryExpr *expr) {
    visitExpr(expr->lhs.get());
    visitExpr(expr->rhs.get());
    const string &op = expr->op;
    if (op == "&&" || op == "||" || op == "==" || op == "!=" || op == "<" ||
        op == ">" || op == "<=" || op == ">=") {
      expr->type = Type::scalar(BaseType::Int);
    } else if (op == "%") {
      expr->type = Type::scalar(BaseType::Int);
    } else if (expr->lhs->type.isFloatScalar() || expr->rhs->type.isFloatScalar()) {
      expr->type = Type::scalar(BaseType::Float);
    } else {
      expr->type = Type::scalar(BaseType::Int);
    }

    if (expr->lhs->isConst && expr->rhs->isConst) {
      expr->isConst = true;
      expr->constVal = evalBinaryConst(op, expr->lhs->constVal, expr->rhs->constVal,
                                       expr->type.base);
    }
  }

ConstValue Semantic::evalBinaryConst(const string &op, ConstValue lhs, ConstValue rhs,
                             BaseType resultType) {
    ConstValue out;
    out.type = resultType;
    bool useFloat = lhs.type == BaseType::Float || rhs.type == BaseType::Float;
    if (op == "&&" || op == "||") {
      bool l = lhs.type == BaseType::Float ? constAsFloat(lhs) != 0.0f
                                           : constAsInt(lhs) != 0;
      bool r = rhs.type == BaseType::Float ? constAsFloat(rhs) != 0.0f
                                           : constAsInt(rhs) != 0;
      out.i = op == "&&" ? (l && r) : (l || r);
      return out;
    }
    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" ||
        op == ">=") {
      if (useFloat) {
        float l = constAsFloat(lhs), r = constAsFloat(rhs);
        if (op == "==") out.i = l == r;
        if (op == "!=") out.i = l != r;
        if (op == "<") out.i = l < r;
        if (op == ">") out.i = l > r;
        if (op == "<=") out.i = l <= r;
        if (op == ">=") out.i = l >= r;
      } else {
        int32_t l = constAsInt(lhs), r = constAsInt(rhs);
        if (op == "==") out.i = l == r;
        if (op == "!=") out.i = l != r;
        if (op == "<") out.i = l < r;
        if (op == ">") out.i = l > r;
        if (op == "<=") out.i = l <= r;
        if (op == ">=") out.i = l >= r;
      }
      return out;
    }
    if (resultType == BaseType::Float) {
      float l = constAsFloat(lhs), r = constAsFloat(rhs);
      if (op == "+") out.f = l + r;
      if (op == "-") out.f = l - r;
      if (op == "*") out.f = l * r;
      if (op == "/") out.f = l / r;
      return out;
    }
    int32_t l = constAsInt(lhs), r = constAsInt(rhs);
    if (op == "+") out.i = l + r;
    if (op == "-") out.i = l - r;
    if (op == "*") out.i = l * r;
    if (op == "/") out.i = r == 0 ? 0 : l / r;
    if (op == "%") out.i = r == 0 ? 0 : l % r;
    return out;
  }

void Semantic::validateMain() const {
  const FuncDef *mainDef = nullptr;
  for (const auto &item : program_.items) {
    if (!item.func) {
      continue;
    }
    if (item.func->name != "main") {
      continue;
    }
    if (mainDef) {
      fail(item.func->line, "multiple definitions of main");
    }
    mainDef = item.func.get();
  }
  if (!mainDef) {
    fail(1, "main function not defined");
  }
  if (!mainDef->params.empty()) {
    fail(mainDef->line, "main function must take no parameters");
  }
  if (mainDef->ret != BaseType::Int) {
    fail(mainDef->line, "main function must return int");
  }
}
