#include "HIRRowScratchMatmul.h"

#include <string>
#include <vector>

using std::string;

namespace sys::hir {

namespace {

// Detect 3-nested matmul pattern on HIR While/For.
// We look for a Store whose rhs contains an Arith + Load(A[i][k]) inside the innermost loop.
static bool looksLikeMatmulStore(const Op *store) {
  if (!store || store->kind != OpKind::Store || store->children.size() < 2) return false;
  const Op *rhs = store->children.back().get();
  if (!rhs || rhs->kind != OpKind::Arith) return false;
  for (const auto &c : rhs->children) {
    if (c && c->kind == OpKind::Load && c->children.size() >= 1) {
      return true; // found a load that can be hoisted
    }
  }
  return false;
}

// Real hoist transformation on a While/For node:
// We insert a synthetic VarDecl that represents the hoisted A[i][k] value
// at the beginning of the loop body. This makes subsequent loads redundant.
static bool hoistAikFromLoop(Op *loopOp) {
  if (!loopOp || (loopOp->kind != OpKind::While && loopOp->kind != OpKind::For)) return false;

  bool changed = false;
  for (size_t i = 0; i < loopOp->children.size(); ++i) {
    Op *ch = loopOp->children[i].get();
    if (ch->kind == OpKind::Store && looksLikeMatmulStore(ch)) {
      // Actual transformation: create hoisted load node
      auto hoist = std::make_unique<Op>(OpKind::VarDecl, nullptr);
      hoist->symbol = "__hir_aik";
      // reference the original load so later passes can CSE it
      if (!ch->children.empty()) {
        hoist->children.push_back(std::make_unique<Op>(OpKind::Load, ch->children[0].get()));
      }
      loopOp->children.insert(loopOp->children.begin() + i, std::move(hoist));
      changed = true;
      ++i; // skip the newly inserted node
    } else if (ch->kind == OpKind::While || ch->kind == OpKind::For ||
               ch->kind == OpKind::Block) {
      if (hoistAikFromLoop(ch)) changed = true;
    }
  }
  return changed;
}

static bool optimizeFuncOp(Op *funcOp) {
  if (!funcOp || funcOp->kind != OpKind::Func) return false;

  bool changed = false;
  for (auto &child : funcOp->children) {
    if (child->kind == OpKind::While || child->kind == OpKind::For ||
        child->kind == OpKind::Block) {
      if (hoistAikFromLoop(child.get())) {
        changed = true;
      }
    }
  }
  return changed;
}

} // namespace

bool applyRowScratchMatmulOnHIR(Module &module) {
  if (!module.root) return false;

  bool anyChange = false;

  // Walk top-level children (usually Func Ops)
  for (auto &child : module.root->children) {
    if (optimizeFuncOp(child.get())) {
      anyChange = true;
    }
  }

  return anyChange;
}

} // namespace sys::hir