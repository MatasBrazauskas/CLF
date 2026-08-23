#pragma once

#include <immintrin.h>
#include <string_view>
#include <cstdint>
#include <array>

enum class Delimiter { Spaces, Braces, Quotes };

struct alignas(32) SimdVector {
    __m256i v;
};

class Simd {
public:
    Simd();
    [[nodiscard]] std::size_t findDelimiter(std::string_view, std::size_t, Delimiter);
    [[nodiscard]] std::size_t findNewLine(std::string_view, std::size_t) const;
private:
    const std::array<SimdVector, 4> Delimiters;

    std::array<std::uint32_t, 3> delimiterChunks{{0, 0, 0}};
    std::array<std::size_t, 3> delimiterChunkBases{{0, 0, 0}};
    const char* currentLineData{};
    std::size_t currentLineSize{};
};
