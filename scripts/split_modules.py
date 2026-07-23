#!/usr/bin/env python3
"""Split src/main.cpp into readable modules without changing behavior."""
from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"
MAIN = SRC / "main.cpp"
lines = MAIN.read_text(encoding="utf-8").splitlines(True)


def chunk(start: int, end: int) -> str:
    """1-based inclusive line range."""
    return "".join(lines[start - 1 : end])


COMMON_INCLUDES = """#pragma once

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

"""

# --- common.hpp ---
common = COMMON_INCLUDES + """namespace toyc {

struct Options {
    bool optimize = false;
    bool enableCtfe = true;
};

inline Options parseOptions(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "-opt") {
            options.optimize = true;
        } else if (arg == "-fno-ctfe") {
            options.enableCtfe = false;
        }
    }
    return options;
}

inline std::string readStdin() {
    return {std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>()};
}

struct SourceLocation {
    int line = 1;
    int column = 1;
    std::size_t offset = 0;
};

[[noreturn]] inline void fail(const SourceLocation& loc, const std::string& message) {
    std::ostringstream out;
    out << loc.line << ':' << loc.column << ": " << message;
    throw std::runtime_error(out.str());
}

inline std::int32_t toInt32(std::int64_t value) {
    return static_cast<std::int32_t>(value);
}

inline std::int64_t printableI32(std::int32_t value) {
    return static_cast<std::int64_t>(value);
}

inline bool fitsI12(int value) {
    return value >= -2048 && value <= 2047;
}

inline std::optional<int> powerOfTwoShift(std::int64_t value) {
    if (value == 0) {
        return std::nullopt;
    }
    std::uint64_t magnitude = value < 0 ? static_cast<std::uint64_t>(-value)
                                        : static_cast<std::uint64_t>(value);
    if ((magnitude & (magnitude - 1)) != 0 || magnitude > (1ull << 31)) {
        return std::nullopt;
    }
    int shift = 0;
    while (magnitude > 1) {
        magnitude >>= 1;
        ++shift;
    }
    return shift;
}

inline int alignTo16(int value) {
    return (value + 15) / 16 * 16;
}

inline std::string functionLabel(const std::string& name) {
    if (name == "main") {
        return "main";
    }
    return "fn_" + name;
}

inline std::string globalLabel(const std::string& name) {
    return "g_" + name;
}

} // namespace toyc
"""

# --- token + lexer (60-307) was in anonymous ns; rewrap ---
# Strip is not needed if we take raw and wrap carefully.

lexer_body = chunk(60, 307)  # TokenKind through Lexer
# indent stays; was already at namespace level inside anonymous ns

lexer_hpp = COMMON_INCLUDES + '#include "common.hpp"\n\nnamespace toyc {\n\n' + lexer_body + "\n} // namespace toyc\n"

# --- ast ---
ast_body = chunk(309, 407)
ast_hpp = '#pragma once\n\n#include "common.hpp"\n#include <memory>\n#include <string>\n#include <vector>\n\nnamespace toyc {\n\n' + ast_body + "\n} // namespace toyc\n"

# --- parser ---
parser_body = chunk(409, 734)
parser_hpp = (
    '#pragma once\n\n#include "ast.hpp"\n#include "lexer.hpp"\n\nnamespace toyc {\n\n'
    + parser_body
    + "\n} // namespace toyc\n"
)

# --- ctfe ---
ctfe_internal = chunk(926, 2109)  # CtfeAbort through emitConstantProgram
ctfe_hpp = """#pragma once

#include \"ast.hpp\"
#include \"common.hpp\"

#include <cstdint>
#include <optional>
#include <string>

namespace toyc {

// Evaluate a closed ToyC program at compile time when possible.
std::optional<std::int32_t> evaluateWholeProgram(CompUnit& unit);
std::string emitConstantProgram(std::int32_t value);

} // namespace toyc
"""

ctfe_cpp = (
    '#include "ctfe.hpp"\n\n'
    "#include <chrono>\n"
    "#include <cstdlib>\n"
    "#include <deque>\n"
    "#include <limits>\n"
    "#include <sstream>\n"
    "#include <unordered_map>\n"
    "#include <unordered_set>\n"
    "#include <utility>\n"
    "#include <vector>\n\n"
    "namespace toyc {\n"
    "namespace {\n\n"
    "using toyc::toInt32;\n"
    "using toyc::printableI32;\n"
    "using toyc::CompUnit;\n"
    "using toyc::Decl;\n"
    "using toyc::Expr;\n"
    "using toyc::ExprKind;\n"
    "using toyc::Function;\n"
    "using toyc::Stmt;\n"
    "using toyc::StmtKind;\n"
    "using toyc::ValueType;\n"
    "using toyc::CtfeOp;\n\n"
    + ctfe_internal
    + "\n\n} // namespace\n\n"
    "std::optional<std::int32_t> evaluateWholeProgram(CompUnit& unit) {\n"
    "    WholeProgramEvaluator evaluator(unit);\n"
    "    return evaluator.evaluate();\n"
    "}\n\n"
    "std::string emitConstantProgram(std::int32_t value) {\n"
    "    return ::toyc::{ // placeholder\n"
)

# Fix ctfe - emitConstantProgram is already in ctfe_internal. So wrapper should just call it,
# but emitConstantProgram is inside anonymous namespace. Export it:

ctfe_cpp = (
    '#include "ctfe.hpp"\n\n'
    "#include <chrono>\n"
    "#include <cstdlib>\n"
    "#include <deque>\n"
    "#include <limits>\n"
    "#include <sstream>\n"
    "#include <unordered_map>\n"
    "#include <utility>\n"
    "#include <vector>\n\n"
    "namespace toyc {\n"
    "namespace {\n\n"
    + ctfe_internal
    + "\n\n} // namespace\n\n"
    "std::optional<std::int32_t> evaluateWholeProgram(CompUnit& unit) {\n"
    "    WholeProgramEvaluator evaluator(unit);\n"
    "    return evaluator.evaluate();\n"
    "}\n\n"
    # emitConstantProgram defined in anonymous ns - need to re-export
    # Actually emitConstantProgram is in the chunk inside anonymous ns.
    # Provide public wrapper that duplicates call - rename internal.
)

# Better approach: keep emitConstantProgram in anonymous ns as emitConstantProgramImpl
# Simplest: don't nest emitConstantProgram in anonymous - put wrappers after closing anon ns
# by splitting chunk: 926-2099 evaluator only, then 2101-2109 emitConstantProgram in toyc ns

ctfe_eval = chunk(926, 2099)
ctfe_emit = chunk(2101, 2109)

ctfe_cpp = (
    '#include "ctfe.hpp"\n\n'
    "#include <chrono>\n"
    "#include <cstdlib>\n"
    "#include <deque>\n"
    "#include <limits>\n"
    "#include <sstream>\n"
    "#include <unordered_map>\n"
    "#include <utility>\n"
    "#include <vector>\n\n"
    "namespace toyc {\n"
    "namespace {\n\n"
    + ctfe_eval
    + "\n\n} // namespace\n\n"
    "std::optional<std::int32_t> evaluateWholeProgram(CompUnit& unit) {\n"
    "    WholeProgramEvaluator evaluator(unit);\n"
    "    return evaluator.evaluate();\n"
    "}\n\n"
    + ctfe_emit
    + "\n\n} // namespace toyc\n"
)

# --- codegen ---
codegen_helpers = chunk(736, 924)  # Symbol through helpers before CtfeAbort
# But helpers include simpleInlineReturnExpr etc. that go until line 925.
# Line 736-925 approximately - CtfeAbort is 926, so 736-925
codegen_helpers = chunk(736, 925)
codegen_class = chunk(2112, 4791)

codegen_hpp = """#pragma once

#include \"ast.hpp\"
#include \"common.hpp\"

#include <string>

namespace toyc {

std::string generateRiscV(const CompUnit& unit, Options options);

} // namespace toyc
"""

codegen_cpp = (
    '#include "codegen.hpp"\n\n'
    "#include <initializer_list>\n"
    "#include <sstream>\n"
    "#include <unordered_map>\n"
    "#include <unordered_set>\n"
    "#include <utility>\n"
    "#include <vector>\n\n"
    "namespace toyc {\n"
    "namespace {\n\n"
    + codegen_helpers
    + "\n"
    + codegen_class
    + "\n\n} // namespace\n\n"
    "std::string generateRiscV(const CompUnit& unit, Options options) {\n"
    "    CodeGenerator generator(unit, options);\n"
    "    return generator.generate();\n"
    "}\n\n"
    "} // namespace toyc\n"
)

main_cpp = """#include \"ast.hpp\"
#include \"codegen.hpp\"
#include \"common.hpp\"
#include \"ctfe.hpp\"
#include \"lexer.hpp\"
#include \"parser.hpp\"

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
        std::cout << toyc::generateRiscV(unit, options);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << \"toyc: \" << error.what() << '\\n';
        return 1;
    }
}
"""

cmake = """cmake_minimum_required(VERSION 3.16)

project(ToyCCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(toyc
    src/main.cpp
    src/ctfe.cpp
    src/codegen.cpp
)

target_include_directories(toyc PRIVATE src)

target_compile_options(toyc PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-O2 -Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/O2 /W4>
)

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME regressions
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_regressions.py $<TARGET_FILE:toyc>
        )
        add_test(NAME ctfe_differential
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_ctfe_differential.py $<TARGET_FILE:toyc>
        )
    endif()
endif()
"""

# Fix lexer: it uses fail() and SourceLocation without toyc:: - they're in same namespace - OK
# Fix parser: uses TokenKind, Expr, fail - same ns - OK
# Fix ctfe: uses toInt32 without qualification - need using or toyc:: 
# Inside anonymous ns nested in toyc, toInt32 from parent toyc is NOT found by ADL for free functions
# Actually nested anonymous in toyc - unqualified lookup finds toyc::toInt32? 
# For `namespace toyc { namespace { void f() { toInt32(1); } } }` - yes, enclosing namespace toyc is searched.

# codegen helpers use toInt32, fail, functionLabel - OK
# CodeGenerator uses Options - OK

# Remove trailing blank issues in helpers chunk - line 925 might be blank before CtfeAbort

(SRC / "common.hpp").write_text(common, encoding="utf-8")
(SRC / "lexer.hpp").write_text(lexer_hpp, encoding="utf-8")
(SRC / "ast.hpp").write_text(ast_hpp, encoding="utf-8")
(SRC / "parser.hpp").write_text(parser_hpp, encoding="utf-8")
(SRC / "ctfe.hpp").write_text(ctfe_hpp, encoding="utf-8")
(SRC / "ctfe.cpp").write_text(ctfe_cpp, encoding="utf-8")
(SRC / "codegen.hpp").write_text(codegen_hpp, encoding="utf-8")
(SRC / "codegen.cpp").write_text(codegen_cpp, encoding="utf-8")
(SRC / "main.cpp").write_text(main_cpp, encoding="utf-8")
(ROOT / "CMakeLists.txt").write_text(cmake, encoding="utf-8")

print("Wrote modular sources:")
for p in ["common.hpp", "ast.hpp", "lexer.hpp", "parser.hpp", "ctfe.hpp", "ctfe.cpp", "codegen.hpp", "codegen.cpp", "main.cpp"]:
    path = SRC / p if p != "CMakeLists.txt" else ROOT / p
    if p == "main.cpp":
        path = SRC / p
    print(f"  {path.relative_to(ROOT)} ({path.stat().st_size} bytes)")
print("  CMakeLists.txt updated")
