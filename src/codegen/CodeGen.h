#ifndef CODEGEN_H
#define CODEGEN_H

#include "OpBase.h"
#include "Ops.h"

namespace sys {

// MLIR-style IR builder (from reference dialect codegen; AST CodeGen omitted).
class Builder {
  BasicBlock *bb = nullptr;
  BasicBlock::iterator at;
  bool init = false;

public:
  struct Guard {
    Builder &builder;
    BasicBlock *bb;
    BasicBlock::iterator at;
    Guard(Builder &b): builder(b), bb(b.bb), at(b.at) {}
    ~Guard() { builder.bb = bb; builder.at = at; }
  };

  void setToRegionStart(Region *region);
  void setToRegionEnd(Region *region);
  void setToBlockStart(BasicBlock *block);
  void setToBlockEnd(BasicBlock *block);
  void setAfterOp(Op *op);
  void setBeforeOp(Op *op);

  template<class T>
  T *create(const std::vector<Value> &v) {
    assert(init);
    auto op = new T(v);
    bb->insert(at, op);
    return op;
  }

  template<class T>
  T *create() {
    assert(init);
    auto op = new T();
    bb->insert(at, op);
    return op;
  }

  template<class T>
  T *create(const std::vector<Attr *> &v) {
    assert(init);
    auto op = new T(v);
    bb->insert(at, op);
    return op;
  }

  template<class T>
  T *create(const std::vector<Value> &v, const std::vector<Attr *> &v2) {
    assert(init);
    auto op = new T(v, v2);
    bb->insert(at, op);
    return op;
  }

  template<class T>
  T *create(Value::Type resultTy, const std::vector<Value> &v) {
    assert(init);
    auto op = new T(resultTy, v);
    bb->insert(at, op);
    return op;
  }

  template<class T>
  T *create(Value::Type resultTy) {
    assert(init);
    auto op = new T(resultTy);
    bb->insert(at, op);
    return op;
  }

  template<class T>
  T *create(Value::Type resultTy, const std::vector<Attr *> &v) {
    assert(init);
    auto op = new T(resultTy, v);
    bb->insert(at, op);
    return op;
  }

  template<class T>
  T *create(Value::Type resultTy, const std::vector<Value> &v,
            const std::vector<Attr *> &v2) {
    assert(init);
    auto op = new T(resultTy, v, v2);
    bb->insert(at, op);
    return op;
  }

  template<class T>
  T *replace(Op *op, const std::vector<Value> &v) {
    setBeforeOp(op);
    auto opnew = create<T>(v);
    op->replaceAllUsesWith(opnew);
    op->erase();
    return opnew;
  }

  template<class T>
  T *replace(Op *op) {
    setBeforeOp(op);
    auto opnew = create<T>();
    op->replaceAllUsesWith(opnew);
    op->erase();
    return opnew;
  }

  template<class T>
  T *replace(Op *op, const std::vector<Attr *> &v) {
    setBeforeOp(op);
    auto opnew = create<T>(v);
    op->replaceAllUsesWith(opnew);
    op->erase();
    return opnew;
  }

  template<class T>
  T *replace(Op *op, const std::vector<Value> &v, const std::vector<Attr *> &v2) {
    setBeforeOp(op);
    auto opnew = create<T>(v, v2);
    op->replaceAllUsesWith(opnew);
    op->erase();
    return opnew;
  }

  template<class T>
  T *replace(Op *op, Value::Type resultTy, const std::vector<Value> &v) {
    setBeforeOp(op);
    auto opnew = create<T>(resultTy, v);
    op->replaceAllUsesWith(opnew);
    op->erase();
    return opnew;
  }

  template<class T>
  T *replace(Op *op, Value::Type resultTy) {
    setBeforeOp(op);
    auto opnew = create<T>(resultTy);
    op->replaceAllUsesWith(opnew);
    op->erase();
    return opnew;
  }

  template<class T>
  T *replace(Op *op, Value::Type resultTy, const std::vector<Attr *> &v) {
    setBeforeOp(op);
    auto opnew = create<T>(resultTy, v);
    op->replaceAllUsesWith(opnew);
    op->erase();
    return opnew;
  }

  template<class T>
  T *replace(Op *op, Value::Type resultTy, const std::vector<Value> &v,
             const std::vector<Attr *> &v2) {
    setBeforeOp(op);
    auto opnew = create<T>(resultTy, v, v2);
    op->replaceAllUsesWith(opnew);
    op->erase();
    return opnew;
  }

  Op *copy(Op *op);
};

}  // namespace sys

#endif
