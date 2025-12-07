#include "detectors/integer_overflow_detector.h"

#include <limits>

namespace safec {

void IntegerOverflowDetector::analyze(Program& program) {
    vulnerabilities_.clear();
    program.accept(*this);
}

void IntegerOverflowDetector::visit(Program& node) {
    for (auto& func : node.functions) {
        func->accept(*this);
    }
}

void IntegerOverflowDetector::visit(FunctionDeclaration& node) {
    if (node.body) {
        node.body->accept(*this);
    }
}

void IntegerOverflowDetector::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void IntegerOverflowDetector::visit(VariableDeclaration& node) {
    if (node.initializer) {
        node.initializer->accept(*this);
    }
}

void IntegerOverflowDetector::visit(ExpressionStatement& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void IntegerOverflowDetector::visit(IfStatement& node) {
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

void IntegerOverflowDetector::visit(WhileStatement& node) {
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void IntegerOverflowDetector::visit(ForStatement& node) {
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

void IntegerOverflowDetector::visit(ReturnStatement& node) {
    if (node.value) {
        node.value->accept(*this);
    }
}

void IntegerOverflowDetector::visit(FunctionCall& node) {
    // Check for malloc with arithmetic that could overflow
    if (node.functionName == "malloc" || node.functionName == "calloc") {
        if (!node.arguments.empty()) {
            if (auto* binExpr = dynamic_cast<BinaryExpression*>(node.arguments[0].get())) {
                if (isArithmeticOperator(binExpr->op)) {
                    addVulnerability(
                        "Integer Overflow in Memory Allocation",
                        "Arithmetic operation in malloc/calloc size calculation could overflow",
                        "Validate arithmetic operations before using in memory allocation. " +
                            std::string("Check for overflow before multiplication/addition"),
                        Severity::HIGH, node.line, node.column);
                }
            }
        }
    }

    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void IntegerOverflowDetector::visit(BinaryExpression& node) {
    checkArithmeticOperation(node);

    if (node.left) {
        node.left->accept(*this);
    }
    if (node.right) {
        node.right->accept(*this);
    }
}

void IntegerOverflowDetector::visit(UnaryExpression& node) {
    if (node.operand) {
        node.operand->accept(*this);
    }
}

void IntegerOverflowDetector::visit(Identifier& node) {
    // Nothing to check
}

void IntegerOverflowDetector::visit(ArrayAccess& node) {
    checkArrayIndexCalculation(node);

    if (node.array) {
        node.array->accept(*this);
    }
    if (node.index) {
        node.index->accept(*this);
    }
}

void IntegerOverflowDetector::visit(NumberLiteral& node) {
    // Nothing to check
}

void IntegerOverflowDetector::visit(StringLiteral& node) {
    // Nothing to check
}

void IntegerOverflowDetector::checkArithmeticOperation(BinaryExpression& node) {
    if (!isArithmeticOperator(node.op)) {
        return;
    }

    // Check for potential overflow with large literal values
    auto* leftLit = dynamic_cast<NumberLiteral*>(node.left.get());
    auto* rightLit = dynamic_cast<NumberLiteral*>(node.right.get());

    if (leftLit && rightLit) {
        long long result = 0;
        bool overflow = false;

        if (node.op == "+") {
            result = static_cast<long long>(leftLit->value) + rightLit->value;
            overflow = (result > std::numeric_limits<int>::max() ||
                        result < std::numeric_limits<int>::min());
        } else if (node.op == "*") {
            result = static_cast<long long>(leftLit->value) * rightLit->value;
            overflow = (result > std::numeric_limits<int>::max() ||
                        result < std::numeric_limits<int>::min());
        } else if (node.op == "-") {
            result = static_cast<long long>(leftLit->value) - rightLit->value;
            overflow = (result > std::numeric_limits<int>::max() ||
                        result < std::numeric_limits<int>::min());
        }

        if (overflow) {
            addVulnerability("Integer Overflow",
                             "Arithmetic operation " + std::to_string(leftLit->value) + " " +
                                 node.op + " " + std::to_string(rightLit->value) +
                                 " results in overflow (result: " + std::to_string(result) + ")",
                             "Use larger integer types (long, long long) or add overflow checks",
                             Severity::MEDIUM, node.line, node.column);
        }
    } else if (node.op == "*" || node.op == "+") {
        // Warn about unchecked arithmetic with variables
        addVulnerability("Potential Integer Overflow",
                         "Unchecked arithmetic operation '" + node.op + "' could overflow",
                         "Add overflow checks before arithmetic operations, especially in loops or "
                         "with user input",
                         Severity::LOW, node.line, node.column);
    }
}

void IntegerOverflowDetector::checkArrayIndexCalculation(ArrayAccess& node) {
    // Check if index is calculated with arithmetic that could overflow
    if (auto* binExpr = dynamic_cast<BinaryExpression*>(node.index.get())) {
        if (isArithmeticOperator(binExpr->op)) {
            addVulnerability(
                "Integer Overflow in Array Index",
                "Array index calculation uses arithmetic that could overflow",
                "Validate index calculations and ensure they remain within valid range",
                Severity::MEDIUM, node.line, node.column);
        }
    }
}

bool IntegerOverflowDetector::isArithmeticOperator(const std::string& op) {
    return op == "+" || op == "-" || op == "*" || op == "/";
}

}  // namespace safec
