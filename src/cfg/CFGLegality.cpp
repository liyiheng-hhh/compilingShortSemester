#include "CFGLegality.h"

#include <algorithm>
#include <unordered_map>
#include <set>

namespace sys::cfg {

namespace {

bool cfgLegIsLegalHIRKind(dhir::OpKind kind) {
  switch (kind) {
  case dhir::OpKind::Module:
  case dhir::OpKind::Func:
  case dhir::OpKind::Block:
  case dhir::OpKind::If:
  case dhir::OpKind::While:
  case dhir::OpKind::For:
  case dhir::OpKind::Call:
  case dhir::OpKind::Load:
  case dhir::OpKind::Store:
  case dhir::OpKind::Arith:
  case dhir::OpKind::Cmp:
  case dhir::OpKind::Return:
  case dhir::OpKind::VarDecl:
  case dhir::OpKind::Break:
  case dhir::OpKind::Continue:
  case dhir::OpKind::ConstInt:
  case dhir::OpKind::ConstFloat:
    return true;
  case dhir::OpKind::Unknown:
    return false;
  }
  return false;
}

bool cfgLegIsLegalCFGKind(OpKind kind) {
  switch (kind) {
  case OpKind::Nop:
  case OpKind::Call:
  case OpKind::Load:
  case OpKind::Store:
  case OpKind::Arith:
  case OpKind::Cmp:
  case OpKind::Phi:
  case OpKind::Ret:
  case OpKind::Br:
  case OpKind::CondBr:
    return true;
  }
  return false;
}

size_t cfgLegProductDims(const std::vector<int> &dims) {
  if (dims.empty())
    return 1;
  size_t prod = 1;
  for (int dim : dims)
    prod *= (size_t) std::max(dim, 1);
  return prod;
}

bool cfgLegIsArrayLike(const SymbolInfo &sym) {
  return !sym.dims.empty() || sym.type == dhir::TypeKind::Array || sym.type == dhir::TypeKind::Pointer;
}

bool cfgLegIsMemoryInst(const Inst &inst) {
  return inst.kind == OpKind::Load || inst.kind == OpKind::Store;
}

bool cfgLegVisitHIR(const dhir::Op *op, std::vector<std::string> &errors) {
  if (!op)
    return true;
  bool ok = true;
  if (!cfgLegIsLegalHIRKind(op->kind)) {
    errors.push_back("hir legality: illegal op kind");
    ok = false;
  }
  for (const auto &child : op->children)
    ok = cfgLegVisitHIR(child.get(), errors) && ok;
  return ok;
}

}  // namespace

bool verifyHIRLegalSet(const dhir::Module &module, std::vector<std::string> &errors) {
  errors.clear();
  if (!module.root) {
    errors.push_back("hir legality: null module root");
    return false;
  }
  return cfgLegVisitHIR(module.root.get(), errors);
}

bool verifyCFGLegalSet(const Module &module, std::vector<std::string> &errors) {
  bool ok = true;
  for (const auto &global : module.globals) {
    if (global.name.empty()) {
      errors.push_back("cfg legality: unnamed global symbol");
      ok = false;
    }
    if (!global.intArrayInit.empty() && !global.floatArrayInit.empty()) {
      errors.push_back("cfg legality: mixed int/float array init for global " + global.name);
      ok = false;
    }
    if (!global.intArrayInit.empty() || !global.floatArrayInit.empty()) {
      size_t expected = cfgLegProductDims(global.dims);
      if (expected == 0)
        expected = 1;
      if (!global.intArrayInit.empty()) {
        if (global.elementType == dhir::TypeKind::Float) {
          errors.push_back("cfg legality: int array init used on float global " + global.name);
          ok = false;
        }
        if (global.intArrayInit.size() != expected) {
          errors.push_back("cfg legality: global array init size mismatch for " + global.name);
          ok = false;
        }
      }
      if (!global.floatArrayInit.empty()) {
        if (global.elementType != dhir::TypeKind::Float) {
          errors.push_back("cfg legality: float array init used on non-float global " + global.name);
          ok = false;
        }
        if (global.floatArrayInit.size() != expected) {
          errors.push_back("cfg legality: global array init size mismatch for " + global.name);
          ok = false;
        }
      }
    }
  }
  if (module.funcs.empty()) {
    errors.push_back("cfg legality: no function");
    return false;
  }

  for (const auto &func : module.funcs) {
    std::unordered_map<std::string, SymbolInfo> symbols;
    for (const auto &sym : module.globals)
      symbols[sym.name] = sym;
    for (const auto &sym : func.params)
      symbols[sym.name] = sym;
    for (const auto &sym : func.locals)
      symbols[sym.name] = sym;

    if (func.name.empty()) {
      errors.push_back("cfg legality: unnamed function");
      ok = false;
    }
    for (const auto &bb : func.blocks) {
      for (const auto &inst : bb.insts) {
        if (!cfgLegIsLegalCFGKind(inst.kind)) {
          errors.push_back("cfg legality: illegal inst kind in func @" + func.name);
          ok = false;
        }
        if (cfgLegIsMemoryInst(inst)) {
          if (inst.symbol.empty()) {
            errors.push_back("cfg legality: empty memory symbol in func @" + func.name);
            ok = false;
            continue;
          }
          auto it = symbols.find(inst.symbol);
          if (it == symbols.end()) {
            errors.push_back("cfg legality: unresolved memory symbol '" + inst.symbol + "' in func @" + func.name);
            ok = false;
            continue;
          }

          bool indexed = inst.kind == OpKind::Load ? !inst.args.empty() : inst.args.size() > 1;
          size_t indexCount = indexed ? (inst.kind == OpKind::Load ? inst.args.size() : inst.args.size() - 1) : 0;
          if (inst.accessRank != (int) indexCount) {
            errors.push_back("cfg legality: access rank mismatch for '" + inst.symbol + "' in func @" + func.name);
            ok = false;
          }
          if (inst.baseKind != it->second.baseKind && inst.baseKind != MemoryBaseKind::Unknown) {
            errors.push_back("cfg legality: memory base kind mismatch for '" + inst.symbol + "' in func @" + func.name);
            ok = false;
          }
          if (!inst.strideBytes.empty() && inst.strideBytes.size() < indexCount) {
            errors.push_back("cfg legality: insufficient stride info for '" + inst.symbol + "' in func @" + func.name);
            ok = false;
          }
          if (indexed && cfgLegIsArrayLike(it->second)) {
            if (!it->second.dims.empty() && indexCount > it->second.dims.size()) {
              errors.push_back("cfg legality: too many indices for '" + inst.symbol + "' in func @" + func.name);
              ok = false;
            }
            // Loads may represent sub-array address materialization when the
            // index chain is partial. Stores are always element writes in our
            // lowering (including flattened array init), so keep element type.
            bool partialLoad = inst.kind == OpKind::Load &&
                               !it->second.dims.empty() &&
                               indexCount < it->second.dims.size();
            dhir::TypeKind expectedType = partialLoad ? dhir::TypeKind::Pointer : it->second.elementType;
            size_t expectedSize = partialLoad ? 8 : (it->second.elemSize ? it->second.elemSize : 4);

            if (inst.type != dhir::TypeKind::Unknown && inst.type != expectedType) {
              errors.push_back("cfg legality: indexed memory type mismatch for '" + inst.symbol + "' in func @" + func.name);
              ok = false;
            }
            if (partialLoad != inst.producesAddress) {
              errors.push_back("cfg legality: address/result mode mismatch for '" + inst.symbol + "' in func @" + func.name);
              ok = false;
            }
            if (expectedSize && inst.memSize && inst.memSize != expectedSize) {
              errors.push_back("cfg legality: indexed memory size mismatch for '" + inst.symbol + "' in func @" + func.name);
              ok = false;
            }
            bool flattenedScalarIndex = indexCount == 1 && it->second.dims.size() > 1 && !inst.producesAddress;
            if (!flattenedScalarIndex && !inst.strideBytes.empty()) {
              size_t usable = std::min(indexCount, it->second.strideBytes.size());
              for (size_t i = 0; i < usable; i++) {
                if (inst.strideBytes[i] != it->second.strideBytes[i]) {
                  errors.push_back("cfg legality: stride mismatch for '" + inst.symbol + "' in func @" + func.name);
                  ok = false;
                  break;
                }
              }
            }
          } else if (!indexed && inst.producesAddress &&
                     !(it->second.type == dhir::TypeKind::Array || it->second.type == dhir::TypeKind::Pointer)) {
            errors.push_back("cfg legality: scalar memory op unexpectedly produces address for '" + inst.symbol + "' in func @" + func.name);
            ok = false;
          }
        }
      }
    }
  }
  return ok;
}

bool verifyHIRToCFGConversion(const dhir::Module &hirModule, const Module &cfgModule, std::vector<std::string> &errors) {
  bool ok = true;
  std::vector<std::string> local;
  if (!verifyHIRLegalSet(hirModule, local))
    ok = false;
  errors.insert(errors.end(), local.begin(), local.end());
  local.clear();
  if (!verifyCFGLegalSet(cfgModule, local))
    ok = false;
  errors.insert(errors.end(), local.begin(), local.end());
  if (cfgModule.funcs.empty()) {
    errors.push_back("hir->cfg legality: empty cfg module");
    ok = false;
  }
  return ok;
}

}  // namespace sys::cfg
