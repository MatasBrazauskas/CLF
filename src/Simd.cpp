#include "Simd.hpp"

#include <bit>
#include <utility>

Simd::Simd()
    : Delimiters{{
        SimdVector{_mm256_set1_epi8(' ')},
        SimdVector{_mm256_set1_epi8(']')},
        SimdVector{_mm256_set1_epi8('"')},
        SimdVector{_mm256_set1_epi8('\n')}
    }}
{}

[[gnu::target("avx2")]]
[[nodiscard]] std::size_t Simd::findDelimiter(const std::string_view t_line, const std::size_t t_offset, const Delimiter t_delimiterOpt) {
    if (currentLineData != t_line.data() || currentLineSize != t_line.size()) {
        delimiterChunks = {0, 0, 0};
        delimiterChunkBases = {0, 0, 0};
        currentLineData = t_line.data();
        currentLineSize = t_line.size();
    }

    const auto delimIdx{std::to_underlying(t_delimiterOpt)};
    const auto& delimiter = Delimiters[delimIdx].v;
    auto& cachedMask = delimiterChunks[delimIdx];
    auto& chunkBase = delimiterChunkBases[delimIdx];

    if (cachedMask != 0) {
        while (cachedMask != 0) {
            const int bitIndex = std::countr_zero(cachedMask);
            const std::size_t absoluteIndex = chunkBase + bitIndex;
            cachedMask &= (cachedMask - 1);

            if (absoluteIndex >= t_offset) {
                return absoluteIndex;
            }
        }
    }

    std::size_t i{t_offset};
    const std::size_t size{t_line.size()};

    for (; i + 32 <= size; i += 32) {
        const __m256i dataChunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(t_line.data() + i));
        const __m256i cmp = _mm256_cmpeq_epi8(dataChunk, delimiter);
        const std::uint32_t mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(cmp));

        if (mask != 0) {
            const int bitIndex = std::countr_zero(mask);

            chunkBase = i;
            cachedMask = mask & (mask - 1);

            return i + bitIndex;
        }
    }

    const char targetChar = static_cast<char>(_mm256_extract_epi8(delimiter, 0));
    for (; i < size; ++i) {
        if (t_line[i] == targetChar) {
            return i;
        }
    }

    return std::string_view::npos;
}

std::size_t Simd::findNewLine(const std::string_view t_line, const std::size_t t_offset) const {
    for (std::size_t i{}; i <= t_line.size(); i += 32) {
        const __m256i dataChunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(t_line.data() + i + t_offset));
        const __m256i cmp = _mm256_cmpeq_epi8(dataChunk, Delimiters[3].v);
        const std::uint32_t mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(cmp));

        if (mask != 0) {
            return t_offset + i + std::countr_zero(mask);
        }
    }
}
