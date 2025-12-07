#include "detectors/format_string_detector.h"

namespace safec {

const std::unordered_set<std::string> FormatStringDetector::formatFunctions_ = {
    "printf", "fprintf", "sprintf", "snprintf", "vprintf", "vfprintf", "vsprintf", "vsnprintf"};

void FormatStringDetector::analyze(Program& program) {
    vulnerabilities_.clear();
    program.accept(*this);
}

void FormatStringDetector::visit(Program& node) {
    for (auto& func : node.functions) {
        func->accept(*this);
    }
}

void FormatStringDetector::visit(FunctionDeclaration& node) {
    if (node.body) {
        node.body->accept(*this);
    }
}

void FormatStringDetector::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void FormatStringDetector::visit(VariableDeclaration& node) {
    if (node.initializer) {
        node.initializer->accept(*this);
    }
}

void FormatStringDetector::visit(ExpressionStatement& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void FormatStringDetector::visit(IfStatement& node) {
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

void FormatStringDetector::visit(WhileStatement& node) {
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void FormatStringDetector::visit(ForStatement& node) {
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

void FormatStringDetector::visit(ReturnStatement& node) {
    if (node.value) {
        node.value->accept(*this);
    }
}

void FormatStringDetector::visit(FunctionCall& node) {
    checkFormatString(node);

    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void FormatStringDetector::visit(BinaryExpression& node) {
    if (node.left) {
        node.left->accept(*this);
    }
    if (node.right) {
        node.right->accept(*this);
    }
}

void FormatStringDetector::visit(UnaryExpression& node) {
    if (node.operand) {
        node.operand->accept(*this);
    }
}

void FormatStringDetector::visit(Identifier& node) {
    // Nothing to check
}

void FormatStringDetector::visit(ArrayAccess& node) {
    if (node.array) {
        node.array->accept(*this);
    }
    if (node.index) {
        node.index->accept(*this);
    }
}

void FormatStringDetector::visit(NumberLiteral& node) {
    // Nothing to check
}

void FormatStringDetector::visit(StringLiteral& node) {
    // Nothing to check
}

void FormatStringDetector::checkFormatString(FunctionCall& node) {
    if (formatFunctions_.find(node.functionName) == formatFunctions_.end()) {
        return;
    }

    // Check if format string is user-controlled (identifier instead of literal)
    if (!node.arguments.empty()) {
        auto* formatArg = node.arguments[0].get();

        // If format string is an identifier (variable), it could be user-controlled
        if (dynamic_cast<Identifier*>(formatArg)) {
            addVulnerability(
                "Format String Vulnerability",
                "Function '" + node.functionName + "' uses a variable as format string, " +
                    "which could be controlled by an attacker",
                "Use a string literal as format string, e.g., " + node.functionName +
                    "(\"%s\", user_input) instead of " + node.functionName + "(user_input)",
                Severity::HIGH, node.line, node.column);
        }

        // Check for %n specifier which can write to memory
        if (auto* formatLit = dynamic_cast<StringLiteral*>(formatArg)) {
            if (formatLit->value.find("%n") != std::string::npos) {
                addVulnerability(
                    "Format String Vulnerability",
                    "Format string contains %n specifier which can write to arbitrary memory",
                    "Avoid using %n in format strings unless absolutely necessary and validated",
                    Severity::CRITICAL, node.line, node.column);
            }

            // Check argument count mismatch
            int specifierCount = countFormatSpecifiers(formatLit->value);
            int argCount =
                static_cast<int>(node.arguments.size()) - 1;  // -1 for format string itself

            if (specifierCount != argCount && specifierCount > 0) {
                addVulnerability(
                    "Format String Mismatch",
                    "Format string expects " + std::to_string(specifierCount) + " arguments but " +
                        std::to_string(argCount) + " provided",
                    "Ensure the number of format specifiers matches the number of arguments",
                    Severity::MEDIUM, node.line, node.column);
            }
        }
    }
}

int FormatStringDetector::countFormatSpecifiers(const std::string& format) {
    int count = 0;
    for (size_t i = 0; i < format.length(); i++) {
        if (format[i] == '%') {
            if (i + 1 < format.length() && format[i + 1] != '%') {
                count++;
            } else if (i + 1 < format.length() && format[i + 1] == '%') {
                i++;  // Skip escaped %
            }
        }
    }
    return count;
}

}  // namespace safec
