#include "Analysis.h"

using namespace sys;

void Alias::runImpl(Region *region) {
  (void)region;
}

void Alias::run() {
  for (auto &[name, fn] : getFunctionMap()) {
    (void)name;
    if (fn->getRegion())
      runImpl(fn->getRegion());
  }
}
