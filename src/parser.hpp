#pragma once

#include "ast.hpp"
#include "lexer.hpp"

namespace toyc {

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

} // namespace toyc
