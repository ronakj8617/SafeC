#ifndef SAFEC_REPORT_GENERATOR_H
#define SAFEC_REPORT_GENERATOR_H

#include <string>
#include <vector>

#include "detectors/detector_base.h"

namespace safec {

class ReportGenerator {
   public:
    static void generateConsoleReport(const std::vector<Vulnerability>& vulnerabilities,
                                      const std::string& filename);

    static void generateJSONReport(const std::vector<Vulnerability>& vulnerabilities,
                                   const std::string& filename, const std::string& outputFile);

    // Generate HTML report
    static void generateHTMLReport(const std::vector<Vulnerability>& vulnerabilities,
                                   const std::string& filename, const std::string& outputFile);

   private:
    static std::string getSeverityColor(Severity sev);
};

}  // namespace safec

#endif  // SAFEC_REPORT_GENERATOR_H
