#ifndef SAFEC_POINTER_SAFETY_DETECTOR_H
#define SAFEC_POINTER_SAFETY_DETECTOR_H

#include <map>
#include <set>
#include <string>

#include "detector_base.h"

namespace safec {

enum class PointerState { UNINITIALIZED, NULL_PTR, VALID, FREED, UNKNOWN };

class PointerSafetyDetector : public DetectorBase {
   public:
    PointerSafetyDetector() = default;

    void analyze(Program& program) override;
    std::string getName() const override {
        return "Pointer Safety Detector";
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
    // Track pointer states
    std::map<std::string, PointerState> pointerStates_;

    // Track which pointers are checked before use
    std::set<std::string> checkedPointers_;

    // Current expression being analyzed
    std::string currentExpr_;

    void checkNullDereference(const std::string& ptrName, int line, int column);
    void checkUninitializedPointer(const std::string& ptrName, int line, int column);
    void checkPointerArithmetic(BinaryExpression& node);
    void updatePointerState(const std::string& ptrName, PointerState state);
    bool isPointerType(const std::string& type);
};

}  // namespace safec

#endif  // SAFEC_POINTER_SAFETY_DETECTOR_H
