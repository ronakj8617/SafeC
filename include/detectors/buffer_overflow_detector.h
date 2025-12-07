#include <unordered_set>

#include "detector_base.h"

namespace safec {

class BufferOverflowDetector : public DetectorBase {
   public:
    BufferOverflowDetector() = default;

    void analyze(Program& program) override;
    std::string getName() const override {
        return "Buffer Overflow Detector";
    }

    // Load configuration from file
    static void loadConfig(const std::string& configPath);

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
    // Unsafe functions that can cause buffer overflows (loaded from config)
    static std::unordered_set<std::string> unsafeFunctions_;

    // Track array sizes
    std::map<std::string, int> arraySizes_;

    void checkUnsafeFunction(FunctionCall& node);
    void checkArrayBounds(ArrayAccess& node);
};

}  // namespace safec
