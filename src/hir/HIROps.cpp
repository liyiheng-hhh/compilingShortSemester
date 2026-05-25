#include "HIROps.h"

#include <iomanip>

namespace sys::hir {

namespace {

void hirDumpOpTree(const Op *op, std::ostream &os, int depth) {
  if (!op) return;
  for (int i = 0; i < depth; i++) os << "  ";
  os << kindName(op->kind);
  if (!op->symbol.empty()) os << " \"" << op->symbol << "\"";
  if (op->hasIntValue) os << " value=" << op->intValue;
  if (op->hasFloatValue) os << " value=" << std::setprecision(9) << op->floatValue;
  os << "\n";
  for (const auto &child : op->children)
    hirDumpOpTree(child.get(), os, depth + 1);
}

}  // namespace

Op::Op(OpKind k, void *origin)
    : kind(k),
      type(TypeKind::Unknown),
      traits(defaultTraits(k)),
      origin(origin),
      hasIntValue(false),
      intValue(0),
      hasFloatValue(false),
      floatValue(0.0) {}

Op *Op::append(std::unique_ptr<Op> child) {
  if (!child) return nullptr;
  children.push_back(std::move(child));
  return children.back().get();
}

TypeKind mapType(const Type &t) {
  if (t.isPointer) return TypeKind::Pointer;
  if (t.base == BaseType::Int) return TypeKind::Int;
  if (t.base == BaseType::Float) return TypeKind::Float;
  return TypeKind::Void;
}

uint32_t defaultTraits(OpKind kind) {
  switch (kind) {
    case OpKind::Arith: case OpKind::Cmp: case OpKind::ConstInt: case OpKind::ConstFloat:
      return Trait::Pure;
    case OpKind::Load: case OpKind::Store:
      return Trait::MemoryEffect;
    case OpKind::If: case OpKind::While: case OpKind::For:
      return Trait::BranchLike | Trait::LoopLike;
    default: return Trait::TraitNone;
  }
}

const char *kindName(OpKind kind) {
  switch (kind) {
    case OpKind::Module: return "Module";
    case OpKind::Func: return "Func";
    case OpKind::Block: return "Block";
    case OpKind::If: return "If";
    case OpKind::While: return "While";
    case OpKind::For: return "For";
    case OpKind::Call: return "Call";
    case OpKind::Load: return "Load";
    case OpKind::Store: return "Store";
    case OpKind::Arith: return "Arith";
    case OpKind::Cmp: return "Cmp";
    case OpKind::Return: return "Return";
    case OpKind::VarDecl: return "VarDecl";
    case OpKind::Break: return "Break";
    case OpKind::Continue: return "Continue";
    case OpKind::ConstInt: return "ConstInt";
    case OpKind::ConstFloat: return "ConstFloat";
    default: return "Unknown";
  }
}

void dump(const Module &module, std::ostream &os) {
  os << "HIR Module\n";
  if (module.root) hirDumpOpTree(module.root.get(), os, 0);
}

}  // namespace sys::hir
