// compiler2026-x phase-C (header layout)
#ifndef BV_MATCHER_H
#define BV_MATCHER_H

#include <vector>
#include "BvExpr.h"
#include "../Matcher.h"


namespace smt {

using sys::Expr;
using sys::Atom;
using sys::List;

class BvRule {
  std::map<std::string_view, BvExpr*> binding;
  std::string_view text;
  std::vector<std::string> externalStrs;
  Expr *pattern;
  int loc = 0;
  bool failed = false;

  std::string_view bvmNextToken();
  Expr *bvmParse();

  bool bvmMatchExpr(Expr *expr, BvExpr *bvexpr);
  int bvmEvalExpr(Expr *expr);
  float bvmEvalFExpr(Expr *expr);
  BvExpr *bvmBuildExpr(Expr *expr);

  void bvmDump(Expr *expr, std::ostream &os);
  void bvmRelease(Expr *expr);
  BvExpr *bvmRewriteRoot(BvExpr *expr);
public:
  using Binding = std::map<std::string, BvExpr*>;
  BvExprContext *ctx = nullptr;

  BvRule(const BvRule &other) = delete;

  BvRule(const char *text);
  ~BvRule();
  BvExpr *rewrite(BvExpr *expr);
  BvExpr *extract(const std::string &name);

  void dump(std::ostream &os = std::cerr);
};

using BvmRule = BvRule;
}

#endif
