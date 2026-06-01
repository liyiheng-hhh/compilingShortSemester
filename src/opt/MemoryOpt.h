#ifndef MEMORY_OPT_H
#define MEMORY_OPT_H

#include "Pass.h"
#include "../codegen/CodeGen.h"

namespace sys {

// store val, addr  where val == prior load from same array cell → erase store
class RemoveRedundantStore : public Pass {
  int removed = 0;

  void runImpl(Region *region);
public:
  RemoveRedundantStore(ModuleOp *module): Pass(module) {}

  std::string name() override { return "remove-redundant-store"; }
  std::map<std::string, int> stats() override;
  void run() override;
};

// Same-block store→load forwarding using base+subscript / alias keys.
class ArrayStoreLoadForward : public Pass {
  int forwarded = 0;

  void runImpl(Region *region);
public:
  ArrayStoreLoadForward(ModuleOp *module): Pass(module) {}

  std::string name() override { return "array-store-load-forward"; }
  std::map<std::string, int> stats() override;
  void run() override;
};

// Erase local arrays (and their stores) that are never loaded.
class RemoveOnlyWriteArray : public Pass {
  int removedArrays = 0;
  int removedStores = 0;

  void runImpl(Region *region);
public:
  RemoveOnlyWriteArray(ModuleOp *module): Pass(module) {}

  std::string name() override { return "remove-only-write-array"; }
  std::map<std::string, int> stats() override;
  void run() override;
};

// Defer allocas to first use (SYSY_CC_ALLOCA_DEFER=1) and coalesce same-block scalars.
class AllocaCoalesce : public Pass {
  int moved = 0;
  int coalesced = 0;

  void runImpl(Region *region);
public:
  AllocaCoalesce(ModuleOp *module): Pass(module) {}

  std::string name() override { return "alloca-coalesce"; }
  std::map<std::string, int> stats() override;
  void run() override;
};

}  // namespace sys

#endif
