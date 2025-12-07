#include "detectors/memory_leak_detector.h"

namespace safec {

void MemoryLeakDetector::analyze(Program& program) {
    vulnerabilities_.clear();
    allocatedPointers_.clear();
    freedPointers_.clear();
    program.accept(*this);
    checkForLeaks();
}

void MemoryLeakDetector::visit(Program& node) {
    for (auto& func : node.functions) {
        func->accept(*this);
    }
}

void MemoryLeakDetector::visit(FunctionDeclaration& node) {
    currentFunction_ = node.name;
    allocatedPointers_.clear();
    freedPointers_.clear();

    if (node.body) {
        node.body->accept(*this);
    }

    checkForLeaks();
}

void MemoryLeakDetector::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void MemoryLeakDetector::visit(VariableDeclaration& node) {
    if (node.initializer) {
        if (auto* call = dynamic_cast<FunctionCall*>(node.initializer.get())) {
            if (call->functionName == "malloc" || call->functionName == "calloc" ||
                call->functionName == "realloc") {
                trackAllocation(node.name, node.line, node.column);
            }
        }
        node.initializer->accept(*this);
    }
}

void MemoryLeakDetector::visit(ExpressionStatement& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void MemoryLeakDetector::visit(IfStatement& node) {
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

void MemoryLeakDetector::visit(WhileStatement& node) {
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void MemoryLeakDetector::visit(ForStatement& node) {
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

void MemoryLeakDetector::visit(ReturnStatement& node) {
    if (node.value) {
        node.value->accept(*this);
    }
}

void MemoryLeakDetector::visit(FunctionCall& node) {
    if (node.functionName == "free" || node.functionName == "delete") {
        if (!node.arguments.empty()) {
            if (auto* arg = dynamic_cast<Identifier*>(node.arguments[0].get())) {
                trackDeallocation(arg->name);
            }
        }
    }

    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void MemoryLeakDetector::visit(BinaryExpression& node) {
    // Track allocations in assignments
    if (node.op == "=") {
        if (auto* lhs = dynamic_cast<Identifier*>(node.left.get())) {
            if (auto* rhs = dynamic_cast<FunctionCall*>(node.right.get())) {
                if (rhs->functionName == "malloc" || rhs->functionName == "calloc" ||
                    rhs->functionName == "realloc") {
                    trackAllocation(lhs->name, node.line, node.column);
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

void MemoryLeakDetector::visit(UnaryExpression& node) {
    if (node.operand) {
        node.operand->accept(*this);
    }
}

void MemoryLeakDetector::visit(Identifier& node) {
    // Nothing to check
}

void MemoryLeakDetector::visit(ArrayAccess& node) {
    if (node.array) {
        node.array->accept(*this);
    }
    if (node.index) {
        node.index->accept(*this);
    }
}

void MemoryLeakDetector::visit(NumberLiteral& node) {
    // Nothing to check
}

void MemoryLeakDetector::visit(StringLiteral& node) {
    // Nothing to check
}

void MemoryLeakDetector::trackAllocation(const std::string& ptrName, int line, int column) {
    allocatedPointers_.insert(ptrName);
}

void MemoryLeakDetector::trackDeallocation(const std::string& ptrName) {
    freedPointers_.insert(ptrName);
}

void MemoryLeakDetector::checkForLeaks() {
    for (const auto& ptr : allocatedPointers_) {
        if (freedPointers_.find(ptr) == freedPointers_.end()) {
            addVulnerability(
                "Memory Leak",
                "Memory allocated to pointer '" + ptr + "' in function '" + currentFunction_ +
                    "' is never freed",
                "Call free() or delete to release allocated memory before function returns",
                Severity::MEDIUM,
                0,  // Line info not available in this context
                0);
        }
    }
}

}  // namespace safec
