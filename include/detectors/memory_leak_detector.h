#include <map>
#include <set>

#include "detector_base.h"

namespace safec {

class MemoryLeakDetector : public DetectorBase {
   public:
    MemoryLeakDetector() = default;

    void analyze(Program& program) override;
    std::string getName() const override {
        return "Memory Leak Detector";
    }

    // Visitor methods
    void visit(BinaryExpression& node) override;
    void visit(UnaryExpression& node) override;
    void visit(Identifier& node) override;
    void visit(NumberLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(FunctionCall& node) override;
    void visit(ArrayAccess& node) override;

    void visit(ExpressionStatement& node) override;
    void visit(VariableDeclaration& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(BlockStatement& node) override;

    void visit(FunctionDeclaration& node) override;
    void visit(Program& node) override;

   private:
    std::set<std::string> allocatedPointers_;
    std::set<std::string> freedPointers_;
    std::string currentFunction_;

    void trackAllocation(const std::string& ptrName, int line, int column);
    void trackDeallocation(const std::string& ptrName);
    void checkForLeaks();
};

}  // namespace safec
