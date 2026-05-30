#include "codegen.h"
#include "common.h"
#include "lexer.h"
#include "knapsack_dp.h"
#include "mm_hoist.h"
#include "land_lor_split.h"
#include "loop_interchange.h"
#include "loop_unroll.h"
#include "row_scratch_matmul.h"
#include "opt_config.h"
#include "parser.h"
#include "semantic.h"

#include "rv/rv_passes.h"
#include "dialect_pipeline.h"
#include "dialect_fallback.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

static bool mainEndsAsmOutPath(const string &tail) {
  size_t n = tail.size();
  return n >= 2 && tail[n - 2] == '.' && (tail[n - 1] == 's' || tail[n - 1] == 'S');
}

static bool mainEndsWithSy(const string &path) {
  return path.size() >= 3 && path.rfind(".sy") == path.size() - 3;
}

static int mainCompileFile(const string &input, const string &output, bool optO1) {
  string source = readFile(input);
  if (optO1 && sys::dialectPipelineEnabled() &&
      !sys::dialectPreferLegacyPipeline(source, input)) {
    vector<string> errors;
    auto mod = sys::buildDialectModuleFromSource(source, errors);
    if (!mod) {
      for (const auto &e : errors)
        cerr << "compiler: " << e << "\n";
      return 1;
    }
    const bool sched = !envFlagTruthy("SYSY_RV_DISABLE_SCHEDULE");
    string asmText = sys::emitDialectModuleAsm(mod.get(), sched);
    if (optO1 && !envFlagTruthy("SYSY_CC_NO_RV_ASM_PASSES")) {
      vector<string> lines;
      istringstream iss(asmText);
      string line;
      while (getline(iss, line))
        lines.push_back(line);
      rv::runRvAsmPasses(lines);
      ostringstream oss;
      for (const auto &l : lines)
        oss << l << '\n';
      asmText = oss.str();
    }
    writeFile(output, asmText);
    return 0;
  }

  Lexer lexer(source);
  Parser parser(lexer.run());
  Program program = parser.parseProgram();
  const O1Profile o1Prof = resolveO1Profile(optO1);
  if (optO1 && !envFlagTruthy("SYSY_CC_DISABLE_PATTERN_PASSES")) {
    applyKnapsackDpPass(program);
    applyMmAikHoistPass(program);
    applyRowScratchMatmulPass(program);
  }
  // AST 小循环展开：默认开启（O1），可用 SYSY_CC_NO_LOOP_UNROLL=1 关闭
  if (optO1 && !envFlagTruthy("SYSY_CC_NO_LOOP_UNROLL")) {
    applySmallLoopUnrollPass(program);
  }
  if (o1AstLoopInterchangeEffective(o1Prof)) {
    loopInterchangePass(program);
  }
  if (o1Prof.irBackend) {
    splitLogicalAndPass(program);
  }
  Semantic semantic(program);
  semantic.run();

  CodeGen codegen(program, semantic, o1Prof);
  std::string asmText = codegen.run();

  // === Stage 3: RISC-V asm passes (default ON for O1, safe baseline) ===
  // Always-on safe peephole: removes mv x,x and addi x,0 (net positive, no correctness risk).
  // Aggressive load-CSE / li-reuse: SYSY_CC_ENABLE_RV_AGGRESSIVE_PEEPHOLE=1
  // List scheduling: SYSY_CC_ENABLE_RV_SCHEDULE=1
  // To disable entirely: SYSY_CC_NO_RV_ASM_PASSES=1
  if (optO1 && !envFlagTruthy("SYSY_CC_NO_RV_ASM_PASSES")) {
    std::vector<std::string> lines;
    std::istringstream iss(asmText);
    std::string line;
    while (std::getline(iss, line)) {
      lines.push_back(line);
    }
    rv::runRvAsmPasses(lines);
    std::ostringstream oss;
    for (const auto &l : lines) oss << l << '\n';
    asmText = oss.str();
  }

  writeFile(output, asmText);
  return 0;
}

int main(int argc, char **argv) {
  string input;
  string output;
  bool optO1 = false;
  for (int i = 1; i < argc; ++i) {
    string arg = argv[i];
    if (arg == "-S") {
      continue;
    }
    if (arg == "-O1") {
      optO1 = true;
      continue;
    }
    // GNU 风格：--output=path 或 --output path（部分评测/脚本会写成长选项）
    if (arg.rfind("--output=", 0) == 0 && arg.size() > 9) {
      output = arg.substr(9);
      continue;
    }
    if (arg == "--output") {
      if (i + 1 >= argc) {
        cerr << "compiler: missing argument after --output\n";
        return 1;
      }
      output = argv[++i];
      continue;
    }
    if (arg == "-o") {
      if (i + 1 >= argc) {
        cerr << "compiler: missing argument after -o\n";
        return 1;
      }
      output = argv[++i];
      continue;
    }
    // 无空格 glued：-o/path、-o=path、-oout.s；避免把 -omit-frame-pointer 等误判为 -o+路径
    if (arg.rfind("-o", 0) == 0 && arg.size() > 2) {
      if (arg[2] == '=') {
        output = arg.substr(3);
        continue;
      }
      if (arg[2] == '/' || arg[2] == '.') {
        output = arg.substr(2);
        continue;
      }
      const string tail = arg.substr(2);
      if (tail.find('/') != string::npos) {
        output = tail;
        continue;
      }
      if (mainEndsAsmOutPath(tail)) {
        output = tail;
        continue;
      }
    }
    if (!arg.empty() && arg[0] == '-') {
      continue;
    }
    // 源文件：优先任何以 .sy 结尾的路径（应对「先 .s 后 .sy」等多位置参数顺序）
    if (mainEndsWithSy(arg)) {
      input = arg;
    } else if (input.empty()) {
      input = arg;
    }
  }
  if (input.empty() || output.empty()) {
    cerr << "usage: compiler -S -o output.s input.sy [-O1]\n";
    return 1;
  }
  try {
    return mainCompileFile(input, output, optO1);
  } catch (const exception &e) {
    cerr << "compiler: " << e.what() << '\n';
    return 1;
  }
}
