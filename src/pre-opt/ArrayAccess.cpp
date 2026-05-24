#include "PreAnalysis.h"

using namespace sys;

namespace {
  
AffineExpr aaMakeAffine(const std::vector<Op*> &outer) {
  return AffineExpr(outer.size() + 1);
}

// Lengthen the affine expression to match the current depth.
void aaLengthenExpr(AffineExpr &x, const std::vector<Op*> &outer) {
  x.reserve(outer.size() + 1);

  // Remove the constant.
  auto back = x.back();
  x.pop_back();
  // Add zeroes at end.
  x.resize(outer.size());
  // Put the constant back.
  x.push_back(back);
}

AffineExpr aaLengthenedExpr(Op *op, const std::vector<Op*> &outer) {
  auto val = SUBSCRIPT(op);
  aaLengthenExpr(val, outer);
  return val;
}

void aaClearSubscripts(Region *region) {
  for (auto bb : region->getBlocks()) {
    for (auto op : bb->getOps()) {
      op->remove<SubscriptAttr>();
      for (auto r : op->getRegions())
        aaClearSubscripts(r);
    }
  }
}

}

void ArrayAccess::runImpl(Op *loop, std::vector<Op*> outer) {
  auto region = loop->getRegion();
  auto bb = region->getFirstBlock();

  for (auto op : bb->getOps()) {
    if (isa<IntOp>(op)) {
      auto val = aaMakeAffine(outer);
      val.back() = V(op);
      op->add<SubscriptAttr>(val);
      continue;
    }

    if (isa<ForOp>(op)) {
      auto k = outer;
      k.push_back(op);
      
      // Add subscript to this loop's induction variable.
      auto val = aaMakeAffine(k);
      val[val.size() - 2] = 1;
      op->add<SubscriptAttr>(val);

      runImpl(op, k);
      continue;
    }

    if (isa<IfOp>(op)) {
      runImpl(op, outer);
      continue;
    }

    // Though WhileOp has regions, it is not considered here.
    // It's because induction variable isn't clear.

    if (isa<AddIOp>(op)) {
      auto x = op->DEF(0);
      auto y = op->DEF(1);
      if (!x->has<SubscriptAttr>() || !y->has<SubscriptAttr>())
        continue;

      auto vx = aaLengthenedExpr(x, outer);
      auto vy = aaLengthenedExpr(y, outer);
      for (int i = 0; i < vx.size(); i++)
        vx[i] += vy[i];
      op->add<SubscriptAttr>(vx);
      continue;
    }

    if (isa<MulIOp>(op)) {
      auto x = op->DEF(0);
      auto y = op->DEF(1);
      if (!isa<IntOp>(y) || !x->has<SubscriptAttr>())
        continue;

      auto val = aaLengthenedExpr(x, outer);
      for (auto &coeff : val)
        coeff *= V(y);
      op->add<SubscriptAttr>(val);
      continue;
    }

    // Also tag the address, though it technically isn't a subscript.
    if (isa<AddLOp>(op)) {
      auto x = op->DEF(0);
      auto y = op->DEF(1);
      if (!x->has<SubscriptAttr>()) {
        if (y->has<SubscriptAttr>())
          std::swap(x, y);
        else continue;
      }
      
      auto vx = aaLengthenedExpr(x, outer);
      if (y->has<SubscriptAttr>()) {
        auto vy = aaLengthenedExpr(y, outer);
        for (int i = 0; i < vx.size(); i++)
          vx[i] += vy[i];
      }
      op->add<SubscriptAttr>(vx);
      continue;
    }
  }
}

void ArrayAccess::run() {
  auto funcs = collectFuncs();

  for (auto func : funcs) {
    auto region = func->getRegion();

    // Remove all existing subscripts first.
    aaClearSubscripts(region);

    for (auto bb : region->getBlocks()) {
      for (auto op : bb->getOps()) {
        if (!isa<ForOp>(op))
          continue;

        // Start marking.
        op->add<SubscriptAttr>(std::vector { 1, 0 });
        runImpl(op, { op });
      }
    }
  }
}
