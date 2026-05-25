// compiler2026-x phase-C (header layout)
#ifndef CFG_OPS_H
#define CFG_OPS_H

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>
#include "../dialect_hir/DhirOps.h"
#include "../dialect_parse/ASTNode.h"


namespace sys::cfg {

enum class OpKind {
  Nop,
  Call,
  Load,
  Store,
  Arith,
  Cmp,
  Phi,
  Ret,
  Br,
  CondBr,
};

enum class MemoryBaseKind {
  Unknown,
  Global,
  Local,
  Param,
};

struct SymbolInfo {
  std::string name;
  dhir::TypeKind type = dhir::TypeKind::Unknown;
  dhir::TypeKind elementType = dhir::TypeKind::Unknown;
  std::vector<int> dims;
  std::vector<size_t> strideBytes;
  MemoryBaseKind baseKind = MemoryBaseKind::Unknown;
  bool isGlobal = false;
  bool isParam = false;
  bool isMutable = true;
  size_t elemSize = 4;
  size_t storageSize = 4;
  bool hasIntInit = false;
  long long intInit = 0;
  bool hasFloatInit = false;
  double floatInit = 0.0;
  std::vector<int> intArrayInit;
  std::vector<float> floatArrayInit;
};

struct Inst {
  OpKind kind = OpKind::Nop;
  dhir::TypeKind type = dhir::TypeKind::Unknown;
  dhir::TypeKind elementType = dhir::TypeKind::Unknown;
  std::string result;
  std::string symbol;
  std::vector<std::string> args;
  size_t memSize = 0;
  std::vector<size_t> strideBytes;
  MemoryBaseKind baseKind = MemoryBaseKind::Unknown;
  int accessRank = 0;
  bool producesAddress = false;
  dhir::TypeKind calleeRetType = dhir::TypeKind::Unknown;
  std::vector<dhir::TypeKind> calleeArgTypes;
  std::vector<int> targets;
  std::vector<int> phiPreds;
};

struct Block {
  std::string name;
  std::vector<Inst> insts;
};

struct Func {
  std::string name;
  dhir::TypeKind returnType = dhir::TypeKind::Unknown;
  std::vector<SymbolInfo> params;
  std::vector<SymbolInfo> locals;
  int entry = 0;
  std::vector<Block> blocks;
};

struct Module {
  ASTNode *originAst = nullptr;
  std::vector<SymbolInfo> globals;
  std::vector<Func> funcs;
};

bool isTerminator(OpKind kind);
const char *kindName(OpKind kind);
void dump(const Module &module, std::ostream &os);

}  // namespace sys::cfg

#endif
