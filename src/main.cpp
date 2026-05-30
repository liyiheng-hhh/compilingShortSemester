#include "codegen.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"

#include "rv/rv_passes.h"
#include "dialect_pipeline.h"

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

static int mainCompileDialectO1(const string &source, const string &output) {
  if (!sys::dialectPipelineEnabled()) {
    cerr << "compiler: -O1 requires dialect pipeline (unset SYSY_CC_NO_DIALECT_PIPELINE)\n";
    return 1;
  }

  vector<string> errors;
  auto mod = sys::buildDialectModuleFromSource(source, errors);
  if (!mod) {
    for (const auto &e : errors)
      cerr << "compiler: " << e << "\n";
    return 1;
  }

  const bool sched = !envFlagTruthy("SYSY_RV_DISABLE_SCHEDULE");
  string asmText = sys::emitDialectModuleAsm(mod.get(), sched);
  if (!envFlagTruthy("SYSY_CC_NO_RV_ASM_PASSES")) {
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

static int mainCompileLegacyO0(const string &source, const string &output) {
  Lexer lexer(source);
  Parser parser(lexer.run());
  Program program = parser.parseProgram();
  Semantic semantic(program);
  semantic.run();

  CodeGen codegen(program, semantic, O1Profile{});
  writeFile(output, codegen.run());
  return 0;
}

static int mainCompileFile(const string &input, const string &output, bool optO1) {
  string source = readFile(input);
  if (optO1)
    return mainCompileDialectO1(source, output);
  return mainCompileLegacyO0(source, output);
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
