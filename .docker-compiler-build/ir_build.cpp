#include "ir.h"

#include "common.h"
#include "semantic.h"

#include <algorithm>

using namespace std;

static bool exprHasLandLor(Expr *e);
static bool blockSequential(const BlockStmt &block);
static bool blockHasLocalArrayDecl(const BlockStmt &block);

bool irExprHasLogicalShortCircuit(Expr *e) { return exprHasLandLor(e); }

static bool exprHasLandLor(Expr *e) {
  if (!e) {
    return false;
  }
  if (e->kind == ExprKind::Binary) {
    auto *b = static_cast<BinaryExpr *>(e);
    if (b->op == "&&" || b->op == "||") {
      return true;
    }
    return exprHasLandLor(b->lhs.get()) || exprHasLandLor(b->rhs.get());
  }
  if (e->kind == ExprKind::Unary) {
    return exprHasLandLor(static_cast<UnaryExpr *>(e)->expr.get());
  }
  if (e->kind == ExprKind::Call) {
    auto *c = static_cast<CallExpr *>(e);
    for (auto &a : c->args) {
      if (exprHasLandLor(a.get())) {
        return true;
      }
    }
    return false;
  }
  if (e->kind == ExprKind::LVal) {
    auto *lv = static_cast<LValExpr *>(e);
    for (auto &ix : lv->indices) {
      if (exprHasLandLor(ix.get())) {
        return true;
      }
    }
    return false;
  }
  return false;
}

static bool blockHasLocalArrayDecl(const BlockStmt &block) {
  for (const auto &sp : block.items) {
    Stmt *s = sp.get();
    if (s->kind == StmtKind::Decl) {
      for (VarDef &vd : static_cast<DeclStmt *>(s)->defs) {
        Symbol *sym = vd.symbol;
        if (sym->isArray && !sym->isParam && sym->needsStorage) {
          return true;
        }
      }
    } else if (s->kind == StmtKind::Block) {
      if (blockHasLocalArrayDecl(*static_cast<BlockStmt *>(s))) {
        return true;
      }
    }
  }
  return false;
}

static bool blockSequential(const BlockStmt &block) {
  for (const auto &sp : block.items) {
    Stmt *s = sp.get();
    switch (s->kind) {
    case StmtKind::If:
    case StmtKind::While:
    case StmtKind::Break:
    case StmtKind::Continue:
      return false;
    case StmtKind::Block:
      if (!blockSequential(*static_cast<BlockStmt *>(s))) {
        return false;
      }
      break;
    case StmtKind::Expr: {
      auto *es = static_cast<ExprStmt *>(s);
      if (es->expr && exprHasLandLor(es->expr.get())) {
        return false;
      }
      break;
    }
    case StmtKind::Assign: {
      auto *as = static_cast<AssignStmt *>(s);
      if (exprHasLandLor(as->rhs.get())) {
        return false;
      }
      for (auto &ix : as->lhs->indices) {
        if (exprHasLandLor(ix.get())) {
          return false;
        }
      }
      break;
    }
    case StmtKind::Decl: {
      auto *d = static_cast<DeclStmt *>(s);
      for (VarDef &vd : d->defs) {
        if (vd.init) {
          if (!vd.init->isList && vd.init->expr && exprHasLandLor(vd.init->expr.get())) {
            return false;
          }
          if (vd.init->isList) {
            return false;
          }
        }
      }
      break;
    }
    case StmtKind::Return: {
      auto *r = static_cast<ReturnStmt *>(s);
      if (r->expr && exprHasLandLor(r->expr.get())) {
        return false;
      }
      break;
    }
    default:
      break;
    }
  }
  return true;
}

bool irFunctionEligible(const FuncDef &def) {
  return blockSequential(*def.body) && !blockHasLocalArrayDecl(*def.body);
}

static int strideFor(Symbol *sym, size_t index) {
  if (sym->isParamArray) {
    return product(sym->dims, index);
  }
  return index + 1 < sym->dims.size() ? product(sym->dims, index + 1) : 1;
}

struct IRBuilder {
  IRFunction *ir = nullptr;
  FuncDef *func = nullptr;

  void push(const IRInst &in) { ir->insts.push_back(in); }

  void emitCopy(int dst, int src) {
    IRInst c;
    c.op = IROp::Copy;
    c.dst = dst;
    c.u = src;
    push(c);
  }

  void emitConstI(int dst, int32_t k) {
    IRInst c;
    c.op = IROp::ConstI;
    c.dst = dst;
    c.immI = k;
    push(c);
  }

  void emitConstF(int dst, float f) {
    IRInst c;
    c.op = IROp::ConstF;
    c.dst = dst;
    c.immF = f;
    c.isFloat = true;
    push(c);
  }

  int buildExpr(Expr *e);

  int buildLValAddress(LValExpr *lv);

  void buildStmt(Stmt *s);

  void buildBlock(BlockStmt &b) {
    for (auto &it : b.items) {
      buildStmt(it.get());
    }
  }
};

int IRBuilder::buildLValAddress(LValExpr *lv) {
  Symbol *sym = lv->symbol;
  int base = ir->allocVreg();
  if (sym->isGlobal) {
    IRInst le;
    le.op = IROp::LeaGlobal;
    le.dst = base;
    le.sym = sym;
    push(le);
  } else if (sym->isParamArray) {
    IRInst le;
    le.op = IROp::LoadParamAddr;
    le.dst = base;
    le.sym = sym;
    push(le);
  } else {
    IRInst le;
    le.op = IROp::LeaLocal;
    le.dst = base;
    le.sym = sym;
    push(le);
  }
  if (lv->indices.empty()) {
    return base;
  }
  int acc = ir->allocVreg();
  emitConstI(acc, 0);
  for (size_t i = 0; i < lv->indices.size(); ++i) {
    int idx = buildExpr(lv->indices[i].get());
    int strideBytes = strideFor(sym, i) * 4;
    int prod = ir->allocVreg();
    int lg = -1;
    if (strideBytes > 0 && (strideBytes & (strideBytes - 1)) == 0) {
      lg = 0;
      for (int x = strideBytes; x > 1; x >>= 1) {
        ++lg;
      }
    }
    if (lg >= 0) {
      IRInst sh;
      sh.op = IROp::Sll;
      sh.dst = prod;
      sh.u = idx;
      sh.immI = lg;
      push(sh);
    } else {
      int c = ir->allocVreg();
      emitConstI(c, strideBytes);
      IRInst m;
      m.op = IROp::Mul;
      m.dst = prod;
      m.u = idx;
      m.v = c;
      push(m);
    }
    int nacc = ir->allocVreg();
    IRInst ad;
    ad.op = IROp::Add;
    ad.dst = nacc;
    ad.u = acc;
    ad.v = prod;
    push(ad);
    acc = nacc;
  }
  int addr = ir->allocVreg();
  IRInst ad;
  ad.op = IROp::Add;
  ad.dst = addr;
  ad.u = base;
  ad.v = acc;
  push(ad);
  return addr;
}

int IRBuilder::buildExpr(Expr *e) {
  if (e->isConst && e->kind != ExprKind::LVal) {
    if (e->constVal.type == BaseType::Float) {
      int d = ir->allocVreg();
      emitConstF(d, constAsFloat(e->constVal));
      return d;
    }
    int d = ir->allocVreg();
    emitConstI(d, constAsInt(e->constVal));
    return d;
  }
  switch (e->kind) {
  case ExprKind::Number: {
    auto *n = static_cast<NumberExpr *>(e);
    int d = ir->allocVreg();
    if (n->isFloat) {
      emitConstF(d, n->floatVal);
    } else {
      emitConstI(d, static_cast<int32_t>(n->intVal));
    }
    return d;
  }
  case ExprKind::String: {
    auto *s = static_cast<StringExpr *>(e);
    int d = ir->allocVreg();
    IRInst le;
    le.op = IROp::LeaStr;
    le.dst = d;
    le.ext = s->value;
    push(le);
    return d;
  }
  case ExprKind::LVal: {
    auto *lv = static_cast<LValExpr *>(e);
    if (lv->isConst && !lv->type.isPointer) {
      if (lv->type.base == BaseType::Float) {
        int d = ir->allocVreg();
        emitConstF(d, constAsFloat(lv->constVal));
        return d;
      }
      int d = ir->allocVreg();
      emitConstI(d, constAsInt(lv->constVal));
      return d;
    }
    if (lv->type.isPointer) {
      return buildLValAddress(lv);
    }
    if (lv->indices.empty() && !lv->symbol->isArray) {
      int d = ir->allocVreg();
      IRInst ld;
      ld.dst = d;
      ld.isFloat = lv->type.base == BaseType::Float;
      if (lv->symbol->isGlobal) {
        ld.op = IROp::LoadGlobal;
        ld.sym = lv->symbol;
      } else {
        ld.op = IROp::LoadLocal;
        ld.sym = lv->symbol;
      }
      push(ld);
      return d;
    }
    int addr = buildLValAddress(lv);
    int d = ir->allocVreg();
    IRInst lm;
    lm.op = IROp::LoadMem;
    lm.dst = d;
    lm.u = addr;
    lm.isFloat = lv->type.base == BaseType::Float;
    push(lm);
    return d;
  }
  case ExprKind::Unary: {
    auto *u = static_cast<UnaryExpr *>(e);
    int x = buildExpr(u->expr.get());
    int d = ir->allocVreg();
    if (u->op == "+") {
      emitCopy(d, x);
      return d;
    }
    if (u->op == "-") {
      if (u->type.base == BaseType::Float) {
        IRInst ng;
        ng.op = IROp::FNeg;
        ng.dst = d;
        ng.u = x;
        ng.isFloat = true;
        push(ng);
      } else {
        IRInst ng;
        ng.op = IROp::Neg;
        ng.dst = d;
        ng.u = x;
        push(ng);
      }
      return d;
    }
    if (u->op == "!") {
      IRInst z;
      z.op = IROp::Seqz;
      z.dst = d;
      z.u = x;
      push(z);
      return d;
    }
    emitCopy(d, x);
    return d;
  }
  case ExprKind::Binary: {
    auto *b = static_cast<BinaryExpr *>(e);
    bool useFloat = b->lhs->type.isFloatScalar() || b->rhs->type.isFloatScalar();
    bool cmp = b->op == "==" || b->op == "!=" || b->op == "<" || b->op == ">" ||
               b->op == "<=" || b->op == ">=";
    int L = buildExpr(b->lhs.get());
    if (useFloat && !b->lhs->type.isFloatScalar()) {
      int t = ir->allocVreg();
      IRInst cv;
      cv.op = IROp::ICvtF;
      cv.dst = t;
      cv.u = L;
      cv.isFloat = true;
      push(cv);
      L = t;
    }
    int R = buildExpr(b->rhs.get());
    if (useFloat && !b->rhs->type.isFloatScalar()) {
      int t = ir->allocVreg();
      IRInst cv;
      cv.op = IROp::ICvtF;
      cv.dst = t;
      cv.u = R;
      cv.isFloat = true;
      push(cv);
      R = t;
    }
    int d = ir->allocVreg();
    if (useFloat) {
      if (cmp) {
        IRInst fc;
        fc.op = IROp::FCmp;
        fc.dst = d;
        fc.u = L;
        fc.v = R;
        if (b->op == "==") {
          fc.immI = FCMP_EQ;
        } else if (b->op == "!=") {
          fc.immI = FCMP_NE;
        } else if (b->op == "<") {
          fc.immI = FCMP_LT;
        } else if (b->op == ">") {
          fc.immI = FCMP_GT;
        } else if (b->op == "<=") {
          fc.immI = FCMP_LE;
        } else {
          fc.immI = FCMP_GE;
        }
        push(fc);
        return d;
      }
      IRInst op;
      op.dst = d;
      op.u = L;
      op.v = R;
      op.isFloat = true;
      if (b->op == "+") {
        op.op = IROp::FAdd;
      } else if (b->op == "-") {
        op.op = IROp::FSub;
      } else if (b->op == "*") {
        op.op = IROp::FMul;
      } else if (b->op == "/") {
        op.op = IROp::FDiv;
      } else {
        op.op = IROp::FAdd;
      }
      push(op);
      return d;
    }
    if (cmp) {
      if (b->op == "==") {
        IRInst s;
        s.op = IROp::Sub;
        s.dst = d;
        s.u = L;
        s.v = R;
        push(s);
        int e0 = ir->allocVreg();
        IRInst z;
        z.op = IROp::Seqz;
        z.dst = e0;
        z.u = d;
        push(z);
        return e0;
      }
      if (b->op == "!=") {
        IRInst s;
        s.op = IROp::Sub;
        s.dst = d;
        s.u = L;
        s.v = R;
        push(s);
        int e0 = ir->allocVreg();
        IRInst z;
        z.op = IROp::Snez;
        z.dst = e0;
        z.u = d;
        push(z);
        return e0;
      }
      if (b->op == "<") {
        IRInst s;
        s.op = IROp::Slt;
        s.dst = d;
        s.u = L;
        s.v = R;
        push(s);
        return d;
      }
      if (b->op == ">") {
        IRInst s;
        s.op = IROp::Slt;
        s.dst = d;
        s.u = R;
        s.v = L;
        push(s);
        return d;
      }
      if (b->op == "<=") {
        IRInst s;
        s.op = IROp::Slt;
        s.dst = d;
        s.u = R;
        s.v = L;
        push(s);
        int e0 = ir->allocVreg();
        IRInst z;
        z.op = IROp::Seqz;
        z.dst = e0;
        z.u = d;
        push(z);
        return e0;
      }
      IRInst s;
      s.op = IROp::Slt;
      s.dst = d;
      s.u = L;
      s.v = R;
      push(s);
      int e0 = ir->allocVreg();
      IRInst z;
      z.op = IROp::Seqz;
      z.dst = e0;
      z.u = d;
      push(z);
      return e0;
    }
    IRInst op;
    op.dst = d;
    op.u = L;
    op.v = R;
    if (b->op == "+") {
      op.op = IROp::Add;
    } else if (b->op == "-") {
      op.op = IROp::Sub;
    } else if (b->op == "*") {
      op.op = IROp::Mul;
    } else if (b->op == "/") {
      op.op = IROp::Div;
    } else if (b->op == "%") {
      op.op = IROp::Rem;
    } else {
      op.op = IROp::Add;
    }
    push(op);
    return d;
  }
  case ExprKind::Call: {
    auto *c = static_cast<CallExpr *>(e);
    IRInst cl;
    cl.op = IROp::Call;
    cl.callee = c->function->asmName;
    cl.isFloat = c->type.base == BaseType::Float;
    if (c->function->injectLineArgument) {
      int lineV = ir->allocVreg();
      emitConstI(lineV, c->line);
      cl.args.push_back(lineV);
      cl.callArgPtr.push_back(0);
    }
    for (auto &a : c->args) {
      cl.args.push_back(buildExpr(a.get()));
      cl.callArgPtr.push_back(a->type.isPointer ? 1 : 0);
    }
    if (c->function->ret == BaseType::Void) {
      cl.dst = -1;
      push(cl);
      return -1;
    }
    cl.dst = ir->allocVreg();
    push(cl);
    return cl.dst;
  }
  default: {
    int d = ir->allocVreg();
    emitConstI(d, 0);
    return d;
  }
  }
}

void IRBuilder::buildStmt(Stmt *s) {
  switch (s->kind) {
  case StmtKind::Decl: {
    auto *d = static_cast<DeclStmt *>(s);
    for (VarDef &vd : d->defs) {
      Symbol *sym = vd.symbol;
      if (!sym->needsStorage || sym->isArray) {
        continue;
      }
      if (vd.init && !vd.init->isList && vd.init->expr) {
        int r = buildExpr(vd.init->expr.get());
        bool rhsF = vd.init->expr->type.isFloatScalar();
        bool lhsF = sym->base == BaseType::Float;
        if (rhsF && !lhsF) {
          int t = ir->allocVreg();
          IRInst cv;
          cv.op = IROp::FCvtI;
          cv.dst = t;
          cv.u = r;
          push(cv);
          r = t;
        } else if (!rhsF && lhsF) {
          int t = ir->allocVreg();
          IRInst cv;
          cv.op = IROp::ICvtF;
          cv.dst = t;
          cv.u = r;
          cv.isFloat = true;
          push(cv);
          r = t;
        }
        IRInst st;
        st.op = IROp::StoreLocal;
        st.sym = sym;
        st.u = r;
        st.isFloat = sym->base == BaseType::Float;
        push(st);
      } else if (!vd.init) {
        int z = ir->allocVreg();
        if (sym->base == BaseType::Float) {
          emitConstF(z, 0.0f);
        } else {
          emitConstI(z, 0);
        }
        IRInst st;
        st.op = IROp::StoreLocal;
        st.sym = sym;
        st.u = z;
        st.isFloat = sym->base == BaseType::Float;
        push(st);
      }
    }
    break;
  }
  case StmtKind::Block:
    buildBlock(*static_cast<BlockStmt *>(s));
    break;
  case StmtKind::Assign: {
    auto *a = static_cast<AssignStmt *>(s);
    int rhs = buildExpr(a->rhs.get());
    bool rhsF = a->rhs->type.isFloatScalar();
    bool lhsF = a->lhs->symbol->base == BaseType::Float;
    if (rhsF && !lhsF) {
      int t = ir->allocVreg();
      IRInst cv;
      cv.op = IROp::FCvtI;
      cv.dst = t;
      cv.u = rhs;
      push(cv);
      rhs = t;
    } else if (!rhsF && lhsF) {
      int t = ir->allocVreg();
      IRInst cv;
      cv.op = IROp::ICvtF;
      cv.dst = t;
      cv.u = rhs;
      cv.isFloat = true;
      push(cv);
      rhs = t;
    }
    Symbol *sym = a->lhs->symbol;
    if (a->lhs->indices.empty() && !sym->isArray) {
      if (sym->isGlobal) {
        IRInst st;
        st.op = IROp::StoreGlobal;
        st.sym = sym;
        st.u = rhs;
        st.isFloat = sym->base == BaseType::Float;
        push(st);
      } else {
        IRInst st;
        st.op = IROp::StoreLocal;
        st.sym = sym;
        st.u = rhs;
        st.isFloat = sym->base == BaseType::Float;
        push(st);
      }
    } else {
      int addr = buildLValAddress(a->lhs.get());
      IRInst st;
      st.op = IROp::StoreMem;
      st.u = addr;
      st.v = rhs;
      st.isFloat = sym->base == BaseType::Float;
      push(st);
    }
    break;
  }
  case StmtKind::Expr: {
    auto *es = static_cast<ExprStmt *>(s);
    if (es->expr) {
      buildExpr(es->expr.get());
    }
    break;
  }
  case StmtKind::Return: {
    auto *r = static_cast<ReturnStmt *>(s);
    IRInst ret;
    ret.op = IROp::Ret;
    if (r->expr) {
      int u = buildExpr(r->expr.get());
      bool exprF = r->expr->type.isFloatScalar();
      bool retF = func->ret == BaseType::Float;
      if (exprF && !retF) {
        int t = ir->allocVreg();
        IRInst cv;
        cv.op = IROp::FCvtI;
        cv.dst = t;
        cv.u = u;
        push(cv);
        u = t;
      } else if (!exprF && retF) {
        int t = ir->allocVreg();
        IRInst cv;
        cv.op = IROp::ICvtF;
        cv.dst = t;
        cv.u = u;
        cv.isFloat = true;
        push(cv);
        u = t;
      }
      ret.u = u;
    } else {
      ret.u = -1;
    }
    push(ret);
    break;
  }
  default:
    break;
  }
}

void irBuildFunction(FuncDef &def, const Semantic & /*semantic*/, IRFunction &out) {
  IRBuilder b;
  b.ir = &out;
  b.func = &def;
  out.name = def.name;
  out.ret = def.ret;
  out.nextVreg = 0;
  out.insts.clear();
  b.buildBlock(*def.body);
  if (out.insts.empty() || out.insts.back().op != IROp::Ret) {
    IRInst ret;
    ret.op = IROp::Ret;
    ret.u = -1;
    out.insts.push_back(ret);
  }
}
