#ifndef SAFEC_DIRECTORY_SCANNER_H
#define SAFEC_DIRECTORY_SCANNER_H

#include <string>
#include <vector>

namespace safec {

class DirectoryScanner {
   public:
    // Find all C/C++ files in a directory (recursively)
    static std::vector<std::string> findCppFiles(const std::string& directory);

    // Check if a file has C/C++ extension
    static bool isCppFile(const std::string& filename);

   private:
    static void scanDirectory(const std::string& directory, std::vector<std::string>& files);
};

}  // namespace safec

#endif  // SAFEC_DIRECTORY_SCANNER_H
