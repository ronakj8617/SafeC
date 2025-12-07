#include "detectors/buffer_overflow_detector.h"

#include <iostream>

#include "config_loader.h"

namespace safec {

// Initialize with config file or defaults
std::unordered_set<std::string> BufferOverflowDetector::unsafeFunctions_ = {};

void BufferOverflowDetector::loadConfig(const std::string& configPath) {
    std::vector<UnsafeFunctionInfo> functions;
    if (ConfigLoader::loadUnsafeFunctions(configPath, functions)) {
        unsafeFunctions_ = ConfigLoader::getUnsafeFunctionNames(functions);
        std::cout << "Loaded " << unsafeFunctions_.size() << " unsafe functions from config\n";
    } else {
        // Fallback to defaults
        unsafeFunctions_ = {"strcpy", "strcat", "sprintf", "gets", "scanf", "vsprintf"};
        std::cout << "Using default unsafe functions list\n";
    }
}

void BufferOverflowDetector::analyze(Program& program) {
    // Load config if not already loaded
    if (unsafeFunctions_.empty()) {
        loadConfig("config/unsafe_functions.csv");
    }
    vulnerabilities_.clear();
    arraySizes_.clear();
    program.accept(*this);
}

void BufferOverflowDetector::visit(Program& node) {
    for (auto& func : node.functions) {
        func->accept(*this);
    }
}

void BufferOverflowDetector::visit(FunctionDeclaration& node) {
    if (node.body) {
        node.body->accept(*this);
    }
}

void BufferOverflowDetector::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void BufferOverflowDetector::visit(VariableDeclaration& node) {
    if (node.isArray) {
        arraySizes_[node.name] = node.arraySize;
    }
    if (node.initializer) {
        node.initializer->accept(*this);
    }
}

void BufferOverflowDetector::visit(ExpressionStatement& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void BufferOverflowDetector::visit(IfStatement& node) {
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

void BufferOverflowDetector::visit(WhileStatement& node) {
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }
}

void BufferOverflowDetector::visit(ForStatement& node) {
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

void BufferOverflowDetector::visit(ReturnStatement& node) {
    if (node.value) {
        node.value->accept(*this);
    }
}

void BufferOverflowDetector::visit(FunctionCall& node) {
    checkUnsafeFunction(node);

    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void BufferOverflowDetector::visit(ArrayAccess& node) {
    checkArrayBounds(node);

    if (node.array) {
        node.array->accept(*this);
    }
    if (node.index) {
        node.index->accept(*this);
    }
}

void BufferOverflowDetector::visit(BinaryExpression& node) {
    if (node.left) {
        node.left->accept(*this);
    }
    if (node.right) {
        node.right->accept(*this);
    }
}

void BufferOverflowDetector::visit(UnaryExpression& node) {
    if (node.operand) {
        node.operand->accept(*this);
    }
}

void BufferOverflowDetector::visit(Identifier& node) {
    // Nothing to check for plain identifiers
}

void BufferOverflowDetector::visit(NumberLiteral& node) {
    // Nothing to check for literals
}

void BufferOverflowDetector::visit(StringLiteral& node) {
    // Nothing to check for literals
}

void BufferOverflowDetector::checkUnsafeFunction(FunctionCall& node) {
    if (unsafeFunctions_.find(node.functionName) != unsafeFunctions_.end()) {
        std::string description = "Use of unsafe function '" + node.functionName +
                                  "' which does not perform bounds checking";
        std::string recommendation;

        if (node.functionName == "strcpy") {
            recommendation = "Use strncpy() instead and ensure proper null termination";
        } else if (node.functionName == "strcat") {
            recommendation = "Use strncat() instead and ensure proper null termination";
        } else if (node.functionName == "sprintf") {
            recommendation = "Use snprintf() instead to limit output size";
        } else if (node.functionName == "gets") {
            recommendation = "Use fgets() instead to limit input size";
        } else if (node.functionName == "scanf") {
            recommendation = "Use width specifiers (e.g., %10s) to limit input size";
        } else {
            recommendation = "Use a safer alternative with bounds checking";
        }

        addVulnerability("Buffer Overflow", description, recommendation, Severity::HIGH, node.line,
                         node.column);
    }
}

void BufferOverflowDetector::checkArrayBounds(ArrayAccess& node) {
    // Try to extract array name and index
    auto* arrayId = dynamic_cast<Identifier*>(node.array.get());
    auto* indexLit = dynamic_cast<NumberLiteral*>(node.index.get());

    if (arrayId && indexLit) {
        auto it = arraySizes_.find(arrayId->name);
        if (it != arraySizes_.end()) {
            int arraySize = it->second;
            int index = indexLit->value;

            if (index >= arraySize || index < 0) {
                std::string description =
                    "Array '" + arrayId->name + "' accessed with out-of-bounds index " +
                    std::to_string(index) + " (array size: " + std::to_string(arraySize) + ")";
                std::string recommendation = "Ensure array indices are within bounds [0, " +
                                             std::to_string(arraySize - 1) + "]";

                addVulnerability("Buffer Overflow", description, recommendation, Severity::CRITICAL,
                                 node.line, node.column);
            }
        }
    }
}

}  // namespace safec
