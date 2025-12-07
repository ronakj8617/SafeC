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

void ReportGenerator::generateHTMLReport(const std::vector<Vulnerability>& vulnerabilities,
                                         const std::string& filename,
                                         const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create HTML report file: " << outputFile << "\n";
        return;
    }

    // Count by severity
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

    // HTML Header with modern styling
    file << "<!DOCTYPE html>\n";
    file << "<html lang=\"en\">\n";
    file << "<head>\n";
    file << "    <meta charset=\"UTF-8\">\n";
    file << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    file << "    <title>SafeC Security Analysis Report</title>\n";
    file << "    <style>\n";
    file << "        * { margin: 0; padding: 0; box-sizing: border-box; }\n";
    file << "        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; "
            "background: #f5f7fa; color: #333; line-height: 1.6; }\n";
    file << "        .container { max-width: 1200px; margin: 0 auto; padding: 20px; }\n";
    file << "        .header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); "
            "color: white; padding: 40px 20px; border-radius: 10px; margin-bottom: 30px; "
            "box-shadow: 0 4px 6px rgba(0,0,0,0.1); }\n";
    file << "        .header h1 { font-size: 2.5em; margin-bottom: 10px; }\n";
    file << "        .header p { font-size: 1.1em; opacity: 0.9; }\n";
    file << "        .summary { display: grid; grid-template-columns: repeat(auto-fit, "
            "minmax(200px, 1fr)); gap: 20px; margin-bottom: 30px; }\n";
    file << "        .summary-card { background: white; padding: 25px; border-radius: 10px; "
            "box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-left: 4px solid; }\n";
    file << "        .summary-card.critical { border-color: #e74c3c; }\n";
    file << "        .summary-card.high { border-color: #e67e22; }\n";
    file << "        .summary-card.medium { border-color: #f39c12; }\n";
    file << "        .summary-card.low { border-color: #3498db; }\n";
    file << "        .summary-card h3 { color: #7f8c8d; font-size: 0.9em; text-transform: "
            "uppercase; margin-bottom: 10px; }\n";
    file << "        .summary-card .count { font-size: 2.5em; font-weight: bold; }\n";
    file << "        .vulnerability { background: white; padding: 25px; margin-bottom: 20px; "
            "border-radius: 10px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); border-left: 5px solid; "
            "}\n";
    file << "        .vulnerability.critical { border-color: #e74c3c; }\n";
    file << "        .vulnerability.high { border-color: #e67e22; }\n";
    file << "        .vulnerability.medium { border-color: #f39c12; }\n";
    file << "        .vulnerability.low { border-color: #3498db; }\n";
    file << "        .vuln-header { display: flex; justify-content: space-between; align-items: "
            "center; margin-bottom: 15px; }\n";
    file << "        .vuln-title { font-size: 1.3em; font-weight: bold; color: #2c3e50; }\n";
    file << "        .badge { padding: 5px 15px; border-radius: 20px; font-size: 0.85em; "
            "font-weight: bold; color: white; }\n";
    file << "        .badge.critical { background: #e74c3c; }\n";
    file << "        .badge.high { background: #e67e22; }\n";
    file << "        .badge.medium { background: #f39c12; }\n";
    file << "        .badge.low { background: #3498db; }\n";
    file << "        .location { color: #7f8c8d; margin-bottom: 15px; font-family: 'Courier New', "
            "monospace; }\n";
    file << "        .description { margin-bottom: 15px; color: #555; }\n";
    file << "        .recommendation { background: #ecf0f1; padding: 15px; border-radius: 5px; "
            "border-left: 3px solid #3498db; }\n";
    file << "        .recommendation strong { color: #2980b9; }\n";
    file << "        .no-vulns { background: white; padding: 40px; text-align: center; "
            "border-radius: 10px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n";
    file << "        .no-vulns h2 { color: #27ae60; font-size: 2em; margin-bottom: 10px; }\n";
    file << "        .footer { text-align: center; margin-top: 40px; padding: 20px; color: "
            "#7f8c8d; }\n";
    file << "    </style>\n";
    file << "</head>\n";
    file << "<body>\n";
    file << "    <div class=\"container\">\n";

    // Header
    file << "        <div class=\"header\">\n";
    file << "            <h1>🛡️ SafeC Security Analysis Report</h1>\n";
    file << "            <p>Analysis Target: <strong>" << filename << "</strong></p>\n";
    file << "        </div>\n";

    // Summary Cards
    file << "        <div class=\"summary\">\n";
    file << "            <div class=\"summary-card critical\">\n";
    file << "                <h3>Critical</h3>\n";
    file << "                <div class=\"count\">" << critical << "</div>\n";
    file << "            </div>\n";
    file << "            <div class=\"summary-card high\">\n";
    file << "                <h3>High</h3>\n";
    file << "                <div class=\"count\">" << high << "</div>\n";
    file << "            </div>\n";
    file << "            <div class=\"summary-card medium\">\n";
    file << "                <h3>Medium</h3>\n";
    file << "                <div class=\"count\">" << medium << "</div>\n";
    file << "            </div>\n";
    file << "            <div class=\"summary-card low\">\n";
    file << "                <h3>Low</h3>\n";
    file << "                <div class=\"count\">" << low << "</div>\n";
    file << "            </div>\n";
    file << "        </div>\n";

    // Vulnerabilities
    if (vulnerabilities.empty()) {
        file << "        <div class=\"no-vulns\">\n";
        file << "            <h2>✓ No Vulnerabilities Detected!</h2>\n";
        file << "            <p>Your code passed all security checks.</p>\n";
        file << "        </div>\n";
    } else {
        for (size_t i = 0; i < vulnerabilities.size(); i++) {
            const auto& vuln = vulnerabilities[i];
            std::string severityClass = DetectorBase::severityToString(vuln.severity);
            std::transform(severityClass.begin(), severityClass.end(), severityClass.begin(),
                           ::tolower);

            file << "        <div class=\"vulnerability " << severityClass << "\">\n";
            file << "            <div class=\"vuln-header\">\n";
            file << "                <div class=\"vuln-title\">[" << (i + 1) << "] " << vuln.type
                 << "</div>\n";
            file << "                <span class=\"badge " << severityClass << "\">"
                 << DetectorBase::severityToString(vuln.severity) << "</span>\n";
            file << "            </div>\n";

            if (!vuln.file.empty()) {
                file << "            <div class=\"location\">📁 " << vuln.file;
            } else {
                file << "            <div class=\"location\">📍 ";
            }
            file << " | Line " << vuln.line << ", Column " << vuln.column << "</div>\n";

            file << "            <div class=\"description\">" << vuln.description << "</div>\n";
            file << "            <div class=\"recommendation\"><strong>💡 Recommendation:</strong> "
                 << vuln.recommendation << "</div>\n";
            file << "        </div>\n";
        }
    }

    // Footer
    file << "        <div class=\"footer\">\n";
    file << "            <p>Generated by SafeC Static Analyzer v1.0.0</p>\n";
    file << "            <p>Total Vulnerabilities Found: <strong>" << vulnerabilities.size()
         << "</strong></p>\n";
    file << "        </div>\n";
    file << "    </div>\n";
    file << "</body>\n";
    file << "</html>\n";

    file.close();
    std::cout << "HTML report written to: " << outputFile << "\n";
}

}  // namespace safec
