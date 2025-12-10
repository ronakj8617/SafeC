#ifndef SAFEC_PARSER_H
#define SAFEC_PARSER_H

#include <memory>
#include <vector>

#include "ast.h"
#include "lexer.h"

namespace safec {

class Parser {
   public:
    explicit Parser(const std::vector<Token>& tokens);

    // Parse the entire program
    std::unique_ptr<Program> parse();

    // Fallback parser for Juliet-style class methods (public for external use)
    std::unique_ptr<Program> parseJulietFallback(const std::string& sourceCode);

    // Error reporting
    bool hasErrors() const {
        return !errors_.empty();
    }
    const std::vector<std::string>& getErrors() const {
        return errors_;
    }

   private:
    std::vector<Token> tokens_;
    size_t current_;
    std::vector<std::string> errors_;

    // Helper functions
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    void consume(TokenType type, const std::string& message);
    bool isAtEnd();

    void error(const std::string& message);
    void synchronize();

    // Parsing functions
    std::unique_ptr<FunctionDeclaration> parseFunctionDeclaration();
    std::unique_ptr<VariableDeclaration> parseVariableDeclaration();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<BlockStatement> parseBlockStatement();
    std::unique_ptr<IfStatement> parseIfStatement();
    std::unique_ptr<WhileStatement> parseWhileStatement();
    std::unique_ptr<ForStatement> parseForStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();

    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseAssignment();
    std::unique_ptr<Expression> parseLogicalOr();
    std::unique_ptr<Expression> parseLogicalAnd();
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseComparison();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseFactor();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parsePostfix();
    std::unique_ptr<Expression> parsePrimary();

    // Helper for fallback parser: find matching brace
    size_t findMatchingBrace(const std::string& code, size_t startPos);
};

}  // namespace safec

#endif  // SAFEC_PARSER_H
