// compiler2026-x phase-2 (semantic split)

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
    validateMain();
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
