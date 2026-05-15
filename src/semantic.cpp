#include "semantic.h"

#include <algorithm>
#include <limits>

using namespace std;

Semantic::Semantic(Program &program) : program_(program) {}

void Semantic::run() {
    addRuntimeFunctions();
    predeclareUserFunctions();
    for (auto &item : program_.items) {
      if (item.decl) {
        visitGlobalDecl(*item.decl);
      } else if (item.func) {
        visitFunction(*item.func);
      }
    }
  }

[[noreturn]] void Semantic::fail(int line, const string &message) const {
    ostringstream os;
    os << "line " << line << ": " << message;
    throw CompileError(os.str());
  }

Symbol *Semantic::newSymbol() {
    symbols_.push_back(make_unique<Symbol>());
    return symbols_.back().get();
  }

Function *Semantic::newFunction(const string &name, BaseType ret) {
    functionStorage_.push_back(make_unique<Function>());
    Function *fn = functionStorage_.back().get();
    fn->name = name;
    fn->asmName = name;
    fn->ret = ret;
    functions_[name] = fn;
    return fn;
  }

void Semantic::addRuntime(const string &name, BaseType ret, vector<ParamType> params,
                  string asmName, bool injectLine, bool variadic) {
    Function *fn = newFunction(name, ret);
    fn->runtime = true;
    fn->params = std::move(params);
    fn->asmName = asmName.empty() ? name : std::move(asmName);
    fn->injectLineArgument = injectLine;
    fn->variadic = variadic;
  }

void Semantic::addRuntimeFunctions() {
    addRuntime("getint", BaseType::Int, {});
    addRuntime("getch", BaseType::Int, {});
    addRuntime("getfloat", BaseType::Float, {});
    addRuntime("getarray", BaseType::Int,
               {ParamType{BaseType::Int, true, {}}});
    addRuntime("getfarray", BaseType::Int,
               {ParamType{BaseType::Float, true, {}}});
    addRuntime("putint", BaseType::Void, {ParamType{BaseType::Int, false, {}}});
    addRuntime("putch", BaseType::Void, {ParamType{BaseType::Int, false, {}}});
    addRuntime("putfloat", BaseType::Void,
               {ParamType{BaseType::Float, false, {}}});
    addRuntime("putarray", BaseType::Void,
               {ParamType{BaseType::Int, false, {}},
                ParamType{BaseType::Int, true, {}}});
    addRuntime("putfarray", BaseType::Void,
               {ParamType{BaseType::Int, false, {}},
                ParamType{BaseType::Float, true, {}}});
    addRuntime("starttime", BaseType::Void, {ParamType{BaseType::Int, false, {}}},
               "_sysy_starttime", true);
    addRuntime("stoptime", BaseType::Void, {ParamType{BaseType::Int, false, {}}},
               "_sysy_stoptime", true);
    addRuntime("putf", BaseType::Void, {ParamType{BaseType::Int, true, {}}},
               "putf", false, true);
  }

void Semantic::predeclareUserFunctions() {
    for (auto &item : program_.items) {
      if (!item.func) {
        continue;
      }
      FuncDef &def = *item.func;
      Function *fn = newFunction(def.name, def.ret);
      fn->def = &def;
      def.function = fn;
      for (const Param &p : def.params) {
        ParamType pt;
        pt.base = p.base;
        pt.isArray = p.isArray;
        fn->params.push_back(std::move(pt));
      }
    }
  }

void Semantic::enterScope() { scopes_.push_back({}); }

void Semantic::leaveScope() { scopes_.pop_back(); }

Symbol *Semantic::lookupVar(const string &name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second;
      }
    }
    auto g = globals_.find(name);
    if (g != globals_.end()) {
      return g->second;
    }
    return nullptr;
  }

void Semantic::addLocal(Symbol *sym, int line) {
    if (scopes_.empty()) {
      fail(line, "internal scope error");
    }
    auto &scope = scopes_.back();
    if (scope.count(sym->name)) {
      fail(line, "redefinition of " + sym->name);
    }
    scope[sym->name] = sym;
  }

int Semantic::allocFrame(int size, int align) {
    currentFunction_->frameUsed = alignTo(currentFunction_->frameUsed, align);
    currentFunction_->frameUsed += size;
    return -currentFunction_->frameUsed;
  }

void Semantic::allocateSymbolStorage(Symbol *sym) {
    if (sym->isGlobal || !sym->needsStorage) {
      return;
    }
    if (sym->isParamArray) {
      sym->offset = allocFrame(8, 8);
      return;
    }
    int size = 4;
    int align = 4;
    if (sym->isArray) {
      size = product(sym->dims) * 4;
    }
    sym->offset = allocFrame(size, align);
  }

Symbol *Semantic::declareSymbol(const string &name, BaseType base, bool isConst,
                        bool isGlobal, bool isArray, bool isParam,
                        bool isParamArray, const vector<int> &dims, int line) {
    Symbol *sym = newSymbol();
    sym->name = name;
    sym->label = name;
    sym->base = base;
    sym->isConst = isConst;
    sym->isGlobal = isGlobal;
    sym->isArray = isArray || isParamArray;
    sym->isParam = isParam;
    sym->isParamArray = isParamArray;
    sym->dims = dims;
    sym->needsStorage = !(isConst && !sym->isArray && !isParam);
    if (isGlobal) {
      if (globals_.count(name)) {
        fail(line, "redefinition of " + name);
      }
      globals_[name] = sym;
      if (sym->needsStorage) {
        globalsInOrder_.push_back(sym);
      }
    } else {
      allocateSymbolStorage(sym);
      addLocal(sym, line);
    }
    return sym;
  }

int Semantic::evalConstInt(Expr *expr) {
    visitExpr(expr);
    if (!expr->isConst) {
      fail(expr->line, "expected constant expression");
    }
    return constAsInt(expr->constVal);
  }

vector<int> Semantic::evalDims(vector<ExprPtr> &dims) {
    vector<int> out;
    for (auto &dimExpr : dims) {
      int value = evalConstInt(dimExpr.get());
      if (value < 0) {
        fail(dimExpr->line, "array dimension must be non-negative");
      }
      out.push_back(value);
    }
    return out;
  }

void Semantic::visitGlobalDecl(DeclStmt &decl) {
    for (VarDef &def : decl.defs) {
      vector<int> dims = evalDims(def.dims);
      bool isArray = !dims.empty();
      Symbol *sym = declareSymbol(def.name, decl.base, decl.isConst, true,
                                  isArray, false, false, dims, def.line);
      def.symbol = sym;
      if (isArray) {
        if (def.init) {
          sym->initValues = flattenConstInit(def.init.get(), dims, decl.base);
        } else {
          sym->initValues.assign(product(dims), zeroConst(decl.base));
        }
      } else {
        ConstValue value = zeroConst(decl.base);
        if (def.init) {
          if (def.init->isList || !def.init->expr) {
            fail(def.line, "scalar initializer must be an expression");
          }
          visitExpr(def.init->expr.get());
          if (!def.init->expr->isConst) {
            fail(def.line, "global initializer must be constant");
          }
          value = castConst(def.init->expr->constVal, decl.base);
        }
        sym->hasConstValue = decl.isConst;
        sym->constValue = value;
        if (sym->needsStorage) {
          sym->initValues = {value};
        }
      }
    }
  }

ConstValue Semantic::zeroConst(BaseType base) {
    ConstValue v;
    v.type = base;
    v.i = 0;
    v.f = 0.0f;
    return v;
  }

vector<ConstValue> Semantic::flattenConstInit(InitVal *init, const vector<int> &dims,
                                      BaseType base) {
    vector<ConstValue> values(product(dims), zeroConst(base));
    if (!init) {
      return values;
    }
    fillConstAggregate(init, dims, 0, 0, values, base);
    return values;
  }

int Semantic::fillConstAggregate(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<ConstValue> &values, BaseType base) {
    if (!init->isList) {
      if (start >= static_cast<int>(values.size())) {
        fail(init->expr ? init->expr->line : 0, "too many initializer elements");
      }
      visitExpr(init->expr.get());
      if (!init->expr->isConst) {
        fail(init->expr->line, "initializer is not constant");
      }
      values[start] = castConst(init->expr->constVal, base);
      return start + 1;
    }

    int subSize = depth >= dims.size() ? 1 : product(dims, depth);
    if (init->list.empty()) {
      return start + subSize;
    }
    int idx = start;
    for (auto &child : init->list) {
      if (child->isList) {
        size_t childDepth = chooseInitChildDepth(dims, depth, idx);
        idx = fillConstAggregate(child.get(), dims, childDepth, idx, values, base);
      } else {
        idx = fillConstAggregate(child.get(), dims, dims.size(), idx, values, base);
      }
      if (idx > start + subSize) {
        fail(child->expr ? child->expr->line : 0, "too many initializer elements");
      }
    }
    return start + subSize;
  }

size_t Semantic::chooseInitChildDepth(const vector<int> &dims, size_t depth, int flatIndex) {
    size_t childDepth = min(depth + 1, dims.size());
    while (childDepth < dims.size()) {
      int childSize = product(dims, childDepth);
      if (childSize == 0 || flatIndex % childSize == 0) {
        break;
      }
      ++childDepth;
    }
    return childDepth;
  }

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
    if (op == "<<" || op == ">>") {
      if (expr->lhs->type.isFloatScalar() || expr->rhs->type.isFloatScalar()) {
        fail(expr->line, "shift operators require int operands");
      }
      expr->type = Type::scalar(BaseType::Int);
      if (expr->lhs->isConst && expr->rhs->isConst) {
        expr->isConst = true;
        expr->constVal =
            evalBinaryConst(op, expr->lhs->constVal, expr->rhs->constVal, BaseType::Int);
      }
      return;
    }
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
    if (op == "<<") {
      int sh = r & 31;
      out.i = static_cast<int32_t>(static_cast<uint32_t>(l) << sh);
      return out;
    }
    if (op == ">>") {
      out.i = static_cast<int32_t>(l >> (r & 31));
      return out;
    }
    return out;
  }
