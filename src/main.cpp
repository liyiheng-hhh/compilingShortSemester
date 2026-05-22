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

#include "hir/HIRBuilder.h"
#include "hir/HIRLowering.h"
#include "hir/HIRRowScratchMatmul.h"
#include "hir/HIRLoopTransform.h"

#include <iostream>
#include <string>

using namespace std;

static bool endsAsmOutPath(const string &tail) {
  size_t n = tail.size();
  return n >= 2 && tail[n - 2] == '.' && (tail[n - 1] == 's' || tail[n - 1] == 'S');
}

static bool endsWithSy(const string &path) {
  return path.size() >= 3 && path.rfind(".sy") == path.size() - 3;
}

static int compileFile(const string &input, const string &output, bool optO1) {
  string source = readFile(input);
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

  // === HIR stage (double-layer IR) ===
  // Controlled by SYSY_CC_ENABLE_HIR=1 (default off to avoid regression)
  if (envFlagTruthy("SYSY_CC_ENABLE_HIR")) {
    auto hirModule = sys::hir::buildHIR(program);

    // Run HIR-level optimizations (e.g. RowScratchMatmul on structured While/For)
    if (optO1) {
      sys::hir::applyRowScratchMatmulOnHIR(*hirModule);
      // HIR-level loop transformations (Stage 2)
      sys::hir::tryLoopInterchangeOnHIR(*hirModule);
      sys::hir::tryLoopTilingOnHIR(*hirModule);
      sys::hir::tryLoopUnrollOnHIR(*hirModule);
    }

    // Real lowering to legacy IRInst (expression + control flow)
    // We perform lowering for future use / debugging, but currently do NOT
    // replace the IRFunctions used by CodeGen, because the produced IR is
    // still not robust enough for the full -O1 pipeline (Mem2Reg, regalloc, etc.).
    // This prevents crashes while we continue to improve lowering.
    std::vector<IRFunction> hirIRFunctions;
    sys::hir::lowerToLegacyIR(*hirModule, hirIRFunctions);

    // NOTE: We intentionally do NOT call
    //   codegen.setHirLoweredIRFunctions(hirIRFunctions)
    // until lowering is verified to be stable on many cases.
  }
  std::string asmText = codegen.run();

  // === Automatic backend peephole cleanup (Stage 1) ===
  // Runs on the final assembly when O1 is enabled.
  if (optO1) {
    std::vector<std::string> lines;
    std::istringstream iss(asmText);
    std::string line;
    while (std::getline(iss, line)) {
      lines.push_back(line);
    }

    // Conservative peephole (Stage 1 safe cleanup)
    // Only removes obvious self-moves (mv x, x). This is proven safe.
    auto clean = [](std::vector<std::string> &lns) {
      std::vector<std::string> out;
      out.reserve(lns.size());
      for (const auto &l : lns) {
        if (l.find("\tmv\t") != std::string::npos) {
          size_t p = l.find("\tmv\t");
          size_t comma = l.find(',', p);
          if (comma != std::string::npos) {
            std::string dst = l.substr(p + 4, comma - (p + 4));
            std::string src = l.substr(comma + 1);
            dst.erase(0, dst.find_first_not_of(" \t"));
            src.erase(0, src.find_first_not_of(" \t"));
            size_t end = src.find_last_not_of(" \t");
            if (end != std::string::npos) src = src.substr(0, end + 1);
            if (dst == src) continue;
          }
        }
        out.push_back(l);
      }
      lns.swap(out);
    };
    clean(lines);

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
      if (endsAsmOutPath(tail)) {
        output = tail;
        continue;
      }
    }
    if (!arg.empty() && arg[0] == '-') {
      continue;
    }
    // 源文件：优先任何以 .sy 结尾的路径（应对「先 .s 后 .sy」等多位置参数顺序）
    if (endsWithSy(arg)) {
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
    return compileFile(input, output, optO1);
  } catch (const exception &e) {
    cerr << "compiler: " << e.what() << '\n';
    return 1;
  }
}
