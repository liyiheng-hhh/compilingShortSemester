// compiler2026-x phase-E (codegen builder)
// compiler2026-x phase-E (codegen)

#include "CodeGen.h"
#include "../utils/DynamicCast.h"
#include "Attrs.h"
#include "OpBase.h"
#include "Ops.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace sys;

void Builder::setToRegionStart(Region *region) {
  setToBlockStart(region->getFirstBlock());
}

void Builder::setToRegionEnd(Region *region) {
  setToBlockEnd(region->getFirstBlock());
}

void Builder::setToBlockStart(BasicBlock *block) {
  bb = block;
  at = bb->begin();
  init = true;
}

void Builder::setToBlockEnd(BasicBlock *block) {
  bb = block;
  at = bb->end();
  init = true;
}

void Builder::setBeforeOp(Op *op) {
  bb = op->parent;
  at = op->place;
  init = true;
}

void Builder::setAfterOp(Op *op) {
  setBeforeOp(op);
  ++at;
}

Op *Builder::copy(Op *op) {
  auto opnew = new Op(op->opid, op->resultTy, op->operands);
  for (auto attr : op->attrs) {
    auto cloned = attr->clone();
    cloned->refcnt++;
    opnew->attrs.push_back(cloned);
  }
  opnew->opname = op->opname;
  bb->insert(at, opnew);
  return opnew;
}
