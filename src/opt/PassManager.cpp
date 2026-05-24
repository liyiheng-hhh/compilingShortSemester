#include "PassManager.h"

#include "Passes.h"
#include "../common.h"
#include "../utils/Exec.h"

#include <cctype>
#include <cstdlib>
#include <iostream>

namespace sys {

namespace {

std::string readCompareFile(const std::string &path) {
  std::ifstream ifs(path);
  if (!ifs)
    return {};
  std::stringstream ss;
  ss << ifs.rdbuf();
  return ss.str();
}

std::string envOrEmpty(const char *name) {
  if (const char *v = std::getenv(name))
    return v;
  return {};
}

bool pmSparseComparePass(const std::string &passName) {
  static const char *kSparse[] = {
    "flatten-cfg", "gvn", "dce", "inline", "localize", "globalize",
    "mem2reg", "alias", "regular-fold", "dae", "dse", "dle",
    "canonicalize-loop", "loop-rotate", "licm", "const-loop-unroll", "scev",
    "aggressive-dce", "simplify-cfg", "select", "gcm",
    "late-inline", "inline-store", "synth-const-array",
    "remove-empty-loop", "inst-schedule",
  };
  for (const char *name : kSparse) {
    if (passName == name)
      return true;
  }
  return false;
}

bool pmShouldComparePass(const PassDebugOptions &debug, const std::string &passName) {
  if (!debug.compareOnlyPass.empty())
    return passName == debug.compareOnlyPass;
  if (debug.compareSparse && !pmSparseComparePass(passName))
    return false;
  return true;
}

}  // namespace

PassDebugOptions loadPassDebugOptionsFromEnv() {
  PassDebugOptions opts;
  opts.verify = envFlagTruthy("SYSY_CC_VERIFY_PASSES");
  opts.stats = envFlagTruthy("SYSY_CC_PASS_STATS");
  opts.verbose = envFlagTruthy("SYSY_CC_VERBOSE_PASSES");
  opts.printAfter = envOrEmpty("SYSY_CC_PRINT_AFTER");
  opts.printBefore = envOrEmpty("SYSY_CC_PRINT_BEFORE");
  opts.compareWith = envOrEmpty("SYSY_CC_COMPARE_WITH");
  opts.simulateInput = envOrEmpty("SYSY_CC_SIMULATE_INPUT");
  opts.stopAfterPass = envOrEmpty("SYSY_CC_STOP_AFTER_PASS");
  opts.compareOnlyPass = envOrEmpty("SYSY_CC_COMPARE_ONLY_PASS");
  opts.compareSparse = envFlagTruthy("SYSY_CC_COMPARE_SPARSE");
  if (opts.compareWith.empty())
    opts.compareWith = envOrEmpty("SYSY_CC_PASS_COMPARE_WITH");
  if (opts.simulateInput.empty())
    opts.simulateInput = envOrEmpty("SYSY_CC_PASS_SIMULATE_INPUT");
  return opts;
}

PassManager::PassManager(ModuleOp *module, PassDebugOptions debug)
    : module(module), debug(std::move(debug)) {
  if (this->debug.compareWith.empty())
    return;

  std::string truth = readCompareFile(this->debug.compareWith);
  while (!truth.empty() && std::isspace(static_cast<unsigned char>(truth.back())))
    truth.pop_back();

  auto pos = truth.rfind('\n');
  if (pos == std::string::npos) {
    expectedExit = std::stoi(truth);
    truth.clear();
  } else {
    expectedExit = std::stoi(truth.substr(pos + 1));
    truth.erase(pos);
  }

  while (!truth.empty() && std::isspace(static_cast<unsigned char>(truth.back())))
    truth.pop_back();
  expectedOutput = std::move(truth);

  if (!this->debug.simulateInput.empty())
    simulateInput = readCompareFile(this->debug.simulateInput);
}

void PassManager::run() {
  pastFlatten = false;
  pastMem2Reg = false;

  if (envFlagTruthy("SYSY_CC_DUMP_PIPELINE")) {
    dumpPipeline(std::cerr);
    if (envFlagTruthy("SYSY_CC_DUMP_PIPELINE_ONLY"))
      return;
  }

  for (auto &passPtr : passes) {
    const std::string &passName = passPtr->name();

    if (passName == "flatten-cfg")
      pastFlatten = true;
    if (passName == "mem2reg")
      pastMem2Reg = true;

    if (!debug.printBefore.empty() && passName == debug.printBefore) {
      std::cerr << "===== Before " << passName << " =====\n\n";
      module->dump(std::cerr);
      std::cerr << "\n\n";
    }

    passPtr->run();
    passPtr->cleanup();

    if (debug.verbose || (!debug.printAfter.empty() && passName == debug.printAfter)) {
      std::cerr << "===== After " << passName << " =====\n\n";
      module->dump(std::cerr);
      std::cerr << "\n\n";
    }

    if (debug.verify && pastMem2Reg) {
      std::cerr << "verify " << passName << "... ";
      Verify(module).run();
      std::cerr << "ok\n";
    }

    if (!debug.compareWith.empty() && pastFlatten && pmShouldComparePass(debug, passName)) {
      std::cerr << "compare " << passName << "... " << std::flush;
      exec::Interpreter itp(module);
      std::stringstream input(simulateInput);
      itp.run(input);
      std::string actual = itp.out();
      while (!actual.empty() && std::isspace(static_cast<unsigned char>(actual.back())))
        actual.pop_back();

      if (actual != expectedOutput) {
        std::cerr << "MISMATCH\n";
        std::cerr << "expected:\n" << expectedOutput << "\n";
        std::cerr << "actual:\n" << actual << "\n";
        std::cerr << "first mismatch after pass: " << passName << "\n";
        std::exit(1);
      }
      if (expectedExit != itp.exitcode()) {
        std::cerr << "EXIT MISMATCH got " << itp.exitcode()
                  << " expected " << expectedExit << "\n";
        std::cerr << "first mismatch after pass: " << passName << "\n";
        std::exit(1);
      }
      std::cerr << "ok\n";
    }

    if (!debug.stopAfterPass.empty() && passName == debug.stopAfterPass)
      break;

    if (debug.stats) {
      std::cerr << passName << ":\n";
      auto stats = passPtr->stats();
      if (stats.empty())
        std::cerr << "  <no stats>\n";
      for (const auto &[k, v] : stats)
        std::cerr << "  " << k << " : " << v << "\n";
    }
  }
}

void PassManager::dumpPipeline(std::ostream &os) const {
  for (size_t i = 0; i < passes.size(); i++)
    os << i << ": " << passes[i]->name() << "\n";
}

}  // namespace sys
