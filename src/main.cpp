#include <iostream>
#include <memory>
#include <vector>

#include "detectors/buffer_overflow_detector.h"
#include "detectors/format_string_detector.h"
#include "detectors/integer_overflow_detector.h"
#include "detectors/memory_leak_detector.h"
#include "detectors/use_after_free_detector.h"
#include "lexer.h"
#include "parser.h"
#include "report_generator.h"
#include "utils.h"

using namespace safec;

void printUsage(const char* programName) {
    std::cout << "SafeC - Static Analyzer for C/C++ Vulnerability Detection\n\n";
    std::cout << "Usage: " << programName << " [options] <source_file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "  -o, --output FILE   Write JSON report to FILE\n";
    std::cout << "  -v, --version       Show version information\n\n";
    std::cout << "Example:\n";
    std::cout << "  " << programName << " vulnerable_code.c\n";
    std::cout << "  " << programName << " -o report.json vulnerable_code.c\n";
}

void printVersion() {
    std::cout << "SafeC version 1.0.0\n";
    std::cout << "A static analyzer for detecting vulnerabilities in C/C++ code\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputFile;
    std::string outputFile;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o requires an output filename\n";
                return 1;
            }
        } else if (arg[0] != '-') {
            inputFile = arg;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }

    // Check if file exists
    if (!Utils::fileExists(inputFile)) {
        std::cerr << "Error: File not found: " << inputFile << "\n";
        return 1;
    }

    // Read source code
    std::string sourceCode = Utils::readFile(inputFile);
    if (sourceCode.empty()) {
        std::cerr << "Error: Could not read file or file is empty\n";
        return 1;
    }

    std::cout << "Analyzing: " << inputFile << "\n";

    // Lexical analysis
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "Lexical analysis complete. Found " << tokens.size() << " tokens.\n";

    // Parsing
    Parser parser(tokens);
    auto program = parser.parse();

    if (parser.hasErrors()) {
        std::cerr << "\nParsing errors:\n";
        for (const auto& error : parser.getErrors()) {
            std::cerr << "  " << error << "\n";
        }
        std::cerr << "\nNote: Analysis will continue with partial AST\n\n";
    } else {
        std::cout << "Parsing complete.\n";
    }

    // Run vulnerability detectors
    std::cout << "Running vulnerability detectors...\n\n";

    std::vector<std::unique_ptr<DetectorBase>> detectors;
    detectors.push_back(std::make_unique<BufferOverflowDetector>());
    detectors.push_back(std::make_unique<UseAfterFreeDetector>());
    detectors.push_back(std::make_unique<MemoryLeakDetector>());
    detectors.push_back(std::make_unique<FormatStringDetector>());
    detectors.push_back(std::make_unique<IntegerOverflowDetector>());

    std::vector<Vulnerability> allVulnerabilities;

    for (auto& detector : detectors) {
        std::cout << "  Running " << detector->getName() << "...\n";
        detector->analyze(*program);
        const auto& vulns = detector->getVulnerabilities();
        allVulnerabilities.insert(allVulnerabilities.end(), vulns.begin(), vulns.end());
    }

    // Generate reports
    ReportGenerator::generateConsoleReport(allVulnerabilities, inputFile);

    if (!outputFile.empty()) {
        ReportGenerator::generateJSONReport(allVulnerabilities, inputFile, outputFile);
    }

    // Return exit code based on findings
    if (allVulnerabilities.empty()) {
        return 0;
    } else {
        // Check for critical vulnerabilities
        for (const auto& vuln : allVulnerabilities) {
            if (vuln.severity == Severity::CRITICAL) {
                return 2;
            }
        }
        return 1;
    }
}
