#!/usr/bin/env python3
"""One-shot splitter: reads compiler.cpp and emits modular sources next to this script's parent."""
from pathlib import Path
from textwrap import dedent

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "compiler.cpp"


def lines_slice(lo: int, hi: int):
    ls = SRC.read_text(encoding="utf-8").splitlines()
    return ls[lo - 1 : hi]


def _strip_string_literals_for_braces(line: str) -> str:
    """Remove string/char literal contents so `{`/`}` inside quotes don't confuse brace matching."""
    out = []
    i = 0
    n = len(line)
    while i < n:
        c = line[i]
        if c == '"':
            out.append(" ")
            i += 1
            while i < n:
                if line[i] == "\\":
                    i += min(2, n - i)
                    continue
                if line[i] == '"':
                    i += 1
                    break
                i += 1
            continue
        if c == "'":
            out.append(" ")
            i += 1
            while i < n:
                if line[i] == "\\":
                    i += min(2, n - i)
                    continue
                if line[i] == "'":
                    i += 1
                    break
                i += 1
            continue
        out.append(c)
        i += 1
    return "".join(out)


def extract_method_body(ls: list[str], start_line_1based: int) -> list[str]:
    """Grab one member function from first line through matching closing `}`."""
    i = start_line_1based - 1
    buf = []
    depth = 0
    started = False
    while i < len(ls):
        line = ls[i]
        buf.append(line)
        scan = _strip_string_literals_for_braces(line)
        for ch in scan:
            if ch == "{":
                depth += 1
                started = True
            elif ch == "}":
                depth -= 1
        i += 1
        if started and depth == 0:
            return buf
    raise RuntimeError(f"unbalanced method starting line {start_line_1based}")


def replace_signature(body_lines: list[str], sig_parts: list[str]) -> list[str]:
    fi = next(i for i, line in enumerate(body_lines) if "{" in line)
    head = body_lines[:fi]
    brace_line = body_lines[fi]
    bp = brace_line.index("{")
    tail = brace_line[bp:]
    suffix = body_lines[fi + 1 :]
    combined_before = "\n".join(head + [brace_line[:bp]])
    rp = combined_before.rfind(")")
    middle = combined_before[rp + 1 :].strip() if rp != -1 else ""
    last_sig = sig_parts[-1].rstrip()
    if middle:
        if middle.startswith(":"):
            merged = last_sig + " " + middle + " " + tail.strip()
        elif last_sig.endswith(middle):
            merged = last_sig + " " + tail.strip()
        else:
            merged = last_sig + " " + middle + " " + tail.strip()
    else:
        merged = last_sig + " " + tail.strip()
    return sig_parts[:-1] + [merged] + suffix


def write(path: Path, text: str):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text.rstrip() + "\n", encoding="utf-8")


def main():
    if not SRC.exists():
        raise SystemExit(f"missing {SRC}")

    # --- common.h / common.cpp ---
    write(
        ROOT / "common.h",
        dedent(
            """\
#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

struct CompileError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

int alignTo(int value, int align);
std::uint32_t floatBits(float value);
std::string escapeAsmString(const std::string &s);
std::string readFile(const std::string &path);
void writeFile(const std::string &path, const std::string &content);
int product(const std::vector<int> &dims, std::size_t from = 0);
"""
        ),
    )

    common_impl = "\n".join(lines_slice(26, 71))
    product_fn = "\n".join(lines_slice(1093, 1102))
    rw = "\n".join(lines_slice(2582, 2598))
    write(
        ROOT / "common.cpp",
        dedent(
            f"""\
#include "common.h"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <limits>
#include <sstream>

using namespace std;

{common_impl}

{product_fn}

{rw}
"""
        ),
    )

    # --- token.h ---
    write(ROOT / "token.h", "#pragma once\n\n" + "\n".join(lines_slice(73, 121)))

    # --- ast.h ---
    ast_body = "\n".join(lines_slice(434, 710))
    write(
        ROOT / "ast.h",
        dedent(
            f"""\
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

{ast_body}
"""
        ),
    )

    # --- lexer ---
    lexer_private_decl = dedent(
        """\
#pragma once

#include "common.h"
#include "token.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

class Lexer {
public:
  explicit Lexer(std::string source);

  std::vector<Token> run();

private:
  std::string src_;
  std::size_t pos_ = 0;
  int line_ = 1;
  int col_ = 1;

  char peek(std::size_t ahead = 0) const;
  char get();

  static bool isIdentStart(char c);
  static bool isIdentPart(char c);

  void skipSpaceAndComments();
  Token scanIdent();
  Token scanNumber();
  Token scanString();
  Token scanPunct();
};
"""
    )
    write(ROOT / "lexer.h", lexer_private_decl)

    lexer_methods = lines_slice(164, 431)
    lexer_cpp = ['#include "lexer.h"', "", "using namespace std;", ""]
    lexer_cpp.append("Lexer::Lexer(string source) : src_(std::move(source)) {}")
    lexer_cpp.append("")
    for line in lexer_methods:
        stripped = line.strip()
        if not stripped:
            lexer_cpp.append("")
            continue
        if stripped.startswith("char peek"):
            lexer_cpp.append("char Lexer::peek(size_t ahead) const " + line[line.index("{") :])
            continue
        if stripped.startswith("char get("):
            lexer_cpp.append("char Lexer::get()" + line[line.index("{") :])
            continue
        if stripped.startswith("static bool isIdentStart"):
            lexer_cpp.append("bool Lexer::isIdentStart(char c)" + line[line.index("{") :])
            continue
        if stripped.startswith("static bool isIdentPart"):
            lexer_cpp.append("bool Lexer::isIdentPart(char c)" + line[line.index("{") :])
            continue
        if stripped.startswith("void skipSpaceAndComments"):
            lexer_cpp.append("void Lexer::skipSpaceAndComments()" + line[line.index("{") :])
            continue
        if stripped.startswith("Token scanIdent"):
            lexer_cpp.append("Token Lexer::scanIdent()" + line[line.index("{") :])
            continue
        if stripped.startswith("Token scanNumber"):
            lexer_cpp.append("Token Lexer::scanNumber()" + line[line.index("{") :])
            continue
        if stripped.startswith("Token scanString"):
            lexer_cpp.append("Token Lexer::scanString()" + line[line.index("{") :])
            continue
        if stripped.startswith("Token scanPunct"):
            lexer_cpp.append("Token Lexer::scanPunct()" + line[line.index("{") :])
            continue
        lexer_cpp.append(line)

    lexer_cpp.insert(3, '#include <cctype>')
    lexer_cpp.insert(4, '#include <cstdlib>')
    lexer_cpp.insert(5, '#include <cstring>')
    lexer_cpp.insert(6, '#include <unordered_map>')
    write(ROOT / "lexer.cpp", "\n".join(lexer_cpp))

    # Lexer::run stays — need to handle vector<Token> Lexer::run()
    run_body = lines_slice(127, 156)
    run_lines = ['vector<Token> Lexer::run() {']
    run_lines.extend(["  " + x.lstrip() if x.strip() else "" for x in run_body[1:]])
    # Insert run after constructor in lexer.cpp
    text = (ROOT / "lexer.cpp").read_text(encoding="utf-8")
    marker = "Lexer::Lexer(string source) : src_(std::move(source)) {}\n\n"
    if marker not in text:
        raise SystemExit("lexer.cpp marker missing")
    insertion = marker + "\n".join(run_lines) + "\n\n"
    text = text.replace(marker, insertion, 1)
    ROOT.joinpath("lexer.cpp").write_text(text, encoding="utf-8")

    # --- parser ---
    parser_decl = dedent(
        """\
#pragma once

#include "ast.h"
#include "common.h"
#include "token.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using std::make_unique;
using std::min;
using std::string;
using std::unique_ptr;
using std::vector;

class Parser {
public:
  explicit Parser(vector<Token> tokens);

  Program parseProgram();

private:
  vector<Token> tokens_;
  std::size_t pos_ = 0;

  const Token &tok(std::size_t ahead = 0) const;

  bool check(TokenKind kind) const;

  bool match(TokenKind kind);

  const Token &expect(TokenKind kind, const string &what);

  CompileError error(const string &message) const;

  static bool isBType(TokenKind kind);

  BaseType parseBType();

  BaseType parseFuncType();

  bool isFuncDefAhead() const;

  unique_ptr<DeclStmt> parseDecl();

  VarDef parseVarDef(bool requireInit);

  unique_ptr<InitVal> parseInitVal();

  unique_ptr<FuncDef> parseFuncDef();

  Param parseParam();

  unique_ptr<BlockStmt> parseBlock();

  StmtPtr parseStmt();

  ExprPtr parseExp();

  unique_ptr<LValExpr> parseLVal();

  ExprPtr parsePrimary();

  ExprPtr parseUnary();

  ExprPtr parseMul();

  ExprPtr parseAdd();

  ExprPtr parseRel();

  ExprPtr parseEq();

  ExprPtr parseLAnd();

  ExprPtr parseLOr();
};
"""
    )
    write(ROOT / "parser.h", parser_decl)

    parser_cpp = ['#include "parser.h"', "", "using namespace std;", ""]
    parser_cpp.append(
        "Parser::Parser(vector<Token> tokens) : tokens_(std::move(tokens)) {}"
    )
    parser_cpp.append("")

    ls_full = SRC.read_text(encoding="utf-8").splitlines()

    def emit_parser(sig_parts: list[str], start_line: int):
        body = extract_method_body(ls_full, start_line)
        parser_cpp.extend(replace_signature(body, sig_parts))
        parser_cpp.append("")

    emit_parser(["Program Parser::parseProgram()"], 716)
    emit_parser(["const Token &Parser::tok(size_t ahead) const"], 741)
    emit_parser(["bool Parser::check(TokenKind kind) const"], 746)
    emit_parser(["bool Parser::match(TokenKind kind)"], 748)
    emit_parser(["const Token &Parser::expect(TokenKind kind, const string &what)"], 756)
    emit_parser(["CompileError Parser::error(const string &message) const"], 763)
    emit_parser(["bool Parser::isBType(TokenKind kind)"], 769)
    emit_parser(["BaseType Parser::parseBType()"], 773)
    emit_parser(["BaseType Parser::parseFuncType()"], 783)
    emit_parser(["bool Parser::isFuncDefAhead() const"], 790)
    emit_parser(["unique_ptr<DeclStmt> Parser::parseDecl()"], 800)
    emit_parser(["VarDef Parser::parseVarDef(bool requireInit)"], 814)
    emit_parser(["unique_ptr<InitVal> Parser::parseInitVal()"], 831)
    emit_parser(["unique_ptr<FuncDef> Parser::parseFuncDef()"], 850)
    emit_parser(["Param Parser::parseParam()"], 870)
    emit_parser(["unique_ptr<BlockStmt> Parser::parseBlock()"], 887)
    emit_parser(["StmtPtr Parser::parseStmt()"], 905)
    emit_parser(["ExprPtr Parser::parseExp()"], 964)
    emit_parser(["unique_ptr<LValExpr> Parser::parseLVal()"], 966)
    emit_parser(["ExprPtr Parser::parsePrimary()"], 976)
    emit_parser(["ExprPtr Parser::parseUnary()"], 1015)
    emit_parser(["ExprPtr Parser::parseMul()"], 1024)
    emit_parser(["ExprPtr Parser::parseAdd()"], 1036)
    emit_parser(["ExprPtr Parser::parseRel()"], 1047)
    emit_parser(["ExprPtr Parser::parseEq()"], 1059)
    emit_parser(["ExprPtr Parser::parseLAnd()"], 1070)
    emit_parser(["ExprPtr Parser::parseLOr()"], 1081)

    write(ROOT / "parser.cpp", "\n".join(parser_cpp))

    # --- semantic ---
    sem_decl = dedent(
        """\
#pragma once

#include "ast.h"
#include "common.h"

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using std::make_unique;
using std::max;
using std::min;
using std::string;
using std::unique_ptr;
using std::vector;

class Semantic {
public:
  explicit Semantic(Program &program);

  void run();

  const vector<unique_ptr<Symbol>> &symbols() const { return symbols_; }

  const vector<Symbol *> &globalsInOrder() const { return globalsInOrder_; }

  const std::unordered_map<string, Function *> &functions() const {
    return functions_;
  }

private:
  Program &program_;
  vector<unique_ptr<Symbol>> symbols_;
  vector<unique_ptr<Function>> functionStorage_;
  vector<Symbol *> globalsInOrder_;
  std::unordered_map<string, Symbol *> globals_;
  std::unordered_map<string, Function *> functions_;
  vector<std::unordered_map<string, Symbol *>> scopes_;
  Function *currentFunction_ = nullptr;

  [[noreturn]] void fail(int line, const string &message) const;

  Symbol *newSymbol();

  Function *newFunction(const string &name, BaseType ret);

  void addRuntime(const string &name, BaseType ret, vector<ParamType> params,
                  string asmName = "", bool injectLine = false,
                  bool variadic = false);

  void addRuntimeFunctions();

  void predeclareUserFunctions();

  void enterScope();

  void leaveScope();

  Symbol *lookupVar(const string &name) const;

  void addLocal(Symbol *sym, int line);

  int allocFrame(int size, int align);

  void allocateSymbolStorage(Symbol *sym);

  Symbol *declareSymbol(const string &name, BaseType base, bool isConst,
                        bool isGlobal, bool isArray, bool isParam,
                        bool isParamArray, const vector<int> &dims, int line);

  int evalConstInt(Expr *expr);

  vector<int> evalDims(vector<ExprPtr> &dims);

  void visitGlobalDecl(DeclStmt &decl);

  ConstValue zeroConst(BaseType base);

  vector<ConstValue> flattenConstInit(InitVal *init, const vector<int> &dims,
                                      BaseType base);

  int fillConstAggregate(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<ConstValue> &values, BaseType base);

  size_t chooseInitChildDepth(const vector<int> &dims, size_t depth,
                              int flatIndex);

  void visitFunction(FuncDef &def);

  void visitBlock(BlockStmt &block, bool createScope);

  void visitStmt(Stmt *stmt);

  void visitLocalDecl(DeclStmt &decl);

  void visitInit(InitVal *init);

  void visitExpr(Expr *expr);

  void visitLVal(LValExpr *expr);

  int flattenIndex(const Symbol &sym, const vector<int> &idxValues);

  void visitCall(CallExpr *expr);

  void visitUnary(UnaryExpr *expr);

  void visitBinary(BinaryExpr *expr);

  ConstValue evalBinaryConst(const string &op, ConstValue lhs, ConstValue rhs,
                             BaseType resultType);
};
"""
    )
    write(ROOT / "semantic.h", sem_decl)

    sem_cpp = [
        '#include "semantic.h"',
        "",
        "#include <algorithm>",
        "#include <limits>",
        "",
        "using namespace std;",
        "",
    ]

    ls_sem = SRC.read_text(encoding="utf-8").splitlines()

    def emit_sem(sig_parts: list[str], start_line: int):
        body = extract_method_body(ls_sem, start_line)
        sem_cpp.extend(replace_signature(body, sig_parts))
        sem_cpp.append("")

    emit_sem(["Semantic::Semantic(Program &program)"], 1106)
    emit_sem(["void Semantic::run()"], 1108)
    emit_sem(["[[noreturn]] void Semantic::fail(int line, const string &message) const"], 1134)
    emit_sem(["Symbol *Semantic::newSymbol()"], 1140)
    emit_sem(["Function *Semantic::newFunction(const string &name, BaseType ret)"], 1145)
    emit_sem(
        [
            "void Semantic::addRuntime(const string &name, BaseType ret, vector<ParamType> params,",
            "                  string asmName, bool injectLine, bool variadic)",
        ],
        1155,
    )
    emit_sem(["void Semantic::addRuntimeFunctions()"], 1166)
    emit_sem(["void Semantic::predeclareUserFunctions()"], 1192)
    emit_sem(["void Semantic::enterScope()"], 1210)
    emit_sem(["void Semantic::leaveScope()"], 1212)
    emit_sem(["Symbol *Semantic::lookupVar(const string &name) const"], 1214)
    emit_sem(["void Semantic::addLocal(Symbol *sym, int line)"], 1228)
    emit_sem(["int Semantic::allocFrame(int size, int align)"], 1239)
    emit_sem(["void Semantic::allocateSymbolStorage(Symbol *sym)"], 1245)
    emit_sem(
        [
            "Symbol *Semantic::declareSymbol(const string &name, BaseType base, bool isConst,",
            "                        bool isGlobal, bool isArray, bool isParam,",
            "                        bool isParamArray, const vector<int> &dims, int line)",
        ],
        1261,
    )
    emit_sem(["int Semantic::evalConstInt(Expr *expr)"], 1290)
    emit_sem(["vector<int> Semantic::evalDims(vector<ExprPtr> &dims)"], 1298)
    emit_sem(["void Semantic::visitGlobalDecl(DeclStmt &decl)"], 1310)
    emit_sem(["ConstValue Semantic::zeroConst(BaseType base)"], 1344)
    emit_sem(
        [
            "vector<ConstValue> Semantic::flattenConstInit(InitVal *init, const vector<int> &dims,",
            "                                      BaseType base)",
        ],
        1352,
    )
    emit_sem(
        [
            "int Semantic::fillConstAggregate(InitVal *init, const vector<int> &dims, size_t depth,",
            "                         int start, vector<ConstValue> &values, BaseType base)",
        ],
        1362,
    )
    emit_sem(
        [
            "size_t Semantic::chooseInitChildDepth(const vector<int> &dims, size_t depth, int flatIndex)",
        ],
        1395,
    )
    emit_sem(["void Semantic::visitFunction(FuncDef &def)"], 1407)
    emit_sem(["void Semantic::visitBlock(BlockStmt &block, bool createScope)"], 1426)
    emit_sem(["void Semantic::visitStmt(Stmt *stmt)"], 1438)
    emit_sem(["void Semantic::visitLocalDecl(DeclStmt &decl)"], 1487)
    emit_sem(["void Semantic::visitInit(InitVal *init)"], 1514)
    emit_sem(["void Semantic::visitExpr(Expr *expr)"], 1524)
    emit_sem(["void Semantic::visitLVal(LValExpr *expr)"], 1560)
    emit_sem(
        ["int Semantic::flattenIndex(const Symbol &sym, const vector<int> &idxValues)"],
        1611,
    )
    emit_sem(["void Semantic::visitCall(CallExpr *expr)"], 1629)
    emit_sem(["void Semantic::visitUnary(UnaryExpr *expr)"], 1641)
    emit_sem(["void Semantic::visitBinary(BinaryExpr *expr)"], 1671)
    emit_sem(
        [
            "ConstValue Semantic::evalBinaryConst(const string &op, ConstValue lhs, ConstValue rhs,",
            "                             BaseType resultType)",
        ],
        1693,
    )

    write(ROOT / "semantic.cpp", "\n".join(sem_cpp))

    # --- codegen ---
    cg_decl = dedent(
        """\
#pragma once

#include "ast.h"
#include "semantic.h"

#include <sstream>
#include <string>
#include <utility>
#include <vector>

using std::pair;
using std::string;
using std::to_string;
using std::vector;

class CodeGen {
public:
  CodeGen(Program &program, const Semantic &semantic);

  string run();

private:
  Program &program_;
  const Semantic &semantic_;
  std::ostringstream out_;
  int labelId_ = 0;
  Function *currentFunction_ = nullptr;
  vector<string> breakLabels_;
  vector<string> continueLabels_;
  vector<pair<string, float>> floatLiterals_;
  vector<pair<string, string>> stringLiterals_;

  void emit(const string &line = "");

  string newLabel(const string &prefix);

  static bool fitsImm12(int value);

  void emitAdjustSp(int delta);

  void emitAddOffset(const string &dst, const string &base, int offset);

  void emitLoadMem(const string &inst, const string &dst, const string &base,
                   int offset);

  void emitStoreMem(const string &inst, const string &src, const string &base,
                    int offset);

  void emitGlobals();

  void emitFunction(FuncDef &def);

  void emitStoreParams(FuncDef &def);

  void emitBlock(BlockStmt &block, bool);

  void emitStmt(Stmt *stmt);

  void emitDecl(DeclStmt &decl);

  int flattenRuntimeInit(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<InitVal *> &flat);

  size_t chooseRuntimeInitChildDepth(const vector<int> &dims, size_t depth,
                                     int flatIndex);

  template <class AddressEmitter>
  void emitStoreToAddress(BaseType base, AddressEmitter emitAddressToA1);

  void emitAssign(AssignStmt &stmt);

  void emitIf(IfStmt &stmt);

  void emitWhile(WhileStmt &stmt);

  void emitReturn(ReturnStmt &stmt);

  void emitExpr(Expr *expr);

  void emitConst(ConstValue v);

  void emitFloatConst(float value);

  void emitStringExpr(StringExpr *expr);

  void emitUnary(UnaryExpr *expr);

  void emitBinary(BinaryExpr *expr);

  void emitIntCompare(const string &op);

  void emitFloatCompare(const string &op);

  void emitCond(Expr *expr, const string &trueLabel, const string &falseLabel);

  void emitBoolFromValue(const Type &type);

  void emitLValValue(LValExpr *expr);

  void emitLValAddress(LValExpr *expr);

  int strideForIndex(Symbol *sym, size_t index);

  void emitCall(CallExpr *expr);

  void emitConvert(const Type &from, const Type &to);

  void emitPushInt(const string &reg);

  void emitPopInt(const string &reg);

  void emitPushFloat(const string &reg);

  void emitPopFloat(const string &reg);

  void emitLiteralPools();
};
"""
    )
    write(ROOT / "codegen.h", cg_decl)

    cg_cpp = [
        '#include "codegen.h"',
        "",
        '#include "common.h"',
        "",
        "#include <algorithm>",
        "",
        "using namespace std;",
        "",
        "\n".join(lines_slice(1745, 1778)),
        "",
    ]

    ls_cg = SRC.read_text(encoding="utf-8").splitlines()

    def emit_cg(sig_parts: list[str], start_line: int):
        body = extract_method_body(ls_cg, start_line)
        cg_cpp.extend(replace_signature(body, sig_parts))
        cg_cpp.append("")

    emit_cg(["CodeGen::CodeGen(Program &program, const Semantic &semantic)"], 1782)
    emit_cg(["string CodeGen::run()"], 1785)
    emit_cg(["void CodeGen::emit(const string &line)"], 1808)
    emit_cg(["string CodeGen::newLabel(const string &prefix)"], 1810)
    emit_cg(["bool CodeGen::fitsImm12(int value)"], 1814)
    emit_cg(["void CodeGen::emitAdjustSp(int delta)"], 1816)
    emit_cg(
        ["void CodeGen::emitAddOffset(const string &dst, const string &base, int offset)"],
        1828,
    )
    emit_cg(
        [
            "void CodeGen::emitLoadMem(const string &inst, const string &dst, const string &base,",
            "                   int offset)",
        ],
        1837,
    )
    emit_cg(
        [
            "void CodeGen::emitStoreMem(const string &inst, const string &src, const string &base,",
            "                    int offset)",
        ],
        1848,
    )
    emit_cg(["void CodeGen::emitGlobals()"], 1859)
    emit_cg(["void CodeGen::emitFunction(FuncDef &def)"], 1897)
    emit_cg(["void CodeGen::emitStoreParams(FuncDef &def)"], 1929)
    emit_cg(["void CodeGen::emitBlock(BlockStmt &block, bool)"], 1966)
    emit_cg(["void CodeGen::emitStmt(Stmt *stmt)"], 1972)
    emit_cg(["void CodeGen::emitDecl(DeclStmt &decl)"], 2014)
    emit_cg(
        [
            "int CodeGen::flattenRuntimeInit(InitVal *init, const vector<int> &dims, size_t depth,",
            "                         int start, vector<InitVal *> &flat)",
        ],
        2059,
    )
    emit_cg(
        [
            "size_t CodeGen::chooseRuntimeInitChildDepth(const vector<int> &dims, size_t depth,",
            "                                     int flatIndex)",
        ],
        2083,
    )
    emit_cg(
        [
            "template <class AddressEmitter>",
            "void CodeGen::emitStoreToAddress(BaseType base, AddressEmitter emitAddressToA1)",
        ],
        2096,
    )
    emit_cg(["void CodeGen::emitAssign(AssignStmt &stmt)"], 2111)
    emit_cg(["void CodeGen::emitIf(IfStmt &stmt)"], 2127)
    emit_cg(["void CodeGen::emitWhile(WhileStmt &stmt)"], 2142)
    emit_cg(["void CodeGen::emitReturn(ReturnStmt &stmt)"], 2158)
    emit_cg(["void CodeGen::emitExpr(Expr *expr)"], 2166)
    emit_cg(["void CodeGen::emitConst(ConstValue v)"], 2193)
    emit_cg(["void CodeGen::emitFloatConst(float value)"], 2201)
    emit_cg(["void CodeGen::emitStringExpr(StringExpr *expr)"], 2213)
    emit_cg(["void CodeGen::emitUnary(UnaryExpr *expr)"], 2221)
    emit_cg(["void CodeGen::emitBinary(BinaryExpr *expr)"], 2240)
    emit_cg(["void CodeGen::emitIntCompare(const string &op)"], 2315)
    emit_cg(["void CodeGen::emitFloatCompare(const string &op)"], 2335)
    emit_cg(
        [
            "void CodeGen::emitCond(Expr *expr, const string &trueLabel, const string &falseLabel)",
        ],
        2352,
    )
    emit_cg(["void CodeGen::emitBoolFromValue(const Type &type)"], 2376)
    emit_cg(["void CodeGen::emitLValValue(LValExpr *expr)"], 2388)
    emit_cg(["void CodeGen::emitLValAddress(LValExpr *expr)"], 2404)
    emit_cg(["int CodeGen::strideForIndex(Symbol *sym, size_t index)"], 2434)
    emit_cg(["void CodeGen::emitCall(CallExpr *expr)"], 2445)
    emit_cg(["void CodeGen::emitConvert(const Type &from, const Type &to)"], 2532)
    emit_cg(["void CodeGen::emitPushInt(const string &reg)"], 2544)
    emit_cg(["void CodeGen::emitPopInt(const string &reg)"], 2549)
    emit_cg(["void CodeGen::emitPushFloat(const string &reg)"], 2554)
    emit_cg(["void CodeGen::emitPopFloat(const string &reg)"], 2559)
    emit_cg(["void CodeGen::emitLiteralPools()"], 2564)

    write(ROOT / "codegen.cpp", "\n".join(cg_cpp))

    # --- main ---
    write(
        ROOT / "main.cpp",
        dedent(
            """\
#include "codegen.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"

#include <iostream>
#include <string>

using namespace std;

static int compileFile(const string &input, const string &output, bool optO1) {
  (void)optO1;
  string source = readFile(input);
  Lexer lexer(source);
  Parser parser(lexer.run());
  Program program = parser.parseProgram();
  Semantic semantic(program);
  semantic.run();
  CodeGen codegen(program, semantic);
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
    if (arg == "-o") {
      if (i + 1 >= argc) {
        cerr << "compiler: missing argument after -o\\n";
        return 1;
      }
      output = argv[++i];
      continue;
    }
    if (!arg.empty() && arg[0] == '-') {
      continue;
    }
    input = arg;
  }
  if (input.empty() || output.empty()) {
    cerr << "usage: compiler -S -o output.s input.sy [-O1]\\n";
    return 1;
  }
  try {
    return compileFile(input, output, optO1);
  } catch (const exception &e) {
    cerr << "compiler: " << e.what() << '\\n';
    return 1;
  }
}
"""
        ),
    )

    print("generated modular sources under", ROOT)


if __name__ == "__main__":
    main()
