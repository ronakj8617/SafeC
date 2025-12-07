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
};

}  // namespace safec

#endif  // SAFEC_UTILS_H
