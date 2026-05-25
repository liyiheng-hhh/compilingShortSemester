#ifndef MATCHER_H
#define MATCHER_H

#include "../codegen/CodeGen.h"

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace sys {

struct Expr {
  int id;
  Expr(int id): id(id) {}
  virtual ~Expr() {}
};

struct Atom : Expr {
  template<class T>
  static bool classof(T *t) { return t->id == 1; }

  std::string_view value;
  Atom(std::string_view value): Expr(1), value(value) {}
};

struct List : Expr {
  template<class T>
  static bool classof(T *t) { return t->id == 2; }

  std::vector<Expr*> elements;
  List(): Expr(2) {}
};


class Rule {
  std::map<std::string_view, Op*> binding;
  std::string_view text;
  std::vector<std::string> externalStrs;
  Expr *pattern;
  Builder builder;
  int loc = 0;
  bool failed = false;

  std::string_view mtNextToken();
  Expr *mtParse();

  bool mtMatchExpr(Expr *expr, Op *op);
  int mtEvalExpr(Expr *expr);
  float mtEvalFExpr(Expr *expr);
  Op *mtBuildExpr(Expr *expr);

  void mtDump(Expr *expr, std::ostream &os);
  void mtRelease(Expr *expr);
public:
  using Binding = std::map<std::string, Op*>;

  Rule(const Rule &other) = delete;

  Rule(const char *text);
  ~Rule();
  bool rewrite(Op *op);
  bool match(Op *op, const Binding &external = {});
  Op *extract(const std::string &name);

  void dump(std::ostream &os = std::cerr);
};

using MtRule = Rule;

}

#endif
