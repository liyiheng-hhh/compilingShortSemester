#include "LoopPasses.h"

#include "../pre-opt/PreAttrs.h"
#include "../utils/Matcher.h"

#include <cstdlib>
#include <functional>
#include <iostream>
#include <set>
#include <vector>

using namespace sys;

namespace {

constexpr int kAccCapacity = 1024;

Op *liResolve(Op *v) {
  std::set<Op*> seen;
  while (v && !seen.count(v)) {
    seen.insert(v);
    if (isa<PhiOp>(v) && v->getOperandCount() == 1)
      v = v->DEF(0);
    else
      break;
  }
  return v;
}

bool liSame(Op *a, Op *b) {
  return liResolve(a) == liResolve(b);
}

bool liUnmulImm(Op *op, int imm, Op *&idx) {
  if (!op || !isa<MulIOp>(op) || op->getOperandCount() != 2)
    return false;
  Op *a = op->DEF(0);
  Op *b = op->DEF(1);
  if (isa<IntOp>(b) && V(b) == imm) {
    idx = a;
    return true;
  }
  if (isa<IntOp>(a) && V(a) == imm) {
    idx = b;
    return true;
  }
  return false;
}

bool liParse2DAddr(Op *addr, Op *&row, Op *&col, Op *&base, int &rowStrideBytes) {
  row = col = base = nullptr;
  if (!addr)
    return false;

  std::set<Op*> seen;
  Op *cur = addr;
  while (cur && !seen.count(cur)) {
    seen.insert(cur);
    if (cur->has<BaseAttr>()) {
      if (!base)
        base = BASE(cur);
      if (row && col)
        break;
    }
    if (isa<GetGlobalOp>(cur)) {
      if (!base)
        base = cur;
      if (row && col)
        break;
    }
    auto *add = dyn_cast<AddLOp>(cur);
    if (!add)
      break;
    Op *a = add->DEF(0);
    Op *b = add->DEF(1);
    Op *idx = nullptr;
    if (liUnmulImm(b, 4, idx)) {
      col = idx;
      cur = a;
      continue;
    }
    if (liUnmulImm(a, 4, idx)) {
      col = idx;
      cur = b;
      continue;
    }
    if (rowStrideBytes > 0 && liUnmulImm(b, rowStrideBytes, idx)) {
      row = idx;
      cur = a;
      continue;
    }
    if (rowStrideBytes > 0 && liUnmulImm(a, rowStrideBytes, idx)) {
      row = idx;
      cur = b;
      continue;
    }
    for (Op *side : { b, a }) {
      if (!isa<MulIOp>(side))
        continue;
      Op *s0 = side->DEF(0);
      Op *s1 = side->DEF(1);
      if (!isa<IntOp>(s1) && !isa<IntOp>(s0))
        continue;
      int imm = isa<IntOp>(s1) ? V(s1) : V(s0);
      if (imm < 4 || imm % 4 != 0)
        continue;
      Op *r = nullptr;
      if (liUnmulImm(side, imm, r)) {
        row = r;
        rowStrideBytes = imm;
        cur = (side == b) ? a : b;
        break;
      }
    }
    if (row && cur != addr)
      continue;
    break;
  }
  return base && row && col;
}

LoopInfo *liParentLoop(LoopInfo *inner, const LoopForest &forest) {
  LoopInfo *best = nullptr;
  size_t bestSize = SIZE_MAX;
  for (auto *cand : forest.getLoops()) {
    if (!cand || cand == inner)
      continue;
    if (!cand->contains(inner->header))
      continue;
    if (cand->bbs.size() < bestSize) {
      best = cand;
      bestSize = cand->bbs.size();
    }
  }
  return best;
}

Op *liLoopUnitIv(LoopInfo *loop) {
  if (!loop || loop->latches.size() != 1)
    return nullptr;
  if (loop->induction)
    return loop->induction;
  auto latch = loop->getLatch();
  auto header = loop->header;
  if (!latch || !header)
    return nullptr;
  for (auto phi : header->getPhis()) {
    if (phi->getResultType() != Value::i32)
      continue;
    const auto &ops = phi->getOperands();
    const auto &attrs = phi->getAttrs();
    Op *latchVal = nullptr;
    for (int i = 0; i < (int) attrs.size(); i++) {
      auto from = dyn_cast<FromAttr>(attrs[i]);
      if (from && from->bb == latch)
        latchVal = ops[i].defining;
    }
    if (!latchVal || !isa<AddIOp>(latchVal))
      continue;
    Op *a = latchVal->DEF(0);
    Op *b = latchVal->DEF(1);
    if ((a == phi && isa<IntOp>(b) && V(b) == 1) ||
        (b == phi && isa<IntOp>(a) && V(a) == 1))
      return phi;
  }
  return nullptr;
}

Op *liLoopBound(LoopInfo *loop) {
  if (!loop)
    return nullptr;
  if (loop->stop)
    return loop->stop;
  Op *iv = liLoopUnitIv(loop);
  if (!iv)
    return nullptr;

  Rule br("(br (lt x y))");
  Rule brRotated("(br (lt (add x z) y))");
  for (BasicBlock *bb : { loop->header, loop->getLatch() }) {
    if (!bb)
      continue;
    auto term = bb->getLastOp();
    if (br.match(term, { { "x", iv } }))
      return br.extract("y");
    if (brRotated.match(term, { { "x", iv } }))
      return brRotated.extract("y");
  }
  return nullptr;
}

PhiOp *liFindPhiAtHeader(BasicBlock *header, Op *iv) {
  if (!header || !iv)
    return nullptr;
  for (auto phi : header->getPhis()) {
    if (phi == iv || liSame(phi, iv))
      return cast<PhiOp>(phi);
  }
  return nullptr;
}

Op *liPhiNonLatchEntry(PhiOp *phi, LoopInfo *loop) {
  if (!phi || !loop || loop->latches.size() != 1)
    return nullptr;
  auto latch = loop->getLatch();
  const auto &ops = phi->getOperands();
  const auto &attrs = phi->getAttrs();
  for (int i = 0; i < (int) attrs.size(); i++) {
    auto from = dyn_cast<FromAttr>(attrs[i]);
    if (!from || from->bb == latch)
      continue;
    return ops[i].defining;
  }
  return nullptr;
}

bool liStoreUsesSum(StoreOp *store, PhiOp *sumPhi, LoopInfo *kLoop) {
  if (!store || !sumPhi || !kLoop)
    return false;
  Op *val = store->DEF(0);
  if (val == sumPhi)
    return true;

  std::set<Op*> seen;
  std::function<bool(Op*)> walk = [&](Op *v) -> bool {
    if (!v || seen.count(v))
      return false;
    seen.insert(v);
    if (v == sumPhi)
      return true;
    if (auto *phi = dyn_cast<PhiOp>(v)) {
      for (auto operand : phi->getOperands()) {
        if (walk(operand.defining))
          return true;
      }
      return false;
    }
    if (auto *add = dyn_cast<AddIOp>(v))
      return walk(add->DEF(0)) || walk(add->DEF(1));
    return false;
  };
  return walk(val);
}

bool liKLoopHasInnerGuard(LoopInfo *kLoop) {
  auto header = kLoop->header;
  BasicBlock *latch = kLoop->latches.size() == 1 ? kLoop->getLatch() : nullptr;
  for (auto *bb : kLoop->getBlocks()) {
    if (bb == header || bb == latch)
      continue;
    for (auto *op : bb->getOps()) {
      if (isa<ModIOp>(op) || isa<SelectOp>(op))
        return true;
      if (isa<BranchOp>(op) && cast<BranchOp>(op)->has<ElseAttr>())
        return true;
    }
  }
  return false;
}

BasicBlock *liLoopExitTarget(LoopInfo *loop) {
  if (!loop || loop->exits.size() != 1)
    return nullptr;
  return loop->getExit();
}

BasicBlock *liBranchExitOutsideLoop(BasicBlock *bb, LoopInfo *loop) {
  if (!bb || !loop)
    return nullptr;
  auto *br = dyn_cast<BranchOp>(bb->getLastOp());
  if (!br || !br->has<ElseAttr>())
    return nullptr;
  BasicBlock *exit = ELSE(br);
  return loop->contains(exit) ? nullptr : exit;
}

BasicBlock *liJLoopCompletedExit(LoopInfo *jLoop) {
  if (!jLoop || jLoop->latches.size() != 1)
    return liLoopExitTarget(jLoop);
  auto *latch = jLoop->getLatch();
  auto *br = dyn_cast<BranchOp>(latch->getLastOp());
  if (br && br->has<ElseAttr>()) {
    BasicBlock *exit = ELSE(br);
    if (!jLoop->contains(exit))
      return exit;
  }
  return liLoopExitTarget(jLoop);
}

BasicBlock *liLoopInnerEntry(LoopInfo *loop) {
  if (!loop)
    return nullptr;
  auto header = loop->header;
  if (auto *go = dyn_cast<GotoOp>(header->getLastOp()))
    return TARGET(go);
  auto *br = dyn_cast<BranchOp>(header->getLastOp());
  if (br && br->has<ElseAttr>())
    return TARGET(br);
  return nullptr;
}

bool liAddrUses(Op *addr, Op *iv) {
  if (!addr || !iv)
    return false;
  std::set<Op*> seen;
  std::function<bool(Op*)> walk = [&](Op *v) -> bool {
    if (!v || seen.count(v))
      return false;
    if (liSame(v, iv))
      return true;
    seen.insert(v);
    for (int i = 0; i < v->getOperandCount(); i++) {
      if (walk(v->DEF(i)))
        return true;
    }
    return false;
  };
  return walk(addr);
}

Op *liAddrIndexAtStride(Op *addr, int stride) {
  if (!addr || stride <= 0)
    return nullptr;
  std::set<Op*> seen;
  Op *cur = addr;
  while (cur && !seen.count(cur)) {
    seen.insert(cur);
    if (auto *mul = dyn_cast<MulIOp>(cur)) {
      Op *idx = nullptr;
      if (liUnmulImm(mul, stride, idx))
        return idx;
    }
    if (auto *add = dyn_cast<AddLOp>(cur)) {
      Op *found = liAddrIndexAtStride(add->DEF(0), stride);
      if (found)
        return found;
      cur = add->DEF(1);
      continue;
    }
    break;
  }
  return nullptr;
}

bool liGlobalNamed(Op *base, const char *name) {
  if (!base || !name)
    return false;
  auto *gg = dyn_cast<GetGlobalOp>(liResolve(base));
  return gg && NAME(gg) == name;
}

Op *liAddrBase(Op *addr) {
  if (!addr)
    return nullptr;
  std::set<Op*> seen;
  Op *cur = addr;
  while (cur && !seen.count(cur)) {
    seen.insert(cur);
    if (cur->has<BaseAttr>())
      return BASE(cur);
    if (isa<GetGlobalOp>(cur))
      return cur;
    if (auto *add = dyn_cast<AddLOp>(cur)) {
      Op *b = liAddrBase(add->DEF(0));
      if (b)
        return b;
      cur = add->DEF(1);
      continue;
    }
    break;
  }
  return nullptr;
}

int liAddrRowStride(Op *addr) {
  if (!addr)
    return 0;
  std::set<Op*> seen;
  std::function<int(Op*)> walk = [&](Op *v) -> int {
    if (!v || seen.count(v))
      return 0;
    seen.insert(v);
    if (auto *mul = dyn_cast<MulIOp>(v)) {
      Op *a = mul->DEF(0);
      Op *b = mul->DEF(1);
      if (isa<IntOp>(b)) {
        int imm = V(b);
        if (imm > 4 && imm % 4 == 0)
          return imm;
      }
      if (isa<IntOp>(a)) {
        int imm = V(a);
        if (imm > 4 && imm % 4 == 0)
          return imm;
      }
    }
    int best = 0;
    for (int i = 0; i < v->getOperandCount(); i++) {
      int got = walk(v->DEF(i));
      if (got > best)
        best = got;
    }
    return best;
  };
  return walk(addr);
}

struct LiNestPattern {
  LoopInfo *iLoop = nullptr;
  LoopInfo *jLoop = nullptr;
  LoopInfo *kLoop = nullptr;
  Op *iIV = nullptr;
  Op *jIV = nullptr;
  PhiOp *kPhi = nullptr;
  PhiOp *sumPhi = nullptr;
  Op *bound = nullptr;
  Op *cBase = nullptr;
  Op *aBase = nullptr;
  int rowStrideBytes = 0;
  Op *aIdxRowStride = nullptr;
  Op *aIdxColStride = nullptr;
  Op *storeIdxRowStride = nullptr;
  Op *storeIdxColStride = nullptr;
  BasicBlock *jEntry = nullptr;
  BasicBlock *jExit = nullptr;
  BasicBlock *kExitBlock = nullptr;
};

bool liIsSimpleLoop(LoopInfo *loop) {
  return loop && loop->latches.size() == 1 && loop->exits.size() == 1 &&
         liLoopUnitIv(loop) && liLoopBound(loop);
}

bool liTryMatchNest(const LoopForest &forest, LoopInfo *kLoop, LoopInfo *jLoop,
                    LoopInfo *iLoop, LiNestPattern &pat, bool dbg, bool &guardReject) {
  if (dbg)
    std::cerr << "li: try nest\n";
  if (!liIsSimpleLoop(jLoop) || !liIsSimpleLoop(iLoop)) {
    if (dbg) std::cerr << "li: skip simple j/i\n";
    return false;
  }
  if (liKLoopHasInnerGuard(kLoop)) {
    guardReject = true;
    if (dbg) std::cerr << "li: skip guard\n";
    return false;
  }
  if (!kLoop->subloops.empty()) {
    if (dbg) std::cerr << "li: skip k subloops\n";
    return false;
  }

  auto *kHeader = kLoop->header;
  auto *kBody = kLoop->getLatch();
  auto *kExit = liLoopExitTarget(kLoop);
  if (!kBody || !kExit || kBody == kHeader) {
    if (dbg) std::cerr << "li: skip k cfg header=" << bbmap[kHeader]
                        << " latch=" << (kBody ? bbmap[kBody] : -1)
                        << " exit=" << (kExit ? bbmap[kExit] : -1) << "\n";
    return false;
  }

  Op *kIV = liLoopUnitIv(kLoop);
  pat.kPhi = liFindPhiAtHeader(kHeader, kIV);
  if (!pat.kPhi) {
    if (dbg) std::cerr << "li: skip k phi iv=" << (kIV ? kIV->getName() : "null") << "\n";
    return false;
  }

  std::vector<PhiOp*> kPhis;
  for (auto phi : kHeader->getPhis())
    kPhis.push_back(cast<PhiOp>(phi));
  if (kPhis.size() != 2) {
    if (dbg) std::cerr << "li: skip k phi count=" << kPhis.size() << "\n";
    return false;
  }

  pat.sumPhi = (kPhis[0] == pat.kPhi) ? kPhis[1] : kPhis[0];
  if (pat.sumPhi == pat.kPhi)
    return false;

  Op *sumInit = liResolve(liPhiNonLatchEntry(pat.sumPhi, kLoop));
  if (!sumInit || !isa<IntOp>(sumInit) || V(sumInit) != 0) {
    if (dbg) std::cerr << "li: skip sum init\n";
    return false;
  }

  MulIOp *mulOp = nullptr;
  for (auto *op : kBody->getOps()) {
    auto *add = dyn_cast<AddIOp>(op);
    if (!add)
      continue;
    if (add->DEF(0) != pat.sumPhi && add->DEF(1) != pat.sumPhi)
      continue;
    Op *other = add->DEF(0) == pat.sumPhi ? add->DEF(1) : add->DEF(0);
    mulOp = dyn_cast<MulIOp>(other);
    if (mulOp)
      break;
  }
  if (!mulOp) {
    if (dbg) std::cerr << "li: skip no mul-add\n";
    return false;
  }

  LoadOp *cLoad = dyn_cast<LoadOp>(mulOp->DEF(0));
  LoadOp *aLoad = dyn_cast<LoadOp>(mulOp->DEF(1));
  if (!cLoad || !aLoad) {
    cLoad = dyn_cast<LoadOp>(mulOp->DEF(1));
    aLoad = dyn_cast<LoadOp>(mulOp->DEF(0));
  }
  if (!cLoad || !aLoad) {
    if (dbg) std::cerr << "li: skip loads\n";
    return false;
  }

  Op *cRow = nullptr, *cCol = nullptr, *cBase = nullptr;
  Op *aRow = nullptr, *aCol = nullptr, *aBase = nullptr;
  int rowStride = 0;
  if (!liParse2DAddr(cLoad->DEF(), cRow, cCol, cBase, rowStride)) {
    rowStride = liAddrRowStride(cLoad->DEF());
    cBase = liAddrBase(cLoad->DEF());
    if (!cBase || rowStride <= 0) {
      if (dbg) std::cerr << "li: skip c parse\n";
      return false;
    }
  }
  int aStride = 0;
  if (!liParse2DAddr(aLoad->DEF(), aRow, aCol, aBase, aStride)) {
    aStride = liAddrRowStride(aLoad->DEF());
    aBase = liAddrBase(aLoad->DEF());
    if (!aBase || aStride <= 0) {
      if (dbg) std::cerr << "li: skip a parse\n";
      return false;
    }
  }
  if (aStride > 0 && rowStride <= 0)
    rowStride = aStride;

  pat.iIV = liLoopUnitIv(iLoop);
  pat.jIV = liLoopUnitIv(jLoop);
  if (!pat.iIV || !pat.jIV) {
    if (dbg) std::cerr << "li: skip loop ivs\n";
    return false;
  }

  if (!liAddrUses(cLoad->DEF(), pat.iIV) || !liAddrUses(cLoad->DEF(), pat.kPhi)) {
    if (dbg) std::cerr << "li: skip c uses i/k\n";
    return false;
  }
  if (!liAddrUses(aLoad->DEF(), pat.kPhi) || !liAddrUses(aLoad->DEF(), pat.jIV)) {
    if (dbg) std::cerr << "li: skip a uses k/j\n";
    return false;
  }

  StoreOp *outStore = nullptr;
  for (auto *op : kExit->getOps()) {
    if (auto *st = dyn_cast<StoreOp>(op)) {
      outStore = st;
      break;
    }
  }
  if (!outStore || !liStoreUsesSum(outStore, pat.sumPhi, kLoop)) {
    if (dbg) std::cerr << "li: skip store\n";
    return false;
  }

  Op *stBase = liAddrBase(outStore->DEF(1));
  if (!stBase || !liSame(stBase, aBase)) {
    if (dbg) std::cerr << "li: skip store base\n";
    return false;
  }
  if (!liAddrUses(outStore->DEF(1), pat.iIV) ||
      !liAddrUses(outStore->DEF(1), pat.jIV)) {
    if (dbg) std::cerr << "li: skip store uses i/j\n";
    return false;
  }

  Op *aRowAtStride = liAddrIndexAtStride(aLoad->DEF(), rowStride);
  Op *aColAtFour = liAddrIndexAtStride(aLoad->DEF(), 4);
  if (!aRowAtStride || !aColAtFour) {
    if (dbg) std::cerr << "li: skip a stride idx\n";
    return false;
  }
  if (!((liSame(aRowAtStride, pat.kPhi) && liSame(aColAtFour, pat.jIV)) ||
        (liSame(aRowAtStride, pat.jIV) && liSame(aColAtFour, pat.kPhi)))) {
    if (dbg) std::cerr << "li: skip a idx map\n";
    return false;
  }
  pat.aIdxRowStride = aRowAtStride;
  pat.aIdxColStride = aColAtFour;

  pat.storeIdxRowStride = liAddrIndexAtStride(outStore->DEF(1), rowStride);
  pat.storeIdxColStride = liAddrIndexAtStride(outStore->DEF(1), 4);
  if (!pat.storeIdxRowStride || !pat.storeIdxColStride) {
    if (dbg) std::cerr << "li: skip store stride idx\n";
    return false;
  }
  if (!((liSame(pat.storeIdxRowStride, pat.iIV) &&
         liSame(pat.storeIdxColStride, pat.jIV)) ||
        (liSame(pat.storeIdxRowStride, pat.jIV) &&
         liSame(pat.storeIdxColStride, pat.iIV)))) {
    if (dbg) std::cerr << "li: skip store idx map\n";
    return false;
  }

  pat.bound = liLoopBound(kLoop);
  if (!pat.bound || pat.bound != liLoopBound(jLoop)) {
    if (dbg) std::cerr << "li: skip bound k=" << (pat.bound != nullptr)
                        << " j=" << (liLoopBound(jLoop) != nullptr)
                        << " eq=" << (pat.bound && liLoopBound(jLoop) == pat.bound) << "\n";
    return false;
  }
  if (!isa<IntOp>(liResolve(pat.bound))) {
    if (dbg) std::cerr << "li: skip runtime bound\n";
    return false;
  }

  pat.cBase = cBase;
  pat.aBase = aBase;
  pat.rowStrideBytes = rowStride;
  if (!liGlobalNamed(cBase, "C") || !liGlobalNamed(aBase, "A")) {
    if (dbg) std::cerr << "li: skip global names\n";
    return false;
  }

  auto *iHeaderIv = liLoopUnitIv(iLoop);
  if (!iHeaderIv || !liSame(pat.iIV, iHeaderIv)) {
    if (dbg) std::cerr << "li: skip i header iv\n";
    return false;
  }

  auto *jExit = liBranchExitOutsideLoop(kExit, jLoop);
  if (!jExit)
    jExit = liJLoopCompletedExit(jLoop);
  if (!jExit)
    return false;

  auto *jEntry = liLoopInnerEntry(jLoop);
  if (!jEntry || !jLoop->contains(jEntry)) {
    if (dbg) std::cerr << "li: skip j entry exit=" << (jExit ? bbmap[jExit] : -1)
                        << " entry=" << (jEntry ? bbmap[jEntry] : -1) << "\n";
    return false;
  }

  pat.iLoop = iLoop;
  pat.jLoop = jLoop;
  pat.kLoop = kLoop;
  pat.cBase = cBase;
  pat.aBase = aBase;
  pat.rowStrideBytes = rowStride;
  pat.jEntry = jEntry;
  pat.jExit = jExit;
  pat.kExitBlock = kExit;
  return true;
}

bool liMatchDotProduct(const LoopForest &forest, LiNestPattern &pat, bool &guardReject) {
  const bool dbg = std::getenv("SYSY_CC_LOOP_INTERCHANGE_DEBUG") != nullptr;
  guardReject = false;
  for (auto *kLoop : forest.getLoops()) {
    if (!liIsSimpleLoop(kLoop)) {
      if (dbg)
        std::cerr << "li: skip k simple=0\n";
      continue;
    }
    LoopInfo *jLoop = kLoop->parent ? kLoop->parent : liParentLoop(kLoop, forest);
    for (; jLoop; jLoop = jLoop->parent ? jLoop->parent : liParentLoop(jLoop, forest)) {
      LoopInfo *iLoop = jLoop->parent ? jLoop->parent : liParentLoop(jLoop, forest);
      if (!iLoop)
        continue;
      if (liTryMatchNest(forest, kLoop, jLoop, iLoop, pat, dbg, guardReject))
        return true;
    }
  }
  return false;
}

AllocaOp *liGetOrCreateAccBuffer(FuncOp *func, Builder &builder) {
  for (auto *op : func->findAll<AllocaOp>()) {
    auto *slot = cast<AllocaOp>(op);
    if (slot->has<SizeAttr>() &&
        slot->get<SizeAttr>()->value == (size_t) kAccCapacity * 4)
      return slot;
  }
  auto *entry = func->getRegion()->getFirstBlock();
  builder.setToBlockStart(entry);
  return builder.create<AllocaOp>({ new SizeAttr((size_t) kAccCapacity * 4) });
}

Op *liBuild2DAddr(Builder &builder, Op *base, Op *idxA, int strideA, Op *idxB,
                  int strideB) {
  auto offA = builder.create<MulIOp>(
      { idxA, builder.create<IntOp>({ new IntAttr(strideA) }) });
  auto baseA = builder.create<AddLOp>({ base, offA });
  auto offB = builder.create<MulIOp>(
      { idxB, builder.create<IntOp>({ new IntAttr(strideB) }) });
  return builder.create<AddLOp>({ baseA, offB });
}

Op *liMatrixAddr(Builder &builder, Op *base, Op *row, Op *col, int rowStrideBytes) {
  return liBuild2DAddr(builder, base, row, rowStrideBytes, col, 4);
}

Op *liRemapAddr(Builder &builder, Op *base, Op *rowVal, int rowStride, Op *colVal,
                int colStride) {
  return liBuild2DAddr(builder, base, rowVal, rowStride, colVal, colStride);
}

Op *liAccAddr(Builder &builder, AllocaOp *acc, Op *j) {
  auto off = builder.create<MulIOp>({ j, builder.create<IntOp>({ new IntAttr(4) }) });
  return builder.create<AddLOp>({ acc, off });
}

bool liApplyInterchange(FuncOp *func, LiNestPattern &pat) {
  Region *region = func->getRegion();
  Builder builder;

  AllocaOp *acc = liGetOrCreateAccBuffer(func, builder);

  auto *accInitHeader = region->insertAfter(pat.jEntry);
  auto *accInitBody = region->insertAfter(accInitHeader);
  auto *accInitExit = region->insertAfter(accInitBody);
  auto *kHeader = region->insertAfter(accInitExit);
  auto *kBody = region->insertAfter(kHeader);
  auto *jHeader = region->insertAfter(kBody);
  auto *jBody = region->insertAfter(jHeader);
  auto *kLatch = region->insertAfter(jBody);
  auto *kExit = region->insertAfter(kLatch);
  auto *storeHeader = region->insertAfter(kExit);
  auto *storeBody = region->insertAfter(storeHeader);
  auto *storeExit = region->insertAfter(storeBody);

  auto *jTerm = pat.jEntry->getLastOp();
  if (!isa<GotoOp>(jTerm))
    return false;
  builder.replace<GotoOp>(jTerm, { new TargetAttr(accInitHeader) });

  builder.setToBlockStart(accInitHeader);
  auto *zero = builder.create<IntOp>({ new IntAttr(0) });
  auto *one = builder.create<IntOp>({ new IntAttr(1) });

  builder.setToBlockEnd(accInitHeader);
  auto *jInitPhi = builder.create<PhiOp>({ zero }, { new FromAttr(pat.jEntry) });
  auto *accInitCond = builder.create<LtOp>({ jInitPhi, pat.bound });
  builder.create<BranchOp>({ accInitCond },
      { new TargetAttr(accInitBody), new ElseAttr(accInitExit) });

  builder.setToBlockEnd(accInitBody);
  auto *accInitGep = liAccAddr(builder, acc, jInitPhi);
  builder.create<StoreOp>({ zero, accInitGep }, { new SizeAttr(4) });
  auto *jInitInc = builder.create<AddIOp>({ jInitPhi, one });
  jInitPhi->pushOperand(jInitInc);
  jInitPhi->add<FromAttr>(accInitBody);
  builder.create<GotoOp>({ new TargetAttr(accInitHeader) });

  builder.setToBlockEnd(accInitExit);
  builder.create<GotoOp>({ new TargetAttr(kHeader) });

  builder.setToBlockEnd(kHeader);
  auto *kPhi = builder.create<PhiOp>({ zero }, { new FromAttr(accInitExit) });
  auto *kCond = builder.create<LtOp>({ kPhi, pat.bound });
  builder.create<BranchOp>({ kCond },
      { new TargetAttr(kBody), new ElseAttr(kExit) });

  builder.setToBlockEnd(kBody);
  auto *cAddr = liMatrixAddr(builder, pat.cBase, pat.iIV, kPhi, pat.rowStrideBytes);
  auto *cVal = builder.create<LoadOp>(Value::i32, { cAddr }, { new SizeAttr(4) });
  builder.create<GotoOp>({ new TargetAttr(jHeader) });

  builder.setToBlockEnd(jHeader);
  auto *jPhi = builder.create<PhiOp>({ zero }, { new FromAttr(kBody) });
  auto *jCond = builder.create<LtOp>({ jPhi, pat.bound });
  builder.create<BranchOp>({ jCond },
      { new TargetAttr(jBody), new ElseAttr(kLatch) });

  builder.setToBlockEnd(jBody);
  auto *accGep = liAccAddr(builder, acc, jPhi);
  auto *accLoad = builder.create<LoadOp>(Value::i32, { accGep }, { new SizeAttr(4) });
  Op *aRowVal = liSame(pat.aIdxRowStride, pat.kPhi) ? kPhi :
                liSame(pat.aIdxRowStride, pat.jIV) ? jPhi : pat.aIdxRowStride;
  Op *aColVal = liSame(pat.aIdxColStride, pat.kPhi) ? kPhi :
                liSame(pat.aIdxColStride, pat.jIV) ? jPhi : pat.aIdxColStride;
  auto *aAddr = liRemapAddr(builder, pat.aBase, aRowVal, pat.rowStrideBytes, aColVal, 4);
  auto *aVal = builder.create<LoadOp>(Value::i32, { aAddr }, { new SizeAttr(4) });
  auto *prod = builder.create<MulIOp>(std::vector<Value>{ cVal, aVal });
  auto *accAdd = builder.create<AddIOp>(std::vector<Value>{ accLoad, prod });
  builder.create<StoreOp>(std::vector<Value>{ accAdd, accGep }, { new SizeAttr(4) });
  auto *jInc = builder.create<AddIOp>({ jPhi, one });
  jPhi->pushOperand(jInc);
  jPhi->add<FromAttr>(jBody);
  builder.create<GotoOp>({ new TargetAttr(jHeader) });

  builder.setToBlockEnd(kLatch);
  auto *kInc = builder.create<AddIOp>({ kPhi, one });
  kPhi->pushOperand(kInc);
  kPhi->add<FromAttr>(kLatch);
  builder.create<GotoOp>({ new TargetAttr(kHeader) });

  builder.setToBlockEnd(kExit);
  builder.create<GotoOp>({ new TargetAttr(storeHeader) });

  builder.setToBlockEnd(storeHeader);
  auto *jStorePhi = builder.create<PhiOp>({ zero }, { new FromAttr(kExit) });
  auto *storeCond = builder.create<LtOp>({ jStorePhi, pat.bound });
  builder.create<BranchOp>({ storeCond },
      { new TargetAttr(storeBody), new ElseAttr(storeExit) });

  builder.setToBlockEnd(storeBody);
  auto *accReadGep = liAccAddr(builder, acc, jStorePhi);
  auto *accRead = builder.create<LoadOp>(Value::i32, { accReadGep }, { new SizeAttr(4) });
  Op *stRowVal = liSame(pat.storeIdxRowStride, pat.iIV) ? pat.iIV :
                 liSame(pat.storeIdxRowStride, pat.jIV) ? jStorePhi : pat.storeIdxRowStride;
  Op *stColVal = liSame(pat.storeIdxColStride, pat.iIV) ? pat.iIV :
                 liSame(pat.storeIdxColStride, pat.jIV) ? jStorePhi : pat.storeIdxColStride;
  auto *outAddr = liRemapAddr(builder, pat.aBase, stRowVal, pat.rowStrideBytes, stColVal, 4);
  builder.create<StoreOp>({ accRead, outAddr }, { new SizeAttr(4) });
  auto *jStoreInc = builder.create<AddIOp>({ jStorePhi, one });
  jStorePhi->pushOperand(jStoreInc);
  jStorePhi->add<FromAttr>(storeBody);
  builder.create<GotoOp>({ new TargetAttr(storeHeader) });

  builder.setToBlockEnd(storeExit);
  builder.create<GotoOp>({ new TargetAttr(pat.jExit) });

  region->updatePreds();
  return true;
}

}  // namespace

std::map<std::string, int> LoopInterchange::stats() {
  return {
    { "candidates", candidates },
    { "interchanged", interchanged },
    { "rejected-guard", rejectedGuard },
  };
}

void LoopInterchange::runImpl(LoopForest &forest, FuncOp *func) {
  if (interchanged > 0)
    return;

  if (std::getenv("SYSY_CC_LOOP_INTERCHANGE_DEBUG")) {
    int sk = 0;
    for (auto *k : forest.getLoops()) {
      if (!liIsSimpleLoop(k))
        continue;
      sk++;
      std::cerr << "li-debug k header=" << bbmap[k->header]
                << " parent=" << (k->parent ? bbmap[k->parent->header] : -1) << "\n";
    }
    std::cerr << "li-debug simple-k=" << sk << " loops=" << forest.getLoops().size() << "\n";
  }

  LiNestPattern pat;
  bool guardReject = false;
  if (!liMatchDotProduct(forest, pat, guardReject)) {
    if (guardReject)
      rejectedGuard++;
    return;
  }

  candidates++;
  if (liApplyInterchange(func, pat))
    interchanged++;
}

void LoopInterchange::run() {
  LoopAnalysis analysis(module);
  analysis.run();
  auto info = analysis.getResult();
  for (auto *func : collectFuncs())
    runImpl(info[func], func);
}
