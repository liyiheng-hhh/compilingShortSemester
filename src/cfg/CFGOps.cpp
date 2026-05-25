#include "CFGOps.h"

namespace sys::cfg {

namespace {

const char *cfgTypeName(dhir::TypeKind kind) {
  switch (kind) {
  case dhir::TypeKind::Unknown:
    return "unknown";
  case dhir::TypeKind::Int:
    return "int";
  case dhir::TypeKind::Float:
    return "float";
  case dhir::TypeKind::Void:
    return "void";
  case dhir::TypeKind::Pointer:
    return "ptr";
  case dhir::TypeKind::Array:
    return "array";
  case dhir::TypeKind::Function:
    return "func";
  }
  return "unknown";
}

const char *cfgMemBaseName(MemoryBaseKind kind) {
  switch (kind) {
  case MemoryBaseKind::Unknown:
    return "unknown";
  case MemoryBaseKind::Global:
    return "global";
  case MemoryBaseKind::Local:
    return "local";
  case MemoryBaseKind::Param:
    return "param";
  }
  return "unknown";
}

void cfgDumpSymbol(const SymbolInfo &sym, std::ostream &os, int depth, const char *prefix) {
  for (int i = 0; i < depth; i++)
    os << "  ";
  os << prefix << " \"" << sym.name << "\" type=" << cfgTypeName(sym.type)
     << " elem=" << cfgTypeName(sym.elementType)
     << " bytes=" << sym.storageSize
     << " base=" << cfgMemBaseName(sym.baseKind);
  if (!sym.dims.empty()) {
    os << " dims=[";
    for (size_t i = 0; i < sym.dims.size(); i++) {
      if (i)
        os << ",";
      os << sym.dims[i];
    }
    os << "]";
  }
  if (!sym.strideBytes.empty()) {
    os << " strides=[";
    for (size_t i = 0; i < sym.strideBytes.size(); i++) {
      if (i)
        os << ",";
      os << sym.strideBytes[i];
    }
    os << "]";
  }
  os << "\n";
}

}  // namespace

bool isTerminator(OpKind kind) {
  return kind == OpKind::Br || kind == OpKind::CondBr || kind == OpKind::Ret;
}

const char *kindName(OpKind kind) {
  switch (kind) {
  case OpKind::Nop:
    return "nop";
  case OpKind::Call:
    return "call";
  case OpKind::Load:
    return "load";
  case OpKind::Store:
    return "store";
  case OpKind::Arith:
    return "arith";
  case OpKind::Cmp:
    return "cmp";
  case OpKind::Phi:
    return "phi";
  case OpKind::Ret:
    return "ret";
  case OpKind::Br:
    return "br";
  case OpKind::CondBr:
    return "cond_br";
  }
  return "unknown";
}

void dump(const Module &module, std::ostream &os) {
  os << "cfg.module\n";
  for (const auto &global : module.globals)
    cfgDumpSymbol(global, os, 1, "cfg.global");
  for (const auto &func : module.funcs) {
    os << "  cfg.func @" << func.name << " ret=" << cfgTypeName(func.returnType)
       << " entry=" << func.entry << "\n";
    for (const auto &param : func.params)
      cfgDumpSymbol(param, os, 2, "cfg.param");
    for (const auto &local : func.locals)
      cfgDumpSymbol(local, os, 2, "cfg.local");
    for (size_t bid = 0; bid < func.blocks.size(); bid++) {
      const auto &bb = func.blocks[bid];
      os << "    ^bb" << bid << " (" << bb.name << ")\n";
      for (const auto &inst : bb.insts) {
        os << "      ";
        if (!inst.result.empty())
          os << inst.result << " = ";
        os << kindName(inst.kind) << ":" << cfgTypeName(inst.type);
        if (!inst.symbol.empty())
          os << " \"" << inst.symbol << "\"";
        if (inst.memSize)
          os << " <size=" << inst.memSize << ">";
        if (inst.kind == OpKind::Load || inst.kind == OpKind::Store) {
          os << " <base=" << cfgMemBaseName(inst.baseKind)
             << ",rank=" << inst.accessRank
             << ",addr=" << (inst.producesAddress ? "1" : "0") << ">";
          if (!inst.strideBytes.empty()) {
            os << " strides=[";
            for (size_t i = 0; i < inst.strideBytes.size(); i++) {
              if (i)
                os << ",";
              os << inst.strideBytes[i];
            }
            os << "]";
          }
        }
        if (!inst.args.empty()) {
          os << " [";
          for (size_t i = 0; i < inst.args.size(); i++) {
            if (i)
              os << ", ";
            os << inst.args[i];
          }
          os << "]";
        }
        if (inst.kind == OpKind::Call) {
          os << " sig=(";
          for (size_t i = 0; i < inst.calleeArgTypes.size(); i++) {
            if (i)
              os << ",";
            os << cfgTypeName(inst.calleeArgTypes[i]);
          }
          os << ")->" << cfgTypeName(inst.calleeRetType);
        }
        if (!inst.targets.empty()) {
          os << " -> [";
          for (size_t i = 0; i < inst.targets.size(); i++) {
            if (i)
              os << ", ";
            os << "bb" << inst.targets[i];
          }
          os << "]";
        }
        if (!inst.phiPreds.empty()) {
          os << " preds=[";
          for (size_t i = 0; i < inst.phiPreds.size(); i++) {
            if (i)
              os << ", ";
            os << "bb" << inst.phiPreds[i];
          }
          os << "]";
        }
        os << "\n";
      }
    }
  }
}

}  // namespace sys::cfg
