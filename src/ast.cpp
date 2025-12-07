#include "ast.h"

namespace safec {

// Visitor implementations for all AST nodes
void BinaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void UnaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void Identifier::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void NumberLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void StringLiteral::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionCall::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ArrayAccess::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ExpressionStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void VariableDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void IfStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void WhileStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ForStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ReturnStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void BlockStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void Program::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// Symbol Table implementation
void SymbolTable::enterScope() {
    scopes_.emplace_back();
}

void SymbolTable::exitScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

void SymbolTable::addSymbol(const std::string& name, const SymbolInfo& info) {
    if (!scopes_.empty()) {
        scopes_.back()[name] = info;
    }
}

SymbolInfo* SymbolTable::lookupSymbol(const std::string& name) {
    // Search from innermost to outermost scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}

}  // namespace safec
