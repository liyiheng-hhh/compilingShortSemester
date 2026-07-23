#include "ast.hpp"
#include "codegen.hpp"
#include "common.hpp"
#include "ctfe.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    try {
        const toyc::Options options = toyc::parseOptions(argc, argv);
        const std::string source = toyc::readStdin();
        toyc::Lexer lexer(source);
        toyc::Parser parser(lexer.lex());
        toyc::CompUnit unit = parser.parseCompUnit();
        if (options.optimize && options.enableCtfe) {
            if (const auto result = toyc::evaluateWholeProgram(unit)) {
                std::cout << toyc::emitConstantProgram(*result);
                return 0;
            }
        }
        std::cout << "    # toyc-ctfe: miss\n";
        std::cout << toyc::generateRiscV(unit, options);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "toyc: " << error.what() << '\n';
        return 1;
    }
}
