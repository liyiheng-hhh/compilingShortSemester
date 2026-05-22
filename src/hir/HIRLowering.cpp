#include "HIRLowering.h"

#include <vector>
#include <string>

#include "../ir.h"  // for real IRInst / IRFunction / IROp
#include "../ast.h"

namespace sys::hir {

// Basic skeleton for real HIR → legacy IRInst lowering (Stage 2).
// We recursively walk the HIR tree and emit flat IRInst + Label/J/Beqz
// to reconstruct control flow for the existing Mem2Reg / GVN pipeline.
//
// This is the foundation that makes HIR a true optimizable IR:
// after this lowering, the rest of the -O1 pipeline sees a normal IRFunction.

namespace {

// Real lowering context for Stage 2.
// Now supports both textual debug output AND real IRInst emission into IRFunction.
struct LowerCtx {
  int nextVreg = 0;
  int nextLabel = 0;
  std::vector<std::string> lowered;   // textual debug
  IRFunction *targetFn = nullptr;     // when non-null, we emit real IRInst

  int allocVreg() { return nextVreg++; }
  std::string newLabel(const std::string &prefix) {
    return prefix + "_" + std::to_string(nextLabel++);
  }

  void emit(const std::string &inst) {
    lowered.push_back(inst);
  }

  // Real IRInst emission helper (Stage 2)
  void emitInst(IROp op, int dst = -1, int u = -1, int v = -1, int32_t imm = 0) {
    if (!targetFn) return;
    IRInst inst;
    inst.op = op;
    inst.dst = dst;
    inst.u = u;
    inst.v = v;
    inst.immI = imm;
    targetFn->insts.push_back(inst);
  }

  void emitLabel(const std::string &name) {
    if (!targetFn) return;
    IRInst inst;
    inst.op = IROp::Label;
    inst.ext = name;
    targetFn->insts.push_back(inst);
  }

  void emitJump(const std::string &target) {
    if (!targetFn) return;
    IRInst inst;
    inst.op = IROp::J;
    inst.ext = target;
    targetFn->insts.push_back(inst);
  }

  void emitBeqz(int condVreg, const std::string &target) {
    if (!targetFn) return;
    IRInst inst;
    inst.op = IROp::Beqz;
    inst.u = condVreg;
    inst.ext = target;
    targetFn->insts.push_back(inst);
  }
};

static void lowerOp(const Op *op, LowerCtx &ctx);

// Expression lowering: Arith / Load / Store / Const → real IRInst + vreg
static int lowerExpr(const Op *op, LowerCtx &ctx) {
  if (!op) return -1;

  switch (op->kind) {
    case OpKind::ConstInt:
      {
        int dst = ctx.allocVreg();
        ctx.emitInst(IROp::ConstI, dst, -1, -1, static_cast<int32_t>(op->intValue));
        ctx.emit("ConstI v" + std::to_string(dst));
        return dst;
      }
    case OpKind::ConstFloat:
      {
        int dst = ctx.allocVreg();
        ctx.emitInst(IROp::ConstF, dst);
        return dst;
      }
    case OpKind::Arith:
      {
        int lhs = lowerExpr(op->children.size() > 0 ? op->children[0].get() : nullptr, ctx);
        int rhs = lowerExpr(op->children.size() > 1 ? op->children[1].get() : nullptr, ctx);
        int dst = ctx.allocVreg();

        // Map common symbols to IROp. Default to Add if unknown.
        IROp opKind = IROp::Add;
        if (!op->symbol.empty()) {
          if (op->symbol == "+" || op->symbol == "Add") opKind = IROp::Add;
          else if (op->symbol == "-" || op->symbol == "Sub") opKind = IROp::Sub;
          else if (op->symbol == "*" || op->symbol == "Mul") opKind = IROp::Mul;
          else if (op->symbol == "/" || op->symbol == "Div") opKind = IROp::Div;
          else if (op->symbol == "%" || op->symbol == "Rem") opKind = IROp::Rem;
        }
        ctx.emitInst(opKind, dst, lhs, rhs);
        return dst;
      }
    case OpKind::Load:
      {
        int addr = lowerExpr(op->children.size() > 0 ? op->children[0].get() : nullptr, ctx);
        int dst = ctx.allocVreg();
        ctx.emitInst(IROp::LoadMem, dst, addr);
        return dst;
      }
    case OpKind::Store:
      {
        int addr = lowerExpr(op->children.size() > 0 ? op->children[0].get() : nullptr, ctx);
        int val  = lowerExpr(op->children.size() > 1 ? op->children[1].get() : nullptr, ctx);
        ctx.emitInst(IROp::StoreMem, -1, addr, val);
        return -1;
      }
    default:
      for (const auto &c : op->children) lowerExpr(c.get(), ctx);
      return -1;
  }
}

// Control-flow lowering: While / For / If → real Label + Beqz + J
static void lowerCF(const Op *op, LowerCtx &ctx) {
  if (!op) return;

  switch (op->kind) {
    case OpKind::While:
    case OpKind::For:
      {
        std::string header = ctx.newLabel("loop_header");
        std::string exitL  = ctx.newLabel("loop_exit");
        ctx.emitLabel(header);

        int condV = -1;
        if (!op->children.empty()) {
          condV = lowerExpr(op->children[0].get(), ctx);
        }
        ctx.emitBeqz(condV, exitL);

        for (size_t i = 1; i < op->children.size(); ++i) {
          lowerOp(op->children[i].get(), ctx);
        }
        ctx.emitJump(header);
        ctx.emitLabel(exitL);
        break;
      }
    case OpKind::If:
      {
        std::string elseL = ctx.newLabel("else");
        std::string endL  = ctx.newLabel("if_end");

        int condV = -1;
        if (!op->children.empty()) {
          condV = lowerExpr(op->children[0].get(), ctx);
        }
        ctx.emitBeqz(condV, elseL);

        if (op->children.size() > 1) lowerOp(op->children[1].get(), ctx);
        ctx.emitJump(endL);

        ctx.emitLabel(elseL);
        if (op->children.size() > 2) lowerOp(op->children[2].get(), ctx);
        ctx.emitLabel(endL);
        break;
      }
    default:
      lowerOp(op, ctx);
      break;
  }
}

static void lowerOp(const Op *op, LowerCtx &ctx) {
  if (!op) return;

  switch (op->kind) {
    case OpKind::Func:
    case OpKind::Block:
      for (const auto &c : op->children) lowerOp(c.get(), ctx);
      break;

    case OpKind::While:
    case OpKind::For:
      lowerCF(op, ctx);
      break;

    case OpKind::If:
      lowerCF(op, ctx);
      break;

    case OpKind::Arith:
    case OpKind::Load:
    case OpKind::Store:
    case OpKind::ConstInt:
    case OpKind::ConstFloat:
      lowerExpr(op, ctx);
      break;

    case OpKind::Return:
      {
        // Real Ret lowering
        int retVal = -1;
        if (!op->children.empty()) {
          retVal = lowerExpr(op->children[0].get(), ctx);
        }
        if (retVal >= 0) {
          ctx.emitInst(IROp::Ret, -1, retVal);
        } else {
          ctx.emitInst(IROp::Ret);
        }
        ctx.emit("Ret (lowered)");
        break;
      }
    case OpKind::Call:
      {
        // Real Call lowering: callee name in symbol, args in children
        std::string callee = op->symbol.empty() ? "unknown" : op->symbol;
        std::vector<int> argVregs;
        for (const auto &c : op->children) {
          int v = lowerExpr(c.get(), ctx);
          if (v >= 0) argVregs.push_back(v);
        }
        int dst = ctx.allocVreg();
        // Emit Call IRInst (dst holds return value if any)
        ctx.emitInst(IROp::Call, dst);
        // Store callee name in ext for later emission
        if (!ctx.targetFn->insts.empty()) {
          ctx.targetFn->insts.back().callee = callee;
          ctx.targetFn->insts.back().args = argVregs;
        }
        ctx.emit("Call " + callee + " (lowered)");
        break;
      }

    default:
      for (const auto &c : op->children) lowerOp(c.get(), ctx);
      break;
  }
}

} // namespace

void lowerToLegacyIR(const Module &module, std::vector<IRFunction> &outFns) {
  if (!module.root) return;

  // Walk top-level children looking for Func nodes
  for (const auto &child : module.root->children) {
    if (!child || child->kind != OpKind::Func) continue;

    IRFunction fn;
    fn.name = child->symbol.empty() ? "hir_func" : child->symbol;
    fn.ret = BaseType::Int;
    fn.nextVreg = 0;

    LowerCtx ctx;
    ctx.targetFn = &fn;

    // Simple function prologue: entry label + parameter loading placeholder
    std::string entryLabel = ctx.newLabel("entry");
    ctx.emitLabel(entryLabel);

    // Parameter handling: if HIR Func has VarDecl children representing params,
    // emit LoadParamAddr so downstream passes see them.
    // For now we emit a placeholder for each leading VarDecl.
    for (const auto &bodyChild : child->children) {
      if (bodyChild && bodyChild->kind == OpKind::VarDecl) {
        int p = ctx.allocVreg();
        ctx.emitInst(IROp::LoadParamAddr, p);
        // Parameter symbol linking can be done in a later pass if needed.
      }
    }

    // Lower the rest of the function body
    for (const auto &bodyChild : child->children) {
      if (bodyChild && bodyChild->kind == OpKind::VarDecl) continue;
      lowerOp(bodyChild.get(), ctx);
    }

    // Guarantee a Ret at the end so downstream passes (regalloc, emission) are happy
    bool hasRet = false;
    for (const auto &inst : fn.insts) {
      if (inst.op == IROp::Ret) { hasRet = true; break; }
    }
    if (!hasRet) {
      ctx.emitInst(IROp::Ret);
    }

    // Rebuild CFG (blocks + succ) so Mem2Reg / GVN / regalloc can work
    if (!fn.insts.empty()) {
      irRefreshCFG(fn);
    }

    outFns.push_back(std::move(fn));
  }
}

}  // namespace sys::hir
