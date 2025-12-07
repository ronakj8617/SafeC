#include "directory_scanner.h"

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace safec {

std::vector<std::string> DirectoryScanner::findCppFiles(const std::string& directory) {
    std::vector<std::string> files;

    try {
        if (!fs::exists(directory)) {
            return files;
        }

        if (fs::is_regular_file(directory)) {
            // If it's a single file, just return it if it's a C/C++ file
            if (isCppFile(directory)) {
                files.push_back(directory);
            }
            return files;
        }

        // Recursively scan directory
        scanDirectory(directory, files);

        // Sort files for consistent ordering
        std::sort(files.begin(), files.end());

    } catch (const fs::filesystem_error& e) {
        // Silently handle filesystem errors
    }

    return files;
}

bool DirectoryScanner::isCppFile(const std::string& filename) {
    std::string ext = fs::path(filename).extension().string();

    // Convert to lowercase for case-insensitive comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return ext == ".c" || ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".c++" ||
           ext == ".h" || ext == ".hpp" || ext == ".hh" || ext == ".hxx" || ext == ".h++";
}

void DirectoryScanner::scanDirectory(const std::string& directory,
                                     std::vector<std::string>& files) {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && isCppFile(entry.path().string())) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        // Silently handle filesystem errors (e.g., permission denied)
    }
}

}  // namespace safec
