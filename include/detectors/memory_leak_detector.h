#include <map>
#include <set>

#include "detector_base.h"

namespace safec {

class MemoryLeakDetector : public DetectorBase {
   public:
    MemoryLeakDetector() = default;

    void analyze(Program& program) override;
    void analyzeWithSource(Program& program, const std::string& sourceCode);
    void setCurrentFile(const std::string& filename) { currentFile_ = filename; }
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
    // Track allocations file-wide (not function-scoped)
    struct AllocationInfo {
        std::string variableName;
        std::string allocationType;  // "new", "new[]", "malloc", "calloc", "realloc"
        int line;
        int column;
        bool freed;
    };
    
    std::map<std::string, AllocationInfo> allocations_;  // variable name -> allocation info
    std::set<std::string> freedVariables_;  // variables that have been freed
    std::string currentFile_;  // Current file being analyzed
    std::map<std::string, std::vector<std::string>> functionPointerParams_;  // function -> pointer params

    void trackAllocation(const std::string& ptrName, const std::string& allocType, int line, int column);
    void trackDeallocation(const std::string& ptrName);
    void checkForLeaks();
    void scanSourceForAllocations(const std::string& sourceCode);
    void checkPointerParameterLeaks();
};

}  // namespace safec
