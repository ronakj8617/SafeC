#ifndef SAFEC_CONFIG_LOADER_H
#define SAFEC_CONFIG_LOADER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace safec {

// Structure to hold unsafe function information
struct UnsafeFunctionInfo {
    std::string name;
    std::string severity;
    std::string safeAlternative;
    std::string description;
};

// Structure to hold format function information
struct FormatFunctionInfo {
    std::string name;
    std::string severity;
    std::string description;
};

// Structure to hold memory function information
struct MemoryFunctionInfo {
    std::string name;
    std::string type;  // ALLOC or FREE
    std::string description;
};

class ConfigLoader {
   public:
    // Load configuration files
    static bool loadUnsafeFunctions(const std::string& filepath,
                                    std::vector<UnsafeFunctionInfo>& functions);

    static bool loadFormatFunctions(const std::string& filepath,
                                    std::vector<FormatFunctionInfo>& functions);

    static bool loadMemoryFunctions(const std::string& filepath,
                                    std::vector<MemoryFunctionInfo>& functions);

    // Helper to get function names as set
    static std::unordered_set<std::string> getUnsafeFunctionNames(
        const std::vector<UnsafeFunctionInfo>& functions);

    static std::unordered_set<std::string> getFormatFunctionNames(
        const std::vector<FormatFunctionInfo>& functions);

   private:
    static std::vector<std::string> parseCsvLine(const std::string& line);
    static std::string trim(const std::string& str);
};

}  // namespace safec

#endif  // SAFEC_CONFIG_LOADER_H
