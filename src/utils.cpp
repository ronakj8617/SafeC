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

std::string Utils::preprocessCode(const std::string& code) {
    std::string result;
    result.reserve(code.size());
    
    bool inString = false;
    bool inChar = false;
    bool inComment = false;
    bool inMultiComment = false;
    bool lastWasNewline = false;
    
    for (size_t i = 0; i < code.size(); ++i) {
        char c = code[i];
        char next = (i + 1 < code.size()) ? code[i + 1] : '\0';
        
        // Handle string literals
        if (!inComment && !inMultiComment && !inChar && c == '"' && (i == 0 || code[i-1] != '\\')) {
            inString = !inString;
            result += c;
            lastWasNewline = false;
            continue;
        }
        
        // Handle char literals
        if (!inComment && !inMultiComment && !inString && c == '\'' && (i == 0 || code[i-1] != '\\')) {
            inChar = !inChar;
            result += c;
            lastWasNewline = false;
            continue;
        }
        
        // Handle single-line comments
        if (!inString && !inChar && !inMultiComment && c == '/' && next == '/') {
            inComment = true;
            result += c;
            continue;
        }
        
        // Handle multi-line comments
        if (!inString && !inChar && !inComment && c == '/' && next == '*') {
            inMultiComment = true;
            result += c;
            continue;
        }
        
        if (inMultiComment && c == '*' && next == '/') {
            inMultiComment = false;
            result += c;
            if (next == '/') {
                result += next;
                i++;
            }
            continue;
        }
        
        if (inComment && c == '\n') {
            inComment = false;
            result += c;
            lastWasNewline = true;
            continue;
        }
        
        if (inComment || inMultiComment) {
            result += c;
            continue;
        }
        
        // Normalize whitespace: replace newlines/tabs with spaces, collapse multiple spaces
        if (c == '\n' || c == '\r' || c == '\t') {
            if (!lastWasNewline && !inString && !inChar) {
                result += ' ';
                lastWasNewline = true;
            }
        } else if (c == ' ') {
            if (!lastWasNewline || (i > 0 && result.back() != ' ')) {
                result += c;
            }
            lastWasNewline = false;
        } else {
            result += c;
            lastWasNewline = false;
        }
    }
    
    return result;
}

bool Utils::shouldExcludeTestFile(const std::string& filePath) {
    // Exclude only intentionally vulnerable CWE test files ("bad" variants)
    // Keep "good" variants so we can verify no false positives
    
    // Extract filename from path
    std::string filename = filePath;
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        filename = filename.substr(lastSlash + 1);
    }
    
    // Exclude files with "_bad" suffix (intentional vulnerabilities)
    // This includes: *_bad.cpp, *_bad.c, *_bad.h, etc.
    if (filename.find("_bad") != std::string::npos) {
        return true;
    }
    
    // Exclude files with badSource or badSink in the name
    if (filename.find("badSource") != std::string::npos ||
        filename.find("badSink") != std::string::npos) {
        return true;
    }
    
    // Exclude testcasesupport directories (support files, not actual test cases)
    if (filePath.find("/testcasesupport/") != std::string::npos ||
        filePath.find("\\testcasesupport\\") != std::string::npos) {
        return true;
    }
    
    // Note: We do NOT exclude all files in Juliet-TestCases or CWE directories
    // because "good" variants should be scanned to verify no false positives
    
    return false;
}

}  // namespace safec
