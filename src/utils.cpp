#include "utils.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace safec {

std::string Utils::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Utils::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

bool Utils::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

std::string Utils::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    return filename.substr(dotPos);
}

}  // namespace safec
