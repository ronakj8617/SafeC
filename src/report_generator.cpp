#include "report_generator.h"

#include <fstream>
#include <iomanip>
#include <iostream>

namespace safec {

void ReportGenerator::generateConsoleReport(const std::vector<Vulnerability>& vulnerabilities,
                                            const std::string& filename) {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SafeC Static Analysis Report\n";
    std::cout << "File: " << filename << "\n";
    std::cout << std::string(80, '=') << "\n\n";

    if (vulnerabilities.empty()) {
        std::cout << "✓ No vulnerabilities detected!\n\n";
        return;
    }

    std::cout << "Found " << vulnerabilities.size() << " potential vulnerabilities:\n\n";

    int count = 1;
    for (const auto& vuln : vulnerabilities) {
        std::cout << "[" << count++ << "] " << vuln.type << " - "
                  << DetectorBase::severityToString(vuln.severity) << "\n";
        std::cout << "    Location: Line " << vuln.line << ", Column " << vuln.column << "\n";
        std::cout << "    Description: " << vuln.description << "\n";
        std::cout << "    Recommendation: " << vuln.recommendation << "\n";
        std::cout << "\n";
    }

    // Summary by severity
    int critical = 0, high = 0, medium = 0, low = 0;
    for (const auto& vuln : vulnerabilities) {
        switch (vuln.severity) {
            case Severity::CRITICAL:
                critical++;
                break;
            case Severity::HIGH:
                high++;
                break;
            case Severity::MEDIUM:
                medium++;
                break;
            case Severity::LOW:
                low++;
                break;
        }
    }

    std::cout << std::string(80, '-') << "\n";
    std::cout << "Summary:\n";
    if (critical > 0)
        std::cout << "  CRITICAL: " << critical << "\n";
    if (high > 0)
        std::cout << "  HIGH:     " << high << "\n";
    if (medium > 0)
        std::cout << "  MEDIUM:   " << medium << "\n";
    if (low > 0)
        std::cout << "  LOW:      " << low << "\n";
    std::cout << std::string(80, '=') << "\n\n";
}

void ReportGenerator::generateJSONReport(const std::vector<Vulnerability>& vulnerabilities,
                                         const std::string& filename,
                                         const std::string& outputFile) {
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Error: Could not create output file: " << outputFile << "\n";
        return;
    }

    out << "{\n";
    out << "  \"file\": \"" << filename << "\",\n";
    out << "  \"vulnerabilities\": [\n";

    for (size_t i = 0; i < vulnerabilities.size(); i++) {
        const auto& vuln = vulnerabilities[i];
        out << "    {\n";
        out << "      \"type\": \"" << vuln.type << "\",\n";
        out << "      \"severity\": \"" << DetectorBase::severityToString(vuln.severity) << "\",\n";
        out << "      \"line\": " << vuln.line << ",\n";
        out << "      \"column\": " << vuln.column << ",\n";
        out << "      \"description\": \"" << vuln.description << "\",\n";
        out << "      \"recommendation\": \"" << vuln.recommendation << "\"\n";
        out << "    }";
        if (i < vulnerabilities.size() - 1) {
            out << ",";
        }
        out << "\n";
    }

    out << "  ]\n";
    out << "}\n";

    std::cout << "JSON report written to: " << outputFile << "\n";
}

}  // namespace safec
