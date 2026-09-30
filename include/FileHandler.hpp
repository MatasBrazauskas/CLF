#pragma once

#include <Simd.hpp>

#include <generator>
#include <vector>

class FileHandler {
public:
    FileHandler() = delete;
    explicit FileHandler(Simd&, std::string_view, std::size_t);
    [[nodiscard]] std::generator<std::string_view> getLine(std::size_t) const;
    [[nodiscard]] const char* getChunkStartAddress(const std::size_t) const;
    ~FileHandler();
private:
    char *mappedFile{};
    std::size_t mappedFileSize{};
    std::vector<std::string_view> fileChunks;
};
