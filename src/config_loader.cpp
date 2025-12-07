#include "config_loader.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace safec {

std::string ConfigLoader::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> ConfigLoader::parseCsvLine(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, ',')) {
        result.push_back(trim(item));
    }

    return result;
}

bool ConfigLoader::loadUnsafeFunctions(const std::string& filepath,
                                       std::vector<UnsafeFunctionInfo>& functions) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filepath << std::endl;
        return false;
    }

    std::string line;
    // Skip header and comments
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        auto parts = parseCsvLine(line);
        if (parts.size() >= 4) {
            UnsafeFunctionInfo info;
            info.name = parts[0];
            info.severity = parts[1];
            info.safeAlternative = parts[2];
            info.description = parts[3];
            functions.push_back(info);
        }
    }

    return true;
}

bool ConfigLoader::loadFormatFunctions(const std::string& filepath,
                                       std::vector<FormatFunctionInfo>& functions) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filepath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        auto parts = parseCsvLine(line);
        if (parts.size() >= 3) {
            FormatFunctionInfo info;
            info.name = parts[0];
            info.severity = parts[1];
            info.description = parts[2];
            functions.push_back(info);
        }
    }

    return true;
}

bool ConfigLoader::loadMemoryFunctions(const std::string& filepath,
                                       std::vector<MemoryFunctionInfo>& functions) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filepath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        auto parts = parseCsvLine(line);
        if (parts.size() >= 3) {
            MemoryFunctionInfo info;
            info.name = parts[0];
            info.type = parts[1];
            info.description = parts[2];
            functions.push_back(info);
        }
    }

    return true;
}

std::unordered_set<std::string> ConfigLoader::getUnsafeFunctionNames(
    const std::vector<UnsafeFunctionInfo>& functions) {
    std::unordered_set<std::string> names;
    for (const auto& func : functions) {
        names.insert(func.name);
    }
    return names;
}

std::unordered_set<std::string> ConfigLoader::getFormatFunctionNames(
    const std::vector<FormatFunctionInfo>& functions) {
    std::unordered_set<std::string> names;
    for (const auto& func : functions) {
        names.insert(func.name);
    }
    return names;
}

}  // namespace safec
