#include "PassManager.h"

#include <iostream>

namespace sys {

void PassManager::run() {
  for (auto &passPtr : passes) {
    passPtr->run();
    passPtr->cleanup();
  }
}

void PassManager::dumpPipeline(std::ostream &os) const {
  for (size_t i = 0; i < passes.size(); i++)
    os << i << ": " << passes[i]->name() << "\n";
}

}  // namespace sys
