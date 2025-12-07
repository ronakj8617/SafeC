#ifndef SAFEC_DETECTOR_BASE_H
#define SAFEC_DETECTOR_BASE_H

#include <string>
#include <vector>

#include "../ast.h"

namespace safec {

// Vulnerability severity levels
enum class Severity { LOW, MEDIUM, HIGH, CRITICAL };

// Vulnerability finding structure
struct Vulnerability {
    std::string type;
    std::string description;
    std::string recommendation;
    Severity severity;
    int line;
    int column;
    std::string codeSnippet;
};

// Base class for all vulnerability detectors
class DetectorBase : public ASTVisitor {
   public:
    virtual ~DetectorBase() = default;

    // Main detection method
    virtual void analyze(Program& program) = 0;

    // Get detected vulnerabilities
    const std::vector<Vulnerability>& getVulnerabilities() const {
        return vulnerabilities_;
    }

    // Get detector name
    virtual std::string getName() const = 0;

    // Helper to add a vulnerability
    void addVulnerability(const std::string& type, const std::string& description,
                          const std::string& recommendation, Severity severity, int line,
                          int column, const std::string& codeSnippet = "") {
        vulnerabilities_.push_back(
            {type, description, recommendation, severity, line, column, codeSnippet});
    }

    // Convert severity to string
    static std::string severityToString(Severity sev) {
        switch (sev) {
            case Severity::LOW:
                return "LOW";
            case Severity::MEDIUM:
                return "MEDIUM";
            case Severity::HIGH:
                return "HIGH";
            case Severity::CRITICAL:
                return "CRITICAL";
            default:
                return "UNKNOWN";
        }
    }

   protected:
    std::vector<Vulnerability> vulnerabilities_;
};

}  // namespace safec

#endif  // SAFEC_DETECTOR_BASE_H
