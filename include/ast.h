#ifndef SAFEC_AST_H
#define SAFEC_AST_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace safec {

// Forward declarations
class ASTVisitor;

// Base AST Node
class ASTNode {
   public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;

    int line;
    int column;
};

// Expression nodes
class Expression : public ASTNode {
   public:
    virtual ~Expression() = default;
};

class BinaryExpression : public Expression {
   public:
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    void accept(ASTVisitor& visitor) override;
};

class UnaryExpression : public Expression {
   public:
    std::string op;
    std::unique_ptr<Expression> operand;

    void accept(ASTVisitor& visitor) override;
};

class Identifier : public Expression {
   public:
    std::string name;

    void accept(ASTVisitor& visitor) override;
};

class NumberLiteral : public Expression {
   public:
    int value;

    void accept(ASTVisitor& visitor) override;
};

class StringLiteral : public Expression {
   public:
    std::string value;

    void accept(ASTVisitor& visitor) override;
};

class FunctionCall : public Expression {
   public:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;

    void accept(ASTVisitor& visitor) override;
};

class ArrayAccess : public Expression {
   public:
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;

    void accept(ASTVisitor& visitor) override;
};

// Statement nodes
class Statement : public ASTNode {
   public:
    virtual ~Statement() = default;
};

class ExpressionStatement : public Statement {
   public:
    std::unique_ptr<Expression> expression;

    void accept(ASTVisitor& visitor) override;
};

class VariableDeclaration : public Statement {
   public:
    std::string type;
    std::string name;
    std::unique_ptr<Expression> initializer;
    bool isArray;
    int arraySize;

    void accept(ASTVisitor& visitor) override;
};

class IfStatement : public Statement {
   public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;

    void accept(ASTVisitor& visitor) override;
};

class WhileStatement : public Statement {
   public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    void accept(ASTVisitor& visitor) override;
};

class ForStatement : public Statement {
   public:
    std::unique_ptr<Statement> initializer;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> increment;
    std::unique_ptr<Statement> body;

    void accept(ASTVisitor& visitor) override;
};

class ReturnStatement : public Statement {
   public:
    std::unique_ptr<Expression> value;

    void accept(ASTVisitor& visitor) override;
};

class BlockStatement : public Statement {
   public:
    std::vector<std::unique_ptr<Statement>> statements;

    void accept(ASTVisitor& visitor) override;
};

// Function declaration
class FunctionDeclaration : public ASTNode {
   public:
    std::string returnType;
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters;  // (type, name)
    std::unique_ptr<BlockStatement> body;

    void accept(ASTVisitor& visitor) override;
};

// Program (root node)
class Program : public ASTNode {
   public:
    std::vector<std::unique_ptr<FunctionDeclaration>> functions;
    std::vector<std::unique_ptr<VariableDeclaration>> globalVariables;

    void accept(ASTVisitor& visitor) override;
};

// Visitor pattern for AST traversal
class ASTVisitor {
   public:
    virtual ~ASTVisitor() = default;

    virtual void visit(BinaryExpression& node) = 0;
    virtual void visit(UnaryExpression& node) = 0;
    virtual void visit(Identifier& node) = 0;
    virtual void visit(NumberLiteral& node) = 0;
    virtual void visit(StringLiteral& node) = 0;
    virtual void visit(FunctionCall& node) = 0;
    virtual void visit(ArrayAccess& node) = 0;

    virtual void visit(ExpressionStatement& node) = 0;
    virtual void visit(VariableDeclaration& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;
    virtual void visit(ForStatement& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(BlockStatement& node) = 0;

    virtual void visit(FunctionDeclaration& node) = 0;
    virtual void visit(Program& node) = 0;
};

// Symbol table for tracking variables and their properties
struct SymbolInfo {
    std::string name;
    std::string type;
    bool isArray;
    int arraySize;
    int declarationLine;
};

class SymbolTable {
   public:
    void enterScope();
    void exitScope();
    void addSymbol(const std::string& name, const SymbolInfo& info);
    SymbolInfo* lookupSymbol(const std::string& name);

   private:
    std::vector<std::map<std::string, SymbolInfo>> scopes_;
};

}  // namespace safec

#endif  // SAFEC_AST_H
