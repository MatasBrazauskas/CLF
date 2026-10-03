#pragma once

#include <string_view>

constexpr auto emptyString = "";

class FileHandler {
public:
    FileHandler() = delete;
    explicit FileHandler(std::string_view filePath);
    ~FileHandler() noexcept;

    [[nodiscard]] std::string_view getLine() const noexcept;
    void addOffset(std::size_t) noexcept;

    std::string_view mappedFile;
    std::size_t index{};
};
