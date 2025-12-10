#include "parser.h"

#include <iostream>

namespace safec {

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens), current_(0) {}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();

    int functionCount = 0;
    int tokenCount = 0;

    while (!isAtEnd()) {
        tokenCount++;
        
        if (tokenCount > 100000) {
            error("Too many tokens processed - possible infinite loop");
            break;
        }

        try {
            size_t positionBefore = current_;
            
            // Skip preprocessor directives
            if (peek().value == "#") {
                while (!isAtEnd() && peek().type != TokenType::SEMICOLON && 
                       peek().value != "\n") {
                    advance();
                }
                continue;
            }

            // Skip namespace declarations
            if (peek().value == "namespace") {
                advance(); // skip "namespace"
                if (check(TokenType::IDENTIFIER)) {
                    advance(); // skip namespace name
                }
                if (peek().value == "{") {
                    advance(); // skip "{"
                }
                continue;
            }

            // Skip closing braces for namespaces
            if (peek().value == "}") {
                advance();
                continue;
            }

            // Try to parse function declaration
            // Functions can start with: return type (int, void, char, etc.)
            // OR any identifier (user-defined types)
            if (isReturnType(peek())) {
                auto func = parseFunctionDeclaration();
                if (func) {
                    functionCount++;
                    program->functions.push_back(std::move(func));
                }
            } else {
                // Skip unknown tokens
                advance();
            }
            
            // If we didn't advance, force advance to prevent infinite loop
            if (current_ == positionBefore && !isAtEnd()) {
                error("Parser stuck at top level - forcing advance");
                advance();
            }
        } catch (const std::exception& e) {
            error(std::string("Exception during parsing: ") + e.what());
            synchronize();
        } catch (...) {
            error("Unknown exception during parsing");
            synchronize();
        }
    }

    if (functionCount > 0 || !errors_.empty()) {
        std::cout << "Parsed " << functionCount << " function(s)";
        if (!errors_.empty()) {
            std::cout << " with " << errors_.size() << " error(s)";
        }
        std::cout << std::endl;
    }

    return program;
}

std::unique_ptr<FunctionDeclaration> Parser::parseFunctionDeclaration() {
    auto func = std::make_unique<FunctionDeclaration>();

    // Return type (can be multiple tokens: "unsigned int", "const char*", etc.)
    std::string returnType;
    
    // Collect all tokens that form the return type
    while (!isAtEnd() && peek().type != TokenType::LPAREN &&
           peek().value != "::") {
        if (peek().type == TokenType::IDENTIFIER && 
            (peek().value == "const" || peek().value == "unsigned" || 
             peek().value == "signed" || peek().value == "volatile")) {
            // These are type qualifiers
            returnType += peek().value;
            returnType += " ";
            func->line = peek().line;
            func->column = peek().column;
            advance();
        } else if (peek().type == TokenType::KEYWORD_INT || 
                   peek().type == TokenType::KEYWORD_VOID ||
                   peek().type == TokenType::KEYWORD_CHAR) {
            // Keyword type
            returnType += peek().value;
            returnType += " ";
            func->line = peek().line;
            func->column = peek().column;
            advance();
        } else if (peek().type == TokenType::IDENTIFIER) {
            // Could be user-defined type or start of function name
            // Peek ahead to distinguish
            size_t lookahead = current_ + 1;
            if (lookahead < tokens_.size()) {
                const Token& next = tokens_[lookahead];
                // If next token is also an identifier (not LPAREN), then current is return type
                if (next.type == TokenType::IDENTIFIER) {
                    returnType += peek().value;
                    returnType += " ";
                    func->line = peek().line;
                    func->column = peek().column;
                    advance();
                } else if (next.type == TokenType::LPAREN) {
                    // This identifier is the function name, not part of return type
                    break;
                } else {
                    // Could be return type
                    returnType += peek().value;
                    returnType += " ";
                    func->line = peek().line;
                    func->column = peek().column;
                    advance();
                }
            } else {
                break;
            }
        } else {
            // Star, ampersand, or other type modifier
            returnType += peek().value;
            func->line = peek().line;
            func->column = peek().column;
            advance();
        }
    }
    
    // Remove trailing space
    if (!returnType.empty() && returnType.back() == ' ') {
        returnType.pop_back();
    }
    func->returnType = returnType;

    // Function name (handle class methods where two identifiers appear before LPAREN)
    std::string functionName;
    
    if (check(TokenType::IDENTIFIER)) {
        std::string firstName = advance().value;
        
        // Check if next token is another identifier (class method pattern)
        if (check(TokenType::IDENTIFIER)) {
            // This is a class method: firstName::secondName
            // The :: was skipped by the lexer, so we just have two identifiers
            functionName = advance().value;  // The actual method name
        } else {
            // Simple function
            functionName = firstName;
        }
    } else {
        error("Expected function or method name");
        return nullptr;
    }
    
    func->name = functionName;

    // Skip qualifiers like "const", "override", etc.
    while (!isAtEnd() && (peek().value == "const" || peek().value == "override" ||
                          peek().value == "virtual" || peek().value == "noexcept")) {
        advance();
    }

    // Parameters
    if (!match(TokenType::LPAREN)) {
        error("Expected '(' after function name");
        return nullptr;
    }

    int paramTokenCount = 0;
    const int MAX_PARAM_TOKENS = 1000;

    while (!check(TokenType::RPAREN) && !isAtEnd()) {
        paramTokenCount++;
        if (paramTokenCount > MAX_PARAM_TOKENS) {
            error("Parameter list too long or malformed - skipping function");
            while (!isAtEnd() && !check(TokenType::SEMICOLON) && !check(TokenType::LBRACE)) {
                advance();
            }
            return nullptr;
        }

        // Parse parameter type (may be compound like "unsigned int", "const char*", etc.)
        std::string paramType;
        
        // Collect all tokens that could be part of the type
        while (!isAtEnd() && (check(TokenType::IDENTIFIER) || check(TokenType::KEYWORD_INT) ||
                              check(TokenType::KEYWORD_CHAR) || check(TokenType::KEYWORD_VOID) ||
                              check(TokenType::STAR) || check(TokenType::AMPERSAND) ||
                              peek().value == "const" || peek().value == "unsigned" ||
                              peek().value == "signed")) {
            paramType += advance().value;
            paramType += " ";
        }

        // If we collected any type tokens, parse the parameter name
        if (!paramType.empty()) {
            paramType.pop_back();  // Remove trailing space
            
            std::string paramName;
            if (check(TokenType::IDENTIFIER)) {
                paramName = advance().value;
                // Handle pointer indicators in parameter name (e.g., *ptr)
                while (check(TokenType::STAR)) {
                    paramName = advance().value + paramName;
                }
            }
            
            func->parameters.push_back({paramType, paramName});

            // Check for comma between parameters
            if (check(TokenType::COMMA)) {
                advance();
            }
        } else if (!check(TokenType::RPAREN)) {
            // No type tokens found and not at closing paren - skip to recover
            error("Unexpected token in parameter list");
            advance();
        }
    }

    if (!match(TokenType::RPAREN)) {
        error("Expected ')' after parameters");
        return nullptr;
    }

    // Skip trailing qualifiers like "const override noexcept"
    while (!isAtEnd() && (peek().value == "const" || peek().value == "override" ||
                          peek().value == "noexcept" || peek().value == "volatile")) {
        advance();
    }

    // Function body or semicolon
    if (check(TokenType::LBRACE)) {
        func->body = parseBlockStatement();
    } else if (match(TokenType::SEMICOLON)) {
        // Function declaration without body
        func->body = nullptr;
    } else {
        error("Expected ';' or '{' after function declaration");
        return nullptr;
    }

    return func;
}

std::unique_ptr<BlockStatement> Parser::parseBlockStatement() {
    auto block = std::make_unique<BlockStatement>();

    consume(TokenType::LBRACE, "Expected '{'");

    // Add safety counter to prevent infinite loops
    int stmtCount = 0;
    const int MAX_STATEMENTS = 10000;  // Reasonable limit for statements in a block

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        stmtCount++;
        if (stmtCount > MAX_STATEMENTS) {
            error("Block statement too long or malformed - skipping rest of block");
            // Skip to closing brace
            while (!isAtEnd() && !check(TokenType::RBRACE)) {
                advance();
            }
            break;
        }

        size_t positionBefore = current_;
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
        
        // If we didn't advance, force advance to prevent infinite loop
        if (current_ == positionBefore) {
            error("Parser stuck - forcing advance");
            advance();
        }
    }

    consume(TokenType::RBRACE, "Expected '}'");

    return block;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    // Variable declaration
    if (check(TokenType::KEYWORD_INT) || check(TokenType::KEYWORD_CHAR)) {
        return parseVariableDeclaration();
    }

    // Control flow
    if (match(TokenType::KEYWORD_IF)) {
        return parseIfStatement();
    }
    if (match(TokenType::KEYWORD_WHILE)) {
        return parseWhileStatement();
    }
    if (match(TokenType::KEYWORD_FOR)) {
        return parseForStatement();
    }
    if (match(TokenType::KEYWORD_RETURN)) {
        return parseReturnStatement();
    }

    // Block statement
    if (check(TokenType::LBRACE)) {
        return parseBlockStatement();
    }

    // Expression statement
    return parseExpressionStatement();
}

std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration() {
    auto decl = std::make_unique<VariableDeclaration>();

    Token typeToken = advance();
    decl->type = typeToken.value;
    decl->line = typeToken.line;
    decl->column = typeToken.column;

    // Check for pointer type (e.g., int*)
    if (check(TokenType::STAR)) {
        advance();
        decl->type += "*";  // Append * to type
    }

    if (!check(TokenType::IDENTIFIER)) {
        error("Expected variable name");
        return nullptr;
    }

    decl->name = advance().value;
    decl->isArray = false;
    decl->arraySize = 0;

    // Check for array
    if (match(TokenType::LBRACKET)) {
        decl->isArray = true;
        if (check(TokenType::NUMBER)) {
            decl->arraySize = std::stoi(advance().value);
        }
        consume(TokenType::RBRACKET, "Expected ']'");
    }

    // Initializer
    if (match(TokenType::ASSIGN)) {
        decl->initializer = parseExpression();
    }

    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");

    return decl;
}

std::unique_ptr<IfStatement> Parser::parseIfStatement() {
    auto ifStmt = std::make_unique<IfStatement>();

    consume(TokenType::LPAREN, "Expected '(' after 'if'");
    ifStmt->condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after condition");

    ifStmt->thenBranch = parseStatement();

    if (match(TokenType::KEYWORD_ELSE)) {
        ifStmt->elseBranch = parseStatement();
    }

    return ifStmt;
}

std::unique_ptr<WhileStatement> Parser::parseWhileStatement() {
    auto whileStmt = std::make_unique<WhileStatement>();

    consume(TokenType::LPAREN, "Expected '(' after 'while'");
    whileStmt->condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after condition");

    whileStmt->body = parseStatement();

    return whileStmt;
}

std::unique_ptr<ForStatement> Parser::parseForStatement() {
    auto forStmt = std::make_unique<ForStatement>();

    consume(TokenType::LPAREN, "Expected '(' after 'for'");

    // Initializer
    if (!check(TokenType::SEMICOLON)) {
        if (check(TokenType::KEYWORD_INT) || check(TokenType::KEYWORD_CHAR)) {
            forStmt->initializer = parseVariableDeclaration();
        } else {
            forStmt->initializer = parseExpressionStatement();
        }
    } else {
        advance();
    }

    // Condition
    if (!check(TokenType::SEMICOLON)) {
        forStmt->condition = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for condition");

    // Increment
    if (!check(TokenType::RPAREN)) {
        forStmt->increment = parseExpression();
    }
    consume(TokenType::RPAREN, "Expected ')' after for clauses");

    forStmt->body = parseStatement();

    return forStmt;
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto returnStmt = std::make_unique<ReturnStatement>();

    if (!check(TokenType::SEMICOLON)) {
        returnStmt->value = parseExpression();
    }

    consume(TokenType::SEMICOLON, "Expected ';' after return statement");

    return returnStmt;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto exprStmt = std::make_unique<ExpressionStatement>();
    exprStmt->expression = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return exprStmt;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseAssignment();
}

std::unique_ptr<Expression> Parser::parseAssignment() {
    auto expr = parseLogicalOr();

    if (match(TokenType::ASSIGN)) {
        auto binExpr = std::make_unique<BinaryExpression>();
        binExpr->op = "=";
        binExpr->left = std::move(expr);
        binExpr->right = parseAssignment();
        return binExpr;
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();

    while (match(TokenType::OR)) {
        auto binExpr = std::make_unique<BinaryExpression>();
        binExpr->op = "||";
        binExpr->left = std::move(expr);
        binExpr->right = parseLogicalAnd();
        expr = std::move(binExpr);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseLogicalAnd() {
    auto expr = parseEquality();

    while (match(TokenType::AND)) {
        auto binExpr = std::make_unique<BinaryExpression>();
        binExpr->op = "&&";
        binExpr->left = std::move(expr);
        binExpr->right = parseEquality();
        expr = std::move(binExpr);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto expr = parseComparison();

    while (match({TokenType::EQUAL, TokenType::NOT_EQUAL})) {
        auto binExpr = std::make_unique<BinaryExpression>();
        binExpr->op = previous().value;
        binExpr->left = std::move(expr);
        binExpr->right = parseComparison();
        expr = std::move(binExpr);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseComparison() {
    auto expr = parseTerm();

    while (match(
        {TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL})) {
        auto binExpr = std::make_unique<BinaryExpression>();
        binExpr->op = previous().value;
        binExpr->left = std::move(expr);
        binExpr->right = parseTerm();
        expr = std::move(binExpr);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match({TokenType::PLUS, TokenType::MINUS})) {
        auto binExpr = std::make_unique<BinaryExpression>();
        binExpr->op = previous().value;
        binExpr->left = std::move(expr);
        binExpr->right = parseFactor();
        expr = std::move(binExpr);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    auto expr = parseUnary();

    while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
        auto binExpr = std::make_unique<BinaryExpression>();
        binExpr->op = previous().value;
        binExpr->left = std::move(expr);
        binExpr->right = parseUnary();
        expr = std::move(binExpr);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseUnary() {
    if (match({TokenType::NOT, TokenType::MINUS, TokenType::STAR, TokenType::AMPERSAND})) {
        auto unaryExpr = std::make_unique<UnaryExpression>();
        unaryExpr->op = previous().value;
        unaryExpr->operand = parseUnary();
        return unaryExpr;
    }

    return parsePostfix();
}

std::unique_ptr<Expression> Parser::parsePostfix() {
    auto expr = parsePrimary();

    while (true) {
        if (match(TokenType::LBRACKET)) {
            // Array access
            auto arrayAccess = std::make_unique<ArrayAccess>();
            arrayAccess->array = std::move(expr);
            arrayAccess->index = parseExpression();
            arrayAccess->line = previous().line;
            arrayAccess->column = previous().column;
            consume(TokenType::RBRACKET, "Expected ']'");
            expr = std::move(arrayAccess);
        } else if (match(TokenType::LPAREN)) {
            // Function call
            auto funcCall = std::make_unique<FunctionCall>();
            if (auto* id = dynamic_cast<Identifier*>(expr.get())) {
                funcCall->functionName = id->name;
                funcCall->line = id->line;
                funcCall->column = id->column;
            }

            // Arguments
            while (!check(TokenType::RPAREN) && !isAtEnd()) {
                // Parse an argument; if parsing fails (nullptr) recover by
                // skipping to the next comma or closing parenthesis to avoid
                // an infinite loop where parseExpression repeatedly returns
                // nullptr without advancing the token stream.
                auto arg = parseExpression();
                if (!arg) {
                    // Skip tokens until we find a comma or closing paren
                    while (!isAtEnd() && !check(TokenType::COMMA) && !check(TokenType::RPAREN)) {
                        advance();
                    }
                    // If there's a comma, consume it so the loop can continue
                    if (check(TokenType::COMMA)) {
                        advance();
                        continue;
                    } else {
                        break;
                    }
                }

                funcCall->arguments.push_back(std::move(arg));

                if (!check(TokenType::RPAREN)) {
                    if (check(TokenType::COMMA)) {
                        advance();
                    } else {
                        // Missing comma - record an error and try to recover
                        consume(TokenType::COMMA, "Expected ',' between arguments");
                        break;
                    }
                }
            }

            consume(TokenType::RPAREN, "Expected ')' after arguments");
            expr = std::move(funcCall);
        } else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        auto numLit = std::make_unique<NumberLiteral>();
        numLit->value = std::stoi(previous().value);
        numLit->line = previous().line;
        numLit->column = previous().column;
        return numLit;
    }

    if (match(TokenType::STRING_LITERAL)) {
        auto strLit = std::make_unique<StringLiteral>();
        strLit->value = previous().value;
        strLit->line = previous().line;
        strLit->column = previous().column;
        return strLit;
    }

    if (match(TokenType::IDENTIFIER)) {
        auto id = std::make_unique<Identifier>();
        id->name = previous().value;
        id->line = previous().line;
        id->column = previous().column;
        return id;
    }

    // Handle malloc, free, new, delete as if they were identifiers (function names)
    if (match({TokenType::KEYWORD_MALLOC, TokenType::KEYWORD_FREE, TokenType::KEYWORD_NEW, TokenType::KEYWORD_DELETE})) {
        std::string keyword = previous().value;
        int kw_line = previous().line;
        int kw_column = previous().column;
        
        // Special handling for 'new' - it takes a type, not parentheses
        // e.g., new int, new char*, new MyClass
        if (keyword == "new") {
            auto funcCall = std::make_unique<FunctionCall>();
            funcCall->functionName = "new";
            funcCall->line = kw_line;
            funcCall->column = kw_column;
            
            // Consume the type being allocated (int, char*, MyClass, etc.)
            // We don't need to parse it correctly, just skip it for now
            // to identify that this is a new allocation
            if (check(TokenType::KEYWORD_INT) || check(TokenType::KEYWORD_CHAR) || 
                check(TokenType::KEYWORD_VOID) || check(TokenType::IDENTIFIER)) {
                advance();  // Skip the type name
                
                // Handle pointer/reference qualifiers
                while (match({TokenType::STAR, TokenType::AMPERSAND})) {
                    // Skip these
                }
                
                // Handle array brackets for new[] syntax
                if (check(TokenType::LBRACKET)) {
                    advance();  // Skip [
                    // Parse the array size expression (or skip it)
                    while (!check(TokenType::RBRACKET) && !isAtEnd()) {
                        advance();
                    }
                    if (match(TokenType::RBRACKET)) {
                        // Matched ]
                    }
                }
            }
            
            return funcCall;
        }
        
        // For other keywords, create an identifier as before
        auto id = std::make_unique<Identifier>();
        id->name = keyword;
        id->line = kw_line;
        id->column = kw_column;
        return id;
    }

    if (match(TokenType::LPAREN)) {
        // Could be a cast like (int*) or (type) or a parenthesized expression
        // Try to detect if this is a cast
        bool isCast = false;
        size_t tempPos = current_;
        int tokenCount = 0;
        
        // Scan to see if this looks like a cast
        while (!isAtEnd() && !check(TokenType::RPAREN) && tokenCount < 10) {
            if (check(TokenType::KEYWORD_INT) || check(TokenType::KEYWORD_CHAR) || 
                check(TokenType::KEYWORD_VOID) || check(TokenType::IDENTIFIER) ||
                check(TokenType::STAR) || check(TokenType::AMPERSAND)) {
                advance();
                tokenCount++;
            } else {
                break;
            }
        }
        
        // If we found ) and only type-like tokens, it's likely a cast
        if (check(TokenType::RPAREN)) {
            isCast = true;
        }
        
        // Reset position
        current_ = tempPos;
        
        if (isCast) {
            // Skip the type cast - consume tokens until RPAREN
            while (!check(TokenType::RPAREN) && !isAtEnd()) {
                advance();
            }
            if (match(TokenType::RPAREN)) {
                // Now parse the actual expression being cast
                // Use parsePostfix to properly handle function calls like malloc()
                return parsePostfix();
            }
        }
        
        // Not a cast, parse as normal parenthesized expression
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    error("Expected expression");
    return nullptr;
}

// Helper methods
Token Parser::peek() {
    if (tokens_.empty()) {
        // Return a dummy EOF token if no tokens
        return Token{TokenType::END_OF_FILE, "", 0, 0};
    }
    if (current_ >= tokens_.size()) {
        return tokens_.back();
    }
    return tokens_[current_];
}

Token Parser::previous() {
    if (current_ > 0 && current_ <= tokens_.size()) {
        return tokens_[current_ - 1];
    }
    if (!tokens_.empty()) {
        return tokens_[0];
    }
    return Token{TokenType::END_OF_FILE, "", 0, 0};
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd())
        return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    error(message);
}

bool Parser::isAtEnd() {
    if (tokens_.empty()) {
        return true;
    }
    if (current_ >= tokens_.size()) {
        return true;
    }
    // Check if current token is EOF without calling peek() (avoid circular dependency)
    return tokens_[current_].type == TokenType::END_OF_FILE;
}

void Parser::error(const std::string& message) {
    Token token = peek();
    std::string errorMsg = "Parse error at line " + std::to_string(token.line) + ", column " +
                           std::to_string(token.column) + ": " + message;
    errors_.push_back(errorMsg);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON)
            return;

        switch (peek().type) {
            case TokenType::KEYWORD_IF:
            case TokenType::KEYWORD_WHILE:
            case TokenType::KEYWORD_FOR:
            case TokenType::KEYWORD_RETURN:
            case TokenType::KEYWORD_INT:
            case TokenType::KEYWORD_CHAR:
            case TokenType::KEYWORD_VOID:
                return;
            default:
                advance();
        }
    }
}

bool Parser::isReturnType(const Token& token) {
    // Check for primitive return types
    if (token.type == TokenType::KEYWORD_INT ||
        token.type == TokenType::KEYWORD_VOID ||
        token.type == TokenType::KEYWORD_CHAR) {
        return true;
    }
    
    // Check for user-defined types (identifiers that could be return types)
    if (token.type == TokenType::IDENTIFIER) {
        // Common patterns: if next token is another identifier or ::, likely a return type
        // Peek ahead to see if this looks like a function signature
        size_t lookahead = current_ + 1;
        if (lookahead < tokens_.size()) {
            const Token& next = tokens_[lookahead];
            // Cases like: "MyClass name" or "std::string name" or "unsigned int name"
            if (next.type == TokenType::IDENTIFIER || next.value == "::") {
                return true;
            }
        }
        return true; // assume identifier could be return type
    }
    
    return false;
}

}  // namespace safec
