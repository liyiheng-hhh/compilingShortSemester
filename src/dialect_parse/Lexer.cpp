#include "Lexer.h"

// compiler2026-x phase-3 (dialect lexer driver)

using namespace sys;

bool Lexer::hasMore() const {
  return loc < input.size();
}
