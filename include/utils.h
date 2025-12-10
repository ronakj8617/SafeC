#ifndef SAFEC_UTILS_H
#define SAFEC_UTILS_H

#include <fstream>
#include <sstream>
#include <string>

namespace safec {

class Utils {
   public:
    // Read file contents
    static std::string readFile(const std::string& filename);

    // Check if file exists
    static bool fileExists(const std::string& filename);

    // Check if path is a directory
    static bool isDirectory(const std::string& path);

    // Get file extension
    static std::string getFileExtension(const std::string& filename);

    // Preprocess code: normalize whitespace and squash line breaks
    static std::string preprocessCode(const std::string& code);
    static bool shouldExcludeTestFile(const std::string& filePath);
};

}  // namespace safec

#endif  // SAFEC_UTILS_H
