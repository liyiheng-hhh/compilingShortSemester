#include "rv_passes.h"

#include "rv_asm.h"

#include "../common.h"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace rv {

namespace {

bool enabled() {
  return !envFlagTruthy("SYSY_CC_NO_RV_SCHEDULE");
}

// List-schedule a contiguous region [start, end) of movable instructions.
static void scheduleRegion(const std::vector<std::string> &lines, size_t start, size_t end,
                           std::vector<std::string> &out, int &reordered) {
  const size_t n = end - start;
  if (n <= 2) {
    for (size_t i = start; i < end; ++i) out.push_back(lines[i]);
    return;
  }

  std::vector<AsmInst> insts;
  insts.reserve(n);
  for (size_t i = start; i < end; ++i) {
    AsmInst in;
    if (!parseAsmLine(lines[i], in)) {
      for (size_t j = start; j < end; ++j) out.push_back(lines[j]);
      return;
    }
    insts.push_back(in);
  }

  // Build dependency: edge i -> j if j uses/def conflicts or memory dep
  std::vector<std::vector<int>> succ(n);
  std::vector<int> indeg(n, 0);

  auto regConflict = [](const AsmInst &a, const AsmInst &b) {
    for (const auto &d : a.defs) {
      for (const auto &u : b.uses) {
        if (d == u) return true;
      }
    }
    for (const auto &d : b.defs) {
      for (const auto &u : a.uses) {
        if (d == u) return true;
      }
    }
    for (const auto &d : a.defs) {
      for (const auto &d2 : b.defs) {
        if (d == d2) return true;
      }
    }
    return false;
  };

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = i + 1; j < n; ++j) {
      bool dep = regConflict(insts[i], insts[j]);
      if (insts[i].isStore && insts[j].isLoad) dep = true;
      if (insts[i].isLoad && insts[j].isStore) dep = true;
      if (insts[i].isStore && insts[j].isStore) dep = true;
      if (!insts[i].memAddr.empty() && insts[i].memAddr == insts[j].memAddr &&
          (insts[i].isLoad || insts[i].isStore) &&
          (insts[j].isLoad || insts[j].isStore))
        dep = true;
      if (dep) {
        succ[i].push_back(static_cast<int>(j));
        ++indeg[j];
      }
    }
  }

  // Ready set: max latency first (schedule slow ops early)
  auto readyCmp = [&](int a, int b) {
    return insts[static_cast<size_t>(a)].latency < insts[static_cast<size_t>(b)].latency;
  };
  std::priority_queue<int, std::vector<int>, decltype(readyCmp)> ready(readyCmp);

  for (size_t i = 0; i < n; ++i) {
    if (indeg[i] == 0) ready.push(static_cast<int>(i));
  }

  std::vector<int> order;
  order.reserve(n);
  std::vector<int> indegWork = indeg;

  while (!ready.empty()) {
    int u = ready.top();
    ready.pop();
    order.push_back(u);
    for (int v : succ[static_cast<size_t>(u)]) {
      if (--indegWork[static_cast<size_t>(v)] == 0) ready.push(v);
    }
  }

  if (order.size() != n) {
    for (size_t i = start; i < end; ++i) out.push_back(lines[i]);
    return;
  }

  bool changed = false;
  for (size_t i = 0; i < n; ++i) {
    if (order[i] != static_cast<int>(i)) changed = true;
  }
  if (changed) reordered += static_cast<int>(n);

  for (int idx : order) out.push_back(insts[static_cast<size_t>(idx)].raw);
}

static void emitBlockWithScheduling(const std::vector<std::string> &lines, size_t start,
                                    size_t end, std::vector<std::string> &out, int &reordered) {
  size_t i = start;
  while (i < end) {
    AsmInst in;
    if (!parseAsmLine(lines[i], in) || in.pinned || in.isStore) {
      out.push_back(lines[i++]);
      continue;
    }
    size_t regionStart = i;
    while (i < end) {
      AsmInst cur;
      if (!parseAsmLine(lines[i], cur) || cur.pinned || cur.isStore) break;
      ++i;
    }
    scheduleRegion(lines, regionStart, i, out, reordered);
  }
}

}  // namespace

void rvSchedule(std::vector<std::string> &lines, PassStats *stats) {
  if (!enabled()) return;

  std::vector<std::pair<size_t, size_t>> blocks;
  splitBasicBlocks(lines, blocks);

  std::vector<std::string> out;
  out.reserve(lines.size());
  int reordered = 0;

  size_t cursor = 0;
  for (const auto &bb : blocks) {
    while (cursor < bb.first) {
      out.push_back(lines[cursor++]);
    }
    emitBlockWithScheduling(lines, bb.first, bb.second, out, reordered);
    cursor = bb.second;
  }
  while (cursor < lines.size()) out.push_back(lines[cursor++]);

  if (stats) stats->scheduled = reordered;
  lines.swap(out);
}

}  // namespace rv
