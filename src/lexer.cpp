#include "lexer.h"

#include <cctype>

namespace safec {

const std::unordered_set<std::string> Lexer::keywords_ = {
    "int",    "char",   "void",   "if",     "else", "while", "for",
    "return", "struct", "sizeof", "malloc", "free", "new",   "delete"};

Lexer::Lexer(const std::string& source) : source_(source), position_(0), line_(1), column_(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
        }
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
    }

    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();

    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", line_, column_);
    }

    char c = currentChar();
    int tokenLine = line_;
    int tokenColumn = column_;

    // Identifiers and keywords
    if (isAlpha(c) || c == '_') {
        return readIdentifierOrKeyword();
    }

    // Numbers
    if (isDigit(c)) {
        return readNumber();
    }

    // String literals
    if (c == '"') {
        return readString();
    }

    // Character literals
    if (c == '\'') {
        return readChar();
    }

    // Two-character operators
    if (c == '=' && peekChar() == '=') {
        advance();
        advance();
        return Token(TokenType::EQUAL, "==", tokenLine, tokenColumn);
    }
    if (c == '!' && peekChar() == '=') {
        advance();
        advance();
        return Token(TokenType::NOT_EQUAL, "!=", tokenLine, tokenColumn);
    }
    if (c == '<' && peekChar() == '=') {
        advance();
        advance();
        return Token(TokenType::LESS_EQUAL, "<=", tokenLine, tokenColumn);
    }
    if (c == '>' && peekChar() == '=') {
        advance();
        advance();
        return Token(TokenType::GREATER_EQUAL, ">=", tokenLine, tokenColumn);
    }
    if (c == '&' && peekChar() == '&') {
        advance();
        advance();
        return Token(TokenType::AND, "&&", tokenLine, tokenColumn);
    }
    if (c == '|' && peekChar() == '|') {
        advance();
        advance();
        return Token(TokenType::OR, "||", tokenLine, tokenColumn);
    }

    // Single-character tokens
    advance();
    switch (c) {
        case '+':
            return Token(TokenType::PLUS, "+", tokenLine, tokenColumn);
        case '-':
            return Token(TokenType::MINUS, "-", tokenLine, tokenColumn);
        case '*':
            return Token(TokenType::STAR, "*", tokenLine, tokenColumn);
        case '/':
            return Token(TokenType::SLASH, "/", tokenLine, tokenColumn);
        case '%':
            return Token(TokenType::PERCENT, "%", tokenLine, tokenColumn);
        case '=':
            return Token(TokenType::ASSIGN, "=", tokenLine, tokenColumn);
        case '<':
            return Token(TokenType::LESS, "<", tokenLine, tokenColumn);
        case '>':
            return Token(TokenType::GREATER, ">", tokenLine, tokenColumn);
        case '!':
            return Token(TokenType::NOT, "!", tokenLine, tokenColumn);
        case '&':
            return Token(TokenType::AMPERSAND, "&", tokenLine, tokenColumn);
        case '|':
            return Token(TokenType::PIPE, "|", tokenLine, tokenColumn);
        case ';':
            return Token(TokenType::SEMICOLON, ";", tokenLine, tokenColumn);
        case ',':
            return Token(TokenType::COMMA, ",", tokenLine, tokenColumn);
        case '(':
            return Token(TokenType::LPAREN, "(", tokenLine, tokenColumn);
        case ')':
            return Token(TokenType::RPAREN, ")", tokenLine, tokenColumn);
        case '{':
            return Token(TokenType::LBRACE, "{", tokenLine, tokenColumn);
        case '}':
            return Token(TokenType::RBRACE, "}", tokenLine, tokenColumn);
        case '[':
            return Token(TokenType::LBRACKET, "[", tokenLine, tokenColumn);
        case ']':
            return Token(TokenType::RBRACKET, "]", tokenLine, tokenColumn);
        default:
            return Token(TokenType::UNKNOWN, std::string(1, c), tokenLine, tokenColumn);
    }
}

Token Lexer::peekToken() {
    size_t savedPos = position_;
    int savedLine = line_;
    int savedCol = column_;

    Token token = nextToken();

    position_ = savedPos;
    line_ = savedLine;
    column_ = savedCol;

    return token;
}

char Lexer::currentChar() {
    if (isAtEnd())
        return '\0';
    return source_[position_];
}

char Lexer::peekChar() {
    if (position_ + 1 >= source_.length())
        return '\0';
    return source_[position_ + 1];
}

void Lexer::advance() {
    if (isAtEnd())
        return;

    if (source_[position_] == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    position_++;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = currentChar();

        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '/' && peekChar() == '/') {
            // Single-line comment
            while (!isAtEnd() && currentChar() != '\n') {
                advance();
            }
        } else if (c == '/' && peekChar() == '*') {
            // Multi-line comment
            advance();  // skip '/'
            advance();  // skip '*'
            while (!isAtEnd()) {
                if (currentChar() == '*' && peekChar() == '/') {
                    advance();  // skip '*'
                    advance();  // skip '/'
                    break;
                }
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::readIdentifierOrKeyword() {
    int tokenLine = line_;
    int tokenColumn = column_;
    std::string value;

    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }

    TokenType type = getKeywordType(value);
    return Token(type, value, tokenLine, tokenColumn);
}

Token Lexer::readNumber() {
    int tokenLine = line_;
    int tokenColumn = column_;
    std::string value;

    while (!isAtEnd() && isDigit(currentChar())) {
        value += currentChar();
        advance();
    }

    return Token(TokenType::NUMBER, value, tokenLine, tokenColumn);
}

Token Lexer::readString() {
    int tokenLine = line_;
    int tokenColumn = column_;
    std::string value;

    advance();  // skip opening quote

    while (!isAtEnd() && currentChar() != '"') {
        if (currentChar() == '\\') {
            advance();
            if (!isAtEnd()) {
                value += currentChar();
                advance();
            }
        } else {
            value += currentChar();
            advance();
        }
    }

    if (!isAtEnd()) {
        advance();  // skip closing quote
    }

    return Token(TokenType::STRING_LITERAL, value, tokenLine, tokenColumn);
}

Token Lexer::readChar() {
    int tokenLine = line_;
    int tokenColumn = column_;
    std::string value;

    advance();  // skip opening quote

    if (!isAtEnd() && currentChar() != '\'') {
        if (currentChar() == '\\') {
            advance();
            if (!isAtEnd()) {
                value += currentChar();
                advance();
            }
        } else {
            value += currentChar();
            advance();
        }
    }

    if (!isAtEnd()) {
        advance();  // skip closing quote
    }

    return Token(TokenType::CHAR_LITERAL, value, tokenLine, tokenColumn);
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isAtEnd() {
    return position_ >= source_.length();
}

TokenType Lexer::getKeywordType(const std::string& word) {
    if (keywords_.find(word) == keywords_.end()) {
        return TokenType::IDENTIFIER;
    }

    if (word == "int")
        return TokenType::KEYWORD_INT;
    if (word == "char")
        return TokenType::KEYWORD_CHAR;
    if (word == "void")
        return TokenType::KEYWORD_VOID;
    if (word == "if")
        return TokenType::KEYWORD_IF;
    if (word == "else")
        return TokenType::KEYWORD_ELSE;
    if (word == "while")
        return TokenType::KEYWORD_WHILE;
    if (word == "for")
        return TokenType::KEYWORD_FOR;
    if (word == "return")
        return TokenType::KEYWORD_RETURN;
    if (word == "struct")
        return TokenType::KEYWORD_STRUCT;
    if (word == "sizeof")
        return TokenType::KEYWORD_SIZEOF;
    if (word == "malloc")
        return TokenType::KEYWORD_MALLOC;
    if (word == "free")
        return TokenType::KEYWORD_FREE;
    if (word == "new")
        return TokenType::KEYWORD_NEW;
    if (word == "delete")
        return TokenType::KEYWORD_DELETE;

    return TokenType::IDENTIFIER;
}

}  // namespace safec
