#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

namespace {

struct CompileError : runtime_error {
  using runtime_error::runtime_error;
};

static int alignTo(int value, int align) {
  return (value + align - 1) / align * align;
}

static uint32_t floatBits(float value) {
  uint32_t bits = 0;
  static_assert(sizeof(bits) == sizeof(value), "float must be 32-bit");
  memcpy(&bits, &value, sizeof(bits));
  return bits;
}

static string escapeAsmString(const string &s) {
  string out;
  for (unsigned char c : s) {
    switch (c) {
    case '\n':
      out += "\\n";
      break;
    case '\t':
      out += "\\t";
      break;
    case '\r':
      out += "\\r";
      break;
    case '\\':
      out += "\\\\";
      break;
    case '"':
      out += "\\\"";
      break;
    case '\0':
      out += "\\000";
      break;
    default:
      if (isprint(c)) {
        out.push_back(static_cast<char>(c));
      } else {
        char buf[8];
        snprintf(buf, sizeof(buf), "\\%03o", c);
        out += buf;
      }
      break;
    }
  }
  return out;
}

enum class TokenKind {
  End,
  Ident,
  IntConst,
  FloatConst,
  String,
  KwConst,
  KwInt,
  KwFloat,
  KwVoid,
  KwIf,
  KwElse,
  KwWhile,
  KwBreak,
  KwContinue,
  KwReturn,
  Plus,
  Minus,
  Star,
  Slash,
  Percent,
  Bang,
  AndAnd,
  OrOr,
  EqEq,
  Neq,
  Lt,
  Gt,
  Le,
  Ge,
  Assign,
  LParen,
  RParen,
  LBrace,
  RBrace,
  LBracket,
  RBracket,
  Semicolon,
  Comma
};

struct Token {
  TokenKind kind = TokenKind::End;
  string text;
  int line = 1;
  int col = 1;
  int32_t intVal = 0;
  float floatVal = 0.0f;
};

class Lexer {
public:
  explicit Lexer(string source) : src_(std::move(source)) {}

  vector<Token> run() {
    vector<Token> tokens;
    while (true) {
      skipSpaceAndComments();
      Token tok;
      tok.line = line_;
      tok.col = col_;
      char c = peek();
      if (c == '\0') {
        tok.kind = TokenKind::End;
        tok.text = "";
        tokens.push_back(tok);
        return tokens;
      }
      if (isIdentStart(c)) {
        tokens.push_back(scanIdent());
        continue;
      }
      if (isdigit(static_cast<unsigned char>(c)) ||
          (c == '.' && isdigit(static_cast<unsigned char>(peek(1))))) {
        tokens.push_back(scanNumber());
        continue;
      }
      if (c == '"') {
        tokens.push_back(scanString());
        continue;
      }
      tokens.push_back(scanPunct());
    }
  }

private:
  string src_;
  size_t pos_ = 0;
  int line_ = 1;
  int col_ = 1;

  char peek(size_t ahead = 0) const {
    size_t p = pos_ + ahead;
    return p < src_.size() ? src_[p] : '\0';
  }

  char get() {
    char c = peek();
    if (c == '\0') {
      return c;
    }
    ++pos_;
    if (c == '\n') {
      ++line_;
      col_ = 1;
    } else {
      ++col_;
    }
    return c;
  }

  static bool isIdentStart(char c) {
    return c == '_' || isalpha(static_cast<unsigned char>(c));
  }

  static bool isIdentPart(char c) {
    return isIdentStart(c) || isdigit(static_cast<unsigned char>(c));
  }

  void skipSpaceAndComments() {
    while (true) {
      while (isspace(static_cast<unsigned char>(peek()))) {
        get();
      }
      if (peek() == '/' && peek(1) == '/') {
        while (peek() != '\0' && peek() != '\n') {
          get();
        }
        continue;
      }
      if (peek() == '/' && peek(1) == '*') {
        get();
        get();
        while (!(peek() == '*' && peek(1) == '/')) {
          if (peek() == '\0') {
            throw CompileError("unterminated block comment");
          }
          get();
        }
        get();
        get();
        continue;
      }
      break;
    }
  }

  Token scanIdent() {
    Token tok;
    tok.line = line_;
    tok.col = col_;
    while (isIdentPart(peek())) {
      tok.text.push_back(get());
    }
    static const unordered_map<string, TokenKind> keywords = {
        {"const", TokenKind::KwConst},       {"int", TokenKind::KwInt},
        {"float", TokenKind::KwFloat},       {"void", TokenKind::KwVoid},
        {"if", TokenKind::KwIf},             {"else", TokenKind::KwElse},
        {"while", TokenKind::KwWhile},       {"break", TokenKind::KwBreak},
        {"continue", TokenKind::KwContinue}, {"return", TokenKind::KwReturn}};
    auto it = keywords.find(tok.text);
    tok.kind = it == keywords.end() ? TokenKind::Ident : it->second;
    return tok;
  }

  Token scanNumber() {
    Token tok;
    tok.line = line_;
    tok.col = col_;
    bool isFloat = false;
    bool isHex = peek() == '0' && (peek(1) == 'x' || peek(1) == 'X');

    if (isHex) {
      tok.text.push_back(get());
      tok.text.push_back(get());
      while (isxdigit(static_cast<unsigned char>(peek()))) {
        tok.text.push_back(get());
      }
      if (peek() == '.') {
        isFloat = true;
        tok.text.push_back(get());
        while (isxdigit(static_cast<unsigned char>(peek()))) {
          tok.text.push_back(get());
        }
      }
      if (peek() == 'p' || peek() == 'P') {
        isFloat = true;
        tok.text.push_back(get());
        if (peek() == '+' || peek() == '-') {
          tok.text.push_back(get());
        }
        while (isdigit(static_cast<unsigned char>(peek()))) {
          tok.text.push_back(get());
        }
      }
    } else {
      while (isdigit(static_cast<unsigned char>(peek()))) {
        tok.text.push_back(get());
      }
      if (peek() == '.') {
        isFloat = true;
        tok.text.push_back(get());
        while (isdigit(static_cast<unsigned char>(peek()))) {
          tok.text.push_back(get());
        }
      }
      if (peek() == 'e' || peek() == 'E') {
        isFloat = true;
        tok.text.push_back(get());
        if (peek() == '+' || peek() == '-') {
          tok.text.push_back(get());
        }
        while (isdigit(static_cast<unsigned char>(peek()))) {
          tok.text.push_back(get());
        }
      }
    }

    if (peek() == 'f' || peek() == 'F' || peek() == 'l' || peek() == 'L') {
      isFloat = true;
      get();
    }

    if (isFloat) {
      tok.kind = TokenKind::FloatConst;
      tok.floatVal = strtof(tok.text.c_str(), nullptr);
    } else {
      tok.kind = TokenKind::IntConst;
      long long v = strtoll(tok.text.c_str(), nullptr, 0);
      tok.intVal = static_cast<int32_t>(v);
    }
    return tok;
  }

  Token scanString() {
    Token tok;
    tok.kind = TokenKind::String;
    tok.line = line_;
    tok.col = col_;
    get();
    while (peek() != '"') {
      if (peek() == '\0') {
        throw CompileError("unterminated string literal");
      }
      if (peek() != '\\') {
        tok.text.push_back(get());
        continue;
      }
      get();
      char c = get();
      switch (c) {
      case 'n':
        tok.text.push_back('\n');
        break;
      case 't':
        tok.text.push_back('\t');
        break;
      case 'r':
        tok.text.push_back('\r');
        break;
      case '\\':
        tok.text.push_back('\\');
        break;
      case '"':
        tok.text.push_back('"');
        break;
      case '0':
        tok.text.push_back('\0');
        break;
      default:
        tok.text.push_back(c);
        break;
      }
    }
    get();
    return tok;
  }

  Token scanPunct() {
    Token tok;
    tok.line = line_;
    tok.col = col_;
    char c = get();
    tok.text.push_back(c);
    auto one = [&](TokenKind kind) {
      tok.kind = kind;
      return tok;
    };
    switch (c) {
    case '+':
      return one(TokenKind::Plus);
    case '-':
      return one(TokenKind::Minus);
    case '*':
      return one(TokenKind::Star);
    case '/':
      return one(TokenKind::Slash);
    case '%':
      return one(TokenKind::Percent);
    case '(':
      return one(TokenKind::LParen);
    case ')':
      return one(TokenKind::RParen);
    case '{':
      return one(TokenKind::LBrace);
    case '}':
      return one(TokenKind::RBrace);
    case '[':
      return one(TokenKind::LBracket);
    case ']':
      return one(TokenKind::RBracket);
    case ';':
      return one(TokenKind::Semicolon);
    case ',':
      return one(TokenKind::Comma);
    case '!':
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::Neq);
      }
      return one(TokenKind::Bang);
    case '&':
      if (peek() == '&') {
        tok.text.push_back(get());
        return one(TokenKind::AndAnd);
      }
      break;
    case '|':
      if (peek() == '|') {
        tok.text.push_back(get());
        return one(TokenKind::OrOr);
      }
      break;
    case '=':
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::EqEq);
      }
      return one(TokenKind::Assign);
    case '<':
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::Le);
      }
      return one(TokenKind::Lt);
    case '>':
      if (peek() == '=') {
        tok.text.push_back(get());
        return one(TokenKind::Ge);
      }
      return one(TokenKind::Gt);
    default:
      break;
    }
    string msg = "unexpected character '";
    msg.push_back(c);
    msg += "'";
    throw CompileError(msg);
  }
};

enum class BaseType { Void, Int, Float };

struct Type {
  BaseType base = BaseType::Int;
  bool isPointer = false;
  vector<int> dims;

  static Type scalar(BaseType b) {
    Type t;
    t.base = b;
    return t;
  }

  static Type pointer(BaseType b, vector<int> remainingDims = {}) {
    Type t;
    t.base = b;
    t.isPointer = true;
    t.dims = std::move(remainingDims);
    return t;
  }

  bool isFloatScalar() const { return !isPointer && base == BaseType::Float; }
  bool isIntScalar() const { return !isPointer && base == BaseType::Int; }
};

struct ConstValue {
  BaseType type = BaseType::Int;
  int32_t i = 0;
  float f = 0.0f;
};

static int32_t constAsInt(ConstValue v) {
  if (v.type == BaseType::Float) {
    return static_cast<int32_t>(v.f);
  }
  return v.i;
}

static float constAsFloat(ConstValue v) {
  if (v.type == BaseType::Float) {
    return v.f;
  }
  return static_cast<float>(v.i);
}

static ConstValue castConst(ConstValue v, BaseType target) {
  ConstValue out;
  out.type = target;
  if (target == BaseType::Float) {
    out.f = constAsFloat(v);
  } else {
    out.i = constAsInt(v);
  }
  return out;
}

struct Symbol;
struct Function;

enum class ExprKind { Number, String, LVal, Call, Unary, Binary };

struct Expr {
  explicit Expr(ExprKind k, int lineNo) : kind(k), line(lineNo) {}
  virtual ~Expr() = default;
  ExprKind kind;
  int line = 1;
  Type type;
  bool isConst = false;
  ConstValue constVal;
};

using ExprPtr = unique_ptr<Expr>;

struct NumberExpr : Expr {
  NumberExpr(int line, int32_t value) : Expr(ExprKind::Number, line) {
    isFloat = false;
    intVal = value;
  }
  NumberExpr(int line, float value) : Expr(ExprKind::Number, line) {
    isFloat = true;
    floatVal = value;
  }
  bool isFloat = false;
  int32_t intVal = 0;
  float floatVal = 0.0f;
};

struct StringExpr : Expr {
  StringExpr(int line, string value)
      : Expr(ExprKind::String, line), value(std::move(value)) {}
  string value;
  string label;
};

struct LValExpr : Expr {
  LValExpr(int line, string n) : Expr(ExprKind::LVal, line), name(std::move(n)) {}
  string name;
  vector<ExprPtr> indices;
  Symbol *symbol = nullptr;
};

struct CallExpr : Expr {
  CallExpr(int line, string n) : Expr(ExprKind::Call, line), name(std::move(n)) {}
  string name;
  vector<ExprPtr> args;
  Function *function = nullptr;
};

struct UnaryExpr : Expr {
  UnaryExpr(int line, string op, ExprPtr expr)
      : Expr(ExprKind::Unary, line), op(std::move(op)), expr(std::move(expr)) {}
  string op;
  ExprPtr expr;
};

struct BinaryExpr : Expr {
  BinaryExpr(int line, string op, ExprPtr lhs, ExprPtr rhs)
      : Expr(ExprKind::Binary, line), op(std::move(op)), lhs(std::move(lhs)),
        rhs(std::move(rhs)) {}
  string op;
  ExprPtr lhs;
  ExprPtr rhs;
};

struct InitVal {
  bool isList = false;
  ExprPtr expr;
  vector<unique_ptr<InitVal>> list;
};

struct VarDef {
  string name;
  vector<ExprPtr> dims;
  unique_ptr<InitVal> init;
  Symbol *symbol = nullptr;
  int line = 1;
};

enum class StmtKind {
  Decl,
  Block,
  Assign,
  Expr,
  If,
  While,
  Break,
  Continue,
  Return
};

struct Stmt {
  explicit Stmt(StmtKind k, int lineNo) : kind(k), line(lineNo) {}
  virtual ~Stmt() = default;
  StmtKind kind;
  int line = 1;
};

using StmtPtr = unique_ptr<Stmt>;

struct DeclStmt : Stmt {
  DeclStmt(int line, bool c, BaseType b)
      : Stmt(StmtKind::Decl, line), isConst(c), base(b) {}
  bool isConst = false;
  BaseType base = BaseType::Int;
  vector<VarDef> defs;
};

struct BlockStmt : Stmt {
  explicit BlockStmt(int line) : Stmt(StmtKind::Block, line) {}
  vector<StmtPtr> items;
};

struct AssignStmt : Stmt {
  AssignStmt(int line, unique_ptr<LValExpr> lhs, ExprPtr rhs)
      : Stmt(StmtKind::Assign, line), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  unique_ptr<LValExpr> lhs;
  ExprPtr rhs;
};

struct ExprStmt : Stmt {
  ExprStmt(int line, ExprPtr expr) : Stmt(StmtKind::Expr, line), expr(std::move(expr)) {}
  ExprPtr expr;
};

struct IfStmt : Stmt {
  IfStmt(int line, ExprPtr cond, StmtPtr thenStmt, StmtPtr elseStmt)
      : Stmt(StmtKind::If, line), cond(std::move(cond)),
        thenStmt(std::move(thenStmt)), elseStmt(std::move(elseStmt)) {}
  ExprPtr cond;
  StmtPtr thenStmt;
  StmtPtr elseStmt;
};

struct WhileStmt : Stmt {
  WhileStmt(int line, ExprPtr cond, StmtPtr body)
      : Stmt(StmtKind::While, line), cond(std::move(cond)), body(std::move(body)) {}
  ExprPtr cond;
  StmtPtr body;
};

struct BreakStmt : Stmt {
  explicit BreakStmt(int line) : Stmt(StmtKind::Break, line) {}
};

struct ContinueStmt : Stmt {
  explicit ContinueStmt(int line) : Stmt(StmtKind::Continue, line) {}
};

struct ReturnStmt : Stmt {
  ReturnStmt(int line, ExprPtr expr) : Stmt(StmtKind::Return, line), expr(std::move(expr)) {}
  ExprPtr expr;
};

struct Param {
  string name;
  BaseType base = BaseType::Int;
  bool isArray = false;
  vector<ExprPtr> tailDims;
  vector<int> dims;
  Symbol *symbol = nullptr;
  int line = 1;
};

struct FuncDef {
  string name;
  BaseType ret = BaseType::Int;
  vector<Param> params;
  unique_ptr<BlockStmt> body;
  Function *function = nullptr;
  int line = 1;
};

struct TopItem {
  unique_ptr<DeclStmt> decl;
  unique_ptr<FuncDef> func;
};

struct Program {
  vector<TopItem> items;
};

struct Symbol {
  string name;
  string label;
  BaseType base = BaseType::Int;
  bool isConst = false;
  bool isGlobal = false;
  bool isArray = false;
  bool isParam = false;
  bool isParamArray = false;
  bool needsStorage = true;
  vector<int> dims;
  int offset = 0;
  bool hasConstValue = false;
  ConstValue constValue;
  vector<ConstValue> initValues;
};

struct ParamType {
  BaseType base = BaseType::Int;
  bool isArray = false;
  vector<int> dims;
};

struct Function {
  string name;
  string asmName;
  BaseType ret = BaseType::Int;
  vector<ParamType> params;
  bool runtime = false;
  bool injectLineArgument = false;
  bool variadic = false;
  FuncDef *def = nullptr;
  int frameUsed = 16;
  int frameSize = 16;
  string returnLabel;
};

class Parser {
public:
  explicit Parser(vector<Token> tokens) : tokens_(std::move(tokens)) {}

  Program parseProgram() {
    Program program;
    while (!check(TokenKind::End)) {
      TopItem item;
      if (check(TokenKind::KwConst)) {
        item.decl = parseDecl();
      } else if (check(TokenKind::KwVoid) || check(TokenKind::KwInt) ||
                 check(TokenKind::KwFloat)) {
        if (isFuncDefAhead()) {
          item.func = parseFuncDef();
        } else {
          item.decl = parseDecl();
        }
      } else {
        throw error("expected declaration or function definition");
      }
      program.items.push_back(std::move(item));
    }
    return program;
  }

private:
  vector<Token> tokens_;
  size_t pos_ = 0;

  const Token &tok(size_t ahead = 0) const {
    size_t p = min(pos_ + ahead, tokens_.size() - 1);
    return tokens_[p];
  }

  bool check(TokenKind kind) const { return tok().kind == kind; }

  bool match(TokenKind kind) {
    if (!check(kind)) {
      return false;
    }
    ++pos_;
    return true;
  }

  const Token &expect(TokenKind kind, const string &what) {
    if (!check(kind)) {
      throw error("expected " + what);
    }
    return tokens_[pos_++];
  }

  CompileError error(const string &message) const {
    ostringstream os;
    os << "line " << tok().line << ":" << tok().col << ": " << message;
    return CompileError(os.str());
  }

  static bool isBType(TokenKind kind) {
    return kind == TokenKind::KwInt || kind == TokenKind::KwFloat;
  }

  BaseType parseBType() {
    if (match(TokenKind::KwInt)) {
      return BaseType::Int;
    }
    if (match(TokenKind::KwFloat)) {
      return BaseType::Float;
    }
    throw error("expected int or float");
  }

  BaseType parseFuncType() {
    if (match(TokenKind::KwVoid)) {
      return BaseType::Void;
    }
    return parseBType();
  }

  bool isFuncDefAhead() const {
    if (tok().kind == TokenKind::KwConst) {
      return false;
    }
    if (!(tok().kind == TokenKind::KwVoid || isBType(tok().kind))) {
      return false;
    }
    return tok(1).kind == TokenKind::Ident && tok(2).kind == TokenKind::LParen;
  }

  unique_ptr<DeclStmt> parseDecl() {
    bool isConst = match(TokenKind::KwConst);
    BaseType base = parseBType();
    auto decl = make_unique<DeclStmt>(tok().line, isConst, base);
    while (true) {
      decl->defs.push_back(parseVarDef(isConst));
      if (!match(TokenKind::Comma)) {
        break;
      }
    }
    expect(TokenKind::Semicolon, "';'");
    return decl;
  }

  VarDef parseVarDef(bool requireInit) {
    VarDef def;
    const Token &name = expect(TokenKind::Ident, "identifier");
    def.name = name.text;
    def.line = name.line;
    while (match(TokenKind::LBracket)) {
      def.dims.push_back(parseExp());
      expect(TokenKind::RBracket, "']'");
    }
    if (match(TokenKind::Assign)) {
      def.init = parseInitVal();
    } else if (requireInit) {
      throw error("const definition requires initializer");
    }
    return def;
  }

  unique_ptr<InitVal> parseInitVal() {
    auto init = make_unique<InitVal>();
    if (match(TokenKind::LBrace)) {
      init->isList = true;
      if (!check(TokenKind::RBrace)) {
        while (true) {
          init->list.push_back(parseInitVal());
          if (!match(TokenKind::Comma)) {
            break;
          }
        }
      }
      expect(TokenKind::RBrace, "'}'");
    } else {
      init->expr = parseExp();
    }
    return init;
  }

  unique_ptr<FuncDef> parseFuncDef() {
    auto fn = make_unique<FuncDef>();
    fn->line = tok().line;
    fn->ret = parseFuncType();
    const Token &name = expect(TokenKind::Ident, "function name");
    fn->name = name.text;
    expect(TokenKind::LParen, "'('");
    if (!check(TokenKind::RParen)) {
      while (true) {
        fn->params.push_back(parseParam());
        if (!match(TokenKind::Comma)) {
          break;
        }
      }
    }
    expect(TokenKind::RParen, "')'");
    fn->body = parseBlock();
    return fn;
  }

  Param parseParam() {
    Param param;
    param.line = tok().line;
    param.base = parseBType();
    const Token &name = expect(TokenKind::Ident, "parameter name");
    param.name = name.text;
    if (match(TokenKind::LBracket)) {
      param.isArray = true;
      expect(TokenKind::RBracket, "']'");
      while (match(TokenKind::LBracket)) {
        param.tailDims.push_back(parseExp());
        expect(TokenKind::RBracket, "']'");
      }
    }
    return param;
  }

  unique_ptr<BlockStmt> parseBlock() {
    int line = tok().line;
    expect(TokenKind::LBrace, "'{'");
    auto block = make_unique<BlockStmt>(line);
    while (!check(TokenKind::RBrace)) {
      if (check(TokenKind::End)) {
        throw error("unterminated block");
      }
      if (check(TokenKind::KwConst) || isBType(tok().kind)) {
        block->items.push_back(parseDecl());
      } else {
        block->items.push_back(parseStmt());
      }
    }
    expect(TokenKind::RBrace, "'}'");
    return block;
  }

  StmtPtr parseStmt() {
    int line = tok().line;
    if (check(TokenKind::LBrace)) {
      return parseBlock();
    }
    if (match(TokenKind::KwIf)) {
      expect(TokenKind::LParen, "'('");
      auto cond = parseExp();
      expect(TokenKind::RParen, "')'");
      auto thenStmt = parseStmt();
      StmtPtr elseStmt;
      if (match(TokenKind::KwElse)) {
        elseStmt = parseStmt();
      }
      return make_unique<IfStmt>(line, std::move(cond), std::move(thenStmt),
                                 std::move(elseStmt));
    }
    if (match(TokenKind::KwWhile)) {
      expect(TokenKind::LParen, "'('");
      auto cond = parseExp();
      expect(TokenKind::RParen, "')'");
      auto body = parseStmt();
      return make_unique<WhileStmt>(line, std::move(cond), std::move(body));
    }
    if (match(TokenKind::KwBreak)) {
      expect(TokenKind::Semicolon, "';'");
      return make_unique<BreakStmt>(line);
    }
    if (match(TokenKind::KwContinue)) {
      expect(TokenKind::Semicolon, "';'");
      return make_unique<ContinueStmt>(line);
    }
    if (match(TokenKind::KwReturn)) {
      ExprPtr expr;
      if (!check(TokenKind::Semicolon)) {
        expr = parseExp();
      }
      expect(TokenKind::Semicolon, "';'");
      return make_unique<ReturnStmt>(line, std::move(expr));
    }
    if (match(TokenKind::Semicolon)) {
      return make_unique<ExprStmt>(line, nullptr);
    }

    if (check(TokenKind::Ident)) {
      size_t save = pos_;
      auto lhs = parseLVal();
      if (match(TokenKind::Assign)) {
        auto rhs = parseExp();
        expect(TokenKind::Semicolon, "';'");
        return make_unique<AssignStmt>(line, std::move(lhs), std::move(rhs));
      }
      pos_ = save;
    }
    auto expr = parseExp();
    expect(TokenKind::Semicolon, "';'");
    return make_unique<ExprStmt>(line, std::move(expr));
  }

  ExprPtr parseExp() { return parseLOr(); }

  unique_ptr<LValExpr> parseLVal() {
    const Token &name = expect(TokenKind::Ident, "identifier");
    auto expr = make_unique<LValExpr>(name.line, name.text);
    while (match(TokenKind::LBracket)) {
      expr->indices.push_back(parseExp());
      expect(TokenKind::RBracket, "']'");
    }
    return expr;
  }

  ExprPtr parsePrimary() {
    if (match(TokenKind::LParen)) {
      auto expr = parseExp();
      expect(TokenKind::RParen, "')'");
      return expr;
    }
    if (check(TokenKind::IntConst)) {
      const Token &t = tokens_[pos_++];
      return make_unique<NumberExpr>(t.line, t.intVal);
    }
    if (check(TokenKind::FloatConst)) {
      const Token &t = tokens_[pos_++];
      return make_unique<NumberExpr>(t.line, t.floatVal);
    }
    if (check(TokenKind::String)) {
      const Token &t = tokens_[pos_++];
      return make_unique<StringExpr>(t.line, t.text);
    }
    if (check(TokenKind::Ident)) {
      if (tok(1).kind == TokenKind::LParen) {
        const Token &name = tokens_[pos_++];
        auto call = make_unique<CallExpr>(name.line, name.text);
        expect(TokenKind::LParen, "'('");
        if (!check(TokenKind::RParen)) {
          while (true) {
            call->args.push_back(parseExp());
            if (!match(TokenKind::Comma)) {
              break;
            }
          }
        }
        expect(TokenKind::RParen, "')'");
        return call;
      }
      return parseLVal();
    }
    throw error("expected expression");
  }

  ExprPtr parseUnary() {
    if (check(TokenKind::Plus) || check(TokenKind::Minus) ||
        check(TokenKind::Bang)) {
      const Token &opTok = tokens_[pos_++];
      return make_unique<UnaryExpr>(opTok.line, opTok.text, parseUnary());
    }
    return parsePrimary();
  }

  ExprPtr parseMul() {
    auto lhs = parseUnary();
    while (check(TokenKind::Star) || check(TokenKind::Slash) ||
           check(TokenKind::Percent)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseUnary();
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

  ExprPtr parseAdd() {
    auto lhs = parseMul();
    while (check(TokenKind::Plus) || check(TokenKind::Minus)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseMul();
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

  ExprPtr parseRel() {
    auto lhs = parseAdd();
    while (check(TokenKind::Lt) || check(TokenKind::Gt) || check(TokenKind::Le) ||
           check(TokenKind::Ge)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseAdd();
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

  ExprPtr parseEq() {
    auto lhs = parseRel();
    while (check(TokenKind::EqEq) || check(TokenKind::Neq)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseRel();
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

  ExprPtr parseLAnd() {
    auto lhs = parseEq();
    while (check(TokenKind::AndAnd)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseEq();
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }

  ExprPtr parseLOr() {
    auto lhs = parseLAnd();
    while (check(TokenKind::OrOr)) {
      const Token &op = tokens_[pos_++];
      auto rhs = parseLAnd();
      lhs = make_unique<BinaryExpr>(op.line, op.text, std::move(lhs),
                                    std::move(rhs));
    }
    return lhs;
  }
};

static int product(const vector<int> &dims, size_t from = 0) {
  int64_t result = 1;
  for (size_t i = from; i < dims.size(); ++i) {
    result *= dims[i];
  }
  if (result > numeric_limits<int>::max()) {
    throw CompileError("array is too large");
  }
  return static_cast<int>(result);
}

class Semantic {
public:
  explicit Semantic(Program &program) : program_(program) {}

  void run() {
    addRuntimeFunctions();
    predeclareUserFunctions();
    for (auto &item : program_.items) {
      if (item.decl) {
        visitGlobalDecl(*item.decl);
      } else if (item.func) {
        visitFunction(*item.func);
      }
    }
  }

  const vector<unique_ptr<Symbol>> &symbols() const { return symbols_; }
  const vector<Symbol *> &globalsInOrder() const { return globalsInOrder_; }
  const unordered_map<string, Function *> &functions() const { return functions_; }

private:
  Program &program_;
  vector<unique_ptr<Symbol>> symbols_;
  vector<unique_ptr<Function>> functionStorage_;
  vector<Symbol *> globalsInOrder_;
  unordered_map<string, Symbol *> globals_;
  unordered_map<string, Function *> functions_;
  vector<unordered_map<string, Symbol *>> scopes_;
  Function *currentFunction_ = nullptr;

  [[noreturn]] void fail(int line, const string &message) const {
    ostringstream os;
    os << "line " << line << ": " << message;
    throw CompileError(os.str());
  }

  Symbol *newSymbol() {
    symbols_.push_back(make_unique<Symbol>());
    return symbols_.back().get();
  }

  Function *newFunction(const string &name, BaseType ret) {
    functionStorage_.push_back(make_unique<Function>());
    Function *fn = functionStorage_.back().get();
    fn->name = name;
    fn->asmName = name;
    fn->ret = ret;
    functions_[name] = fn;
    return fn;
  }

  void addRuntime(const string &name, BaseType ret, vector<ParamType> params,
                  string asmName = "", bool injectLine = false,
                  bool variadic = false) {
    Function *fn = newFunction(name, ret);
    fn->runtime = true;
    fn->params = std::move(params);
    fn->asmName = asmName.empty() ? name : std::move(asmName);
    fn->injectLineArgument = injectLine;
    fn->variadic = variadic;
  }

  void addRuntimeFunctions() {
    addRuntime("getint", BaseType::Int, {});
    addRuntime("getch", BaseType::Int, {});
    addRuntime("getfloat", BaseType::Float, {});
    addRuntime("getarray", BaseType::Int,
               {ParamType{BaseType::Int, true, {}}});
    addRuntime("getfarray", BaseType::Int,
               {ParamType{BaseType::Float, true, {}}});
    addRuntime("putint", BaseType::Void, {ParamType{BaseType::Int, false, {}}});
    addRuntime("putch", BaseType::Void, {ParamType{BaseType::Int, false, {}}});
    addRuntime("putfloat", BaseType::Void,
               {ParamType{BaseType::Float, false, {}}});
    addRuntime("putarray", BaseType::Void,
               {ParamType{BaseType::Int, false, {}},
                ParamType{BaseType::Int, true, {}}});
    addRuntime("putfarray", BaseType::Void,
               {ParamType{BaseType::Int, false, {}},
                ParamType{BaseType::Float, true, {}}});
    addRuntime("starttime", BaseType::Void, {ParamType{BaseType::Int, false, {}}},
               "_sysy_starttime", true);
    addRuntime("stoptime", BaseType::Void, {ParamType{BaseType::Int, false, {}}},
               "_sysy_stoptime", true);
    addRuntime("putf", BaseType::Void, {ParamType{BaseType::Int, true, {}}},
               "putf", false, true);
  }

  void predeclareUserFunctions() {
    for (auto &item : program_.items) {
      if (!item.func) {
        continue;
      }
      FuncDef &def = *item.func;
      Function *fn = newFunction(def.name, def.ret);
      fn->def = &def;
      def.function = fn;
      for (const Param &p : def.params) {
        ParamType pt;
        pt.base = p.base;
        pt.isArray = p.isArray;
        fn->params.push_back(std::move(pt));
      }
    }
  }

  void enterScope() { scopes_.push_back({}); }

  void leaveScope() { scopes_.pop_back(); }

  Symbol *lookupVar(const string &name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second;
      }
    }
    auto g = globals_.find(name);
    if (g != globals_.end()) {
      return g->second;
    }
    return nullptr;
  }

  void addLocal(Symbol *sym, int line) {
    if (scopes_.empty()) {
      fail(line, "internal scope error");
    }
    auto &scope = scopes_.back();
    if (scope.count(sym->name)) {
      fail(line, "redefinition of " + sym->name);
    }
    scope[sym->name] = sym;
  }

  int allocFrame(int size, int align) {
    currentFunction_->frameUsed = alignTo(currentFunction_->frameUsed, align);
    currentFunction_->frameUsed += size;
    return -currentFunction_->frameUsed;
  }

  void allocateSymbolStorage(Symbol *sym) {
    if (sym->isGlobal || !sym->needsStorage) {
      return;
    }
    if (sym->isParamArray) {
      sym->offset = allocFrame(8, 8);
      return;
    }
    int size = 4;
    int align = 4;
    if (sym->isArray) {
      size = product(sym->dims) * 4;
    }
    sym->offset = allocFrame(size, align);
  }

  Symbol *declareSymbol(const string &name, BaseType base, bool isConst,
                        bool isGlobal, bool isArray, bool isParam,
                        bool isParamArray, const vector<int> &dims, int line) {
    Symbol *sym = newSymbol();
    sym->name = name;
    sym->label = name;
    sym->base = base;
    sym->isConst = isConst;
    sym->isGlobal = isGlobal;
    sym->isArray = isArray || isParamArray;
    sym->isParam = isParam;
    sym->isParamArray = isParamArray;
    sym->dims = dims;
    sym->needsStorage = !(isConst && !sym->isArray && !isParam);
    if (isGlobal) {
      if (globals_.count(name)) {
        fail(line, "redefinition of " + name);
      }
      globals_[name] = sym;
      if (sym->needsStorage) {
        globalsInOrder_.push_back(sym);
      }
    } else {
      allocateSymbolStorage(sym);
      addLocal(sym, line);
    }
    return sym;
  }

  int evalConstInt(Expr *expr) {
    visitExpr(expr);
    if (!expr->isConst) {
      fail(expr->line, "expected constant expression");
    }
    return constAsInt(expr->constVal);
  }

  vector<int> evalDims(vector<ExprPtr> &dims) {
    vector<int> out;
    for (auto &dimExpr : dims) {
      int value = evalConstInt(dimExpr.get());
      if (value < 0) {
        fail(dimExpr->line, "array dimension must be non-negative");
      }
      out.push_back(value);
    }
    return out;
  }

  void visitGlobalDecl(DeclStmt &decl) {
    for (VarDef &def : decl.defs) {
      vector<int> dims = evalDims(def.dims);
      bool isArray = !dims.empty();
      Symbol *sym = declareSymbol(def.name, decl.base, decl.isConst, true,
                                  isArray, false, false, dims, def.line);
      def.symbol = sym;
      if (isArray) {
        if (def.init) {
          sym->initValues = flattenConstInit(def.init.get(), dims, decl.base);
        } else {
          sym->initValues.assign(product(dims), zeroConst(decl.base));
        }
      } else {
        ConstValue value = zeroConst(decl.base);
        if (def.init) {
          if (def.init->isList || !def.init->expr) {
            fail(def.line, "scalar initializer must be an expression");
          }
          visitExpr(def.init->expr.get());
          if (!def.init->expr->isConst) {
            fail(def.line, "global initializer must be constant");
          }
          value = castConst(def.init->expr->constVal, decl.base);
        }
        sym->hasConstValue = decl.isConst;
        sym->constValue = value;
        if (sym->needsStorage) {
          sym->initValues = {value};
        }
      }
    }
  }

  ConstValue zeroConst(BaseType base) {
    ConstValue v;
    v.type = base;
    v.i = 0;
    v.f = 0.0f;
    return v;
  }

  vector<ConstValue> flattenConstInit(InitVal *init, const vector<int> &dims,
                                      BaseType base) {
    vector<ConstValue> values(product(dims), zeroConst(base));
    if (!init) {
      return values;
    }
    fillConstAggregate(init, dims, 0, 0, values, base);
    return values;
  }

  int fillConstAggregate(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<ConstValue> &values, BaseType base) {
    if (!init->isList) {
      if (start >= static_cast<int>(values.size())) {
        fail(init->expr ? init->expr->line : 0, "too many initializer elements");
      }
      visitExpr(init->expr.get());
      if (!init->expr->isConst) {
        fail(init->expr->line, "initializer is not constant");
      }
      values[start] = castConst(init->expr->constVal, base);
      return start + 1;
    }

    int subSize = depth >= dims.size() ? 1 : product(dims, depth);
    if (init->list.empty()) {
      return start + subSize;
    }
    int idx = start;
    for (auto &child : init->list) {
      if (child->isList) {
        size_t childDepth = chooseInitChildDepth(dims, depth, idx);
        idx = fillConstAggregate(child.get(), dims, childDepth, idx, values, base);
      } else {
        idx = fillConstAggregate(child.get(), dims, dims.size(), idx, values, base);
      }
      if (idx > start + subSize) {
        fail(child->expr ? child->expr->line : 0, "too many initializer elements");
      }
    }
    return start + subSize;
  }

  size_t chooseInitChildDepth(const vector<int> &dims, size_t depth, int flatIndex) {
    size_t childDepth = min(depth + 1, dims.size());
    while (childDepth < dims.size()) {
      int childSize = product(dims, childDepth);
      if (childSize == 0 || flatIndex % childSize == 0) {
        break;
      }
      ++childDepth;
    }
    return childDepth;
  }

  void visitFunction(FuncDef &def) {
    currentFunction_ = def.function;
    currentFunction_->frameUsed = 16;
    enterScope();
    for (size_t i = 0; i < def.params.size(); ++i) {
      Param &p = def.params[i];
      vector<int> dims = evalDims(p.tailDims);
      p.dims = dims;
      currentFunction_->params[i].dims = dims;
      Symbol *sym = declareSymbol(p.name, p.base, false, false, p.isArray,
                                  true, p.isArray, dims, p.line);
      p.symbol = sym;
    }
    visitBlock(*def.body, true);
    leaveScope();
    currentFunction_->frameSize = alignTo(currentFunction_->frameUsed, 16);
    currentFunction_ = nullptr;
  }

  void visitBlock(BlockStmt &block, bool createScope) {
    if (createScope) {
      enterScope();
    }
    for (auto &item : block.items) {
      visitStmt(item.get());
    }
    if (createScope) {
      leaveScope();
    }
  }

  void visitStmt(Stmt *stmt) {
    switch (stmt->kind) {
    case StmtKind::Decl:
      visitLocalDecl(*static_cast<DeclStmt *>(stmt));
      break;
    case StmtKind::Block:
      visitBlock(*static_cast<BlockStmt *>(stmt), true);
      break;
    case StmtKind::Assign: {
      auto *s = static_cast<AssignStmt *>(stmt);
      visitExpr(s->lhs.get());
      visitExpr(s->rhs.get());
      break;
    }
    case StmtKind::Expr: {
      auto *s = static_cast<ExprStmt *>(stmt);
      if (s->expr) {
        visitExpr(s->expr.get());
      }
      break;
    }
    case StmtKind::If: {
      auto *s = static_cast<IfStmt *>(stmt);
      visitExpr(s->cond.get());
      visitStmt(s->thenStmt.get());
      if (s->elseStmt) {
        visitStmt(s->elseStmt.get());
      }
      break;
    }
    case StmtKind::While: {
      auto *s = static_cast<WhileStmt *>(stmt);
      visitExpr(s->cond.get());
      visitStmt(s->body.get());
      break;
    }
    case StmtKind::Return: {
      auto *s = static_cast<ReturnStmt *>(stmt);
      if (s->expr) {
        visitExpr(s->expr.get());
      }
      break;
    }
    case StmtKind::Break:
    case StmtKind::Continue:
      break;
    }
  }

  void visitLocalDecl(DeclStmt &decl) {
    for (VarDef &def : decl.defs) {
      vector<int> dims = evalDims(def.dims);
      bool isArray = !dims.empty();
      Symbol *sym = declareSymbol(def.name, decl.base, decl.isConst, false,
                                  isArray, false, false, dims, def.line);
      def.symbol = sym;
      if (def.init) {
        visitInit(def.init.get());
      }
      if (decl.isConst && !isArray) {
        if (!def.init || def.init->isList || !def.init->expr ||
            !def.init->expr->isConst) {
          fail(def.line, "const scalar requires constant initializer");
        }
        sym->hasConstValue = true;
        sym->constValue = castConst(def.init->expr->constVal, decl.base);
      }
      if (decl.isConst && isArray) {
        if (!def.init) {
          fail(def.line, "const array requires initializer");
        }
        sym->initValues = flattenConstInit(def.init.get(), dims, decl.base);
      }
    }
  }

  void visitInit(InitVal *init) {
    if (init->isList) {
      for (auto &child : init->list) {
        visitInit(child.get());
      }
    } else if (init->expr) {
      visitExpr(init->expr.get());
    }
  }

  void visitExpr(Expr *expr) {
    switch (expr->kind) {
    case ExprKind::Number: {
      auto *n = static_cast<NumberExpr *>(expr);
      expr->isConst = true;
      if (n->isFloat) {
        expr->type = Type::scalar(BaseType::Float);
        expr->constVal.type = BaseType::Float;
        expr->constVal.f = n->floatVal;
      } else {
        expr->type = Type::scalar(BaseType::Int);
        expr->constVal.type = BaseType::Int;
        expr->constVal.i = n->intVal;
      }
      break;
    }
    case ExprKind::String: {
      expr->type = Type::pointer(BaseType::Int);
      expr->isConst = false;
      break;
    }
    case ExprKind::LVal:
      visitLVal(static_cast<LValExpr *>(expr));
      break;
    case ExprKind::Call:
      visitCall(static_cast<CallExpr *>(expr));
      break;
    case ExprKind::Unary:
      visitUnary(static_cast<UnaryExpr *>(expr));
      break;
    case ExprKind::Binary:
      visitBinary(static_cast<BinaryExpr *>(expr));
      break;
    }
  }

  void visitLVal(LValExpr *expr) {
    Symbol *sym = lookupVar(expr->name);
    if (!sym) {
      fail(expr->line, "undefined variable " + expr->name);
    }
    expr->symbol = sym;
    for (auto &idx : expr->indices) {
      visitExpr(idx.get());
    }
    int rank = sym->isArray ? static_cast<int>(sym->dims.size()) + (sym->isParamArray ? 1 : 0) : 0;
    if (static_cast<int>(expr->indices.size()) < rank) {
      vector<int> remaining;
      if (sym->isParamArray) {
        int consumed = static_cast<int>(expr->indices.size());
        for (int i = max(0, consumed - 1); i < static_cast<int>(sym->dims.size()); ++i) {
          remaining.push_back(sym->dims[i]);
        }
      } else {
        for (size_t i = expr->indices.size(); i < sym->dims.size(); ++i) {
          remaining.push_back(sym->dims[i]);
        }
      }
      expr->type = Type::pointer(sym->base, remaining);
    } else {
      expr->type = Type::scalar(sym->base);
    }

    if (sym->hasConstValue && expr->indices.empty()) {
      expr->isConst = true;
      expr->constVal = sym->constValue;
    } else if (sym->isConst && sym->isArray && !expr->type.isPointer &&
               !sym->initValues.empty()) {
      bool allConst = true;
      vector<int> idxValues;
      for (auto &idx : expr->indices) {
        if (!idx->isConst) {
          allConst = false;
          break;
        }
        idxValues.push_back(constAsInt(idx->constVal));
      }
      if (allConst) {
        int flat = flattenIndex(*sym, idxValues);
        if (flat >= 0 && flat < static_cast<int>(sym->initValues.size())) {
          expr->isConst = true;
          expr->constVal = sym->initValues[flat];
        }
      }
    }
  }

  int flattenIndex(const Symbol &sym, const vector<int> &idxValues) {
    int flat = 0;
    if (sym.isParamArray) {
      vector<int> fullDims = sym.dims;
      for (size_t i = 0; i < idxValues.size(); ++i) {
        int stride = i == 0 ? product(fullDims, 0)
                            : (i - 1 < fullDims.size() ? product(fullDims, i) : 1);
        flat += idxValues[i] * stride;
      }
      return flat;
    }
    for (size_t i = 0; i < idxValues.size(); ++i) {
      int stride = i + 1 < sym.dims.size() ? product(sym.dims, i + 1) : 1;
      flat += idxValues[i] * stride;
    }
    return flat;
  }

  void visitCall(CallExpr *expr) {
    auto it = functions_.find(expr->name);
    if (it == functions_.end()) {
      fail(expr->line, "undefined function " + expr->name);
    }
    expr->function = it->second;
    for (auto &arg : expr->args) {
      visitExpr(arg.get());
    }
    expr->type = Type::scalar(expr->function->ret);
  }

  void visitUnary(UnaryExpr *expr) {
    visitExpr(expr->expr.get());
    expr->isConst = expr->expr->isConst;
    if (expr->op == "!") {
      expr->type = Type::scalar(BaseType::Int);
      if (expr->isConst) {
        expr->constVal.type = BaseType::Int;
        if (expr->expr->constVal.type == BaseType::Float) {
          expr->constVal.i = constAsFloat(expr->expr->constVal) == 0.0f;
        } else {
          expr->constVal.i = constAsInt(expr->expr->constVal) == 0;
        }
      }
      return;
    }
    expr->type = expr->expr->type;
    if (expr->isConst) {
      if (expr->op == "+") {
        expr->constVal = expr->expr->constVal;
      } else if (expr->op == "-") {
        expr->constVal = expr->expr->constVal;
        if (expr->constVal.type == BaseType::Float) {
          expr->constVal.f = -expr->constVal.f;
        } else {
          expr->constVal.i = -expr->constVal.i;
        }
      }
    }
  }

  void visitBinary(BinaryExpr *expr) {
    visitExpr(expr->lhs.get());
    visitExpr(expr->rhs.get());
    const string &op = expr->op;
    if (op == "&&" || op == "||" || op == "==" || op == "!=" || op == "<" ||
        op == ">" || op == "<=" || op == ">=") {
      expr->type = Type::scalar(BaseType::Int);
    } else if (op == "%") {
      expr->type = Type::scalar(BaseType::Int);
    } else if (expr->lhs->type.isFloatScalar() || expr->rhs->type.isFloatScalar()) {
      expr->type = Type::scalar(BaseType::Float);
    } else {
      expr->type = Type::scalar(BaseType::Int);
    }

    if (expr->lhs->isConst && expr->rhs->isConst) {
      expr->isConst = true;
      expr->constVal = evalBinaryConst(op, expr->lhs->constVal, expr->rhs->constVal,
                                       expr->type.base);
    }
  }

  ConstValue evalBinaryConst(const string &op, ConstValue lhs, ConstValue rhs,
                             BaseType resultType) {
    ConstValue out;
    out.type = resultType;
    bool useFloat = lhs.type == BaseType::Float || rhs.type == BaseType::Float;
    if (op == "&&" || op == "||") {
      bool l = lhs.type == BaseType::Float ? constAsFloat(lhs) != 0.0f
                                           : constAsInt(lhs) != 0;
      bool r = rhs.type == BaseType::Float ? constAsFloat(rhs) != 0.0f
                                           : constAsInt(rhs) != 0;
      out.i = op == "&&" ? (l && r) : (l || r);
      return out;
    }
    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" ||
        op == ">=") {
      if (useFloat) {
        float l = constAsFloat(lhs), r = constAsFloat(rhs);
        if (op == "==") out.i = l == r;
        if (op == "!=") out.i = l != r;
        if (op == "<") out.i = l < r;
        if (op == ">") out.i = l > r;
        if (op == "<=") out.i = l <= r;
        if (op == ">=") out.i = l >= r;
      } else {
        int32_t l = constAsInt(lhs), r = constAsInt(rhs);
        if (op == "==") out.i = l == r;
        if (op == "!=") out.i = l != r;
        if (op == "<") out.i = l < r;
        if (op == ">") out.i = l > r;
        if (op == "<=") out.i = l <= r;
        if (op == ">=") out.i = l >= r;
      }
      return out;
    }
    if (resultType == BaseType::Float) {
      float l = constAsFloat(lhs), r = constAsFloat(rhs);
      if (op == "+") out.f = l + r;
      if (op == "-") out.f = l - r;
      if (op == "*") out.f = l * r;
      if (op == "/") out.f = l / r;
      return out;
    }
    int32_t l = constAsInt(lhs), r = constAsInt(rhs);
    if (op == "+") out.i = l + r;
    if (op == "-") out.i = l - r;
    if (op == "*") out.i = l * r;
    if (op == "/") out.i = r == 0 ? 0 : l / r;
    if (op == "%") out.i = r == 0 ? 0 : l % r;
    return out;
  }
};

struct ArgLoc {
  enum Kind { IntReg, FloatReg, Stack } kind = IntReg;
  int index = 0;
  bool valueIsFloat = false;
};

static vector<ArgLoc> computeArgLocations(const vector<ParamType> &params,
                                          int *stackSlots = nullptr) {
  vector<ArgLoc> locs;
  int intRegs = 0;
  int floatRegs = 0;
  int stack = 0;
  for (const ParamType &p : params) {
    ArgLoc loc;
    loc.valueIsFloat = !p.isArray && p.base == BaseType::Float;
    if (loc.valueIsFloat && floatRegs < 8) {
      loc.kind = ArgLoc::FloatReg;
      loc.index = floatRegs++;
    } else {
      if (intRegs < 8) {
        loc.kind = ArgLoc::IntReg;
        loc.index = intRegs++;
      } else {
        loc.kind = ArgLoc::Stack;
        loc.index = stack++;
      }
    }
    locs.push_back(loc);
  }
  if (stackSlots) {
    *stackSlots = stack;
  }
  return locs;
}

class CodeGen {
public:
  CodeGen(Program &program, const Semantic &semantic)
      : program_(program), semantic_(semantic) {}

  string run() {
    emitGlobals();
    emit("\t.text");
    for (auto &item : program_.items) {
      if (item.func) {
        emitFunction(*item.func);
      }
    }
    emitLiteralPools();
    return out_.str();
  }

private:
  Program &program_;
  const Semantic &semantic_;
  ostringstream out_;
  int labelId_ = 0;
  Function *currentFunction_ = nullptr;
  vector<string> breakLabels_;
  vector<string> continueLabels_;
  vector<pair<string, float>> floatLiterals_;
  vector<pair<string, string>> stringLiterals_;

  void emit(const string &line = "") { out_ << line << '\n'; }

  string newLabel(const string &prefix) {
    return ".L" + prefix + "_" + to_string(labelId_++);
  }

  static bool fitsImm12(int value) { return value >= -2048 && value <= 2047; }

  void emitAdjustSp(int delta) {
    if (delta == 0) {
      return;
    }
    if (fitsImm12(delta)) {
      emit("\taddi\tsp, sp, " + to_string(delta));
    } else {
      emit("\tli\tt6, " + to_string(delta));
      emit("\tadd\tsp, sp, t6");
    }
  }

  void emitAddOffset(const string &dst, const string &base, int offset) {
    if (fitsImm12(offset)) {
      emit("\taddi\t" + dst + ", " + base + ", " + to_string(offset));
    } else {
      emit("\tli\t" + dst + ", " + to_string(offset));
      emit("\tadd\t" + dst + ", " + base + ", " + dst);
    }
  }

  void emitLoadMem(const string &inst, const string &dst, const string &base,
                   int offset) {
    if (fitsImm12(offset)) {
      emit("\t" + inst + "\t" + dst + ", " + to_string(offset) + "(" + base + ")");
    } else {
      emit("\tli\tt6, " + to_string(offset));
      emit("\tadd\tt6, " + base + ", t6");
      emit("\t" + inst + "\t" + dst + ", 0(t6)");
    }
  }

  void emitStoreMem(const string &inst, const string &src, const string &base,
                    int offset) {
    if (fitsImm12(offset)) {
      emit("\t" + inst + "\t" + src + ", " + to_string(offset) + "(" + base + ")");
    } else {
      emit("\tli\tt6, " + to_string(offset));
      emit("\tadd\tt6, " + base + ", t6");
      emit("\t" + inst + "\t" + src + ", 0(t6)");
    }
  }

  void emitGlobals() {
    for (Symbol *sym : semantic_.globalsInOrder()) {
      bool allZero = true;
      for (const ConstValue &v : sym->initValues) {
        if ((v.type == BaseType::Float && floatBits(v.f) != 0) ||
            (v.type == BaseType::Int && v.i != 0)) {
          allZero = false;
          break;
        }
      }
      if (sym->isConst) {
        emit("\t.section\t.rodata");
      } else if (allZero) {
        emit("\t.bss");
      } else {
        emit("\t.data");
      }
      emit("\t.align\t2");
      emit("\t.globl\t" + sym->label);
      emit(sym->label + ":");
      int count = sym->isArray ? product(sym->dims) : 1;
      if (allZero) {
        emit("\t.zero\t" + to_string(count * 4));
      } else {
        for (int i = 0; i < count; ++i) {
          ConstValue v = i < static_cast<int>(sym->initValues.size())
                             ? sym->initValues[i]
                             : ConstValue{sym->base, 0, 0.0f};
          if (sym->base == BaseType::Float) {
            emit("\t.word\t" + to_string(floatBits(constAsFloat(v))));
          } else {
            emit("\t.word\t" + to_string(static_cast<int32_t>(constAsInt(v))));
          }
        }
      }
    }
  }

  void emitFunction(FuncDef &def) {
    currentFunction_ = def.function;
    currentFunction_->returnLabel = newLabel("return_" + def.name);
    emit("\t.text");
    emit("\t.align\t1");
    emit("\t.globl\t" + def.name);
    emit("\t.type\t" + def.name + ", @function");
    emit(def.name + ":");
    int frame = currentFunction_->frameSize;
    emitAdjustSp(-frame);
    emit("\tli\tt0, " + to_string(frame));
    emit("\tadd\tt0, sp, t0");
    emit("\tsd\tra, -8(t0)");
    emit("\tsd\ts0, -16(t0)");
    emit("\tmv\ts0, t0");
    emitStoreParams(def);
    emitBlock(*def.body, false);
    if (def.ret == BaseType::Int) {
      emit("\tli\ta0, 0");
    } else if (def.ret == BaseType::Float) {
      emit("\tfmv.w.x\tfa0, zero");
    }
    emit(currentFunction_->returnLabel + ":");
    emit("\tld\tra, -8(s0)");
    emit("\tld\tt0, -16(s0)");
    emit("\tmv\tsp, s0");
    emit("\tmv\ts0, t0");
    emit("\tret");
    emit("\t.size\t" + def.name + ", .-" + def.name);
    currentFunction_ = nullptr;
  }

  void emitStoreParams(FuncDef &def) {
    vector<ParamType> types;
    for (const Param &p : def.params) {
      types.push_back(ParamType{p.base, p.isArray, p.dims});
    }
    auto locs = computeArgLocations(types);
    for (size_t i = 0; i < def.params.size(); ++i) {
      const Param &p = def.params[i];
      const ArgLoc &loc = locs[i];
      int off = p.symbol->offset;
      if (p.isArray) {
        if (loc.kind == ArgLoc::IntReg) {
          emitStoreMem("sd", "a" + to_string(loc.index), "s0", off);
        } else {
          emitLoadMem("ld", "t0", "s0", loc.index * 8);
          emitStoreMem("sd", "t0", "s0", off);
        }
      } else if (p.base == BaseType::Float) {
        if (loc.kind == ArgLoc::FloatReg) {
          emitStoreMem("fsw", "fa" + to_string(loc.index), "s0", off);
        } else if (loc.kind == ArgLoc::IntReg) {
          emitStoreMem("sw", "a" + to_string(loc.index), "s0", off);
        } else {
          emitLoadMem("lw", "t0", "s0", loc.index * 8);
          emitStoreMem("sw", "t0", "s0", off);
        }
      } else {
        if (loc.kind == ArgLoc::IntReg) {
          emitStoreMem("sw", "a" + to_string(loc.index), "s0", off);
        } else {
          emitLoadMem("lw", "t0", "s0", loc.index * 8);
          emitStoreMem("sw", "t0", "s0", off);
        }
      }
    }
  }

  void emitBlock(BlockStmt &block, bool) {
    for (auto &stmt : block.items) {
      emitStmt(stmt.get());
    }
  }

  void emitStmt(Stmt *stmt) {
    switch (stmt->kind) {
    case StmtKind::Decl:
      emitDecl(*static_cast<DeclStmt *>(stmt));
      break;
    case StmtKind::Block:
      emitBlock(*static_cast<BlockStmt *>(stmt), true);
      break;
    case StmtKind::Assign:
      emitAssign(*static_cast<AssignStmt *>(stmt));
      break;
    case StmtKind::Expr: {
      ExprStmt *s = static_cast<ExprStmt *>(stmt);
      if (s->expr) {
        emitExpr(s->expr.get());
      }
      break;
    }
    case StmtKind::If:
      emitIf(*static_cast<IfStmt *>(stmt));
      break;
    case StmtKind::While:
      emitWhile(*static_cast<WhileStmt *>(stmt));
      break;
    case StmtKind::Break:
      if (breakLabels_.empty()) {
        throw CompileError("break outside loop");
      }
      emit("\tj\t" + breakLabels_.back());
      break;
    case StmtKind::Continue:
      if (continueLabels_.empty()) {
        throw CompileError("continue outside loop");
      }
      emit("\tj\t" + continueLabels_.back());
      break;
    case StmtKind::Return:
      emitReturn(*static_cast<ReturnStmt *>(stmt));
      break;
    }
  }

  void emitDecl(DeclStmt &decl) {
    for (VarDef &def : decl.defs) {
      Symbol *sym = def.symbol;
      if (!sym->needsStorage) {
        continue;
      }
      if (!sym->isArray) {
        if (def.init && !def.init->isList) {
          emitExpr(def.init->expr.get());
          emitConvert(def.init->expr->type, Type::scalar(sym->base));
          emitStoreToAddress(sym->base, [this, sym]() {
            emitAddOffset("a1", "s0", sym->offset);
          });
        }
        continue;
      }
      if (def.init) {
        int total = product(sym->dims);
        string zeroLoop = newLabel("zero_array");
        string zeroEnd = newLabel("zero_array_end");
        emitAddOffset("t0", "s0", sym->offset);
        emit("\tli\tt1, " + to_string(total * 4));
        emit(zeroLoop + ":");
        emit("\tbeqz\tt1, " + zeroEnd);
        emit("\tsw\tzero, 0(t0)");
        emit("\taddi\tt0, t0, 4");
        emit("\taddi\tt1, t1, -4");
        emit("\tj\t" + zeroLoop);
        emit(zeroEnd + ":");
        vector<InitVal *> flat(total, nullptr);
        flattenRuntimeInit(def.init.get(), sym->dims, 0, 0, flat);
        for (int i = 0; i < total; ++i) {
          if (!flat[i]) {
            continue;
          }
          emitExpr(flat[i]->expr.get());
          emitConvert(flat[i]->expr->type, Type::scalar(sym->base));
          emitStoreToAddress(sym->base, [this, sym, i]() {
            emitAddOffset("a1", "s0", sym->offset + i * 4);
          });
        }
      }
    }
  }

  int flattenRuntimeInit(InitVal *init, const vector<int> &dims, size_t depth,
                         int start, vector<InitVal *> &flat) {
    if (!init->isList) {
      if (start < static_cast<int>(flat.size())) {
        flat[start] = init;
      }
      return start + 1;
    }
    int subSize = depth >= dims.size() ? 1 : product(dims, depth);
    if (init->list.empty()) {
      return start + subSize;
    }
    int idx = start;
    for (auto &child : init->list) {
      if (child->isList) {
        size_t childDepth = chooseRuntimeInitChildDepth(dims, depth, idx);
        idx = flattenRuntimeInit(child.get(), dims, childDepth, idx, flat);
      } else {
        idx = flattenRuntimeInit(child.get(), dims, dims.size(), idx, flat);
      }
    }
    return start + subSize;
  }

  size_t chooseRuntimeInitChildDepth(const vector<int> &dims, size_t depth,
                                     int flatIndex) {
    size_t childDepth = min(depth + 1, dims.size());
    while (childDepth < dims.size()) {
      int childSize = product(dims, childDepth);
      if (childSize == 0 || flatIndex % childSize == 0) {
        break;
      }
      ++childDepth;
    }
    return childDepth;
  }

  template <class AddressEmitter>
  void emitStoreToAddress(BaseType base, AddressEmitter emitAddressToA1) {
    if (base == BaseType::Float) {
      emitPushFloat("fa0");
      emitAddressToA1();
      emitPopFloat("fa0");
      emit("\tfsw\tfa0, 0(a1)");
    } else {
      emitPushInt("a0");
      emitAddressToA1();
      emitPopInt("a0");
      emit("\tsw\ta0, 0(a1)");
    }
  }

  void emitAssign(AssignStmt &stmt) {
    emitLValAddress(stmt.lhs.get());
    emitPushInt("a0");
    emitExpr(stmt.rhs.get());
    emitConvert(stmt.rhs->type, Type::scalar(stmt.lhs->symbol->base));
    if (stmt.lhs->symbol->base == BaseType::Float) {
      emitPopInt("a1");
      emit("\tfsw\tfa0, 0(a1)");
    } else {
      emitPushInt("a0");
      emitPopInt("t0");
      emitPopInt("a1");
      emit("\tsw\tt0, 0(a1)");
    }
  }

  void emitIf(IfStmt &stmt) {
    string thenLabel = newLabel("if_then");
    string elseLabel = newLabel("if_else");
    string endLabel = newLabel("if_end");
    emitCond(stmt.cond.get(), thenLabel, stmt.elseStmt ? elseLabel : endLabel);
    emit(thenLabel + ":");
    emitStmt(stmt.thenStmt.get());
    emit("\tj\t" + endLabel);
    if (stmt.elseStmt) {
      emit(elseLabel + ":");
      emitStmt(stmt.elseStmt.get());
    }
    emit(endLabel + ":");
  }

  void emitWhile(WhileStmt &stmt) {
    string condLabel = newLabel("while_cond");
    string bodyLabel = newLabel("while_body");
    string endLabel = newLabel("while_end");
    continueLabels_.push_back(condLabel);
    breakLabels_.push_back(endLabel);
    emit(condLabel + ":");
    emitCond(stmt.cond.get(), bodyLabel, endLabel);
    emit(bodyLabel + ":");
    emitStmt(stmt.body.get());
    emit("\tj\t" + condLabel);
    emit(endLabel + ":");
    breakLabels_.pop_back();
    continueLabels_.pop_back();
  }

  void emitReturn(ReturnStmt &stmt) {
    if (stmt.expr) {
      emitExpr(stmt.expr.get());
      emitConvert(stmt.expr->type, Type::scalar(currentFunction_->ret));
    }
    emit("\tj\t" + currentFunction_->returnLabel);
  }

  void emitExpr(Expr *expr) {
    if (expr->isConst && expr->kind != ExprKind::LVal) {
      emitConst(expr->constVal);
      return;
    }
    switch (expr->kind) {
    case ExprKind::Number:
      emitConst(expr->constVal);
      break;
    case ExprKind::String:
      emitStringExpr(static_cast<StringExpr *>(expr));
      break;
    case ExprKind::LVal:
      emitLValValue(static_cast<LValExpr *>(expr));
      break;
    case ExprKind::Call:
      emitCall(static_cast<CallExpr *>(expr));
      break;
    case ExprKind::Unary:
      emitUnary(static_cast<UnaryExpr *>(expr));
      break;
    case ExprKind::Binary:
      emitBinary(static_cast<BinaryExpr *>(expr));
      break;
    }
  }

  void emitConst(ConstValue v) {
    if (v.type == BaseType::Float) {
      emitFloatConst(v.f);
    } else {
      emit("\tli\ta0, " + to_string(v.i));
    }
  }

  void emitFloatConst(float value) {
    uint32_t bits = floatBits(value);
    if (bits == 0) {
      emit("\tfmv.w.x\tfa0, zero");
      return;
    }
    string label = newLabel("float");
    floatLiterals_.push_back({label, value});
    emit("\tlla\tt0, " + label);
    emit("\tflw\tfa0, 0(t0)");
  }

  void emitStringExpr(StringExpr *expr) {
    if (expr->label.empty()) {
      expr->label = newLabel("str");
      stringLiterals_.push_back({expr->label, expr->value});
    }
    emit("\tlla\ta0, " + expr->label);
  }

  void emitUnary(UnaryExpr *expr) {
    emitExpr(expr->expr.get());
    if (expr->op == "+") {
      return;
    }
    if (expr->op == "-") {
      if (expr->type.base == BaseType::Float) {
        emit("\tfneg.s\tfa0, fa0");
      } else {
        emit("\tnegw\ta0, a0");
      }
      return;
    }
    if (expr->op == "!") {
      emitBoolFromValue(expr->expr->type);
      emit("\tseqz\ta0, a0");
    }
  }

  void emitBinary(BinaryExpr *expr) {
    const string &op = expr->op;
    if (op == "&&" || op == "||") {
      string trueLabel = newLabel("logic_true");
      string falseLabel = newLabel("logic_false");
      string endLabel = newLabel("logic_end");
      emitCond(expr, trueLabel, falseLabel);
      emit(trueLabel + ":");
      emit("\tli\ta0, 1");
      emit("\tj\t" + endLabel);
      emit(falseLabel + ":");
      emit("\tli\ta0, 0");
      emit(endLabel + ":");
      return;
    }

    bool resultFloat = expr->type.base == BaseType::Float;
    bool compare = op == "==" || op == "!=" || op == "<" || op == ">" ||
                   op == "<=" || op == ">=";
    bool useFloat = expr->lhs->type.isFloatScalar() || expr->rhs->type.isFloatScalar();

    emitExpr(expr->lhs.get());
    if (expr->lhs->type.isFloatScalar()) {
      emitPushFloat("fa0");
    } else {
      emitPushInt("a0");
    }
    emitExpr(expr->rhs.get());

    if (useFloat) {
      if (!expr->rhs->type.isFloatScalar()) {
        emit("\tfcvt.s.w\tfa0, a0");
      }
      emitPushFloat("fa0");
      emitPopFloat("ft1");
      if (expr->lhs->type.isFloatScalar()) {
        emitPopFloat("ft0");
      } else {
        emitPopInt("t0");
        emit("\tfcvt.s.w\tft0, t0");
      }
      emit("\tfmv.s\tfa0, ft1");
      if (compare) {
        emitFloatCompare(op);
      } else if (op == "+") {
        emit("\tfadd.s\tfa0, ft0, fa0");
      } else if (op == "-") {
        emit("\tfsub.s\tfa0, ft0, fa0");
      } else if (op == "*") {
        emit("\tfmul.s\tfa0, ft0, fa0");
      } else if (op == "/") {
        emit("\tfdiv.s\tfa0, ft0, fa0");
      }
      (void)resultFloat;
      return;
    }

    emitPushInt("a0");
    emitPopInt("a1");
    emitPopInt("a0");
    if (compare) {
      emitIntCompare(op);
    } else if (op == "+") {
      emit("\taddw\ta0, a0, a1");
    } else if (op == "-") {
      emit("\tsubw\ta0, a0, a1");
    } else if (op == "*") {
      emit("\tmulw\ta0, a0, a1");
    } else if (op == "/") {
      emit("\tdivw\ta0, a0, a1");
    } else if (op == "%") {
      emit("\tremw\ta0, a0, a1");
    }
  }

  void emitIntCompare(const string &op) {
    if (op == "==") {
      emit("\tsubw\ta0, a0, a1");
      emit("\tseqz\ta0, a0");
    } else if (op == "!=") {
      emit("\tsubw\ta0, a0, a1");
      emit("\tsnez\ta0, a0");
    } else if (op == "<") {
      emit("\tslt\ta0, a0, a1");
    } else if (op == ">") {
      emit("\tslt\ta0, a1, a0");
    } else if (op == "<=") {
      emit("\tslt\ta0, a1, a0");
      emit("\tseqz\ta0, a0");
    } else if (op == ">=") {
      emit("\tslt\ta0, a0, a1");
      emit("\tseqz\ta0, a0");
    }
  }

  void emitFloatCompare(const string &op) {
    if (op == "==") {
      emit("\tfeq.s\ta0, ft0, fa0");
    } else if (op == "!=") {
      emit("\tfeq.s\ta0, ft0, fa0");
      emit("\tseqz\ta0, a0");
    } else if (op == "<") {
      emit("\tflt.s\ta0, ft0, fa0");
    } else if (op == ">") {
      emit("\tflt.s\ta0, fa0, ft0");
    } else if (op == "<=") {
      emit("\tfle.s\ta0, ft0, fa0");
    } else if (op == ">=") {
      emit("\tfle.s\ta0, fa0, ft0");
    }
  }

  void emitCond(Expr *expr, const string &trueLabel, const string &falseLabel) {
    if (expr->kind == ExprKind::Binary) {
      auto *bin = static_cast<BinaryExpr *>(expr);
      if (bin->op == "&&") {
        string mid = newLabel("land_mid");
        emitCond(bin->lhs.get(), mid, falseLabel);
        emit(mid + ":");
        emitCond(bin->rhs.get(), trueLabel, falseLabel);
        return;
      }
      if (bin->op == "||") {
        string mid = newLabel("lor_mid");
        emitCond(bin->lhs.get(), trueLabel, mid);
        emit(mid + ":");
        emitCond(bin->rhs.get(), trueLabel, falseLabel);
        return;
      }
    }
    emitExpr(expr);
    emitBoolFromValue(expr->type);
    emit("\tbnez\ta0, " + trueLabel);
    emit("\tj\t" + falseLabel);
  }

  void emitBoolFromValue(const Type &type) {
    if (type.isPointer) {
      emit("\tsnez\ta0, a0");
    } else if (type.base == BaseType::Float) {
      emit("\tfmv.w.x\tft0, zero");
      emit("\tfeq.s\ta0, fa0, ft0");
      emit("\tseqz\ta0, a0");
    } else {
      emit("\tsnez\ta0, a0");
    }
  }

  void emitLValValue(LValExpr *expr) {
    if (expr->isConst && !expr->type.isPointer) {
      emitConst(expr->constVal);
      return;
    }
    emitLValAddress(expr);
    if (expr->type.isPointer) {
      return;
    }
    if (expr->type.base == BaseType::Float) {
      emit("\tflw\tfa0, 0(a0)");
    } else {
      emit("\tlw\ta0, 0(a0)");
    }
  }

  void emitLValAddress(LValExpr *expr) {
    Symbol *sym = expr->symbol;
    if (sym->isGlobal) {
      emit("\tlla\ta0, " + sym->label);
    } else if (sym->isParamArray) {
      emitLoadMem("ld", "a0", "s0", sym->offset);
    } else {
      emitAddOffset("a0", "s0", sym->offset);
    }
    if (expr->indices.empty()) {
      return;
    }
    emitPushInt("a0");
    emit("\tli\ta0, 0");
    emitPushInt("a0");
    for (size_t i = 0; i < expr->indices.size(); ++i) {
      emitExpr(expr->indices[i].get());
      emitConvert(expr->indices[i]->type, Type::scalar(BaseType::Int));
      int strideBytes = strideForIndex(sym, i) * 4;
      emit("\tli\tt0, " + to_string(strideBytes));
      emit("\tmul\tt1, a0, t0");
      emit("\tld\tt0, 0(sp)");
      emit("\tadd\tt0, t0, t1");
      emit("\tsd\tt0, 0(sp)");
    }
    emitPopInt("t0");
    emitPopInt("a0");
    emit("\tadd\ta0, a0, t0");
  }

  int strideForIndex(Symbol *sym, size_t index) {
    if (sym->isParamArray) {
      if (index == 0) {
        return sym->dims.empty() ? 1 : product(sym->dims, 0);
      }
      size_t tail = index;
      return tail < sym->dims.size() ? product(sym->dims, tail) : 1;
    }
    return index + 1 < sym->dims.size() ? product(sym->dims, index + 1) : 1;
  }

  void emitCall(CallExpr *expr) {
    Function *fn = expr->function;
    vector<ParamType> params;
    vector<Expr *> args;
    if (fn->injectLineArgument) {
      params.push_back(ParamType{BaseType::Int, false, {}});
    }
    for (size_t i = 0; i < expr->args.size(); ++i) {
      args.push_back(expr->args[i].get());
      if (i < fn->params.size()) {
        params.push_back(fn->params[i]);
      } else {
        ParamType inferred;
        inferred.base = expr->args[i]->type.base;
        inferred.isArray = expr->args[i]->type.isPointer;
        params.push_back(inferred);
      }
    }

    int stackSlots = 0;
    auto locs = computeArgLocations(params, &stackSlots);
    int tempSlots = static_cast<int>(params.size());
    int stackBytes = stackSlots * 8;
    int tempBase = stackBytes;
    int area = alignTo(stackBytes + tempSlots * 8, 16);
    if (area > 0) {
      emitAdjustSp(-area);
    }

    int paramIndex = 0;
    if (fn->injectLineArgument) {
      emit("\tli\ta0, " + to_string(expr->line));
      emitStoreMem("sd", "a0", "sp", tempBase);
      ++paramIndex;
    }
    for (Expr *arg : args) {
      ParamType expected = params[paramIndex];
      emitExpr(arg);
      Type target = expected.isArray ? Type::pointer(expected.base, expected.dims)
                                     : Type::scalar(expected.base);
      if (!expected.isArray) {
        emitConvert(arg->type, target);
      }
      int off = tempBase + paramIndex * 8;
      if (target.isPointer) {
        emitStoreMem("sd", "a0", "sp", off);
      } else if (target.base == BaseType::Float) {
        emitStoreMem("fsw", "fa0", "sp", off);
      } else {
        emitStoreMem("sd", "a0", "sp", off);
      }
      ++paramIndex;
    }

    for (size_t i = 0; i < params.size(); ++i) {
      const ParamType &p = params[i];
      const ArgLoc &loc = locs[i];
      int off = tempBase + static_cast<int>(i) * 8;
      bool isFloat = !p.isArray && p.base == BaseType::Float;
      bool isPtr = p.isArray;
      if (loc.kind == ArgLoc::FloatReg) {
        emitLoadMem("flw", "fa" + to_string(loc.index), "sp", off);
      } else if (loc.kind == ArgLoc::IntReg) {
        if (isFloat) {
          emitLoadMem("lw", "a" + to_string(loc.index), "sp", off);
        } else {
          emitLoadMem("ld", "a" + to_string(loc.index), "sp", off);
        }
      } else {
        int dst = loc.index * 8;
        if (isFloat) {
          emitLoadMem("lw", "t0", "sp", off);
          emitStoreMem("sw", "t0", "sp", dst);
        } else {
          emitLoadMem("ld", "t0", "sp", off);
          emitStoreMem("sd", "t0", "sp", dst);
        }
      }
      (void)isPtr;
    }

    emit("\tcall\t" + fn->asmName);
    if (area > 0) {
      emitAdjustSp(area);
    }
  }

  void emitConvert(const Type &from, const Type &to) {
    if (to.base == BaseType::Void || from.isPointer || to.isPointer ||
        from.base == to.base) {
      return;
    }
    if (from.base == BaseType::Int && to.base == BaseType::Float) {
      emit("\tfcvt.s.w\tfa0, a0");
    } else if (from.base == BaseType::Float && to.base == BaseType::Int) {
      emit("\tfcvt.w.s\ta0, fa0, rtz");
    }
  }

  void emitPushInt(const string &reg) {
    emit("\taddi\tsp, sp, -16");
    emit("\tsd\t" + reg + ", 0(sp)");
  }

  void emitPopInt(const string &reg) {
    emit("\tld\t" + reg + ", 0(sp)");
    emit("\taddi\tsp, sp, 16");
  }

  void emitPushFloat(const string &reg) {
    emit("\taddi\tsp, sp, -16");
    emit("\tfsw\t" + reg + ", 0(sp)");
  }

  void emitPopFloat(const string &reg) {
    emit("\tflw\t" + reg + ", 0(sp)");
    emit("\taddi\tsp, sp, 16");
  }

  void emitLiteralPools() {
    if (floatLiterals_.empty() && stringLiterals_.empty()) {
      return;
    }
    emit("\t.section\t.rodata");
    for (const auto &lit : floatLiterals_) {
      emit("\t.align\t2");
      emit(lit.first + ":");
      emit("\t.word\t" + to_string(floatBits(lit.second)));
    }
    for (const auto &lit : stringLiterals_) {
      emit("\t.align\t1");
      emit(lit.first + ":");
      emit("\t.asciz\t\"" + escapeAsmString(lit.second) + "\"");
    }
  }
};

static string readFile(const string &path) {
  ifstream in(path, ios::binary);
  if (!in) {
    throw CompileError("cannot open input file: " + path);
  }
  ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

static void writeFile(const string &path, const string &content) {
  ofstream out(path, ios::binary);
  if (!out) {
    throw CompileError("cannot open output file: " + path);
  }
  out << content;
}

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

} // namespace

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
        cerr << "compiler: missing argument after -o\n";
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
