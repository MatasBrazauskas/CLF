#pragma once

#include <fstream>
#include <generator>

class FileHandler {
public:
    FileHandler() = delete;
    explicit FileHandler(const std::string& filePath);
    [[nodiscard]] std::generator<std::string_view> getLine() const;
    ~FileHandler();
private:
    std::string_view mappedFile;
};