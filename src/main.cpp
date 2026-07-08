#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
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
#include <utility>
#include <vector>

namespace {

struct Options {
    bool optimize = false;
};

Options parseOptions(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "-opt") {
            options.optimize = true;
        }
    }
    return options;
}

std::string readStdin() {
    return {std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>()};
}

struct SourceLocation {
    int line = 1;
    int column = 1;
    std::size_t offset = 0;
};

[[noreturn]] void fail(const SourceLocation& loc, const std::string& message) {
    std::ostringstream out;
    out << loc.line << ':' << loc.column << ": " << message;
    throw std::runtime_error(out.str());
}

enum class TokenKind {
    End,
    Identifier,
    Number,
    KwConst,
    KwInt,
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
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    EqualEqual,
    BangEqual,
    Assign,
    Semicolon,
    Comma,
    LParen,
    RParen,
    LBrace,
    RBrace,
};

struct Token {
    TokenKind kind = TokenKind::End;
    std::string text;
    std::int64_t number = 0;
    SourceLocation loc;
};

class Lexer {
public:
    explicit Lexer(std::string source) : source_(std::move(source)) {}

    std::vector<Token> lex() {
        std::vector<Token> tokens;
        while (true) {
            skipWhitespaceAndComments();
            const SourceLocation loc = location();
            if (isAtEnd()) {
                tokens.push_back(Token{TokenKind::End, "", 0, loc});
                return tokens;
            }

            const char ch = peek();
            if (isIdentifierStart(ch)) {
                tokens.push_back(readIdentifier());
            } else if (std::isdigit(static_cast<unsigned char>(ch)) != 0) {
                tokens.push_back(readNumber());
            } else {
                tokens.push_back(readPunctuation());
            }
        }
    }

private:
    std::string source_;
    std::size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;

    SourceLocation location() const {
        return SourceLocation{line_, column_, pos_};
    }

    bool isAtEnd() const {
        return pos_ >= source_.size();
    }

    char peek(std::size_t distance = 0) const {
        const std::size_t index = pos_ + distance;
        if (index >= source_.size()) {
            return '\0';
        }
        return source_[index];
    }

    char advance() {
        const char ch = source_[pos_++];
        if (ch == '\n') {
            ++line_;
            column_ = 1;
        } else {
            ++column_;
        }
        return ch;
    }

    bool match(char expected) {
        if (peek() != expected) {
            return false;
        }
        advance();
        return true;
    }

    static bool isIdentifierStart(char ch) {
        return std::isalpha(static_cast<unsigned char>(ch)) != 0 || ch == '_';
    }

    static bool isIdentifierPart(char ch) {
        return std::isalnum(static_cast<unsigned char>(ch)) != 0 || ch == '_';
    }

    void skipWhitespaceAndComments() {
        while (!isAtEnd()) {
            const char ch = peek();
            if (std::isspace(static_cast<unsigned char>(ch)) != 0) {
                advance();
                continue;
            }
            if (ch == '/' && peek(1) == '/') {
                while (!isAtEnd() && peek() != '\n') {
                    advance();
                }
                continue;
            }
            if (ch == '/' && peek(1) == '*') {
                const SourceLocation loc = location();
                advance();
                advance();
                while (!isAtEnd() && !(peek() == '*' && peek(1) == '/')) {
                    advance();
                }
                if (isAtEnd()) {
                    fail(loc, "unterminated block comment");
                }
                advance();
                advance();
                continue;
            }
            break;
        }
    }

    Token readIdentifier() {
        const SourceLocation loc = location();
        std::string text;
        while (isIdentifierPart(peek())) {
            text.push_back(advance());
        }

        static const std::unordered_map<std::string, TokenKind> keywords = {
            {"const", TokenKind::KwConst},       {"int", TokenKind::KwInt},
            {"void", TokenKind::KwVoid},         {"if", TokenKind::KwIf},
            {"else", TokenKind::KwElse},         {"while", TokenKind::KwWhile},
            {"break", TokenKind::KwBreak},       {"continue", TokenKind::KwContinue},
            {"return", TokenKind::KwReturn},
        };

        const auto it = keywords.find(text);
        if (it != keywords.end()) {
            return Token{it->second, text, 0, loc};
        }
        return Token{TokenKind::Identifier, text, 0, loc};
    }

    Token readNumber() {
        const SourceLocation loc = location();
        std::string text;
        while (std::isdigit(static_cast<unsigned char>(peek())) != 0) {
            text.push_back(advance());
        }

        std::int64_t value = 0;
        try {
            value = std::stoll(text);
        } catch (const std::exception&) {
            fail(loc, "integer literal is out of range");
        }
        return Token{TokenKind::Number, text, value, loc};
    }

    Token readPunctuation() {
        const SourceLocation loc = location();
        const char ch = advance();
        switch (ch) {
            case '+':
                return Token{TokenKind::Plus, "+", 0, loc};
            case '-':
                return Token{TokenKind::Minus, "-", 0, loc};
            case '*':
                return Token{TokenKind::Star, "*", 0, loc};
            case '/':
                return Token{TokenKind::Slash, "/", 0, loc};
            case '%':
                return Token{TokenKind::Percent, "%", 0, loc};
            case ';':
                return Token{TokenKind::Semicolon, ";", 0, loc};
            case ',':
                return Token{TokenKind::Comma, ",", 0, loc};
            case '(':
                return Token{TokenKind::LParen, "(", 0, loc};
            case ')':
                return Token{TokenKind::RParen, ")", 0, loc};
            case '{':
                return Token{TokenKind::LBrace, "{", 0, loc};
            case '}':
                return Token{TokenKind::RBrace, "}", 0, loc};
            case '!':
                if (match('=')) {
                    return Token{TokenKind::BangEqual, "!=", 0, loc};
                }
                return Token{TokenKind::Bang, "!", 0, loc};
            case '&':
                if (match('&')) {
                    return Token{TokenKind::AndAnd, "&&", 0, loc};
                }
                break;
            case '|':
                if (match('|')) {
                    return Token{TokenKind::OrOr, "||", 0, loc};
                }
                break;
            case '<':
                if (match('=')) {
                    return Token{TokenKind::LessEqual, "<=", 0, loc};
                }
                return Token{TokenKind::Less, "<", 0, loc};
            case '>':
                if (match('=')) {
                    return Token{TokenKind::GreaterEqual, ">=", 0, loc};
                }
                return Token{TokenKind::Greater, ">", 0, loc};
            case '=':
                if (match('=')) {
                    return Token{TokenKind::EqualEqual, "==", 0, loc};
                }
                return Token{TokenKind::Assign, "=", 0, loc};
            default:
                break;
        }
        fail(loc, std::string("unexpected character '") + ch + "'");
    }
};

enum class ValueType {
    Int,
    Void,
};

enum class ExprKind {
    Number,
    Variable,
    Unary,
    Binary,
    Call,
};

struct Expr {
    ExprKind kind = ExprKind::Number;
    SourceLocation loc;
    std::int64_t number = 0;
    std::string name;
    std::string op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
    std::vector<std::unique_ptr<Expr>> args;
};

struct Decl {
    bool isConst = false;
    std::string name;
    std::unique_ptr<Expr> init;
    SourceLocation loc;
};

enum class StmtKind {
    Block,
    Empty,
    ExprStmt,
    Assign,
    DeclStmt,
    If,
    While,
    Break,
    Continue,
    Return,
};

struct Stmt {
    StmtKind kind = StmtKind::Empty;
    SourceLocation loc;
    std::vector<std::unique_ptr<Stmt>> statements;
    std::unique_ptr<Expr> expr;
    std::string name;
    std::unique_ptr<Decl> decl;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
};

struct Function {
    ValueType returnType = ValueType::Int;
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<Stmt> body;
    SourceLocation loc;
};

struct CompUnit {
    std::vector<std::unique_ptr<Decl>> globals;
    std::vector<std::unique_ptr<Function>> functions;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

    CompUnit parseCompUnit() {
        CompUnit unit;
        while (!check(TokenKind::End)) {
            if (check(TokenKind::KwConst)) {
                unit.globals.push_back(parseDecl());
                continue;
            }
            if (check(TokenKind::KwInt) || check(TokenKind::KwVoid)) {
                if (isFunctionDefinitionAhead()) {
                    unit.functions.push_back(parseFunction());
                } else {
                    unit.globals.push_back(parseDecl());
                }
                continue;
            }
            fail(peek().loc, "expected declaration or function definition");
        }
        return unit;
    }

private:
    std::vector<Token> tokens_;
    std::size_t pos_ = 0;

    const Token& peek(std::size_t distance = 0) const {
        const std::size_t index = pos_ + distance;
        if (index >= tokens_.size()) {
            return tokens_.back();
        }
        return tokens_[index];
    }

    bool check(TokenKind kind, std::size_t distance = 0) const {
        return peek(distance).kind == kind;
    }

    bool match(TokenKind kind) {
        if (!check(kind)) {
            return false;
        }
        ++pos_;
        return true;
    }

    const Token& expect(TokenKind kind, const std::string& message) {
        if (!check(kind)) {
            fail(peek().loc, message);
        }
        return tokens_[pos_++];
    }

    bool isFunctionDefinitionAhead() const {
        return (check(TokenKind::KwInt) || check(TokenKind::KwVoid)) &&
               check(TokenKind::Identifier, 1) && check(TokenKind::LParen, 2);
    }

    ValueType parseType() {
        if (match(TokenKind::KwInt)) {
            return ValueType::Int;
        }
        if (match(TokenKind::KwVoid)) {
            return ValueType::Void;
        }
        fail(peek().loc, "expected type");
    }

    std::unique_ptr<Function> parseFunction() {
        auto function = std::make_unique<Function>();
        function->loc = peek().loc;
        function->returnType = parseType();
        function->name = expect(TokenKind::Identifier, "expected function name").text;
        expect(TokenKind::LParen, "expected '(' after function name");
        if (!check(TokenKind::RParen)) {
            do {
                expect(TokenKind::KwInt, "expected int parameter");
                function->params.push_back(expect(TokenKind::Identifier, "expected parameter name").text);
            } while (match(TokenKind::Comma));
        }
        expect(TokenKind::RParen, "expected ')' after parameter list");
        function->body = parseBlock();
        return function;
    }

    std::unique_ptr<Decl> parseDecl() {
        auto decl = std::make_unique<Decl>();
        decl->loc = peek().loc;
        if (match(TokenKind::KwConst)) {
            decl->isConst = true;
            expect(TokenKind::KwInt, "expected int after const");
        } else {
            expect(TokenKind::KwInt, "expected int declaration");
        }
        decl->name = expect(TokenKind::Identifier, "expected identifier in declaration").text;
        expect(TokenKind::Assign, "expected initializer");
        decl->init = parseExpr();
        expect(TokenKind::Semicolon, "expected ';' after declaration");
        return decl;
    }

    std::unique_ptr<Stmt> parseBlock() {
        auto stmt = std::make_unique<Stmt>();
        stmt->kind = StmtKind::Block;
        stmt->loc = peek().loc;
        expect(TokenKind::LBrace, "expected '{'");
        while (!check(TokenKind::RBrace)) {
            if (check(TokenKind::End)) {
                fail(peek().loc, "unterminated block");
            }
            stmt->statements.push_back(parseStmt());
        }
        expect(TokenKind::RBrace, "expected '}'");
        return stmt;
    }

    std::unique_ptr<Stmt> parseStmt() {
        if (check(TokenKind::LBrace)) {
            return parseBlock();
        }

        auto stmt = std::make_unique<Stmt>();
        stmt->loc = peek().loc;

        if (match(TokenKind::Semicolon)) {
            stmt->kind = StmtKind::Empty;
            return stmt;
        }
        if (check(TokenKind::KwConst) || check(TokenKind::KwInt)) {
            stmt->kind = StmtKind::DeclStmt;
            stmt->decl = parseDecl();
            return stmt;
        }
        if (match(TokenKind::KwIf)) {
            stmt->kind = StmtKind::If;
            expect(TokenKind::LParen, "expected '(' after if");
            stmt->expr = parseExpr();
            expect(TokenKind::RParen, "expected ')' after condition");
            stmt->thenBranch = parseStmt();
            if (match(TokenKind::KwElse)) {
                stmt->elseBranch = parseStmt();
            }
            return stmt;
        }
        if (match(TokenKind::KwWhile)) {
            stmt->kind = StmtKind::While;
            expect(TokenKind::LParen, "expected '(' after while");
            stmt->expr = parseExpr();
            expect(TokenKind::RParen, "expected ')' after condition");
            stmt->thenBranch = parseStmt();
            return stmt;
        }
        if (match(TokenKind::KwBreak)) {
            stmt->kind = StmtKind::Break;
            expect(TokenKind::Semicolon, "expected ';' after break");
            return stmt;
        }
        if (match(TokenKind::KwContinue)) {
            stmt->kind = StmtKind::Continue;
            expect(TokenKind::Semicolon, "expected ';' after continue");
            return stmt;
        }
        if (match(TokenKind::KwReturn)) {
            stmt->kind = StmtKind::Return;
            if (!check(TokenKind::Semicolon)) {
                stmt->expr = parseExpr();
            }
            expect(TokenKind::Semicolon, "expected ';' after return");
            return stmt;
        }
        if (check(TokenKind::Identifier) && check(TokenKind::Assign, 1)) {
            stmt->kind = StmtKind::Assign;
            stmt->name = expect(TokenKind::Identifier, "expected assignment target").text;
            expect(TokenKind::Assign, "expected '='");
            stmt->expr = parseExpr();
            expect(TokenKind::Semicolon, "expected ';' after assignment");
            return stmt;
        }

        stmt->kind = StmtKind::ExprStmt;
        stmt->expr = parseExpr();
        expect(TokenKind::Semicolon, "expected ';' after expression");
        return stmt;
    }

    std::unique_ptr<Expr> parseExpr() {
        return parseLOrExpr();
    }

    std::unique_ptr<Expr> parseLOrExpr() {
        auto expr = parseLAndExpr();
        while (match(TokenKind::OrOr)) {
            expr = makeBinary("||", std::move(expr), parseLAndExpr());
        }
        return expr;
    }

    std::unique_ptr<Expr> parseLAndExpr() {
        auto expr = parseRelExpr();
        while (match(TokenKind::AndAnd)) {
            expr = makeBinary("&&", std::move(expr), parseRelExpr());
        }
        return expr;
    }

    std::unique_ptr<Expr> parseRelExpr() {
        auto expr = parseAddExpr();
        while (true) {
            if (match(TokenKind::Less)) {
                expr = makeBinary("<", std::move(expr), parseAddExpr());
            } else if (match(TokenKind::Greater)) {
                expr = makeBinary(">", std::move(expr), parseAddExpr());
            } else if (match(TokenKind::LessEqual)) {
                expr = makeBinary("<=", std::move(expr), parseAddExpr());
            } else if (match(TokenKind::GreaterEqual)) {
                expr = makeBinary(">=", std::move(expr), parseAddExpr());
            } else if (match(TokenKind::EqualEqual)) {
                expr = makeBinary("==", std::move(expr), parseAddExpr());
            } else if (match(TokenKind::BangEqual)) {
                expr = makeBinary("!=", std::move(expr), parseAddExpr());
            } else {
                return expr;
            }
        }
    }

    std::unique_ptr<Expr> parseAddExpr() {
        auto expr = parseMulExpr();
        while (true) {
            if (match(TokenKind::Plus)) {
                expr = makeBinary("+", std::move(expr), parseMulExpr());
            } else if (match(TokenKind::Minus)) {
                expr = makeBinary("-", std::move(expr), parseMulExpr());
            } else {
                return expr;
            }
        }
    }

    std::unique_ptr<Expr> parseMulExpr() {
        auto expr = parseUnaryExpr();
        while (true) {
            if (match(TokenKind::Star)) {
                expr = makeBinary("*", std::move(expr), parseUnaryExpr());
            } else if (match(TokenKind::Slash)) {
                expr = makeBinary("/", std::move(expr), parseUnaryExpr());
            } else if (match(TokenKind::Percent)) {
                expr = makeBinary("%", std::move(expr), parseUnaryExpr());
            } else {
                return expr;
            }
        }
    }

    std::unique_ptr<Expr> parseUnaryExpr() {
        if (match(TokenKind::Plus)) {
            return makeUnary("+", parseUnaryExpr());
        }
        if (match(TokenKind::Minus)) {
            return makeUnary("-", parseUnaryExpr());
        }
        if (match(TokenKind::Bang)) {
            return makeUnary("!", parseUnaryExpr());
        }
        return parsePrimaryExpr();
    }

    std::unique_ptr<Expr> parsePrimaryExpr() {
        const SourceLocation loc = peek().loc;
        if (match(TokenKind::Number)) {
            auto expr = std::make_unique<Expr>();
            expr->kind = ExprKind::Number;
            expr->loc = loc;
            expr->number = tokens_[pos_ - 1].number;
            return expr;
        }
        if (match(TokenKind::Identifier)) {
            const std::string name = tokens_[pos_ - 1].text;
            if (match(TokenKind::LParen)) {
                auto expr = std::make_unique<Expr>();
                expr->kind = ExprKind::Call;
                expr->loc = loc;
                expr->name = name;
                if (!check(TokenKind::RParen)) {
                    do {
                        expr->args.push_back(parseExpr());
                    } while (match(TokenKind::Comma));
                }
                expect(TokenKind::RParen, "expected ')' after argument list");
                return expr;
            }
            auto expr = std::make_unique<Expr>();
            expr->kind = ExprKind::Variable;
            expr->loc = loc;
            expr->name = name;
            return expr;
        }
        if (match(TokenKind::LParen)) {
            auto expr = parseExpr();
            expect(TokenKind::RParen, "expected ')'");
            return expr;
        }
        fail(peek().loc, "expected expression");
    }

    static std::unique_ptr<Expr> makeUnary(std::string op, std::unique_ptr<Expr> operand) {
        auto expr = std::make_unique<Expr>();
        expr->kind = ExprKind::Unary;
        expr->loc = operand->loc;
        expr->op = std::move(op);
        expr->lhs = std::move(operand);
        return expr;
    }

    static std::unique_ptr<Expr> makeBinary(std::string op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs) {
        auto expr = std::make_unique<Expr>();
        expr->kind = ExprKind::Binary;
        expr->loc = lhs->loc;
        expr->op = std::move(op);
        expr->lhs = std::move(lhs);
        expr->rhs = std::move(rhs);
        return expr;
    }
};

enum class SymbolKind {
    LocalVar,
    GlobalVar,
    Const,
};

struct Symbol {
    SymbolKind kind = SymbolKind::LocalVar;
    int offset = 0;
    std::string label;
    std::int32_t constValue = 0;
    std::string reg;
};

struct FunctionInfo {
    ValueType returnType = ValueType::Int;
    std::vector<std::string> params;
    std::string label;
    const Function* function = nullptr;
    const Expr* inlineReturn = nullptr;
};

std::int32_t toInt32(std::int64_t value) {
    return static_cast<std::int32_t>(value);
}

std::int64_t printableI32(std::int32_t value) {
    return static_cast<std::int64_t>(value);
}

bool fitsI12(int value) {
    return value >= -2048 && value <= 2047;
}

std::optional<int> powerOfTwoShift(std::int64_t value) {
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

int alignTo16(int value) {
    return (value + 15) / 16 * 16;
}

std::string functionLabel(const std::string& name) {
    if (name == "main") {
        return "main";
    }
    return "fn_" + name;
}

std::string globalLabel(const std::string& name) {
    return "g_" + name;
}

const Expr* simpleInlineReturnExpr(const Function& function) {
    if (function.returnType != ValueType::Int || !function.body ||
        function.body->kind != StmtKind::Block || function.body->statements.size() != 1) {
        return nullptr;
    }
    const Stmt& stmt = *function.body->statements.front();
    if (stmt.kind != StmtKind::Return || !stmt.expr) {
        return nullptr;
    }
    return stmt.expr.get();
}

void emitRegAdd(std::ostream& out, const std::string& dst, const std::string& base, int amount) {
    if (amount == 0) {
        if (dst != base) {
            out << "    addi " << dst << ", " << base << ", 0\n";
        }
        return;
    }
    if (fitsI12(amount)) {
        out << "    addi " << dst << ", " << base << ", " << amount << '\n';
    } else {
        out << "    li t6, " << amount << '\n';
        out << "    add " << dst << ", " << base << ", t6\n";
    }
}

void emitLoadOffset(std::ostream& out, const std::string& dst, int offset, const std::string& base) {
    if (fitsI12(offset)) {
        out << "    lw " << dst << ", " << offset << '(' << base << ")\n";
    } else {
        out << "    li t6, " << offset << '\n';
        out << "    add t6, " << base << ", t6\n";
        out << "    lw " << dst << ", 0(t6)\n";
    }
}

void emitStoreOffset(std::ostream& out, const std::string& src, int offset, const std::string& base) {
    if (fitsI12(offset)) {
        out << "    sw " << src << ", " << offset << '(' << base << ")\n";
    } else {
        out << "    li t6, " << offset << '\n';
        out << "    add t6, " << base << ", t6\n";
        out << "    sw " << src << ", 0(t6)\n";
    }
}

class CodeGenerator {
public:
    CodeGenerator(const CompUnit& unit, Options options) : unit_(unit), options_(options) {}

    std::string generate() {
        collectFunctions();
        collectGlobals();

        std::ostringstream out;
        if (!dataLines_.empty()) {
            out << "    .data\n";
            out << "    .align 2\n";
            for (const std::string& line : dataLines_) {
                out << line;
            }
            out << '\n';
        }

        out << "    .text\n";
        out << "    .globl main\n";
        for (const auto& function : unit_.functions) {
            out << generateFunction(*function);
        }
        return out.str();
    }

private:
    const CompUnit& unit_;
    Options options_;
    std::unordered_map<std::string, Symbol> globals_;
    std::unordered_map<std::string, FunctionInfo> functions_;
    std::vector<std::string> dataLines_;

    class FunctionEmitter {
    public:
        FunctionEmitter(CodeGenerator& parent, const Function& function)
            : parent_(parent),
              function_(function),
              bodyEntryLabel_(".L_entry_" + functionLabel(function.name)),
              returnLabel_(".L_return_" + functionLabel(function.name)) {}

        std::string emit() {
            containsCall_ = stmtContainsCall(*function_.body);
            body_ << "    # parameters\n";
            pushScope();
            for (std::size_t i = 0; i < function_.params.size(); ++i) {
                Symbol symbol = allocateLocal();
                paramSymbols_.push_back(symbol);
                currentScope()[function_.params[i]] = symbol;
                if (i < 8) {
                    storeSymbol(symbol, argRegisterName(i));
                } else {
                    emitLoadOffset(body_, "t0", static_cast<int>((i - 8) * 4), "s0");
                    storeSymbol(symbol, "t0");
                }
            }

            body_ << bodyEntryLabel_ << ":\n";
            emitStmt(*function_.body);
            body_ << "    li a0, 0\n";
            body_ << returnLabel_ << ":\n";
            restoreSavedLocalRegisters();
            emitLoadOffset(body_, "ra", -4, "s0");
            emitLoadOffset(body_, "t0", -8, "s0");
            body_ << "    addi sp, s0, 0\n";
            body_ << "    addi s0, t0, 0\n";
            body_ << "    ret\n\n";

            const int frameSize = alignTo16(frameHeaderSize() + slotCount_ * 4);
            std::ostringstream out;
            out << functionLabel(function_.name) << ":\n";
            emitRegAdd(out, "sp", "sp", -frameSize);
            emitStoreOffset(out, "ra", frameSize - 4, "sp");
            emitStoreOffset(out, "s0", frameSize - 8, "sp");
            emitRegAdd(out, "s0", "sp", frameSize);
            saveLocalRegisters(out);
            out << body_.str();
            return out.str();
        }

    private:
        CodeGenerator& parent_;
        const Function& function_;
        std::ostringstream body_;
        std::vector<std::unordered_map<std::string, Symbol>> scopes_;
        std::vector<std::pair<std::string, std::string>> loopStack_;
        int slotCount_ = 0;
        int localRegCount_ = 0;
        int tempRegDepth_ = 0;
        int tempStackDepth_ = 0;
        std::vector<bool> tempLocationIsReg_;
        int labelCounter_ = 0;
        int inlineDepth_ = 0;
        bool containsCall_ = false;
        std::vector<Symbol> paramSymbols_;
        std::string bodyEntryLabel_;
        std::string returnLabel_;

        std::unordered_map<std::string, Symbol>& currentScope() {
            if (scopes_.empty()) {
                throw std::runtime_error("internal error: missing local scope");
            }
            return scopes_.back();
        }

        void pushScope() {
            scopes_.push_back({});
        }

        void popScope() {
            scopes_.pop_back();
        }

        bool useRegisterLocals() const {
            return parent_.options_.optimize;
        }

        int savedLocalRegisterCount() const {
            return useRegisterLocals() ? 11 : 0;
        }

        int allocatableLocalRegisterCount() const {
            if (!useRegisterLocals()) {
                return 0;
            }
            return containsCall_ ? 11 : 18;
        }

        int frameHeaderSize() const {
            return 8 + savedLocalRegisterCount() * 4;
        }

        std::string localRegisterName(int index) const {
            if (index >= 11) {
                return "a" + std::to_string(index - 10);
            }
            return "s" + std::to_string(index + 1);
        }

        std::string argRegisterName(std::size_t index) const {
            return "a" + std::to_string(index);
        }

        Symbol allocateLocal() {
            if (useRegisterLocals() && localRegCount_ < allocatableLocalRegisterCount()) {
                Symbol symbol;
                symbol.kind = SymbolKind::LocalVar;
                symbol.reg = localRegisterName(localRegCount_++);
                return symbol;
            }

            Symbol symbol;
            symbol.kind = SymbolKind::LocalVar;
            symbol.offset = allocateSlot();
            return symbol;
        }

        int allocateSlot() {
            const int offset = -(frameHeaderSize() + 4 + slotCount_ * 4);
            ++slotCount_;
            return offset;
        }

        void saveLocalRegisters(std::ostream& out) const {
            if (!useRegisterLocals()) {
                return;
            }
            const int savedCount = std::min(localRegCount_, savedLocalRegisterCount());
            for (int i = 0; i < savedCount; ++i) {
                emitStoreOffset(out, localRegisterName(i), -12 - i * 4, "s0");
            }
        }

        void restoreSavedLocalRegisters() {
            if (!useRegisterLocals()) {
                return;
            }
            const int savedCount = std::min(localRegCount_, savedLocalRegisterCount());
            for (int i = 0; i < savedCount; ++i) {
                emitLoadOffset(body_, localRegisterName(i), -12 - i * 4, "s0");
            }
        }

        std::string newLabel(const std::string& hint) {
            return ".L_" + functionLabel(function_.name) + "_" + hint + "_" + std::to_string(labelCounter_++);
        }

        std::optional<Symbol> lookup(const std::string& name) const {
            for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
                const auto found = it->find(name);
                if (found != it->end()) {
                    return found->second;
                }
            }
            const auto global = parent_.globals_.find(name);
            if (global != parent_.globals_.end()) {
                return global->second;
            }
            return std::nullopt;
        }

        void loadSymbol(const Symbol& symbol, const std::string& dst) {
            if (symbol.kind == SymbolKind::Const) {
                body_ << "    li " << dst << ", " << printableI32(symbol.constValue) << '\n';
            } else if (symbol.kind == SymbolKind::LocalVar) {
                if (!symbol.reg.empty()) {
                    body_ << "    addi " << dst << ", " << symbol.reg << ", 0\n";
                } else {
                    emitLoadOffset(body_, dst, symbol.offset, "s0");
                }
            } else {
                body_ << "    la t0, " << symbol.label << '\n';
                body_ << "    lw " << dst << ", 0(t0)\n";
            }
        }

        void storeSymbol(const Symbol& symbol, const std::string& src) {
            if (symbol.kind == SymbolKind::LocalVar) {
                if (!symbol.reg.empty()) {
                    if (symbol.reg != src) {
                        body_ << "    addi " << symbol.reg << ", " << src << ", 0\n";
                    }
                } else {
                    emitStoreOffset(body_, src, symbol.offset, "s0");
                }
            } else {
                body_ << "    la t0, " << symbol.label << '\n';
                body_ << "    sw " << src << ", 0(t0)\n";
            }
        }

        void copySymbol(const Symbol& dst, const Symbol& src) {
            if (src.kind == SymbolKind::LocalVar && !src.reg.empty()) {
                storeSymbol(dst, src.reg);
                return;
            }
            loadSymbol(src, "a0");
            storeSymbol(dst, "a0");
        }

        std::optional<std::int32_t> evalConst(const Expr& expr) const {
            switch (expr.kind) {
                case ExprKind::Number:
                    return toInt32(expr.number);
                case ExprKind::Variable: {
                    const auto symbol = lookup(expr.name);
                    if (symbol && symbol->kind == SymbolKind::Const) {
                        return symbol->constValue;
                    }
                    return std::nullopt;
                }
                case ExprKind::Unary: {
                    const auto value = evalConst(*expr.lhs);
                    if (!value) {
                        return std::nullopt;
                    }
                    if (expr.op == "+") {
                        return *value;
                    }
                    if (expr.op == "-") {
                        return toInt32(-static_cast<std::int64_t>(*value));
                    }
                    if (expr.op == "!") {
                        return static_cast<std::int32_t>(*value == 0 ? 1 : 0);
                    }
                    return std::nullopt;
                }
                case ExprKind::Binary:
                    return evalConstBinary(expr);
                case ExprKind::Call:
                    return std::nullopt;
            }
            return std::nullopt;
        }

        std::optional<std::int32_t> evalConstBinary(const Expr& expr) const {
            if (expr.op == "&&") {
                const auto lhs = evalConst(*expr.lhs);
                if (!lhs) {
                    return std::nullopt;
                }
                if (*lhs == 0) {
                    return 0;
                }
                const auto rhs = evalConst(*expr.rhs);
                if (!rhs) {
                    return std::nullopt;
                }
                return static_cast<std::int32_t>(*rhs != 0 ? 1 : 0);
            }
            if (expr.op == "||") {
                const auto lhs = evalConst(*expr.lhs);
                if (!lhs) {
                    return std::nullopt;
                }
                if (*lhs != 0) {
                    return 1;
                }
                const auto rhs = evalConst(*expr.rhs);
                if (!rhs) {
                    return std::nullopt;
                }
                return static_cast<std::int32_t>(*rhs != 0 ? 1 : 0);
            }

            const auto lhs = evalConst(*expr.lhs);
            const auto rhs = evalConst(*expr.rhs);
            if (!lhs || !rhs) {
                return std::nullopt;
            }

            const std::int64_t left = *lhs;
            const std::int64_t right = *rhs;
            if (expr.op == "+") {
                return toInt32(left + right);
            }
            if (expr.op == "-") {
                return toInt32(left - right);
            }
            if (expr.op == "*") {
                return toInt32(left * right);
            }
            if (expr.op == "/") {
                if (right == 0) {
                    return std::nullopt;
                }
                return toInt32(left / right);
            }
            if (expr.op == "%") {
                if (right == 0) {
                    return std::nullopt;
                }
                return toInt32(left % right);
            }
            if (expr.op == "<") {
                return static_cast<std::int32_t>(left < right ? 1 : 0);
            }
            if (expr.op == ">") {
                return static_cast<std::int32_t>(left > right ? 1 : 0);
            }
            if (expr.op == "<=") {
                return static_cast<std::int32_t>(left <= right ? 1 : 0);
            }
            if (expr.op == ">=") {
                return static_cast<std::int32_t>(left >= right ? 1 : 0);
            }
            if (expr.op == "==") {
                return static_cast<std::int32_t>(left == right ? 1 : 0);
            }
            if (expr.op == "!=") {
                return static_cast<std::int32_t>(left != right ? 1 : 0);
            }
            return std::nullopt;
        }

        void emitStmt(const Stmt& stmt) {
            switch (stmt.kind) {
                case StmtKind::Block:
                    pushScope();
                    for (const auto& child : stmt.statements) {
                        emitStmt(*child);
                    }
                    popScope();
                    break;
                case StmtKind::Empty:
                    break;
                case StmtKind::ExprStmt:
                    emitExpr(*stmt.expr);
                    break;
                case StmtKind::Assign:
                    emitAssign(stmt);
                    break;
                case StmtKind::DeclStmt:
                    emitDecl(*stmt.decl);
                    break;
                case StmtKind::If:
                    emitIf(stmt);
                    break;
                case StmtKind::While:
                    emitWhile(stmt);
                    break;
                case StmtKind::Break:
                    if (loopStack_.empty()) {
                        fail(stmt.loc, "break outside loop");
                    }
                    body_ << "    jal zero, " << loopStack_.back().second << '\n';
                    break;
                case StmtKind::Continue:
                    if (loopStack_.empty()) {
                        fail(stmt.loc, "continue outside loop");
                    }
                    body_ << "    jal zero, " << loopStack_.back().first << '\n';
                    break;
                case StmtKind::Return:
                    if (stmt.expr && parent_.options_.optimize && emitTailRecursiveReturn(*stmt.expr)) {
                        break;
                    }
                    if (stmt.expr) {
                        emitExpr(*stmt.expr);
                    } else {
                        body_ << "    li a0, 0\n";
                    }
                    body_ << "    jal zero, " << returnLabel_ << '\n';
                    break;
            }
        }

        bool emitTailRecursiveReturn(const Expr& expr) {
            if (expr.kind != ExprKind::Call || expr.name != function_.name ||
                expr.args.size() != paramSymbols_.size()) {
                return false;
            }

            pushScope();
            std::vector<Symbol> argSymbols;
            argSymbols.reserve(expr.args.size());
            for (const auto& arg : expr.args) {
                Symbol symbol = allocateLocal();
                if (!symbol.reg.empty()) {
                    emitExprTo(*arg, symbol.reg);
                } else {
                    emitExpr(*arg);
                    storeSymbol(symbol, "a0");
                }
                argSymbols.push_back(symbol);
            }

            for (std::size_t i = 0; i < argSymbols.size(); ++i) {
                copySymbol(paramSymbols_[i], argSymbols[i]);
            }
            popScope();
            body_ << "    jal zero, " << bodyEntryLabel_ << '\n';
            return true;
        }

        void emitDecl(const Decl& decl) {
            if (decl.isConst) {
                const auto value = evalConst(*decl.init);
                if (!value) {
                    fail(decl.loc, "const initializer is not a compile-time value");
                }
                currentScope()[decl.name] = Symbol{SymbolKind::Const, 0, "", *value, ""};
                return;
            }

            Symbol symbol = allocateLocal();
            if (parent_.options_.optimize && !symbol.reg.empty()) {
                emitExprTo(*decl.init, symbol.reg);
                currentScope()[decl.name] = std::move(symbol);
                return;
            }
            emitExpr(*decl.init);
            storeSymbol(symbol, "a0");
            currentScope()[decl.name] = std::move(symbol);
        }

        void emitAssign(const Stmt& stmt) {
            const auto symbol = lookup(stmt.name);
            if (!symbol) {
                fail(stmt.loc, "unknown assignment target: " + stmt.name);
            }
            if (symbol->kind == SymbolKind::Const) {
                fail(stmt.loc, "cannot assign to const: " + stmt.name);
            }

            if (parent_.options_.optimize && emitOptimizedAssign(stmt, *symbol)) {
                return;
            }

            emitExpr(*stmt.expr);
            if (symbol->kind == SymbolKind::LocalVar) {
                storeSymbol(*symbol, "a0");
            } else {
                body_ << "    la t0, " << symbol->label << '\n';
                body_ << "    sw a0, 0(t0)\n";
            }
        }

        bool emitOptimizedAssign(const Stmt& stmt, const Symbol& symbol) {
            if (symbol.kind != SymbolKind::LocalVar || symbol.reg.empty() || !stmt.expr) {
                return false;
            }

            const Expr& expr = *stmt.expr;
            if (isDirectValue(expr)) {
                emitValueTo(expr, symbol.reg);
                return true;
            }

            if (expr.kind != ExprKind::Binary || expr.op == "&&" || expr.op == "||") {
                if (!exprReferencesVariable(expr, stmt.name)) {
                    emitExprTo(expr, symbol.reg);
                    return true;
                }
                return false;
            }
            if (expr.lhs->kind != ExprKind::Variable || expr.lhs->name != stmt.name) {
                if (!exprReferencesVariable(expr, stmt.name)) {
                    emitExprTo(expr, symbol.reg);
                    return true;
                }
                return false;
            }

            const auto rhsConst = evalConst(*expr.rhs);
            if (rhsConst) {
                if ((expr.op == "+" || expr.op == "-") && *rhsConst == 0) {
                    return true;
                }
                if ((expr.op == "*" || expr.op == "/") && *rhsConst == 1) {
                    return true;
                }
                if (expr.op == "*" && *rhsConst == 0) {
                    body_ << "    li " << symbol.reg << ", 0\n";
                    return true;
                }
                if (expr.op == "%" && (*rhsConst == 1 || *rhsConst == -1)) {
                    body_ << "    li " << symbol.reg << ", 0\n";
                    return true;
                }
                if (expr.op == "*") {
                    const auto shift = powerOfTwoShift(*rhsConst);
                    if (shift) {
                        if (*rhsConst < 0 && *shift == 0) {
                            body_ << "    sub " << symbol.reg << ", zero, " << symbol.reg << '\n';
                        } else {
                            body_ << "    slli " << symbol.reg << ", " << symbol.reg << ", " << *shift << '\n';
                            if (*rhsConst < 0) {
                                body_ << "    sub " << symbol.reg << ", zero, " << symbol.reg << '\n';
                            }
                        }
                        return true;
                    }
                    if (emitMultiplyBySparseConstant(symbol.reg, symbol.reg, *rhsConst)) {
                        return true;
                    }
                }
            }
            if (rhsConst && (expr.op == "+" || expr.op == "-")) {
                const int imm = expr.op == "+" ? *rhsConst : -*rhsConst;
                if (fitsI12(imm)) {
                    body_ << "    addi " << symbol.reg << ", " << symbol.reg << ", " << imm << '\n';
                } else {
                    body_ << "    li t0, " << printableI32(*rhsConst) << '\n';
                    if (expr.op == "+") {
                        body_ << "    add " << symbol.reg << ", " << symbol.reg << ", t0\n";
                    } else {
                        body_ << "    sub " << symbol.reg << ", " << symbol.reg << ", t0\n";
                    }
                }
                return true;
            }

            if (!isDirectValue(*expr.rhs)) {
                return false;
            }
            const std::string rhsReg = emitValueAsRegister(*expr.rhs, "t0");
            if (expr.op == "+") {
                body_ << "    add " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "-") {
                body_ << "    sub " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "*") {
                body_ << "    mul " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "/") {
                body_ << "    div " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "%") {
                body_ << "    rem " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == "<") {
                body_ << "    slt " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
            } else if (expr.op == ">") {
                body_ << "    slt " << symbol.reg << ", " << rhsReg << ", " << symbol.reg << '\n';
            } else if (expr.op == "<=") {
                body_ << "    slt " << symbol.reg << ", " << rhsReg << ", " << symbol.reg << '\n';
                body_ << "    xori " << symbol.reg << ", " << symbol.reg << ", 1\n";
            } else if (expr.op == ">=") {
                body_ << "    slt " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
                body_ << "    xori " << symbol.reg << ", " << symbol.reg << ", 1\n";
            } else if (expr.op == "==") {
                body_ << "    sub " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
                body_ << "    sltiu " << symbol.reg << ", " << symbol.reg << ", 1\n";
            } else if (expr.op == "!=") {
                body_ << "    sub " << symbol.reg << ", " << symbol.reg << ", " << rhsReg << '\n';
                body_ << "    sltu " << symbol.reg << ", zero, " << symbol.reg << '\n';
            } else {
                return false;
            }
            return true;
        }

        void emitIf(const Stmt& stmt) {
            if (parent_.options_.optimize) {
                const auto condition = evalConst(*stmt.expr);
                if (condition) {
                    if (*condition != 0) {
                        emitStmt(*stmt.thenBranch);
                    } else if (stmt.elseBranch) {
                        emitStmt(*stmt.elseBranch);
                    }
                    return;
                }
            }

            const std::string elseLabel = newLabel("else");
            const std::string endLabel = newLabel("endif");
            if (parent_.options_.optimize) {
                emitBranchIfFalse(*stmt.expr, elseLabel);
            } else {
                emitExpr(*stmt.expr);
                body_ << "    beq a0, zero, " << elseLabel << '\n';
            }
            emitStmt(*stmt.thenBranch);
            if (stmt.elseBranch) {
                body_ << "    jal zero, " << endLabel << '\n';
                body_ << elseLabel << ":\n";
                emitStmt(*stmt.elseBranch);
                body_ << endLabel << ":\n";
            } else {
                body_ << elseLabel << ":\n";
            }
        }

        void emitWhile(const Stmt& stmt) {
            if (parent_.options_.optimize) {
                const auto condition = evalConst(*stmt.expr);
                if (condition && *condition == 0) {
                    return;
                }
            }

            const std::string condLabel = newLabel("while_cond");
            const std::string endLabel = newLabel("while_end");
            body_ << condLabel << ":\n";
            if (parent_.options_.optimize) {
                emitBranchIfFalse(*stmt.expr, endLabel);
            } else {
                emitExpr(*stmt.expr);
                body_ << "    beq a0, zero, " << endLabel << '\n';
            }
            loopStack_.push_back({condLabel, endLabel});
            emitStmt(*stmt.thenBranch);
            loopStack_.pop_back();
            body_ << "    jal zero, " << condLabel << '\n';
            body_ << endLabel << ":\n";
        }

        void emitExpr(const Expr& expr) {
            if (parent_.options_.optimize) {
                const auto value = evalConst(expr);
                if (value) {
                    body_ << "    li a0, " << printableI32(*value) << '\n';
                    return;
                }
            }

            switch (expr.kind) {
                case ExprKind::Number:
                    body_ << "    li a0, " << printableI32(toInt32(expr.number)) << '\n';
                    break;
                case ExprKind::Variable:
                    emitVariable(expr);
                    break;
                case ExprKind::Unary:
                    emitUnary(expr);
                    break;
                case ExprKind::Binary:
                    emitBinary(expr);
                    break;
                case ExprKind::Call:
                    emitCall(expr);
                    break;
            }
        }

        void emitExprTo(const Expr& expr, const std::string& dst) {
            if (dst == "a0") {
                emitExpr(expr);
                return;
            }
            if (parent_.options_.optimize) {
                const auto value = evalConst(expr);
                if (value) {
                    body_ << "    li " << dst << ", " << printableI32(*value) << '\n';
                    return;
                }
            }

            switch (expr.kind) {
                case ExprKind::Number:
                case ExprKind::Variable:
                    emitValueTo(expr, dst);
                    return;
                case ExprKind::Unary:
                    if (emitUnaryTo(expr, dst)) {
                        return;
                    }
                    break;
                case ExprKind::Binary:
                    if (expr.op != "&&" && expr.op != "||" && emitBinaryTo(expr, dst)) {
                        return;
                    }
                    break;
                case ExprKind::Call:
                    break;
            }

            emitExpr(expr);
            emitMove(dst, "a0");
        }

        void emitVariable(const Expr& expr) {
            const auto symbol = lookup(expr.name);
            if (!symbol) {
                fail(expr.loc, "unknown variable: " + expr.name);
            }
            if (symbol->kind == SymbolKind::Const) {
                body_ << "    li a0, " << printableI32(symbol->constValue) << '\n';
            } else if (symbol->kind == SymbolKind::LocalVar) {
                loadSymbol(*symbol, "a0");
            } else {
                body_ << "    la t0, " << symbol->label << '\n';
                body_ << "    lw a0, 0(t0)\n";
            }
        }

        void emitUnary(const Expr& expr) {
            emitExpr(*expr.lhs);
            if (expr.op == "+") {
                return;
            }
            if (expr.op == "-") {
                body_ << "    sub a0, zero, a0\n";
                return;
            }
            if (expr.op == "!") {
                body_ << "    sltiu a0, a0, 1\n";
                return;
            }
            fail(expr.loc, "unknown unary operator");
        }

        bool emitUnaryTo(const Expr& expr, const std::string& dst) {
            emitExprTo(*expr.lhs, dst);
            if (expr.op == "+") {
                return true;
            }
            if (expr.op == "-") {
                body_ << "    sub " << dst << ", zero, " << dst << '\n';
                return true;
            }
            if (expr.op == "!") {
                body_ << "    sltiu " << dst << ", " << dst << ", 1\n";
                return true;
            }
            return false;
        }

        void emitBranchIfFalse(const Expr& expr, const std::string& falseLabel) {
            if (parent_.options_.optimize) {
                const auto value = evalConst(expr);
                if (value) {
                    if (*value == 0) {
                        body_ << "    jal zero, " << falseLabel << '\n';
                    }
                    return;
                }
            }

            if (expr.kind == ExprKind::Unary && expr.op == "!") {
                emitBranchIfTrue(*expr.lhs, falseLabel);
                return;
            }

            if (expr.kind == ExprKind::Binary) {
                if (expr.op == "&&") {
                    emitBranchIfFalse(*expr.lhs, falseLabel);
                    emitBranchIfFalse(*expr.rhs, falseLabel);
                    return;
                }
                if (expr.op == "||") {
                    const std::string trueLabel = newLabel("or_true");
                    emitBranchIfTrue(*expr.lhs, trueLabel);
                    emitBranchIfFalse(*expr.rhs, falseLabel);
                    body_ << trueLabel << ":\n";
                    return;
                }
                if (emitComparisonBranch(expr, false, falseLabel)) {
                    return;
                }
            }

            emitExpr(expr);
            body_ << "    beq a0, zero, " << falseLabel << '\n';
        }

        void emitBranchIfTrue(const Expr& expr, const std::string& trueLabel) {
            if (parent_.options_.optimize) {
                const auto value = evalConst(expr);
                if (value) {
                    if (*value != 0) {
                        body_ << "    jal zero, " << trueLabel << '\n';
                    }
                    return;
                }
            }

            if (expr.kind == ExprKind::Unary && expr.op == "!") {
                emitBranchIfFalse(*expr.lhs, trueLabel);
                return;
            }

            if (expr.kind == ExprKind::Binary) {
                if (expr.op == "&&") {
                    const std::string falseLabel = newLabel("and_false");
                    emitBranchIfFalse(*expr.lhs, falseLabel);
                    emitBranchIfTrue(*expr.rhs, trueLabel);
                    body_ << falseLabel << ":\n";
                    return;
                }
                if (expr.op == "||") {
                    emitBranchIfTrue(*expr.lhs, trueLabel);
                    emitBranchIfTrue(*expr.rhs, trueLabel);
                    return;
                }
                if (emitComparisonBranch(expr, true, trueLabel)) {
                    return;
                }
            }

            emitExpr(expr);
            body_ << "    bne a0, zero, " << trueLabel << '\n';
        }

        void emitBinary(const Expr& expr) {
            if (expr.op == "&&") {
                emitLogicalAnd(expr);
                return;
            }
            if (expr.op == "||") {
                emitLogicalOr(expr);
                return;
            }
            if (parent_.options_.optimize && emitRepeatedOperandBinary(expr)) {
                return;
            }
            if (parent_.options_.optimize && emitImmediateBinary(expr)) {
                return;
            }
            if (parent_.options_.optimize && emitDirectValueBinary(expr)) {
                return;
            }

            emitExpr(*expr.lhs);
            pushA0(exprContainsCall(*expr.rhs));
            emitExpr(*expr.rhs);
            const std::string lhsReg = popValue("t0");

            if (expr.op == "+") {
                body_ << "    add a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "-") {
                body_ << "    sub a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "*") {
                body_ << "    mul a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "/") {
                body_ << "    div a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "%") {
                body_ << "    rem a0, " << lhsReg << ", a0\n";
            } else if (expr.op == "<") {
                body_ << "    slt a0, " << lhsReg << ", a0\n";
            } else if (expr.op == ">") {
                body_ << "    slt a0, a0, " << lhsReg << '\n';
            } else if (expr.op == "<=") {
                body_ << "    slt a0, a0, " << lhsReg << '\n';
                body_ << "    xori a0, a0, 1\n";
            } else if (expr.op == ">=") {
                body_ << "    slt a0, " << lhsReg << ", a0\n";
                body_ << "    xori a0, a0, 1\n";
            } else if (expr.op == "==") {
                body_ << "    sub a0, " << lhsReg << ", a0\n";
                body_ << "    sltiu a0, a0, 1\n";
            } else if (expr.op == "!=") {
                body_ << "    sub a0, " << lhsReg << ", a0\n";
                body_ << "    sltu a0, zero, a0\n";
            } else {
                fail(expr.loc, "unknown binary operator");
            }
        }

        bool isDirectValue(const Expr& expr) const {
            return expr.kind == ExprKind::Number || expr.kind == ExprKind::Variable;
        }

        void emitValueTo(const Expr& expr, const std::string& reg) {
            if (expr.kind == ExprKind::Number) {
                body_ << "    li " << reg << ", " << printableI32(toInt32(expr.number)) << '\n';
                return;
            }
            if (expr.kind == ExprKind::Variable) {
                const auto symbol = lookup(expr.name);
                if (!symbol) {
                    fail(expr.loc, "unknown variable: " + expr.name);
                }
                loadSymbol(*symbol, reg);
                return;
            }
            throw std::runtime_error("internal error: unsupported direct value");
        }

        bool emitBinaryTo(const Expr& expr, const std::string& dst) {
            if (emitRepeatedOperandBinaryTo(expr, dst)) {
                return true;
            }
            if (emitImmediateBinaryTo(expr, dst)) {
                return true;
            }
            if (emitDirectValueBinaryTo(expr, dst)) {
                return true;
            }
            return false;
        }

        std::string emitValueAsRegister(const Expr& expr, const std::string& scratchReg) {
            if (expr.kind == ExprKind::Number) {
                body_ << "    li " << scratchReg << ", " << printableI32(toInt32(expr.number)) << '\n';
                return scratchReg;
            }
            if (expr.kind == ExprKind::Variable) {
                const auto symbol = lookup(expr.name);
                if (!symbol) {
                    fail(expr.loc, "unknown variable: " + expr.name);
                }
                if (symbol->kind == SymbolKind::LocalVar && !symbol->reg.empty()) {
                    return symbol->reg;
                }
                loadSymbol(*symbol, scratchReg);
                return scratchReg;
            }
            throw std::runtime_error("internal error: unsupported direct value");
        }

        void emitBinaryOpTo(const Expr& expr, const std::string& dst, const std::string& lhsReg,
                            const std::string& rhsReg) {
            if (expr.op == "+") {
                body_ << "    add " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "-") {
                body_ << "    sub " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "*") {
                body_ << "    mul " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "/") {
                body_ << "    div " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "%") {
                body_ << "    rem " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == "<") {
                body_ << "    slt " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
            } else if (expr.op == ">") {
                body_ << "    slt " << dst << ", " << rhsReg << ", " << lhsReg << '\n';
            } else if (expr.op == "<=") {
                body_ << "    slt " << dst << ", " << rhsReg << ", " << lhsReg << '\n';
                body_ << "    xori " << dst << ", " << dst << ", 1\n";
            } else if (expr.op == ">=") {
                body_ << "    slt " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
                body_ << "    xori " << dst << ", " << dst << ", 1\n";
            } else if (expr.op == "==") {
                body_ << "    sub " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
                body_ << "    sltiu " << dst << ", " << dst << ", 1\n";
            } else if (expr.op == "!=") {
                body_ << "    sub " << dst << ", " << lhsReg << ", " << rhsReg << '\n';
                body_ << "    sltu " << dst << ", zero, " << dst << '\n';
            }
        }

        bool sameExpr(const Expr& lhs, const Expr& rhs) const {
            if (lhs.kind != rhs.kind) {
                return false;
            }
            switch (lhs.kind) {
                case ExprKind::Number:
                    return toInt32(lhs.number) == toInt32(rhs.number);
                case ExprKind::Variable:
                    return lhs.name == rhs.name;
                case ExprKind::Unary:
                    return lhs.op == rhs.op && sameExpr(*lhs.lhs, *rhs.lhs);
                case ExprKind::Binary:
                    return lhs.op == rhs.op && sameExpr(*lhs.lhs, *rhs.lhs) &&
                           sameExpr(*lhs.rhs, *rhs.rhs);
                case ExprKind::Call:
                    if (lhs.name != rhs.name || lhs.args.size() != rhs.args.size()) {
                        return false;
                    }
                    for (std::size_t i = 0; i < lhs.args.size(); ++i) {
                        if (!sameExpr(*lhs.args[i], *rhs.args[i])) {
                            return false;
                        }
                    }
                    return true;
            }
            return false;
        }

        bool exprReferencesVariable(const Expr& expr, const std::string& name) const {
            switch (expr.kind) {
                case ExprKind::Number:
                    return false;
                case ExprKind::Variable:
                    return expr.name == name;
                case ExprKind::Unary:
                    return exprReferencesVariable(*expr.lhs, name);
                case ExprKind::Binary:
                    return exprReferencesVariable(*expr.lhs, name) ||
                           exprReferencesVariable(*expr.rhs, name);
                case ExprKind::Call:
                    for (const auto& arg : expr.args) {
                        if (exprReferencesVariable(*arg, name)) {
                            return true;
                        }
                    }
                    return false;
            }
            return false;
        }

        bool emitRepeatedOperandBinary(const Expr& expr) {
            return emitRepeatedOperandBinaryTo(expr, "a0");
        }

        bool emitRepeatedOperandBinaryTo(const Expr& expr, const std::string& dst) {
            if (!sameExpr(*expr.lhs, *expr.rhs) || exprContainsCall(*expr.lhs)) {
                return false;
            }

            if (expr.op == "+") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    slli " << dst << ", " << dst << ", 1\n";
                return true;
            }
            if (expr.op == "-") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    li " << dst << ", 0\n";
                return true;
            }
            if (expr.op == "*") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    mul " << dst << ", " << dst << ", " << dst << '\n';
                return true;
            }
            if (expr.op == "<" || expr.op == ">" || expr.op == "!=") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    li " << dst << ", 0\n";
                return true;
            }
            if (expr.op == "<=" || expr.op == ">=" || expr.op == "==") {
                emitExprTo(*expr.lhs, dst);
                body_ << "    li " << dst << ", 1\n";
                return true;
            }
            return false;
        }

        void emitMove(const std::string& dst, const std::string& src) {
            if (dst != src) {
                body_ << "    addi " << dst << ", " << src << ", 0\n";
            }
        }

        std::string chooseScratch(std::initializer_list<std::string> avoid) const {
            for (const char* candidateName : {"t0", "t5", "t6"}) {
                const std::string candidate(candidateName);
                bool used = false;
                for (const std::string& reg : avoid) {
                    if (candidate == reg) {
                        used = true;
                        break;
                    }
                }
                if (!used) {
                    return candidate;
                }
            }
            return "t6";
        }

        void emitShiftedTerm(const std::string& dst, const std::string& baseReg, int shift) {
            if (shift == 0) {
                emitMove(dst, baseReg);
            } else {
                body_ << "    slli " << dst << ", " << baseReg << ", " << shift << '\n';
            }
        }

        bool emitMultiplyBySparseConstant(const std::string& dst, const std::string& lhsReg, int imm) {
            const std::int64_t value = imm;
            std::uint64_t magnitude = value < 0 ? static_cast<std::uint64_t>(-value)
                                                : static_cast<std::uint64_t>(value);
            if (magnitude == 0 || (magnitude & (magnitude - 1)) == 0) {
                return false;
            }

            std::vector<int> shifts;
            for (int bit = 0; bit < 31; ++bit) {
                if ((magnitude & (1ull << bit)) != 0) {
                    shifts.push_back(bit);
                }
            }
            if (shifts.size() != 2) {
                return false;
            }

            std::string baseReg = lhsReg;
            std::string scratch = chooseScratch({dst, lhsReg});
            if (dst == lhsReg) {
                emitMove(scratch, lhsReg);
                baseReg = scratch;
                scratch = chooseScratch({dst, baseReg});
            }

            emitShiftedTerm(dst, baseReg, shifts[0]);
            emitShiftedTerm(scratch, baseReg, shifts[1]);
            body_ << "    add " << dst << ", " << dst << ", " << scratch << '\n';
            if (value < 0) {
                body_ << "    sub " << dst << ", zero, " << dst << '\n';
            }
            return true;
        }

        bool isComparisonOp(const std::string& op) const {
            return op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=";
        }

        void emitComparisonJump(const std::string& op, const std::string& lhsReg,
                                const std::string& rhsReg, bool branchIfTrue,
                                const std::string& label) {
            if (branchIfTrue) {
                if (op == "<") {
                    body_ << "    blt " << lhsReg << ", " << rhsReg << ", " << label << '\n';
                } else if (op == ">") {
                    body_ << "    blt " << rhsReg << ", " << lhsReg << ", " << label << '\n';
                } else if (op == "<=") {
                    body_ << "    bge " << rhsReg << ", " << lhsReg << ", " << label << '\n';
                } else if (op == ">=") {
                    body_ << "    bge " << lhsReg << ", " << rhsReg << ", " << label << '\n';
                } else if (op == "==") {
                    body_ << "    beq " << lhsReg << ", " << rhsReg << ", " << label << '\n';
                } else if (op == "!=") {
                    body_ << "    bne " << lhsReg << ", " << rhsReg << ", " << label << '\n';
                }
                return;
            }

            if (op == "<") {
                body_ << "    bge " << lhsReg << ", " << rhsReg << ", " << label << '\n';
            } else if (op == ">") {
                body_ << "    bge " << rhsReg << ", " << lhsReg << ", " << label << '\n';
            } else if (op == "<=") {
                body_ << "    blt " << rhsReg << ", " << lhsReg << ", " << label << '\n';
            } else if (op == ">=") {
                body_ << "    blt " << lhsReg << ", " << rhsReg << ", " << label << '\n';
            } else if (op == "==") {
                body_ << "    bne " << lhsReg << ", " << rhsReg << ", " << label << '\n';
            } else if (op == "!=") {
                body_ << "    beq " << lhsReg << ", " << rhsReg << ", " << label << '\n';
            }
        }

        bool emitComparisonBranch(const Expr& expr, bool branchIfTrue, const std::string& label) {
            if (!isComparisonOp(expr.op)) {
                return false;
            }

            if (isDirectValue(*expr.lhs) && isDirectValue(*expr.rhs)) {
                const bool lhsZero = expr.lhs->kind == ExprKind::Number && toInt32(expr.lhs->number) == 0;
                const bool rhsZero = expr.rhs->kind == ExprKind::Number && toInt32(expr.rhs->number) == 0;
                const std::string lhsReg = lhsZero ? "zero" : emitValueAsRegister(*expr.lhs, "t5");
                const std::string rhsReg = rhsZero ? "zero" : emitValueAsRegister(*expr.rhs, "t0");
                emitComparisonJump(expr.op, lhsReg, rhsReg, branchIfTrue, label);
                return true;
            }

            emitExpr(*expr.lhs);
            pushA0(exprContainsCall(*expr.rhs));
            emitExpr(*expr.rhs);
            const std::string lhsReg = popValue("t0");
            emitComparisonJump(expr.op, lhsReg, "a0", branchIfTrue, label);
            return true;
        }

        bool isValueBinaryOp(const std::string& op) const {
            return op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
                   isComparisonOp(op);
        }

        bool emitDirectValueBinaryTo(const Expr& expr, const std::string& dst) {
            if (!isValueBinaryOp(expr.op) || !isDirectValue(*expr.lhs) || !isDirectValue(*expr.rhs)) {
                return false;
            }

            const std::string lhsScratch = dst == "t0" ? "t5" : dst;
            const std::string rhsScratch = dst == "t0" ? "t6" : "t0";
            const std::string lhsReg = emitValueAsRegister(*expr.lhs, lhsScratch);
            const std::string rhsReg = emitValueAsRegister(*expr.rhs, rhsScratch);
            emitBinaryOpTo(expr, dst, lhsReg, rhsReg);
            return true;
        }

        bool emitDirectValueBinary(const Expr& expr) {
            return emitDirectValueBinaryTo(expr, "a0");
        }

        bool emitImmediateBinaryTo(const Expr& expr, const std::string& dst) {
            if (!isValueBinaryOp(expr.op)) {
                return false;
            }
            const auto rhs = evalConst(*expr.rhs);
            if (!rhs) {
                return false;
            }
            const int imm = *rhs;
            std::string lhsReg = dst;
            if (isDirectValue(*expr.lhs)) {
                lhsReg = emitValueAsRegister(*expr.lhs, dst);
            } else {
                emitExpr(*expr.lhs);
                lhsReg = "a0";
            }
            const std::string immReg = lhsReg == "t0" ? "t5" : "t0";

            if ((expr.op == "+" || expr.op == "-") && imm == 0) {
                emitMove(dst, lhsReg);
                return true;
            }
            if ((expr.op == "*" || expr.op == "/") && imm == 1) {
                emitMove(dst, lhsReg);
                return true;
            }
            if (expr.op == "*" && imm == 0) {
                body_ << "    li " << dst << ", 0\n";
                return true;
            }
            if (expr.op == "%" && (imm == 1 || imm == -1)) {
                body_ << "    li " << dst << ", 0\n";
                return true;
            }
            if (expr.op == "*") {
                const auto shift = powerOfTwoShift(imm);
                if (shift) {
                    if (imm < 0 && *shift == 0) {
                        body_ << "    sub " << dst << ", zero, " << lhsReg << '\n';
                    } else {
                        body_ << "    slli " << dst << ", " << lhsReg << ", " << *shift << '\n';
                        if (imm < 0) {
                            body_ << "    sub " << dst << ", zero, " << dst << '\n';
                        }
                    }
                    return true;
                }
                if (emitMultiplyBySparseConstant(dst, lhsReg, imm)) {
                    return true;
                }
            }

            if (expr.op == "+" && fitsI12(imm)) {
                body_ << "    addi " << dst << ", " << lhsReg << ", " << imm << '\n';
                return true;
            }
            if (expr.op == "+") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    add " << dst << ", " << lhsReg << ", " << immReg << '\n';
                return true;
            }
            if (expr.op == "-" && fitsI12(-imm)) {
                body_ << "    addi " << dst << ", " << lhsReg << ", " << -imm << '\n';
                return true;
            }
            if (expr.op == "-") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    sub " << dst << ", " << lhsReg << ", " << immReg << '\n';
                return true;
            }
            if (expr.op == "<" && fitsI12(imm)) {
                body_ << "    slti " << dst << ", " << lhsReg << ", " << imm << '\n';
                return true;
            }
            if (expr.op == "<") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    slt " << dst << ", " << lhsReg << ", " << immReg << '\n';
                return true;
            }
            if (expr.op == "==" && fitsI12(-imm)) {
                body_ << "    addi " << dst << ", " << lhsReg << ", " << -imm << '\n';
                body_ << "    sltiu " << dst << ", " << dst << ", 1\n";
                return true;
            }
            if (expr.op == "==") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    sub " << dst << ", " << lhsReg << ", " << immReg << '\n';
                body_ << "    sltiu " << dst << ", " << dst << ", 1\n";
                return true;
            }
            if (expr.op == "!=" && fitsI12(-imm)) {
                body_ << "    addi " << dst << ", " << lhsReg << ", " << -imm << '\n';
                body_ << "    sltu " << dst << ", zero, " << dst << '\n';
                return true;
            }
            if (expr.op == "!=") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                body_ << "    sub " << dst << ", " << lhsReg << ", " << immReg << '\n';
                body_ << "    sltu " << dst << ", zero, " << dst << '\n';
                return true;
            }
            if (expr.op == "*" || expr.op == "/" || expr.op == "%" || expr.op == "<=" ||
                expr.op == ">" || expr.op == ">=") {
                body_ << "    li " << immReg << ", " << printableI32(imm) << '\n';
                if (expr.op == "*") {
                    body_ << "    mul " << dst << ", " << lhsReg << ", " << immReg << '\n';
                } else if (expr.op == "/") {
                    body_ << "    div " << dst << ", " << lhsReg << ", " << immReg << '\n';
                } else if (expr.op == "%") {
                    body_ << "    rem " << dst << ", " << lhsReg << ", " << immReg << '\n';
                } else if (expr.op == "<=") {
                    body_ << "    slt " << dst << ", " << immReg << ", " << lhsReg << '\n';
                    body_ << "    xori " << dst << ", " << dst << ", 1\n";
                } else if (expr.op == ">") {
                    body_ << "    slt " << dst << ", " << immReg << ", " << lhsReg << '\n';
                } else {
                    body_ << "    slt " << dst << ", " << lhsReg << ", " << immReg << '\n';
                    body_ << "    xori " << dst << ", " << dst << ", 1\n";
                }
                return true;
            }
            return false;
        }

        bool emitImmediateBinary(const Expr& expr) {
            return emitImmediateBinaryTo(expr, "a0");
        }

        void emitLogicalAnd(const Expr& expr) {
            const std::string falseLabel = newLabel("and_false");
            const std::string endLabel = newLabel("and_end");
            if (parent_.options_.optimize) {
                emitBranchIfFalse(*expr.lhs, falseLabel);
                emitBranchIfFalse(*expr.rhs, falseLabel);
            } else {
                emitExpr(*expr.lhs);
                body_ << "    beq a0, zero, " << falseLabel << '\n';
                emitExpr(*expr.rhs);
                body_ << "    beq a0, zero, " << falseLabel << '\n';
            }
            body_ << "    li a0, 1\n";
            body_ << "    jal zero, " << endLabel << '\n';
            body_ << falseLabel << ":\n";
            body_ << "    li a0, 0\n";
            body_ << endLabel << ":\n";
        }

        void emitLogicalOr(const Expr& expr) {
            const std::string trueLabel = newLabel("or_true");
            const std::string endLabel = newLabel("or_end");
            if (parent_.options_.optimize) {
                emitBranchIfTrue(*expr.lhs, trueLabel);
                emitBranchIfTrue(*expr.rhs, trueLabel);
            } else {
                emitExpr(*expr.lhs);
                body_ << "    bne a0, zero, " << trueLabel << '\n';
                emitExpr(*expr.rhs);
                body_ << "    bne a0, zero, " << trueLabel << '\n';
            }
            body_ << "    li a0, 0\n";
            body_ << "    jal zero, " << endLabel << '\n';
            body_ << trueLabel << ":\n";
            body_ << "    li a0, 1\n";
            body_ << endLabel << ":\n";
        }

        void emitCall(const Expr& expr) {
            const auto function = parent_.functions_.find(expr.name);
            if (function == parent_.functions_.end()) {
                fail(expr.loc, "unknown function: " + expr.name);
            }

            if (parent_.options_.optimize && emitInlineCall(expr, function->second)) {
                return;
            }

            const bool directArgs = parent_.options_.optimize && canPassDirectArgs(expr);
            const bool noCallArgs = parent_.options_.optimize && !directArgs && canPassNoCallArgs(expr);
            const std::size_t regArgCount = std::min<std::size_t>(expr.args.size(), 8);
            const std::size_t stackArgCount = expr.args.size() > 8 ? expr.args.size() - 8 : 0;
            const std::size_t tempArgCount = (directArgs || noCallArgs) ? 0 : regArgCount;
            const int tempBase = static_cast<int>(stackArgCount * 4);
            const int reserve = alignTo16(static_cast<int>((stackArgCount + tempArgCount) * 4));
            if (reserve > 0) {
                emitRegAdd(body_, "sp", "sp", -reserve);
            }
            if (directArgs) {
                for (std::size_t i = 8; i < expr.args.size(); ++i) {
                    emitValueTo(*expr.args[i], "t0");
                    emitStoreOffset(body_, "t0", static_cast<int>((i - 8) * 4), "sp");
                }
                for (std::size_t i = 0; i < regArgCount; ++i) {
                    emitValueTo(*expr.args[i], argRegisterName(i));
                }
            } else if (noCallArgs) {
                for (std::size_t i = 8; i < expr.args.size(); ++i) {
                    emitExpr(*expr.args[i]);
                    emitStoreOffset(body_, "a0", static_cast<int>((i - 8) * 4), "sp");
                }
                for (std::size_t i = regArgCount; i > 0; --i) {
                    const std::size_t index = i - 1;
                    emitExpr(*expr.args[index]);
                    if (index != 0) {
                        body_ << "    addi " << argRegisterName(index) << ", a0, 0\n";
                    }
                }
            } else {
                for (std::size_t i = 0; i < expr.args.size(); ++i) {
                    emitExpr(*expr.args[i]);
                    if (i < 8) {
                        emitStoreOffset(body_, "a0", tempBase + static_cast<int>(i * 4), "sp");
                    } else {
                        emitStoreOffset(body_, "a0", static_cast<int>((i - 8) * 4), "sp");
                    }
                }
                for (std::size_t i = 0; i < regArgCount; ++i) {
                    emitLoadOffset(body_, argRegisterName(i), tempBase + static_cast<int>(i * 4), "sp");
                }
            }
            body_ << "    call " << function->second.label << '\n';
            if (reserve > 0) {
                emitRegAdd(body_, "sp", "sp", reserve);
            }
        }

        bool canPassDirectArgs(const Expr& expr) const {
            for (const auto& arg : expr.args) {
                if (!isDirectValue(*arg)) {
                    return false;
                }
            }
            return true;
        }

        bool canPassNoCallArgs(const Expr& expr) const {
            for (const auto& arg : expr.args) {
                if (exprContainsCall(*arg)) {
                    return false;
                }
            }
            return true;
        }

        bool emitInlineCall(const Expr& expr, const FunctionInfo& info) {
            if (!info.inlineReturn || info.params.size() != expr.args.size() ||
                info.function == &function_ || inlineDepth_ >= 8) {
                return false;
            }

            ++inlineDepth_;
            pushScope();
            for (std::size_t i = 0; i < expr.args.size(); ++i) {
                Symbol symbol;
                if (!bindInlineArg(*expr.args[i], symbol)) {
                    symbol = allocateLocal();
                    if (!symbol.reg.empty()) {
                        emitExprTo(*expr.args[i], symbol.reg);
                    } else {
                        emitExpr(*expr.args[i]);
                        storeSymbol(symbol, "a0");
                    }
                }
                currentScope()[info.params[i]] = std::move(symbol);
            }
            emitExpr(*info.inlineReturn);
            popScope();
            --inlineDepth_;
            return true;
        }

        bool bindInlineArg(const Expr& arg, Symbol& symbol) {
            if (arg.kind == ExprKind::Number) {
                symbol = Symbol{SymbolKind::Const, 0, "", toInt32(arg.number), ""};
                return true;
            }
            if (arg.kind != ExprKind::Variable) {
                return false;
            }
            const auto value = lookup(arg.name);
            if (!value || value->kind == SymbolKind::GlobalVar) {
                return false;
            }
            symbol = *value;
            return true;
        }

        bool declContainsCall(const Decl& decl) const {
            return decl.init && exprContainsCall(*decl.init);
        }

        bool stmtContainsCall(const Stmt& stmt) const {
            switch (stmt.kind) {
                case StmtKind::Block:
                    for (const auto& child : stmt.statements) {
                        if (stmtContainsCall(*child)) {
                            return true;
                        }
                    }
                    return false;
                case StmtKind::Empty:
                case StmtKind::Break:
                case StmtKind::Continue:
                    return false;
                case StmtKind::ExprStmt:
                case StmtKind::Assign:
                case StmtKind::Return:
                    return stmt.expr && exprContainsCall(*stmt.expr);
                case StmtKind::DeclStmt:
                    return stmt.decl && declContainsCall(*stmt.decl);
                case StmtKind::If:
                    return (stmt.expr && exprContainsCall(*stmt.expr)) ||
                           (stmt.thenBranch && stmtContainsCall(*stmt.thenBranch)) ||
                           (stmt.elseBranch && stmtContainsCall(*stmt.elseBranch));
                case StmtKind::While:
                    return (stmt.expr && exprContainsCall(*stmt.expr)) ||
                           (stmt.thenBranch && stmtContainsCall(*stmt.thenBranch));
            }
            return false;
        }

        bool exprContainsCall(const Expr& expr) const {
            switch (expr.kind) {
                case ExprKind::Number:
                case ExprKind::Variable:
                    return false;
                case ExprKind::Unary:
                    return exprContainsCall(*expr.lhs);
                case ExprKind::Binary:
                    return exprContainsCall(*expr.lhs) || exprContainsCall(*expr.rhs);
                case ExprKind::Call:
                    return true;
            }
            return false;
        }

        std::string tempRegisterName(int index) const {
            static const std::vector<std::string> regs = {"t1", "t2", "t3", "t4"};
            return regs.at(static_cast<std::size_t>(index));
        }

        int tempRegisterCount() const {
            return 4;
        }

        void pushA0(bool forceStack = false) {
            if (parent_.options_.optimize && !forceStack && tempRegDepth_ < tempRegisterCount()) {
                body_ << "    addi " << tempRegisterName(tempRegDepth_) << ", a0, 0\n";
                ++tempRegDepth_;
                tempLocationIsReg_.push_back(true);
                return;
            }
            body_ << "    addi sp, sp, -16\n";
            body_ << "    sw a0, 12(sp)\n";
            ++tempStackDepth_;
            tempLocationIsReg_.push_back(false);
        }

        void popTo(const std::string& reg) {
            const std::string valueReg = popValue(reg);
            if (valueReg != reg) {
                body_ << "    addi " << reg << ", " << valueReg << ", 0\n";
            }
        }

        std::string popValue(const std::string& fallbackReg) {
            if (tempLocationIsReg_.empty()) {
                throw std::runtime_error("internal error: expression stack underflow");
            }
            const bool useReg = tempLocationIsReg_.back();
            tempLocationIsReg_.pop_back();
            if (useReg) {
                --tempRegDepth_;
                return tempRegisterName(tempRegDepth_);
            }
            if (tempStackDepth_ <= 0) {
                throw std::runtime_error("internal error: expression stack underflow");
            }
            --tempStackDepth_;
            body_ << "    lw " << fallbackReg << ", 12(sp)\n";
            body_ << "    addi sp, sp, 16\n";
            return fallbackReg;
        }
    };

    void collectFunctions() {
        for (const auto& function : unit_.functions) {
            FunctionInfo info;
            info.returnType = function->returnType;
            info.params = function->params;
            info.label = functionLabel(function->name);
            info.function = function.get();
            info.inlineReturn = simpleInlineReturnExpr(*function);
            functions_[function->name] = std::move(info);
        }
    }

    std::optional<Symbol> lookupGlobal(const std::string& name) const {
        const auto it = globals_.find(name);
        if (it == globals_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    std::optional<std::int32_t> evalGlobalConst(const Expr& expr) const {
        switch (expr.kind) {
            case ExprKind::Number:
                return toInt32(expr.number);
            case ExprKind::Variable: {
                const auto symbol = lookupGlobal(expr.name);
                if (symbol && symbol->kind == SymbolKind::Const) {
                    return symbol->constValue;
                }
                return std::nullopt;
            }
            case ExprKind::Unary: {
                const auto value = evalGlobalConst(*expr.lhs);
                if (!value) {
                    return std::nullopt;
                }
                if (expr.op == "+") {
                    return *value;
                }
                if (expr.op == "-") {
                    return toInt32(-static_cast<std::int64_t>(*value));
                }
                if (expr.op == "!") {
                    return static_cast<std::int32_t>(*value == 0 ? 1 : 0);
                }
                return std::nullopt;
            }
            case ExprKind::Binary:
                return evalGlobalBinary(expr);
            case ExprKind::Call:
                return std::nullopt;
        }
        return std::nullopt;
    }

    std::optional<std::int32_t> evalGlobalBinary(const Expr& expr) const {
        if (expr.op == "&&") {
            const auto lhs = evalGlobalConst(*expr.lhs);
            if (!lhs) {
                return std::nullopt;
            }
            if (*lhs == 0) {
                return 0;
            }
            const auto rhs = evalGlobalConst(*expr.rhs);
            if (!rhs) {
                return std::nullopt;
            }
            return static_cast<std::int32_t>(*rhs != 0 ? 1 : 0);
        }
        if (expr.op == "||") {
            const auto lhs = evalGlobalConst(*expr.lhs);
            if (!lhs) {
                return std::nullopt;
            }
            if (*lhs != 0) {
                return 1;
            }
            const auto rhs = evalGlobalConst(*expr.rhs);
            if (!rhs) {
                return std::nullopt;
            }
            return static_cast<std::int32_t>(*rhs != 0 ? 1 : 0);
        }

        const auto lhs = evalGlobalConst(*expr.lhs);
        const auto rhs = evalGlobalConst(*expr.rhs);
        if (!lhs || !rhs) {
            return std::nullopt;
        }
        const std::int64_t left = *lhs;
        const std::int64_t right = *rhs;
        if (expr.op == "+") {
            return toInt32(left + right);
        }
        if (expr.op == "-") {
            return toInt32(left - right);
        }
        if (expr.op == "*") {
            return toInt32(left * right);
        }
        if (expr.op == "/") {
            if (right == 0) {
                return std::nullopt;
            }
            return toInt32(left / right);
        }
        if (expr.op == "%") {
            if (right == 0) {
                return std::nullopt;
            }
            return toInt32(left % right);
        }
        if (expr.op == "<") {
            return static_cast<std::int32_t>(left < right ? 1 : 0);
        }
        if (expr.op == ">") {
            return static_cast<std::int32_t>(left > right ? 1 : 0);
        }
        if (expr.op == "<=") {
            return static_cast<std::int32_t>(left <= right ? 1 : 0);
        }
        if (expr.op == ">=") {
            return static_cast<std::int32_t>(left >= right ? 1 : 0);
        }
        if (expr.op == "==") {
            return static_cast<std::int32_t>(left == right ? 1 : 0);
        }
        if (expr.op == "!=") {
            return static_cast<std::int32_t>(left != right ? 1 : 0);
        }
        return std::nullopt;
    }

    void collectGlobals() {
        for (const auto& decl : unit_.globals) {
            const auto value = evalGlobalConst(*decl->init);
            if (!value) {
                fail(decl->loc, "global initializer is not a compile-time value");
            }

            if (decl->isConst) {
                globals_[decl->name] = Symbol{SymbolKind::Const, 0, "", *value, ""};
            } else {
                const std::string label = globalLabel(decl->name);
                globals_[decl->name] = Symbol{SymbolKind::GlobalVar, 0, label, 0, ""};
                std::ostringstream line;
                line << label << ":\n";
                line << "    .word " << printableI32(*value) << '\n';
                dataLines_.push_back(line.str());
            }
        }
    }

    std::string generateFunction(const Function& function) {
        FunctionEmitter emitter(*this, function);
        return emitter.emit();
    }
};

} // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parseOptions(argc, argv);
        const std::string source = readStdin();
        Lexer lexer(source);
        Parser parser(lexer.lex());
        CompUnit unit = parser.parseCompUnit();
        CodeGenerator generator(unit, options);
        std::cout << generator.generate();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "toyc: " << error.what() << '\n';
        return 1;
    }
}
