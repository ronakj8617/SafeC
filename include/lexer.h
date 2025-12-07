#ifndef SAFEC_LEXER_H
#define SAFEC_LEXER_H

#include <string>
#include <unordered_set>
#include <vector>

namespace safec {

// Token types
enum class TokenType {
    // Keywords
    KEYWORD_INT,
    KEYWORD_CHAR,
    KEYWORD_VOID,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_RETURN,
    KEYWORD_STRUCT,
    KEYWORD_SIZEOF,
    KEYWORD_MALLOC,
    KEYWORD_FREE,
    KEYWORD_NEW,
    KEYWORD_DELETE,

    // Identifiers and literals
    IDENTIFIER,
    NUMBER,
    STRING_LITERAL,
    CHAR_LITERAL,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    AND,
    OR,
    NOT,
    AMPERSAND,
    PIPE,

    // Punctuation
    SEMICOLON,
    COMMA,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,

    // Special
    END_OF_FILE,
    UNKNOWN
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

// Lexer class
class Lexer {
   public:
    explicit Lexer(const std::string& source);

    // Tokenize the entire source
    std::vector<Token> tokenize();

    // Get next token
    Token nextToken();

    // Peek at next token without consuming
    Token peekToken();

   private:
    std::string source_;
    size_t position_;
    int line_;
    int column_;

    // Helper functions
    char currentChar();
    char peekChar();
    void advance();
    void skipWhitespace();
    void skipComment();

    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readString();
    Token readChar();

    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
    bool isAtEnd();

    TokenType getKeywordType(const std::string& word);

    static const std::unordered_set<std::string> keywords_;
};

}  // namespace safec

#endif  // SAFEC_LEXER_H
