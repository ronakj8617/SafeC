#include "detectors/pointer_safety_detector.h"

#include <iostream>

namespace safec {

void PointerSafetyDetector::analyze(Program& program) {
    vulnerabilities_.clear();
    pointerStates_.clear();
    checkedPointers_.clear();
    program.accept(*this);
}

void PointerSafetyDetector::visit(Program& node) {
    for (auto& func : node.functions) {
        func->accept(*this);
    }
}

void PointerSafetyDetector::visit(FunctionDeclaration& node) {
    // Clear state for new function
    pointerStates_.clear();
    checkedPointers_.clear();

    if (node.body) {
        node.body->accept(*this);
    }
}

void PointerSafetyDetector::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void PointerSafetyDetector::visit(VariableDeclaration& node) {
    // Check if this is a pointer declaration
    if (isPointerType(node.type)) {
        if (node.initializer) {
            // Check if initialized to NULL
            auto* numLit = dynamic_cast<NumberLiteral*>(node.initializer.get());
            if (numLit && numLit->value == 0) {
                pointerStates_[node.name] = PointerState::NULL_PTR;
            } else {
                pointerStates_[node.name] = PointerState::VALID;
            }
            node.initializer->accept(*this);
        } else {
            // Uninitialized pointer
            pointerStates_[node.name] = PointerState::UNINITIALIZED;

            addVulnerability("Uninitialized Pointer",
                             "Pointer '" + node.name + "' declared but not initialized",
                             "Initialize pointer to NULL or valid address: " + node.type + " " +
                                 node.name + " = NULL;",
                             Severity::HIGH, node.line, node.column);
        }
    }
}

void PointerSafetyDetector::visit(ExpressionStatement& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void PointerSafetyDetector::visit(IfStatement& node) {
    // Check if condition is a null check
    if (node.condition) {
        auto* binExpr = dynamic_cast<BinaryExpression*>(node.condition.get());
        if (binExpr && (binExpr->op == "==" || binExpr->op == "!=")) {
            auto* id = dynamic_cast<Identifier*>(binExpr->left.get());
            auto* numLit = dynamic_cast<NumberLiteral*>(binExpr->right.get());

            if (id && numLit && numLit->value == 0) {
                // This is a null check
                checkedPointers_.insert(id->name);
            }
        }
        node.condition->accept(*this);
    }

    if (node.thenBranch) {
        node.thenBranch->accept(*this);
    }
    if (node.elseBranch) {
        node.elseBranch->accept(*this);
    }
}

void PointerSafetyDetector::visit(WhileStatement& node) {
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void PointerSafetyDetector::visit(ForStatement& node) {
    if (node.initializer) {
        node.initializer->accept(*this);
    }
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.increment) {
        node.increment->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void PointerSafetyDetector::visit(ReturnStatement& node) {
    if (node.value) {
        node.value->accept(*this);
    }
}

void PointerSafetyDetector::visit(FunctionCall& node) {
    // Check for malloc/calloc/realloc
    if (node.functionName == "malloc" || node.functionName == "calloc" ||
        node.functionName == "realloc") {
        // This returns a pointer that should be checked
        currentExpr_ = "allocation";
    }

    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void PointerSafetyDetector::visit(ArrayAccess& node) {
    // Check if array is a pointer and if it's been checked
    if (node.array) {
        auto* id = dynamic_cast<Identifier*>(node.array.get());
        if (id) {
            checkNullDereference(id->name, node.line, node.column);
            checkUninitializedPointer(id->name, node.line, node.column);
        }
        node.array->accept(*this);
    }
    if (node.index) {
        node.index->accept(*this);
    }
}

void PointerSafetyDetector::visit(BinaryExpression& node) {
    // Check for pointer arithmetic
    if (node.op == "+" || node.op == "-") {
        checkPointerArithmetic(node);
    }

    // Check for assignment to pointer
    if (node.op == "=") {
        auto* leftId = dynamic_cast<Identifier*>(node.left.get());
        if (leftId && pointerStates_.count(leftId->name)) {
            // Check what's being assigned
            auto* rightNum = dynamic_cast<NumberLiteral*>(node.right.get());
            if (rightNum && rightNum->value == 0) {
                updatePointerState(leftId->name, PointerState::NULL_PTR);
            } else {
                updatePointerState(leftId->name, PointerState::VALID);
            }
        }
    }

    if (node.left) {
        node.left->accept(*this);
    }
    if (node.right) {
        node.right->accept(*this);
    }
}

void PointerSafetyDetector::visit(UnaryExpression& node) {
    // Check for dereference operator
    if (node.op == "*") {
        if (node.operand) {
            auto* id = dynamic_cast<Identifier*>(node.operand.get());
            if (id) {
                checkNullDereference(id->name, 0, 0);
                checkUninitializedPointer(id->name, 0, 0);
            }
        }
    }

    if (node.operand) {
        node.operand->accept(*this);
    }
}

void PointerSafetyDetector::visit(Identifier& node) {
    // Check if this identifier is a pointer being used
    if (pointerStates_.count(node.name)) {
        checkNullDereference(node.name, node.line, node.column);
    }
}

void PointerSafetyDetector::visit(NumberLiteral&) {
    // Nothing to check
}

void PointerSafetyDetector::visit(StringLiteral&) {
    // Nothing to check
}

void PointerSafetyDetector::checkNullDereference(const std::string& ptrName, int line, int column) {
    auto it = pointerStates_.find(ptrName);
    if (it != pointerStates_.end() && it->second == PointerState::NULL_PTR) {
        // Check if this pointer has been null-checked
        if (checkedPointers_.find(ptrName) == checkedPointers_.end()) {
            addVulnerability("Null Pointer Dereference",
                             "Pointer '" + ptrName + "' may be NULL and is used without null check",
                             "Add null check: if (" + ptrName + " != NULL) { ... }",
                             Severity::CRITICAL, line, column);
        }
    }
}

void PointerSafetyDetector::checkUninitializedPointer(const std::string& ptrName, int line,
                                                      int column) {
    auto it = pointerStates_.find(ptrName);
    if (it != pointerStates_.end() && it->second == PointerState::UNINITIALIZED) {
        addVulnerability("Uninitialized Pointer Use",
                         "Pointer '" + ptrName + "' used before initialization",
                         "Initialize pointer before use", Severity::CRITICAL, line, column);
    }
}

void PointerSafetyDetector::checkPointerArithmetic(BinaryExpression& node) {
    auto* leftId = dynamic_cast<Identifier*>(node.left.get());
    auto* rightId = dynamic_cast<Identifier*>(node.right.get());

    bool leftIsPointer = leftId && pointerStates_.count(leftId->name);
    bool rightIsPointer = rightId && pointerStates_.count(rightId->name);

    if (leftIsPointer || rightIsPointer) {
        addVulnerability(
            "Pointer Arithmetic", "Pointer arithmetic detected - may lead to out-of-bounds access",
            "Ensure pointer arithmetic stays within allocated bounds", Severity::MEDIUM, 0, 0);
    }
}

void PointerSafetyDetector::updatePointerState(const std::string& ptrName, PointerState state) {
    pointerStates_[ptrName] = state;
}

bool PointerSafetyDetector::isPointerType(const std::string& type) {
    // Simple check - contains '*' or ends with pointer-like keywords
    return type.find('*') != std::string::npos || type.find("ptr") != std::string::npos ||
           type.find("Ptr") != std::string::npos;
}

}  // namespace safec
