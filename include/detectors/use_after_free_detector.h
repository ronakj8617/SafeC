#include <map>
#include <set>

#include "detector_base.h"

namespace safec {

class UseAfterFreeDetector : public DetectorBase {
   public:
    UseAfterFreeDetector() = default;

    void analyze(Program& program) override;
    std::string getName() const override {
        return "Use-After-Free Detector";
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
    enum class PointerState { ALLOCATED, FREED, UNKNOWN };

    std::map<std::string, PointerState> pointerStates_;
    std::set<std::string> freedPointers_;

    void checkAllocation(FunctionCall& node);
    void checkDeallocation(FunctionCall& node);
    void checkPointerUse(Identifier& node);
};

}  // namespace safec
