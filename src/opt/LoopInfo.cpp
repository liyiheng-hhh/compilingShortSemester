#include "LoopPasses.h"

// compiler2026-x phase-D (trivial opt dedup)

using namespace sys;

namespace {

void liDumpIdList(std::ostream &os, const std::set<BasicBlock*> &blocks) {
  for (auto bb : blocks)
    os << bbmap[bb] << " ";
}

} // namespace

void LoopInfo::dump(std::ostream &os) {
  os << "Blocks: ";
  liDumpIdList(os, bbs);
  os << "\nPreheader: " << (preheader ? std::to_string(bbmap[preheader]) : "none") << "\n";
  os << "Header: " << bbmap[header] << "\nExits: ";
  liDumpIdList(os, exits);
  os << "\nLatches: ";
  liDumpIdList(os, latches);
  os << "\n";
}

void LoopForest::dump(std::ostream &os) {
  for (auto loop : loops) {
    loop->dump(os);
    os << "\n\n";
  }
}
