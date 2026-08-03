#pragma once

#include <fstream>
#include <generator>

class FileHandler {
public:
    FileHandler() = delete;
    explicit FileHandler(const std::string& filePath);
    std::generator<std::string> getLine();
    ~FileHandler();
private:
    std::ifstream fileStream;
};