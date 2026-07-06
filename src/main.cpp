#include "codegen.h"
#include "common.h"
#include "lexer.h"
#include "opt_config.h"
#include "parser.h"
#include "semantic.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

static string readStdin() {
  ostringstream oss;
  oss << cin.rdbuf();
  return oss.str();
}

static int compileSource(const string &source, bool opt) {
  Lexer lexer(source);
  Parser parser(lexer.run());
  Program program = parser.parseProgram();
  Semantic semantic(program);
  semantic.run();

  O1Profile profile = resolveO1Profile(opt);
  CodeGen codegen(program, semantic, profile);
  cout << codegen.run();
  return 0;
}

int main(int argc, char **argv) {
  bool opt = false;
  for (int i = 1; i < argc; ++i) {
    if (string(argv[i]) == "-opt") {
      opt = true;
    }
  }
  try {
    return compileSource(readStdin(), opt);
  } catch (const exception &e) {
    cerr << "compiler: " << e.what() << '\n';
    return 1;
  }
}
