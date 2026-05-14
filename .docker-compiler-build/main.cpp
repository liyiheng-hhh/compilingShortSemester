#include "codegen.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"

#include <iostream>
#include <string>

using namespace std;

static bool endsAsmOutPath(const string &tail) {
  size_t n = tail.size();
  return n >= 2 && tail[n - 2] == '.' && (tail[n - 1] == 's' || tail[n - 1] == 'S');
}

static int compileFile(const string &input, const string &output, bool optO1) {
  string source = readFile(input);
  Lexer lexer(source);
  Parser parser(lexer.run());
  Program program = parser.parseProgram();
  Semantic semantic(program);
  semantic.run();
  CodeGen codegen(program, semantic, optO1);
  writeFile(output, codegen.run());
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
    if (arg.rfind("--output=", 0) == 0 && arg.size() > 9) {
      output = arg.substr(9);
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
    input = arg;
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
