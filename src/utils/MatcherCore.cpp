// compiler2026-x phase-2 (matcher split)

#include "Matcher.h"
#include "../codegen/Attrs.h"
#include <cstdlib>
#include <iostream>

using namespace sys;

Rule::Rule(const char *text): text(text) {
  pattern = parse();
}

Rule::~Rule() {
  release(pattern);
}

void Rule::release(Expr *expr) {
  if (auto list = dyn_cast<List>(expr)) {
    for (auto elem : list->elements)
      release(elem);
  }
  delete expr;
}

void Rule::dump(std::ostream &os) {
  dump(pattern, os);
  os << "\n===== binding starts =====\n";
  for (auto [k, v] : binding) {
    os << k << " = ";
    v->dump(os);
  }
  os << "\n===== binding ends =====\n";
}

void Rule::dump(Expr *expr, std::ostream &os) {
  if (auto atom = dyn_cast<Atom>(expr)) {
    os << atom->value;
    return;
  }
  auto list = dyn_cast<List>(expr);
  os << "(";
  dump(list->elements[0], os);
  for (size_t i = 1; i < list->elements.size(); i++) {
    os << " ";
    dump(list->elements[i], os);
  }
  os << ")";
}

std::string_view Rule::nextToken() {
  while (loc < text.size() && std::isspace(text[loc]))
    ++loc;
  
  if (loc >= text.size())
    return "";

  if (text[loc] == '(' || text[loc] == ')')
    return text.substr(loc++, 1);

  int start = loc;
  while (loc < text.size() && !std::isspace(text[loc]) && text[loc] != '(' && text[loc] != ')')
    ++loc;

  return text.substr(start, loc - start);
}

Expr *Rule::parse() {
  std::string_view tok = nextToken();

  if (tok == "(") {
    auto list = new List;
    for (;;) {
      std::string_view peek = text.substr(loc, 1);
      if (peek == ")") {
        nextToken();
        break;
      }
      list->elements.push_back(parse());
    }
    return list;
  }

  return new Atom(tok);
}
