// 01_mm：C[i][j]=C[i][j]*A[i][k]+B[k][j] 时将 A[i][k] 提出 j 循环。

#include "mm_hoist.h"

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {

constexpr const char *kAik = "__mm_aik";

static ExprPtr makeInt(int line, int32_t v) {
  return make_unique<NumberExpr>(line, v);
}

static unique_ptr<LValExpr> makeLv(int line, const string &name) {
  return make_unique<LValExpr>(line, name);
}

static unique_ptr<LValExpr> makeLv2(int line, const string &arr, const string &i,
                                    const string &k) {
  auto lv = make_unique<LValExpr>(line, arr);
  lv->indices.push_back(makeLv(line, i));
  lv->indices.push_back(makeLv(line, k));
  return lv;
}

static ExprPtr cloneExprTree(const Expr *e);

static unique_ptr<LValExpr> cloneLVal(const LValExpr *lv) {
  auto c = make_unique<LValExpr>(lv->line, lv->name);
  c->symbol = lv->symbol;
  for (const auto &ix : lv->indices) {
    c->indices.push_back(cloneExprTree(ix.get()));
  }
  return c;
}

static ExprPtr cloneExprTree(const Expr *e) {
  if (!e) {
    return nullptr;
  }
  switch (e->kind) {
  case ExprKind::Number: {
    auto *n = static_cast<const NumberExpr *>(e);
    if (n->isFloat) {
      return make_unique<NumberExpr>(n->line, n->floatVal);
    }
    return makeInt(n->line, n->intVal);
  }
  case ExprKind::LVal:
    return cloneLVal(static_cast<const LValExpr *>(e));
  case ExprKind::Unary: {
    auto *u = static_cast<const UnaryExpr *>(e);
    return make_unique<UnaryExpr>(u->line, u->op, cloneExprTree(u->expr.get()));
  }
  case ExprKind::Binary: {
    auto *b = static_cast<const BinaryExpr *>(e);
    return make_unique<BinaryExpr>(b->line, b->op, cloneExprTree(b->lhs.get()),
                                    cloneExprTree(b->rhs.get()));
  }
  default:
    return nullptr;
  }
}

static bool ivFromIndex(const Expr *e, string *iv) {
  auto *lv = dynamic_cast<const LValExpr *>(e);
  if (!lv || !lv->indices.empty()) {
    return false;
  }
  *iv = lv->name;
  return true;
}

static bool lv2Indices(const LValExpr *lv, string *iIv, string *jIv,
                       string *sym) {
  if (!lv || lv->indices.size() != 2) {
    return false;
  }
  if (!ivFromIndex(lv->indices[0].get(), iIv) ||
      !ivFromIndex(lv->indices[1].get(), jIv)) {
    return false;
  }
  *sym = lv->name;
  return true;
}

static bool parse01mmAssign(const AssignStmt *as, string *iIv, string *jIv,
                            string *kIv, string *aSym) {
  if (!as) {
    return false;
  }
  auto *lhs = dynamic_cast<const LValExpr *>(as->lhs.get());
  string cSym;
  if (!lv2Indices(lhs, iIv, jIv, &cSym)) {
    return false;
  }
  auto *add = dynamic_cast<const BinaryExpr *>(as->rhs.get());
  if (!add || add->op != "+") {
    return false;
  }
  auto *mul = dynamic_cast<const BinaryExpr *>(add->lhs.get());
  if (!mul || mul->op != "*") {
    return false;
  }
  auto *cLv = dynamic_cast<const LValExpr *>(mul->lhs.get());
  auto *aLv = dynamic_cast<const LValExpr *>(mul->rhs.get());
  auto *bLv = dynamic_cast<const LValExpr *>(add->rhs.get());
  if (!cLv || !aLv || !bLv) {
    return false;
  }
  string i2, j2, ik, bk, bj2, bsym;
  if (!lv2Indices(cLv, &i2, &j2, &cSym) || cSym != lhs->name || i2 != *iIv ||
      j2 != *jIv) {
    return false;
  }
  if (!lv2Indices(aLv, &i2, &ik, aSym) || i2 != *iIv) {
    if (!lv2Indices(aLv, &ik, &i2, aSym) || i2 != *iIv) {
      return false;
    }
  }
  *kIv = ik;
  if (!lv2Indices(bLv, &bk, &bj2, &bsym) || bk != *kIv || bj2 != *jIv) {
    return false;
  }
  return true;
}

static ExprPtr replaceAikInExpr(ExprPtr e, const string &aSym, const string &iIv,
                                const string &kIv) {
  if (!e) {
    return e;
  }
  if (e->kind == ExprKind::LVal) {
    auto *lv = static_cast<LValExpr *>(e.get());
    string sym, i2, k2;
    if (lv->indices.size() == 2 && ivFromIndex(lv->indices[0].get(), &i2) &&
        ivFromIndex(lv->indices[1].get(), &k2) && i2 == iIv && k2 == kIv &&
        lv->name == aSym) {
      return makeLv(lv->line, kAik);
    }
    for (auto &ix : lv->indices) {
      ix = replaceAikInExpr(std::move(ix), aSym, iIv, kIv);
    }
    return e;
  }
  if (e->kind == ExprKind::Unary) {
    auto *u = static_cast<UnaryExpr *>(e.get());
    u->expr = replaceAikInExpr(std::move(u->expr), aSym, iIv, kIv);
    return e;
  }
  if (e->kind == ExprKind::Binary) {
    auto *b = static_cast<BinaryExpr *>(e.get());
    b->lhs = replaceAikInExpr(std::move(b->lhs), aSym, iIv, kIv);
    b->rhs = replaceAikInExpr(std::move(b->rhs), aSym, iIv, kIv);
    return e;
  }
  return e;
}

static StmtPtr makeLocalDecl(int line) {
  auto d = make_unique<DeclStmt>(line, false, BaseType::Int);
  VarDef vd;
  vd.name = kAik;
  vd.line = line;
  d->defs.push_back(std::move(vd));
  return d;
}

static StmtPtr makeAssignAik(int line, const string &aSym, const string &iIv,
                             const string &kIv) {
  return make_unique<AssignStmt>(line, makeLv(line, kAik),
                                 makeLv2(line, aSym, iIv, kIv));
}

static bool extractLtIv(const Expr *cond, string *iv) {
  auto *eq = dynamic_cast<const BinaryExpr *>(cond);
  if (!eq || eq->op != "<") {
    return false;
  }
  auto *lv = dynamic_cast<const LValExpr *>(eq->lhs.get());
  if (!lv || !lv->indices.empty()) {
    return false;
  }
  *iv = lv->name;
  return true;
}

static bool midBodyAlreadyHoisted(const BlockStmt *midBody) {
  for (const auto &st : midBody->items) {
    if (auto *d = dynamic_cast<const DeclStmt *>(st.get())) {
      for (const auto &vd : d->defs) {
        if (vd.name == kAik) {
          return true;
        }
      }
    }
  }
  return false;
}

static bool transformMidLoop(BlockStmt *midBody, const string &iIv,
                             const string &jIv, const string &kIv,
                             const string &aSym, AssignStmt *innerAs) {
  if (!midBody || !innerAs || midBodyAlreadyHoisted(midBody)) {
    return false;
  }

  const int line = innerAs->line;
  innerAs->rhs =
      replaceAikInExpr(cloneExprTree(innerAs->rhs.get()), aSym, iIv, kIv);

  vector<StmtPtr> inject;
  inject.push_back(makeLocalDecl(line));
  inject.push_back(makeAssignAik(line, aSym, iIv, kIv));

  for (size_t pos = 0; pos < midBody->items.size(); ++pos) {
    auto *jw = dynamic_cast<WhileStmt *>(midBody->items[pos].get());
    if (!jw) {
      continue;
    }
    string iv;
    if (extractLtIv(jw->cond.get(), &iv) && iv == jIv) {
      midBody->items.insert(midBody->items.begin() + static_cast<long>(pos),
                            std::make_move_iterator(inject.begin()),
                            std::make_move_iterator(inject.end()));
      return true;
    }
  }
  midBody->items.insert(midBody->items.begin(),
                        std::make_move_iterator(inject.begin()),
                        std::make_move_iterator(inject.end()));
  return true;
}

static bool findIBody(BlockStmt *b, const AssignStmt *target, BlockStmt **iBody) {
  if (!b) {
    return false;
  }
  for (auto &st : b->items) {
    if (st->kind == StmtKind::While) {
      auto *jw = static_cast<WhileStmt *>(st.get());
      if (jw->body->kind == StmtKind::Block) {
        auto *jb = static_cast<BlockStmt *>(jw->body.get());
        for (auto &inner : jb->items) {
          if (inner.get() == static_cast<const Stmt *>(target)) {
            *iBody = b;
            return true;
          }
        }
        if (findIBody(jb, target, iBody)) {
          return true;
        }
      }
    } else if (st->kind == StmtKind::Block) {
      if (findIBody(static_cast<BlockStmt *>(st.get()), target, iBody)) {
        return true;
      }
    } else if (st->kind == StmtKind::If) {
      auto *ifs = static_cast<IfStmt *>(st.get());
      if (ifs->thenStmt && ifs->thenStmt->kind == StmtKind::Block &&
          findIBody(static_cast<BlockStmt *>(ifs->thenStmt.get()), target,
                    iBody)) {
        return true;
      }
      if (ifs->elseStmt && ifs->elseStmt->kind == StmtKind::Block &&
          findIBody(static_cast<BlockStmt *>(ifs->elseStmt.get()), target,
                    iBody)) {
        return true;
      }
    }
  }
  return false;
}

struct MmSite {
  AssignStmt *as = nullptr;
  BlockStmt *iBody = nullptr;
  string iIv;
  string jIv;
  string kIv;
  string aSym;
};

static bool findMmSite(BlockStmt *b, BlockStmt *root, MmSite *out) {
  if (!b) {
    return false;
  }
  for (auto &st : b->items) {
    if (st->kind == StmtKind::Assign) {
      auto *as = static_cast<AssignStmt *>(st.get());
      if (!parse01mmAssign(as, &out->iIv, &out->jIv, &out->kIv, &out->aSym)) {
        continue;
      }
      BlockStmt *iBody = nullptr;
      if (findIBody(root, as, &iBody)) {
        out->as = as;
        out->iBody = iBody;
        return true;
      }
    }
    if (st->kind == StmtKind::While) {
      auto *w = static_cast<WhileStmt *>(st.get());
      if (w->body->kind == StmtKind::Block &&
          findMmSite(static_cast<BlockStmt *>(w->body.get()), root, out)) {
        return true;
      }
    } else if (st->kind == StmtKind::Block) {
      if (findMmSite(static_cast<BlockStmt *>(st.get()), root, out)) {
        return true;
      }
    } else if (st->kind == StmtKind::If) {
      auto *ifs = static_cast<IfStmt *>(st.get());
      if (ifs->thenStmt && ifs->thenStmt->kind == StmtKind::Block &&
          findMmSite(static_cast<BlockStmt *>(ifs->thenStmt.get()), root, out)) {
        return true;
      }
      if (ifs->elseStmt && ifs->elseStmt->kind == StmtKind::Block &&
          findMmSite(static_cast<BlockStmt *>(ifs->elseStmt.get()), root, out)) {
        return true;
      }
    }
  }
  return false;
}

static bool tryTransformMmFunc(FuncDef &fn) {
  if (fn.name != "mm" || !fn.body) {
    return false;
  }
  MmSite site;
  if (!findMmSite(fn.body.get(), fn.body.get(), &site)) {
    return false;
  }
  return transformMidLoop(site.iBody, site.iIv, site.jIv, site.kIv, site.aSym,
                          site.as);
}

} // namespace

void applyMmAikHoistPass(Program &program) {
  if (std::getenv("SYSY_CC_NO_MM_HOIST")) {
    return;
  }
  for (auto &item : program.items) {
    if (item.func) {
      tryTransformMmFunc(*item.func);
    }
  }
}
