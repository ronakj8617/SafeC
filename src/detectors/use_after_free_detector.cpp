#include "detectors/use_after_free_detector.h"

namespace safec {

void UseAfterFreeDetector::analyze(Program& program) {
    vulnerabilities_.clear();
    pointerStates_.clear();
    freedPointers_.clear();
    program.accept(*this);
}

void UseAfterFreeDetector::visit(Program& node) {
    for (auto& func : node.functions) {
        func->accept(*this);
    }
}

void UseAfterFreeDetector::visit(FunctionDeclaration& node) {
    if (node.body) {
        node.body->accept(*this);
    }
}

void UseAfterFreeDetector::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void UseAfterFreeDetector::visit(VariableDeclaration& node) {
    if (node.initializer) {
        node.initializer->accept(*this);

        // Check if this is a malloc/new allocation
        if (auto* call = dynamic_cast<FunctionCall*>(node.initializer.get())) {
            if (call->functionName == "malloc" || call->functionName == "calloc" ||
                call->functionName == "realloc") {
                pointerStates_[node.name] = PointerState::ALLOCATED;
            }
        }
    }
}

void UseAfterFreeDetector::visit(ExpressionStatement& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void UseAfterFreeDetector::visit(IfStatement& node) {
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.thenBranch) {
        node.thenBranch->accept(*this);
    }
    if (node.elseBranch) {
        node.elseBranch->accept(*this);
    }
}

void UseAfterFreeDetector::visit(WhileStatement& node) {
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void UseAfterFreeDetector::visit(ForStatement& node) {
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

void UseAfterFreeDetector::visit(ReturnStatement& node) {
    if (node.value) {
        node.value->accept(*this);
    }
}

void UseAfterFreeDetector::visit(FunctionCall& node) {
    checkAllocation(node);
    checkDeallocation(node);

    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void UseAfterFreeDetector::visit(BinaryExpression& node) {
    // Check for assignment to freed pointer
    if (node.op == "=") {
        if (auto* lhs = dynamic_cast<Identifier*>(node.left.get())) {
            if (auto* rhs = dynamic_cast<FunctionCall*>(node.right.get())) {
                if (rhs->functionName == "malloc" || rhs->functionName == "calloc" ||
                    rhs->functionName == "realloc") {
                    pointerStates_[lhs->name] = PointerState::ALLOCATED;
                }
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

void UseAfterFreeDetector::visit(UnaryExpression& node) {
    // Check for dereference of freed pointer
    if (node.op == "*") {
        if (auto* id = dynamic_cast<Identifier*>(node.operand.get())) {
            checkPointerUse(*id);
        }
    }

    if (node.operand) {
        node.operand->accept(*this);
    }
}

void UseAfterFreeDetector::visit(Identifier& node) {
    // Identifier usage is checked in context (unary, array access, etc.)
}

void UseAfterFreeDetector::visit(ArrayAccess& node) {
    // Check if accessing freed pointer
    if (auto* arrayId = dynamic_cast<Identifier*>(node.array.get())) {
        checkPointerUse(*arrayId);
    }

    if (node.array) {
        node.array->accept(*this);
    }
    if (node.index) {
        node.index->accept(*this);
    }
}

void UseAfterFreeDetector::visit(NumberLiteral& node) {
    // Nothing to check
}

void UseAfterFreeDetector::visit(StringLiteral& node) {
    // Nothing to check
}

void UseAfterFreeDetector::checkAllocation(FunctionCall& node) {
    if (node.functionName == "malloc" || node.functionName == "calloc" ||
        node.functionName == "realloc") {
        // Allocation tracked in assignment context
    }
}

void UseAfterFreeDetector::checkDeallocation(FunctionCall& node) {
    if (node.functionName == "free" || node.functionName == "delete") {
        if (!node.arguments.empty()) {
            if (auto* arg = dynamic_cast<Identifier*>(node.arguments[0].get())) {
                std::string ptrName = arg->name;

                // Check for double-free
                if (pointerStates_[ptrName] == PointerState::FREED) {
                    addVulnerability(
                        "Double-Free", "Pointer '" + ptrName + "' is being freed multiple times",
                        "Set pointer to NULL after freeing and check for NULL before freeing",
                        Severity::CRITICAL, node.line, node.column);
                } else {
                    pointerStates_[ptrName] = PointerState::FREED;
                    freedPointers_.insert(ptrName);
                }
            }
        }
    }
}

void UseAfterFreeDetector::checkPointerUse(Identifier& node) {
    if (pointerStates_[node.name] == PointerState::FREED) {
        addVulnerability("Use-After-Free",
                         "Pointer '" + node.name + "' is being used after it has been freed",
                         "Do not access memory after freeing it. Set pointer to NULL after free()",
                         Severity::CRITICAL, node.line, node.column);
    }
}

}  // namespace safec
