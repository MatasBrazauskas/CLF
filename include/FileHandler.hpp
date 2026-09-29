#pragma once

#include <string_view>

#include "Simd.hpp"

class FileHandler {
public:
    FileHandler() = delete;
    explicit FileHandler(std::string_view filePath);
    ~FileHandler();

    [[nodiscard]] std::string_view getLine(const Simd&);

    std::string_view mappedFile;
    std::size_t index{};
};
