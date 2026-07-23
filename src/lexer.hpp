#pragma once

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

#include "common.hpp"

namespace toyc {

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

} // namespace toyc
