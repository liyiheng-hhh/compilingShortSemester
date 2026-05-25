#include "PreLoopPasses.h"

// compiler2026-x phase-D (trivial opt dedup)

using namespace sys;

namespace {

void lwrStoreIncrement(Builder &builder, Op *iv, Value ivAddr, Value incr) {
  builder.create<StoreOp>({ builder.create<AddIOp>({ iv, incr }), ivAddr },
                        { new SizeAttr(4) });
}

void lwrPatchTerms(Builder &builder, Op *loop, Op *iv, Value ivAddr) {
  auto terms = loop->findAll<BreakOp>();
  auto conts = loop->findAll<ContinueOp>();
  terms.insert(terms.end(), conts.begin(), conts.end());
  auto incr = loop->getOperand(2);
  for (auto op : terms) {
    builder.setBeforeOp(op);
    lwrStoreIncrement(builder, iv, ivAddr, incr);
  }
  builder.setToBlockEnd(loop->getRegion()->getLastBlock());
  lwrStoreIncrement(builder, iv, ivAddr, incr);
}

} // namespace

void Lower::run() {
  Builder builder;
  for (auto loop : module->findAll<ForOp>()) {
    builder.setBeforeOp(loop);
    auto ivAddr = loop->getOperand(3);
    auto region = loop->getRegion();

    builder.setToRegionStart(region);
    auto iv = builder.create<LoadOp>(Value::i32, { ivAddr }, { new SizeAttr(4) });
    loop->replaceAllUsesWith(iv);
    lwrPatchTerms(builder, loop, iv, ivAddr);

    builder.setBeforeOp(loop);
    auto wloop = builder.create<WhileOp>();
    auto before = wloop->appendRegion();
    auto after = wloop->appendRegion();

    for (auto it = region->begin(); it != region->end();) {
      auto next = it;
      ++next;
      (*it)->moveToEnd(after);
      it = next;
    }

    before->appendBlock();
    builder.setToRegionStart(before);
    auto load = builder.create<LoadOp>(Value::i32, { ivAddr }, { new SizeAttr(4) });
    builder.create<ProceedOp>({ builder.create<LtOp>({ load, loop->getOperand(1) }) });

    builder.setBeforeOp(wloop);
    builder.create<StoreOp>({ loop->getOperand(0), ivAddr }, { new SizeAttr(4) });
    loop->erase();
  }
}
