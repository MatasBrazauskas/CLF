#pragma once

#pragma once

#include <Simd.hpp>
#include <generator>
#include <vector>

class FileHandler {
public:
    FileHandler() = delete;
    explicit FileHandler(std::string_view, std::size_t, Simd&);
    [[nodiscard]] std::generator<std::string_view> getLine(int) const;
    ~FileHandler();
private:
    char *mappedFile{};
    std::size_t mappedFileSize{};
    std::vector<std::string_view> fileChunks;
};
