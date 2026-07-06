// compiler2026-x phase-2 (parser split)

#include "parser.h"

using namespace std;

Parser::Parser(vector<Token> tokens) : tokens_(std::move(tokens)) {}

Program Parser::parseProgram() {
    Program program;
    while (!check(TokenKind::End)) {
      TopItem item;
      if (check(TokenKind::KwConst)) {
        item.decl = parseDecl();
      } else if (check(TokenKind::KwVoid) || check(TokenKind::KwInt)) {
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

const Token &Parser::tok(size_t ahead) const {
    size_t p = min(pos_ + ahead, tokens_.size() - 1);
    return tokens_[p];
  }

bool Parser::check(TokenKind kind) const { return tok().kind == kind; }

bool Parser::match(TokenKind kind) {
    if (!check(kind)) {
      return false;
    }
    ++pos_;
    return true;
  }

const Token &Parser::expect(TokenKind kind, const string &what) {
    if (!check(kind)) {
      throw error("expected " + what);
    }
    return tokens_[pos_++];
  }

CompileError Parser::error(const string &message) const {
    ostringstream os;
    os << "line " << tok().line << ":" << tok().col << ": " << message;
    return CompileError(os.str());
  }

BaseType Parser::parseBType() {
    if (match(TokenKind::KwInt)) {
      return BaseType::Int;
    }
    throw error("expected int");
  }

BaseType Parser::parseFuncType() {
    if (match(TokenKind::KwVoid)) {
      return BaseType::Void;
    }
    return parseBType();
  }

unique_ptr<DeclStmt> Parser::parseDecl() {
    bool isConst = match(TokenKind::KwConst);
    BaseType base = parseBType();
    auto decl = make_unique<DeclStmt>(tok().line, isConst, base);
    decl->defs.push_back(parseVarDef(true));
    expect(TokenKind::Semicolon, "';'");
    return decl;
  }

VarDef Parser::parseVarDef(bool requireInit) {
    VarDef def;
    const Token &name = expect(TokenKind::Ident, "identifier");
    def.name = name.text;
    def.line = name.line;
    if (!match(TokenKind::Assign)) {
      if (requireInit) {
        throw error("declaration requires initializer");
      }
      return def;
    }
    def.init = parseInitVal();
    return def;
  }

unique_ptr<InitVal> Parser::parseInitVal() {
    auto init = make_unique<InitVal>();
    init->expr = parseExp();
    return init;
  }

unique_ptr<FuncDef> Parser::parseFuncDef() {
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

Param Parser::parseParam() {
    Param param;
    param.line = tok().line;
    param.base = parseBType();
    const Token &name = expect(TokenKind::Ident, "parameter name");
    param.name = name.text;
    return param;
  }

unique_ptr<BlockStmt> Parser::parseBlock() {
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

StmtPtr Parser::parseStmt() {
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
