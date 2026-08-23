#pragma once

#include <fstream>
#include <generator>

#include "Simd.hpp"

class FileHandler {
public:
    FileHandler() = delete;
    explicit FileHandler(std::string_view filePath);
    [[nodiscard]] std::generator<std::string_view> getLine() const;
    ~FileHandler();
private:
    std::string_view mappedFile;
};
