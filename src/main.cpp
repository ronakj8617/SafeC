#include <iostream>
#include <memory>
#include <vector>

#include "detectors/buffer_overflow_detector.h"
#include "detectors/format_string_detector.h"
#include "detectors/integer_overflow_detector.h"
#include "detectors/memory_leak_detector.h"
#include "detectors/pointer_safety_detector.h"
#include "detectors/use_after_free_detector.h"
#include "directory_scanner.h"
#include "lexer.h"
#include "parser.h"
#include "report_generator.h"
#include "utils.h"

using namespace safec;

void printUsage(const char* programName) {
    std::cout << "SafeC - Static Analyzer for C/C++ Vulnerability Detection\n\n";
    std::cout << "Usage: " << programName << " [options] <source_file_or_directory>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "  -o, --output FILE   Write JSON report to FILE\n";
    std::cout << "  --html FILE         Write HTML report to FILE\n";
    std::cout << "  -v, --version       Show version information\n";
    std::cout << "  -d, --directory     Scan directory recursively for C/C++ files\n";
    std::cout << "  --include-tests     Include CWE test files (default: excluded)\n";
    std::cout << "  --force             Alias for --include-tests\n\n";
    std::cout << "Note: CWE test files (*_bad.*, testcases/, etc.) are excluded by default\n";
    std::cout << "      as they contain intentionally vulnerable code for testing purposes.\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " vulnerable_code.c\n";
    std::cout << "  " << programName << " -o report.json vulnerable_code.c\n";
    std::cout << "  " << programName << " --html report.html vulnerable_code.c\n";
    std::cout << "  " << programName << " -d /path/to/project\n";
    std::cout << "  " << programName << " -d --html report.html /path/to/project\n";
}

void printVersion() {
    std::cout << "SafeC version 1.0.0\n";
    std::cout << "A static analyzer for detecting vulnerabilities in C/C++ code\n";
}

std::vector<Vulnerability> analyzeFile(const std::string& inputFile,
                                       std::unique_ptr<Program>& program,
                                       const std::string& sourceCode) {
    std::vector<std::unique_ptr<DetectorBase>> detectors;
    detectors.push_back(std::make_unique<BufferOverflowDetector>());
    detectors.push_back(std::make_unique<UseAfterFreeDetector>());
    detectors.push_back(std::make_unique<MemoryLeakDetector>());
    detectors.push_back(std::make_unique<FormatStringDetector>());
    detectors.push_back(std::make_unique<IntegerOverflowDetector>());
    detectors.push_back(std::make_unique<PointerSafetyDetector>());

    std::vector<Vulnerability> vulnerabilities;

    for (auto& detector : detectors) {
        // Use source-code-aware analysis for memory leak detector
        if (auto* memLeakDetector = dynamic_cast<MemoryLeakDetector*>(detector.get())) {
            memLeakDetector->setCurrentFile(inputFile);
            memLeakDetector->analyzeWithSource(*program, sourceCode);
        } else if (auto* overflowDetector = dynamic_cast<IntegerOverflowDetector*>(detector.get())) {
            overflowDetector->setCurrentFile(inputFile);
            overflowDetector->analyze(*program);
        } else {
            detector->analyze(*program);
        }
        const auto& vulns = detector->getVulnerabilities();
        vulnerabilities.insert(vulnerabilities.end(), vulns.begin(), vulns.end());
    }

    return vulnerabilities;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputPath;
    std::string outputFile;
    std::string htmlFile;
    bool scanDirectory = false;
    bool includeTestFiles = false;

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
        } else if (arg == "--html") {
            if (i + 1 < argc) {
                htmlFile = argv[++i];
            } else {
                std::cerr << "Error: --html requires an output filename\n";
                return 1;
            }
        } else if (arg == "-d" || arg == "--directory") {
            scanDirectory = true;
        } else if (arg == "--include-tests" || arg == "--force") {
            includeTestFiles = true;
        } else if (arg[0] != '-') {
            inputPath = arg;
        }
    }

    if (inputPath.empty()) {
        std::cerr << "Error: No input file or directory specified\n";
        printUsage(argv[0]);
        return 1;
    }

    // Get list of files to analyze
    std::vector<std::string> filesToAnalyze;
    std::vector<std::string> excludedFiles;

    if (scanDirectory || Utils::isDirectory(inputPath)) {
        std::cout << "Scanning directory: " << inputPath << "\n";
        std::vector<std::string> allFiles = DirectoryScanner::findCppFiles(inputPath);
        
        // Filter out CWE test files (unless --include-tests is specified)
        for (const auto& file : allFiles) {
            if (!includeTestFiles && Utils::shouldExcludeTestFile(file)) {
                excludedFiles.push_back(file);
            } else {
                filesToAnalyze.push_back(file);
            }
        }
        
        std::cout << "Found " << allFiles.size() << " C/C++ file(s)";
        if (!excludedFiles.empty()) {
            std::cout << " (excluded " << excludedFiles.size() << " test file(s))";
        }
        std::cout << "\n\n";

        if (filesToAnalyze.empty()) {
            std::cerr << "No C/C++ files found in directory (after excluding test files)\n";
            return 1;
        }
    } else {
        // Single file - check if it should be excluded
        if (!Utils::fileExists(inputPath)) {
            std::cerr << "Error: File not found: " << inputPath << "\n";
            return 1;
        }
        if (!includeTestFiles && Utils::shouldExcludeTestFile(inputPath)) {
            std::cerr << "Warning: File appears to be a CWE test file and will be excluded.\n";
            std::cerr << "Use --include-tests or --force to analyze test files anyway.\n";
            return 1;
        }
        filesToAnalyze.push_back(inputPath);
    }

    // Analyze all files
    std::vector<Vulnerability> allVulnerabilities;
    int filesAnalyzed = 0;
    int filesFailed = 0;

    for (const auto& inputFile : filesToAnalyze) {
        std::cout << "Analyzing: " << inputFile << "\n";

        // Read source code
        std::string sourceCode = Utils::readFile(inputFile);
        if (sourceCode.empty()) {
            std::cerr << "  Warning: Could not read file or file is empty\n\n";
            filesFailed++;
            continue;
        }

        std::unique_ptr<Program> program;
        bool useFallback = false;

        // Check if this looks like a Juliet test case - use fallback parser directly
        // Juliet test cases have very specific patterns that our parser struggles with
        bool isJulietTest = (sourceCode.find("CWE") != std::string::npos) && 
                           (sourceCode.find("bad()") != std::string::npos || 
                            sourceCode.find("good()") != std::string::npos ||
                            sourceCode.find("::") != std::string::npos);

        if (isJulietTest) {
            // For Juliet test cases, try fallback parser first
            try {
                std::vector<Token> emptyTokens;
                Parser fallbackParser(emptyTokens);
                program = fallbackParser.parseJulietFallback(sourceCode);
                if (!program->functions.empty()) {
                    std::cout << "  Using fallback parser for Juliet-style test case\n";
                } else {
                    // Fallback didn't find functions, try regular parser
                    useFallback = false;
                }
            } catch (...) {
                useFallback = false; // Will try regular parser
            }
        }

        // Try regular parsing if fallback wasn't used or didn't find functions
        if (!isJulietTest || !program || program->functions.empty()) {
            try {
                // Lexical analysis
                Lexer lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                // Parsing
                Parser parser(tokens);
                auto regularProgram = parser.parse();

                // If regular parser found functions, use it; otherwise keep fallback result
                if (!regularProgram->functions.empty()) {
                    program = std::move(regularProgram);
                } else if (!program || program->functions.empty()) {
                    // Neither found functions, try fallback as last resort
                    if (!isJulietTest) {
                        std::vector<Token> emptyTokens;
                        Parser fallbackParser(emptyTokens);
                        program = fallbackParser.parseJulietFallback(sourceCode);
                        if (!program->functions.empty()) {
                            std::cout << "  Using fallback parser as last resort\n";
                        }
                    }
                }
            } catch (...) {
                // If regular parsing throws and we don't have a fallback result, try fallback
                if (!program || program->functions.empty()) {
                    try {
                        std::vector<Token> emptyTokens;
                        Parser fallbackParser(emptyTokens);
                        program = fallbackParser.parseJulietFallback(sourceCode);
                    } catch (...) {
                        // Both failed
                    }
                }
            }
        }

        // Use fallback parser if needed
        if (useFallback) {
            try {
                std::vector<Token> emptyTokens;
                Parser fallbackParser(emptyTokens);
                program = fallbackParser.parseJulietFallback(sourceCode);
                if (!program->functions.empty()) {
                    std::cout << "  Using fallback parser for Juliet-style methods\n";
                }
            } catch (...) {
                std::cerr << "  Warning: Both parsers failed\n";
                filesFailed++;
                continue;
            }
        }

        if (!program || program->functions.empty()) {
            std::cerr << "  Warning: No functions found\n\n";
            filesFailed++;
            continue;
        }

        // Run detectors
        auto vulnerabilities = analyzeFile(inputFile, program, sourceCode);

        // Add file path to each vulnerability
        for (auto& vuln : vulnerabilities) {
            vuln.file = inputFile;
        }

        allVulnerabilities.insert(allVulnerabilities.end(), vulnerabilities.begin(),
                                  vulnerabilities.end());
        filesAnalyzed++;

        std::cout << "  Found " << vulnerabilities.size() << " potential issue(s)\n\n";
    }

    // Generate reports
    std::cout
        << "\n================================================================================\n";
    std::cout << "SafeC Analysis Summary\n";
    std::cout
        << "================================================================================\n";
    std::cout << "Files analyzed: " << filesAnalyzed << "\n";
    std::cout << "Files with errors: " << filesFailed << "\n";
    std::cout << "Total vulnerabilities found: " << allVulnerabilities.size() << "\n";
    std::cout
        << "================================================================================\n\n";

    ReportGenerator::generateConsoleReport(allVulnerabilities, inputPath);

    if (!outputFile.empty()) {
        ReportGenerator::generateJSONReport(allVulnerabilities, inputPath, outputFile);
    }

    if (!htmlFile.empty()) {
        ReportGenerator::generateHTMLReport(allVulnerabilities, inputPath, htmlFile);
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
