#include "HIRLoopTransform.h"

namespace sys::hir {

namespace {

// Walk and mark While/For as candidates for interchange/tiling/unroll.
// Real implementation would reorder children or duplicate body nodes.

static bool transformLoops(Op *op) {
  if (!op) return false;
  bool changed = false;
  if (op->kind == OpKind::While || op->kind == OpKind::For) {
    // Placeholder: in a full pass we would analyze trip count, dependence,
    // then either swap inner/outer loops or insert tiled loops.
    // For now we just record that the node was visited.
    op->symbol = (op->symbol.empty() ? "" : op->symbol + "_") + "xform_candidate";
    changed = true;
  }
  for (auto &c : op->children) {
    if (transformLoops(c.get())) changed = true;
  }
  return changed;
}

} // namespace

bool tryLoopInterchangeOnHIR(Module &module) {
  if (!module.root) return false;
  return transformLoops(module.root.get());
}

bool tryLoopTilingOnHIR(Module &module, int /*tileSize*/) {
  if (!module.root) return false;
  return transformLoops(module.root.get());
}

bool tryLoopUnrollOnHIR(Module &module, int /*factor*/) {
  if (!module.root) return false;
  return transformLoops(module.root.get());
}

} // namespace sys::hir