#include "include/lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace safec;

int main() {
    std::ifstream file("./Juliet-TestCases/CWE401_Memory_Leak/s02/CWE401_Memory_Leak__new_int_81_bad.cpp");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "Total tokens: " << tokens.size() << std::endl;
    std::cout << "\nFirst 100 tokens:\n";
    
    for (size_t i = 0; i < std::min(tokens.size(), size_t(100)); i++) {
        const auto& token = tokens[i];
        std::cout << i << ": ";
        
        if (token.type == TokenType::KEYWORD_INT) std::cout << "KEYWORD_INT";
        else if (token.type == TokenType::KEYWORD_VOID) std::cout << "KEYWORD_VOID";
        else if (token.type == TokenType::IDENTIFIER) std::cout << "IDENTIFIER";
        else if (token.type == TokenType::LPAREN) std::cout << "LPAREN";
        else if (token.type == TokenType::RPAREN) std::cout << "RPAREN";
        else if (token.type == TokenType::LBRACE) std::cout << "LBRACE";
        else if (token.type == TokenType::RBRACE) std::cout << "RBRACE";
        else if (token.type == TokenType::SEMICOLON) std::cout << "SEMICOLON";
        else if (token.type == TokenType::STAR) std::cout << "STAR";
        else std::cout << "OTHER";
        
        std::cout << " = '" << token.value << "'" << std::endl;
    }
    
    return 0;
}
