#include "codegen.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"

#include <iostream>
#include <string>

using namespace std;

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
    // 支持 -o path 与常见无空格形式 -o/path（评测机多用绝对路径，常为 -o/tmp/...）
    if (arg.rfind("-o", 0) == 0) {
      if (arg.size() == 2) {
        if (i + 1 >= argc) {
          cerr << "compiler: missing argument after -o\n";
          return 1;
        }
        output = argv[++i];
      } else {
        output = arg.substr(2);
      }
      continue;
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
