#include "HIRToCFG.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <limits>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "../dialect_hir/DhirBuilder.h"
#include "../utils/DynamicCast.h"

namespace sys::cfg {

namespace {

size_t productDims(const std::vector<int> &dims) {
  if (dims.empty())
    return 1;
  size_t prod = 1;
  for (int dim : dims)
    prod *= (size_t) std::max(dim, 1);
  return prod;
}

size_t typeSize(Type *ty);

size_t scalarSize(Type *ty) {
  if (!ty)
    return 4;
  if (isa<IntType>(ty) || isa<FloatType>(ty))
    return 4;
  if (isa<PointerType>(ty))
    return 8;
  if (isa<VoidType>(ty))
    return 0;
  if (isa<FunctionType>(ty))
    return 8;
  if (isa<ArrayType>(ty))
    return 8;
  return 4;
}

dhir::TypeKind mapElementType(Type *ty) {
  if (!ty)
    return dhir::TypeKind::Unknown;
  if (isa<IntType>(ty))
    return dhir::TypeKind::Int;
  if (isa<FloatType>(ty))
    return dhir::TypeKind::Float;
  if (auto ptr = dyn_cast<PointerType>(ty))
    return mapElementType(ptr->pointee);
  if (auto arr = dyn_cast<ArrayType>(ty))
    return mapElementType(arr->base);
  return dhir::TypeKind::Unknown;
}

size_t typeSize(Type *ty) {
  if (!ty)
    return 4;
  if (auto arr = dyn_cast<ArrayType>(ty))
    return typeSize(arr->base) * productDims(arr->dims);
  return scalarSize(ty);
}

std::vector<int> typeDims(Type *ty) {
  if (!ty)
    return {};
  if (auto arr = dyn_cast<ArrayType>(ty))
    return arr->dims;
  if (auto ptr = dyn_cast<PointerType>(ty)) {
    if (auto arr = dyn_cast<ArrayType>(ptr->pointee)) {
      // Parameter arrays like int a[][N][M] are represented as pointer-to-array.
      // Keep an unknown leading dimension so stride of the first index uses
      // known trailing dims (N, M, ...).
      std::vector<int> dims = arr->dims;
      dims.insert(dims.begin(), 0);
      return dims;
    }
  }
  return {};
}

std::vector<size_t> computeStrideBytes(const std::vector<int> &dims, size_t elemSize) {
  if (dims.empty())
    return {};
  std::vector<size_t> strides(dims.size(), elemSize ? elemSize : 4);
  for (size_t i = 0; i < dims.size(); i++) {
    size_t stride = elemSize ? elemSize : 4;
    for (size_t j = i + 1; j < dims.size(); j++)
      stride *= (size_t) std::max(dims[j], 1);
    strides[i] = stride;
  }
  return strides;
}

SymbolInfo buildSymbolInfo(const std::string &name, Type *ty, bool isGlobal, bool isParam, bool isMutable) {
  SymbolInfo info;
  info.name = name;
  info.type = dhir::mapType(ty);
  info.elementType = mapElementType(ty);
  info.dims = typeDims(ty);
  info.isGlobal = isGlobal;
  info.isParam = isParam;
  info.isMutable = isMutable;
  info.baseKind = isGlobal ? MemoryBaseKind::Global : (isParam ? MemoryBaseKind::Param : MemoryBaseKind::Local);
  info.elemSize = 4;
  if (info.elementType == dhir::TypeKind::Float || info.elementType == dhir::TypeKind::Int)
    info.elemSize = 4;
  else if (info.type == dhir::TypeKind::Pointer || info.type == dhir::TypeKind::Array)
    info.elemSize = 8;
  info.storageSize = typeSize(ty);
  if (info.storageSize == 0)
    info.storageSize = (info.type == dhir::TypeKind::Pointer || info.type == dhir::TypeKind::Array) ? 8 : 4;
  info.strideBytes = computeStrideBytes(info.dims, info.elemSize);
  return info;
}

std::string formatFloat(double value) {
  std::ostringstream oss;
  // Preserve enough decimal digits to round-trip through strtof() in
  // CFG->legacy lowering without introducing avoidable ULP drift.
  oss << std::setprecision(std::numeric_limits<float>::max_digits10) << value;
  return oss.str();
}

struct FuncLoweringState {
  int tempId = 0;
  int blockId = 0;
};

class Lowerer {
  const dhir::Module &hirModule;
  std::vector<std::string> &errors;
  std::unordered_map<const dhir::Op*, std::string> resolvedSymbols;
  std::unordered_map<std::string, int> declCounts;
  std::unordered_set<std::string> globalSymbols;

public:
  explicit Lowerer(const dhir::Module &hirModule, std::vector<std::string> &errors):
    hirModule(hirModule), errors(errors) {}

  Module run() {
    Module cfgModule;
    cfgModule.originAst = hirModule.originAst;

    if (!hirModule.root || hirModule.root->kind != dhir::OpKind::Module) {
      errors.push_back("hir->cfg: invalid HIR root");
      return cfgModule;
    }

    collectGlobals(*hirModule.root, cfgModule);

    for (const auto &child : hirModule.root->children) {
      if (!child)
        continue;
      if (child->kind != dhir::OpKind::Func)
        continue;
      cfgModule.funcs.push_back(lowerFunc(*child, cfgModule));
    }

    if (cfgModule.funcs.empty())
      errors.push_back("hir->cfg: no function found");
    return cfgModule;
  }

private:
  std::string getResolvedSymbol(const dhir::Op *op) const {
    if (!op)
      return "";
    auto it = resolvedSymbols.find(op);
    if (it != resolvedSymbols.end())
      return it->second;
    return op->symbol;
  }

  static FnDeclNode *findFnDeclInAst(ASTNode *root, const std::string &name) {
    if (!root)
      return nullptr;
    if (auto *fn = dyn_cast<FnDeclNode>(root)) {
      if (fn->name == name)
        return fn;
      return nullptr;
    }
    if (auto *blk = dyn_cast<BlockNode>(root)) {
      for (auto *node : blk->nodes) {
        if (auto *fn = findFnDeclInAst(node, name))
          return fn;
      }
    }
    return nullptr;
  }

  static FnDeclNode *lookupFnDecl(const dhir::Op &funcOp, const Module &cfgModule) {
    if (auto *fn = dyn_cast<FnDeclNode>(funcOp.origin))
      return fn;
    if (cfgModule.originAst)
      return findFnDeclInAst(cfgModule.originAst, funcOp.symbol);
    return nullptr;
  }

  void populateFunctionParams(Func &func, const dhir::Op &funcOp, const Module &cfgModule) {
    auto *fn = lookupFnDecl(funcOp, cfgModule);
    if (!fn)
      return;
    auto *fnTy = dyn_cast<FunctionType>(fn->type);
    if (!fnTy)
      return;
    func.returnType = dhir::mapType(fnTy->ret);
    func.params.clear();
    for (size_t i = 0; i < fn->args.size(); i++) {
      Type *argTy = i < fnTy->params.size() ? fnTy->params[i] : nullptr;
      func.params.push_back(buildSymbolInfo(fn->args[i], argTy, false, true, true));
    }
  }

  void resolveSymbolsRec(const dhir::Op *op,
                         std::vector<std::unordered_map<std::string, std::string>> &scopes,
                         std::unordered_map<std::string, int> &counters) {
    if (!op)
      return;

    if (op->kind == dhir::OpKind::While) {
      if (!op->children.empty())
        resolveSymbolsRec(op->children[0].get(), scopes, counters);
      if (op->children.size() >= 2) {
        scopes.emplace_back();
        resolveSymbolsRec(op->children[1].get(), scopes, counters);
        scopes.pop_back();
      }
      return;
    }

    if (op->kind == dhir::OpKind::If) {
      if (!op->children.empty())
        resolveSymbolsRec(op->children[0].get(), scopes, counters);
      for (size_t i = 1; i < op->children.size(); i++) {
        scopes.emplace_back();
        resolveSymbolsRec(op->children[i].get(), scopes, counters);
        scopes.pop_back();
      }
      return;
    }

    bool pushed = false;
    bool lexicalBlock = false;
    if (op->kind == dhir::OpKind::Block) {
      // TransparentBlockNode is frequently used as a declaration wrapper
      // (e.g. `int a = ...;`) and should not create a nested lexical scope
      // by itself. But mixed-content transparent blocks do represent real
      // statement scopes and must isolate shadowed symbols.
      bool transparent = op->origin && isa<TransparentBlockNode>(op->origin);
      if (!transparent) {
        lexicalBlock = true;
      } else {
        bool declWrapper = !op->children.empty();
        for (const auto &child : op->children) {
          if (!child || child->kind != dhir::OpKind::VarDecl) {
            declWrapper = false;
            break;
          }
        }
        lexicalBlock = !declWrapper;
      }
    }
    if (lexicalBlock || op->kind == dhir::OpKind::For) {
      scopes.emplace_back();
      pushed = true;
    }

    if (op->kind == dhir::OpKind::VarDecl && !op->symbol.empty()) {
      bool collides = globalSymbols.count(op->symbol) > 0;
      for (auto it = scopes.rbegin(); !collides && it != scopes.rend(); ++it)
        collides = it->count(op->symbol) > 0;
      bool needRename = declCounts[op->symbol] > 1 || collides;
      std::string renamed = op->symbol;
      if (needRename) {
        int id = counters[op->symbol]++;
        renamed = op->symbol + "$" + std::to_string(id);
      }
      scopes.back()[op->symbol] = renamed;
      resolvedSymbols[op] = renamed;
    } else if ((op->kind == dhir::OpKind::Load || op->kind == dhir::OpKind::Store) && !op->symbol.empty()) {
      std::string resolved = op->symbol;
      for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(op->symbol);
        if (found != it->end()) {
          resolved = found->second;
          break;
        }
      }
      resolvedSymbols[op] = resolved;
    }

    for (const auto &child : op->children)
      resolveSymbolsRec(child.get(), scopes, counters);

    if (pushed)
      scopes.pop_back();
  }

  void countVarDecls(const dhir::Op *op) {
    if (!op)
      return;
    if (op->kind == dhir::OpKind::VarDecl && !op->symbol.empty())
      declCounts[op->symbol]++;
    for (const auto &child : op->children)
      countVarDecls(child.get());
  }

  void buildResolvedSymbols(const dhir::Op &funcOp, const Module &cfgModule) {
    resolvedSymbols.clear();
    declCounts.clear();
    globalSymbols.clear();
    std::vector<std::unordered_map<std::string, std::string>> scopes;
    scopes.emplace_back();
    std::unordered_map<std::string, int> counters;

    if (hirModule.root) {
      std::function<void(const dhir::Op*)> collectGlobalsRec = [&](const dhir::Op *node) {
        if (!node)
          return;
        if (node->kind == dhir::OpKind::Func)
          return;
        if (node->kind == dhir::OpKind::VarDecl && !node->symbol.empty())
          globalSymbols.insert(node->symbol);
        for (const auto &child : node->children)
          collectGlobalsRec(child.get());
      };
      collectGlobalsRec(hirModule.root.get());
    }

    if (!funcOp.children.empty())
      countVarDecls(funcOp.children[0].get());

    FnDeclNode *fn = lookupFnDecl(funcOp, cfgModule);
    if (fn) {
      for (const auto &arg : fn->args)
        scopes.back()[arg] = arg;
    }
    if (!funcOp.children.empty())
      resolveSymbolsRec(funcOp.children[0].get(), scopes, counters);
  }

  static bool isTerminated(const Func &func, int bid) {
    if (bid < 0 || bid >= (int) func.blocks.size())
      return true;
    const auto &insts = func.blocks[bid].insts;
    if (insts.empty())
      return false;
    return isTerminator(insts.back().kind);
  }

  static void normalizePhiOrder(Func &func) {
    for (auto &bb : func.blocks) {
      if (bb.insts.empty())
        continue;
      std::vector<Inst> phis;
      std::vector<Inst> rest;
      phis.reserve(bb.insts.size());
      rest.reserve(bb.insts.size());
      for (auto &inst : bb.insts) {
        if (inst.kind == OpKind::Phi)
          phis.push_back(std::move(inst));
        else
          rest.push_back(std::move(inst));
      }
      std::vector<Inst> merged;
      merged.reserve(phis.size() + rest.size());
      for (auto &inst : phis)
        merged.push_back(std::move(inst));
      for (auto &inst : rest)
        merged.push_back(std::move(inst));
      bb.insts.swap(merged);
    }
  }

  static dhir::TypeKind inferExprType(const dhir::Op *op) {
    if (!op)
      return dhir::TypeKind::Unknown;
    if (op->kind == dhir::OpKind::Cmp)
      return dhir::TypeKind::Int;
    return op->type;
  }

  static size_t typeBytes(dhir::TypeKind kind) {
    switch (kind) {
    case dhir::TypeKind::Int:
    case dhir::TypeKind::Float:
      return 4;
    case dhir::TypeKind::Pointer:
    case dhir::TypeKind::Array:
    case dhir::TypeKind::Function:
      return 8;
    case dhir::TypeKind::Void:
      return 0;
    case dhir::TypeKind::Unknown:
      return 4;
    }
    return 4;
  }

  int newBlock(Func &func, FuncLoweringState &st, const std::string &prefix) {
    Block bb;
    bb.name = prefix + "." + std::to_string(st.blockId++);
    func.blocks.push_back(std::move(bb));
    return (int) func.blocks.size() - 1;
  }

  static void emit(Func &func, int bid, const Inst &inst) {
    if (bid < 0 || bid >= (int) func.blocks.size())
      return;
    func.blocks[bid].insts.push_back(inst);
  }

  void ensureTerminator(Func &func, int bid) {
    if (bid < 0 || bid >= (int) func.blocks.size())
      return;
    if (isTerminated(func, bid))
      return;
    Inst ret;
    ret.kind = OpKind::Ret;
    ret.args.push_back("#0");
    emit(func, bid, ret);
  }

  std::string newTemp(FuncLoweringState &st) {
    return "%t" + std::to_string(st.tempId++);
  }

  static std::unordered_map<std::string, SymbolInfo> toSymbolMap(const Func &func, const Module &module) {
    std::unordered_map<std::string, SymbolInfo> map;
    for (const auto &sym : module.globals)
      map[sym.name] = sym;
    for (const auto &sym : func.params)
      map[sym.name] = sym;
    for (const auto &sym : func.locals)
      map[sym.name] = sym;
    return map;
  }

  static void addStoreSym(std::set<std::string> *stores, const std::string &sym) {
    if (!stores || sym.empty())
      return;
    stores->insert(sym);
  }

  void collectGlobals(const dhir::Op &root, Module &cfgModule) {
    std::unordered_set<std::string> seen;
    std::function<void(const dhir::Op*)> visit = [&](const dhir::Op *op) {
      if (!op)
        return;
      if (op->kind == dhir::OpKind::Func)
        return;
      if (op->kind == dhir::OpKind::VarDecl && !op->symbol.empty() && !seen.count(op->symbol)) {
        const auto *origin = dyn_cast<VarDeclNode>(op->origin);
        Type *ty = origin ? origin->type : op->origin ? op->origin->type : nullptr;
        auto info = buildSymbolInfo(op->symbol, ty, true, false, origin ? origin->mut : true);

        if (origin && origin->init) {
          if (auto i = dyn_cast<IntNode>(origin->init)) {
            info.hasIntInit = true;
            info.intInit = i->value;
          } else if (auto f = dyn_cast<FloatNode>(origin->init)) {
            info.hasFloatInit = true;
            info.floatInit = f->value;
          } else if (auto arr = dyn_cast<ConstArrayNode>(origin->init)) {
            size_t elems = productDims(info.dims);
            if (elems == 0)
              elems = 1;
            if (arr->isFloat) {
              if (arr->vf)
                info.floatArrayInit.assign(arr->vf, arr->vf + elems);
            } else {
              if (arr->vi)
                info.intArrayInit.assign(arr->vi, arr->vi + elems);
            }
          }
        }
        seen.insert(op->symbol);
        cfgModule.globals.push_back(std::move(info));
      }
      for (const auto &child : op->children)
        visit(child.get());
    };

    visit(&root);
  }

  void collectLocalsRec(const dhir::Op *op, std::vector<SymbolInfo> &locals, std::unordered_set<std::string> &seen) {
    if (!op)
      return;
    std::string sym = getResolvedSymbol(op);
    if (op->kind == dhir::OpKind::VarDecl && !sym.empty() && !seen.count(sym)) {
      const auto *origin = op->origin ? dyn_cast<VarDeclNode>(op->origin) : nullptr;
      Type *ty = origin ? origin->type : op->origin ? op->origin->type : nullptr;
      locals.push_back(buildSymbolInfo(sym, ty, false, false, origin ? origin->mut : true));
      seen.insert(sym);
    }
    for (const auto &child : op->children)
      collectLocalsRec(child.get(), locals, seen);
  }

  std::string lowerExpr(const dhir::Op *op, Func &func, FuncLoweringState &st, int &cur,
                        const std::unordered_map<std::string, SymbolInfo> &symbols) {
    if (!op)
      return "#0";

    switch (op->kind) {
    case dhir::OpKind::ConstInt:
      if (op->hasIntValue)
        return "#" + std::to_string(op->intValue);
      return "#0";
    case dhir::OpKind::ConstFloat:
      if (op->hasFloatValue)
        return "f#" + formatFloat(op->floatValue);
      return "f#0.0";
    case dhir::OpKind::Load: {
      Inst inst;
      inst.kind = OpKind::Load;
      inst.type = op->type;
      inst.symbol = getResolvedSymbol(op);
      auto it = symbols.find(inst.symbol);
      if (it != symbols.end()) {
        inst.elementType = it->second.elementType;
        bool indexed = !op->children.empty();
        attachMemorySemantics(inst, &it->second, op->children.size());
        if (indexed) {
          bool partialIndex = !it->second.dims.empty() && op->children.size() < it->second.dims.size();
          if (partialIndex) {
            inst.type = dhir::TypeKind::Pointer;
            inst.memSize = 8;
            inst.producesAddress = true;
          } else {
            inst.type = it->second.elementType;
            inst.memSize = it->second.elemSize;
          }
        } else if (it->second.type == dhir::TypeKind::Array || it->second.type == dhir::TypeKind::Pointer) {
          inst.type = dhir::TypeKind::Pointer;
          inst.memSize = 8;
          inst.producesAddress = true;
        } else {
          inst.memSize = std::max((size_t) 4, it->second.storageSize);
        }
      } else {
        inst.memSize = typeBytes(inst.type);
      }
      inst.result = newTemp(st);
      for (const auto &idx : op->children)
        inst.args.push_back(lowerExpr(idx.get(), func, st, cur, symbols));
      emit(func, cur, inst);
      return inst.result;
    }
    case dhir::OpKind::Call: {
      Inst inst;
      inst.kind = OpKind::Call;
      inst.type = op->type;
      inst.calleeRetType = op->type;
      inst.symbol = op->symbol;
      for (const auto &arg : op->children) {
        inst.calleeArgTypes.push_back(inferExprType(arg.get()));
        inst.args.push_back(lowerExpr(arg.get(), func, st, cur, symbols));
      }
      if (op->type != dhir::TypeKind::Void)
        inst.result = newTemp(st);
      emit(func, cur, inst);
      return inst.result.empty() ? "#0" : inst.result;
    }
    case dhir::OpKind::Arith:
      if (op->children.size() == 2 && (op->symbol == "&&" || op->symbol == "||")) {
        bool isAnd = op->symbol == "&&";
        int rhsId = newBlock(func, st, isAnd ? "sc.and.rhs" : "sc.or.rhs");
        int shortId = newBlock(func, st, isAnd ? "sc.and.short" : "sc.or.short");
        int mergeId = newBlock(func, st, isAnd ? "sc.and.merge" : "sc.or.merge");

        auto lhsCond = normalizeCond(op->children[0].get(), func, st, cur, symbols);
        Inst cbr;
        cbr.kind = OpKind::CondBr;
        cbr.args = { lhsCond };
        cbr.targets = isAnd ? std::vector<int> { rhsId, shortId } : std::vector<int> { shortId, rhsId };
        emit(func, cur, cbr);

        int rhsEnd = rhsId;
        auto rhsCond = normalizeCond(op->children[1].get(), func, st, rhsEnd, symbols);
        if (!isTerminated(func, rhsEnd)) {
          Inst br;
          br.kind = OpKind::Br;
          br.targets.push_back(mergeId);
          emit(func, rhsEnd, br);
        }

        if (!isTerminated(func, shortId)) {
          Inst br;
          br.kind = OpKind::Br;
          br.targets.push_back(mergeId);
          emit(func, shortId, br);
        }

        Inst phi;
        phi.kind = OpKind::Phi;
        phi.type = dhir::TypeKind::Int;
        phi.result = newTemp(st);
        phi.phiPreds = { rhsEnd, shortId };
        phi.args = { rhsCond, isAnd ? "#0" : "#1" };
        emit(func, mergeId, phi);

        cur = mergeId;
        return phi.result;
      }
      [[fallthrough]];
    case dhir::OpKind::Cmp: {
      Inst inst;
      inst.kind = (op->kind == dhir::OpKind::Cmp) ? OpKind::Cmp : OpKind::Arith;
      inst.type = (op->kind == dhir::OpKind::Cmp) ? dhir::TypeKind::Int : op->type;
      if (op->kind == dhir::OpKind::Cmp && !op->children.empty())
        inst.elementType = inferExprType(op->children[0].get());
      else
        inst.elementType = op->type;
      inst.symbol = op->symbol;
      inst.result = newTemp(st);
      for (const auto &child : op->children)
        inst.args.push_back(lowerExpr(child.get(), func, st, cur, symbols));
      emit(func, cur, inst);
      return inst.result;
    }
    default:
      break;
    }

    if (!op->children.empty())
      return lowerExpr(op->children[0].get(), func, st, cur, symbols);
    return "#0";
  }

  static std::string defaultToken(dhir::TypeKind type) {
    return type == dhir::TypeKind::Float ? "f#0.0" : "#0";
  }

  void attachMemorySemantics(Inst &inst, const SymbolInfo *sym, size_t indexCount, bool flattenedLinearIndex = false) {
    if (!sym)
      return;
    inst.baseKind = sym->baseKind;
    inst.accessRank = (int) indexCount;
    inst.strideBytes.clear();
    if (indexCount == 0)
      return;
    if (flattenedLinearIndex) {
      size_t elemSize = sym->elemSize ? sym->elemSize : 4;
      inst.strideBytes.push_back(elemSize);
      return;
    }
    size_t usable = std::min(indexCount, sym->strideBytes.size());
    inst.strideBytes.insert(inst.strideBytes.end(), sym->strideBytes.begin(), sym->strideBytes.begin() + usable);
  }

  std::string lowerASTExpr(ASTNode *node, dhir::TypeKind fallbackType, Func &func,
                           FuncLoweringState &st, int &cur,
                           const std::unordered_map<std::string, SymbolInfo> &symbols) {
    if (!node)
      return defaultToken(fallbackType);

    dhir::Builder builder;
    auto exprModule = builder.build(node);
    if (!exprModule.root || exprModule.root->children.empty() || !exprModule.root->children[0]) {
      errors.push_back("hir->cfg: cannot lower array init expression");
      return defaultToken(fallbackType);
    }
    return lowerExpr(exprModule.root->children[0].get(), func, st, cur, symbols);
  }

  std::string normalizeCond(const dhir::Op *cond, Func &func, FuncLoweringState &st, int &cur,
                            const std::unordered_map<std::string, SymbolInfo> &symbols) {
    auto value = lowerExpr(cond, func, st, cur, symbols);
    auto ty = inferExprType(cond);
    if (ty == dhir::TypeKind::Int || ty == dhir::TypeKind::Unknown)
      return value;

    Inst cmp;
    cmp.kind = OpKind::Cmp;
    cmp.type = dhir::TypeKind::Int;
    cmp.symbol = "!=";
    cmp.result = newTemp(st);
    cmp.args.push_back(value);
    cmp.args.push_back(ty == dhir::TypeKind::Float ? "f#0.0" : "#0");
    emit(func, cur, cmp);
    return cmp.result;
  }

  void emitCondBranch(const dhir::Op *cond, Func &func, FuncLoweringState &st, int cur,
                      const std::unordered_map<std::string, SymbolInfo> &symbols,
                      int trueTarget, int falseTarget) {
    if (!cond) {
      Inst br;
      br.kind = OpKind::Br;
      br.targets.push_back(falseTarget);
      emit(func, cur, br);
      return;
    }

    if (cond->kind == dhir::OpKind::Arith && cond->children.size() == 2) {
      if (cond->symbol == "&&") {
        int rhsId = newBlock(func, st, "sc.and.rhs");
        emitCondBranch(cond->children[0].get(), func, st, cur, symbols, rhsId, falseTarget);
        emitCondBranch(cond->children[1].get(), func, st, rhsId, symbols, trueTarget, falseTarget);
        return;
      }
      if (cond->symbol == "||") {
        int rhsId = newBlock(func, st, "sc.or.rhs");
        emitCondBranch(cond->children[0].get(), func, st, cur, symbols, trueTarget, rhsId);
        emitCondBranch(cond->children[1].get(), func, st, rhsId, symbols, trueTarget, falseTarget);
        return;
      }
    }

    int at = cur;
    auto condVal = normalizeCond(cond, func, st, at, symbols);
    Inst cbr;
    cbr.kind = OpKind::CondBr;
    cbr.args.push_back(condVal);
    cbr.targets = { trueTarget, falseTarget };
    emit(func, at, cbr);
  }

  void emitLocalArrayInit(const std::string &symbol, const VarDeclNode *var, Func &func, FuncLoweringState &st, int &cur,
                          const std::unordered_map<std::string, SymbolInfo> &symbols,
                          std::set<std::string> *stores) {
    if (!var || !var->init)
      return;
    auto local = dyn_cast<LocalArrayNode>(var->init);
    auto arrTy = dyn_cast<ArrayType>(var->type);
    if (!local || !arrTy)
      return;

    auto it = symbols.find(symbol);
    if (it == symbols.end()) {
      errors.push_back("hir->cfg: local array init symbol not found: " + symbol);
      return;
    }
    if (it->second.dims.empty()) {
      errors.push_back("hir->cfg: local array init target is not array: " + symbol);
      return;
    }

    size_t declaredSize = productDims(it->second.dims);
    size_t astSize = productDims(arrTy->dims);
    if (declaredSize == 0)
      declaredSize = 1;
    if (astSize == 0)
      astSize = 1;
    if (declaredSize != astSize) {
      errors.push_back("hir->cfg: local array init dimension mismatch for " + symbol);
    }

    size_t elemSize = it->second.elemSize ? it->second.elemSize : 4;
    if (declaredSize <= 65536) {
      for (size_t i = 0; i < declaredSize; i++) {
        Inst zstore;
        zstore.kind = OpKind::Store;
        zstore.symbol = symbol;
        zstore.args = { "#" + std::to_string(i), defaultToken(it->second.elementType) };
        zstore.type = it->second.elementType;
        zstore.memSize = elemSize;
        attachMemorySemantics(zstore, &it->second, 1, /*flattenedLinearIndex=*/ true);
        emit(func, cur, zstore);
      }
      addStoreSym(stores, symbol);
    } else {
      int condId = newBlock(func, st, "arr.init.cond");
      int bodyId = newBlock(func, st, "arr.init.body");
      int exitId = newBlock(func, st, "arr.init.exit");

      Inst jump;
      jump.kind = OpKind::Br;
      jump.targets.push_back(condId);
      emit(func, cur, jump);

      std::string idxPhi = newTemp(st);
      std::string idxNext = newTemp(st);

      Inst phi;
      phi.kind = OpKind::Phi;
      phi.type = dhir::TypeKind::Int;
      phi.result = idxPhi;
      phi.phiPreds = { cur, bodyId };
      phi.args = { "#0", idxNext };
      emit(func, condId, phi);

      Inst cmp;
      cmp.kind = OpKind::Cmp;
      cmp.type = dhir::TypeKind::Int;
      cmp.symbol = "<";
      cmp.result = newTemp(st);
      cmp.args = { idxPhi, "#" + std::to_string(declaredSize) };
      emit(func, condId, cmp);

      Inst cbr;
      cbr.kind = OpKind::CondBr;
      cbr.args = { cmp.result };
      cbr.targets = { bodyId, exitId };
      emit(func, condId, cbr);

      Inst zstore;
      zstore.kind = OpKind::Store;
      zstore.symbol = symbol;
      zstore.args = { idxPhi, defaultToken(it->second.elementType) };
      zstore.type = it->second.elementType;
      zstore.memSize = elemSize;
      attachMemorySemantics(zstore, &it->second, 1, /*flattenedLinearIndex=*/ true);
      emit(func, bodyId, zstore);

      Inst add;
      add.kind = OpKind::Arith;
      add.type = dhir::TypeKind::Int;
      add.symbol = "+";
      add.result = idxNext;
      add.args = { idxPhi, "#1" };
      emit(func, bodyId, add);

      Inst back;
      back.kind = OpKind::Br;
      back.targets.push_back(condId);
      emit(func, bodyId, back);

      cur = exitId;
      addStoreSym(stores, symbol);
    }

    for (size_t i = 0; i < astSize; i++) {
      ASTNode *elem = (local->va && i < astSize) ? local->va[i] : nullptr;
      if (!elem)
        continue;
      std::string token = lowerASTExpr(elem, it->second.elementType, func, st, cur, symbols);
      Inst store;
      store.kind = OpKind::Store;
      store.symbol = symbol;
      store.args = { "#" + std::to_string(i), token };
      store.type = it->second.elementType;
      store.memSize = elemSize;
      attachMemorySemantics(store, &it->second, 1, /*flattenedLinearIndex=*/ true);
      emit(func, cur, store);
      addStoreSym(stores, symbol);
    }
  }

  int lowerStmt(const dhir::Op *op, Func &func, FuncLoweringState &st, int cur,
                int breakTarget, int continueTarget,
                const std::unordered_map<std::string, SymbolInfo> &symbols,
                std::set<std::string> *stores) {
    if (!op)
      return cur;

    if (isTerminated(func, cur)) {
      int cont = newBlock(func, st, "dead");
      cur = cont;
    }

    switch (op->kind) {
    case dhir::OpKind::Block: {
      int at = cur;
      for (const auto &child : op->children)
        at = lowerStmt(child.get(), func, st, at, breakTarget, continueTarget, symbols, stores);
      return at;
    }
    case dhir::OpKind::VarDecl: {
      std::string sym = getResolvedSymbol(op);
      auto it = symbols.find(sym);
      bool isArray = it != symbols.end() && !it->second.dims.empty();
      if (isArray) {
        auto *var = dyn_cast<VarDeclNode>(op->origin);
        emitLocalArrayInit(sym, var, func, st, cur, symbols, stores);
        return cur;
      }
      if (!op->children.empty()) {
        auto value = lowerExpr(op->children[0].get(), func, st, cur, symbols);
        Inst store;
        store.kind = OpKind::Store;
        store.symbol = sym;
        store.args.push_back(value);
        if (it != symbols.end()) {
          store.type = it->second.type;
          store.memSize = std::max((size_t) 4, it->second.storageSize);
          attachMemorySemantics(store, &it->second, 0);
        }
        emit(func, cur, store);
        addStoreSym(stores, sym);
      }
      return cur;
    }
    case dhir::OpKind::Store: {
      if (op->children.empty())
        return cur;
      Inst store;
      store.kind = OpKind::Store;
      store.symbol = getResolvedSymbol(op);
      for (size_t i = 0; i + 1 < op->children.size(); i++)
        store.args.push_back(lowerExpr(op->children[i].get(), func, st, cur, symbols));
      store.args.push_back(lowerExpr(op->children.back().get(), func, st, cur, symbols));
      auto it = symbols.find(store.symbol);
      if (it != symbols.end()) {
        bool indexed = store.args.size() > 1;
        store.type = indexed ? it->second.elementType : it->second.type;
        store.memSize = indexed ? it->second.elemSize : std::max((size_t) 4, it->second.storageSize);
        attachMemorySemantics(store, &it->second, indexed ? store.args.size() - 1 : 0);
      }
      emit(func, cur, store);
      addStoreSym(stores, store.symbol);
      return cur;
    }
    case dhir::OpKind::Call:
    case dhir::OpKind::Arith:
    case dhir::OpKind::Cmp:
    case dhir::OpKind::Load:
      (void) lowerExpr(op, func, st, cur, symbols);
      return cur;
    case dhir::OpKind::Return: {
      Inst ret;
      ret.kind = OpKind::Ret;
      if (!op->children.empty())
        ret.args.push_back(lowerExpr(op->children[0].get(), func, st, cur, symbols));
      else
        ret.args.push_back("#0");
      emit(func, cur, ret);
      return cur;
    }
    case dhir::OpKind::Break: {
      if (breakTarget < 0) {
        errors.push_back("hir->cfg: break outside loop");
        return cur;
      }
      Inst br;
      br.kind = OpKind::Br;
      br.targets.push_back(breakTarget);
      emit(func, cur, br);
      return cur;
    }
    case dhir::OpKind::Continue: {
      if (continueTarget < 0) {
        errors.push_back("hir->cfg: continue outside loop");
        return cur;
      }
      Inst br;
      br.kind = OpKind::Br;
      br.targets.push_back(continueTarget);
      emit(func, cur, br);
      return cur;
    }
    case dhir::OpKind::If: {
      if (op->children.size() < 2) {
        errors.push_back("hir->cfg: malformed if op");
        return cur;
      }
      int thenId = newBlock(func, st, "if.then");
      int elseId = newBlock(func, st, "if.else");
      int mergeId = newBlock(func, st, "if.merge");

      emitCondBranch(op->children[0].get(), func, st, cur, symbols, thenId, elseId);

      std::set<std::string> thenStores;
      int thenEnd = lowerStmt(op->children[1].get(), func, st, thenId, breakTarget, continueTarget, symbols, &thenStores);
      if (!isTerminated(func, thenEnd)) {
        Inst br;
        br.kind = OpKind::Br;
        br.targets.push_back(mergeId);
        emit(func, thenEnd, br);
      }

      std::set<std::string> elseStores;
      int elseEnd = elseId;
      if (op->children.size() >= 3)
        elseEnd = lowerStmt(op->children[2].get(), func, st, elseId, breakTarget, continueTarget, symbols, &elseStores);
      if (!isTerminated(func, elseEnd)) {
        Inst br;
        br.kind = OpKind::Br;
        br.targets.push_back(mergeId);
        emit(func, elseEnd, br);
      }

      if (stores) {
        stores->insert(thenStores.begin(), thenStores.end());
        stores->insert(elseStores.begin(), elseStores.end());
      }
      return mergeId;
    }
    case dhir::OpKind::While: {
      if (op->children.size() < 2) {
        errors.push_back("hir->cfg: malformed while op");
        return cur;
      }
      int condId = newBlock(func, st, "while.cond");
      int bodyId = newBlock(func, st, "while.body");
      int exitId = newBlock(func, st, "while.exit");

      Inst jump;
      jump.kind = OpKind::Br;
      jump.targets.push_back(condId);
      emit(func, cur, jump);

      emitCondBranch(op->children[0].get(), func, st, condId, symbols, bodyId, exitId);

      std::set<std::string> bodyStores;
      int bodyEnd = lowerStmt(op->children[1].get(), func, st, bodyId, exitId, condId, symbols, &bodyStores);
      if (!isTerminated(func, bodyEnd)) {
        Inst back;
        back.kind = OpKind::Br;
        back.targets.push_back(condId);
        emit(func, bodyEnd, back);
      }

      for (const auto &sym : bodyStores)
        addStoreSym(stores, sym);
      return exitId;
    }
    case dhir::OpKind::For: {
      if (op->children.size() < 4) {
        errors.push_back("hir->cfg: malformed for op");
        return cur;
      }
      cur = lowerStmt(op->children[0].get(), func, st, cur, breakTarget, continueTarget, symbols, stores);

      int condId = newBlock(func, st, "for.cond");
      int bodyId = newBlock(func, st, "for.body");
      int stepId = newBlock(func, st, "for.step");
      int exitId = newBlock(func, st, "for.exit");

      Inst toCond;
      toCond.kind = OpKind::Br;
      toCond.targets.push_back(condId);
      emit(func, cur, toCond);

      emitCondBranch(op->children[1].get(), func, st, condId, symbols, bodyId, exitId);

      int bodyEnd = lowerStmt(op->children[3].get(), func, st, bodyId, exitId, stepId, symbols, stores);
      if (!isTerminated(func, bodyEnd)) {
        Inst toStep;
        toStep.kind = OpKind::Br;
        toStep.targets.push_back(stepId);
        emit(func, bodyEnd, toStep);
      }

      int stepEnd = lowerStmt(op->children[2].get(), func, st, stepId, exitId, stepId, symbols, stores);
      if (!isTerminated(func, stepEnd)) {
        Inst back;
        back.kind = OpKind::Br;
        back.targets.push_back(condId);
        emit(func, stepEnd, back);
      }
      return exitId;
    }
    default:
      return cur;
    }
  }

  void repairMemoryInsts(Func &func) {
    std::unordered_map<std::string, SymbolInfo> symMap;
    for (const auto &sym : func.params)
      symMap[sym.name] = sym;
    for (const auto &sym : func.locals)
      symMap[sym.name] = sym;

    for (auto &bb : func.blocks) {
      for (auto &inst : bb.insts) {
        if (inst.kind != OpKind::Load && inst.kind != OpKind::Store)
          continue;
        auto it = symMap.find(inst.symbol);
        if (it == symMap.end())
          continue;
        size_t indexCount = inst.kind == OpKind::Load ? inst.args.size()
                                                      : (inst.args.size() > 0 ? inst.args.size() - 1 : 0);
        if (inst.memSize == 0) {
          if (indexCount > 0) {
            bool partialLoad = inst.kind == OpKind::Load && !it->second.dims.empty() &&
                               indexCount < it->second.dims.size();
            inst.memSize = partialLoad ? 8 : it->second.elemSize;
          } else if (it->second.type == dhir::TypeKind::Array || it->second.type == dhir::TypeKind::Pointer) {
            inst.memSize = 8;
          } else {
            inst.memSize = std::max((size_t) 4, it->second.storageSize);
          }
        }
        if (inst.baseKind == MemoryBaseKind::Unknown)
          attachMemorySemantics(inst, &it->second, indexCount);
      }
    }
  }

  void supplementLocalsFromCfg(Func &func, const Module &cfgModule) {
    std::unordered_set<std::string> reserved;
    for (const auto &sym : cfgModule.globals)
      reserved.insert(sym.name);
    for (const auto &sym : func.params)
      reserved.insert(sym.name);
    for (const auto &sym : func.locals)
      reserved.insert(sym.name);

    for (const auto &bb : func.blocks) {
      for (const auto &inst : bb.insts) {
        if (inst.kind != OpKind::Load && inst.kind != OpKind::Store)
          continue;
        if (inst.symbol.empty() || reserved.count(inst.symbol))
          continue;
        SymbolInfo placeholder;
        placeholder.name = inst.symbol;
        placeholder.type = inst.type != dhir::TypeKind::Unknown ? inst.type : dhir::TypeKind::Int;
        placeholder.elementType =
            inst.elementType != dhir::TypeKind::Unknown ? inst.elementType : placeholder.type;
        placeholder.baseKind =
            inst.baseKind != MemoryBaseKind::Unknown ? inst.baseKind : MemoryBaseKind::Local;
        placeholder.elemSize = inst.memSize ? inst.memSize : 4;
        placeholder.storageSize = placeholder.elemSize;
        func.locals.push_back(placeholder);
        reserved.insert(placeholder.name);
      }
    }
  }

  Func lowerFunc(const dhir::Op &funcOp, const Module &cfgModule) {
    Func func;
    FuncLoweringState st;
    func.name = funcOp.symbol.empty() ? "anonymous" : funcOp.symbol;

    buildResolvedSymbols(funcOp, cfgModule);
    populateFunctionParams(func, funcOp, cfgModule);

    std::unordered_set<std::string> seen;
    for (const auto &param : func.params)
      seen.insert(param.name);
    if (!funcOp.children.empty())
      collectLocalsRec(funcOp.children[0].get(), func.locals, seen);

    func.entry = newBlock(func, st, "entry");

    auto symbols = toSymbolMap(func, cfgModule);

    int cur = func.entry;
    if (!funcOp.children.empty())
      cur = lowerStmt(funcOp.children[0].get(), func, st, cur, -1, -1, symbols, nullptr);
    normalizePhiOrder(func);
    ensureTerminator(func, cur);

    for (int bid = 0; bid < (int) func.blocks.size(); bid++)
      ensureTerminator(func, bid);
    supplementLocalsFromCfg(func, cfgModule);
    repairMemoryInsts(func);
    return func;
  }
};

}  // namespace

Module lowerFromHIR(const dhir::Module &hirModule, std::vector<std::string> &errors) {
  Lowerer lowering(hirModule, errors);
  return lowering.run();
}

}  // namespace sys::cfg
