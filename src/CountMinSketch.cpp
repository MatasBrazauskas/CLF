#include "CountMinSketch.hpp"

#include "StatHandler.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <vector>

CountMinSketch::CountMinSketch(const double t_epsilon, const double t_delta)
    : width{std::bit_ceil(static_cast<std::size_t>(std::ceil(std::exp(1.0) / t_epsilon)))},
    depth{static_cast<std::size_t>(std::ceil(std::log(1.0 / t_delta)))},
    counters(width * depth, {0, 0}) {}

Temp CountMinSketch::increment(const std::string_view t_key, const std::size_t t_bytesCnt)
{
    const std::size_t h1 = fnv1a(t_key);
    const std::size_t h2 = mix(h1);

    ReqAndBytesCnt result{std::numeric_limits<std::size_t>::max(), std::numeric_limits<std::size_t>::max()};

    for (std::size_t row{}; row < depth; ++row) {
        const std::size_t index = ((h1 + row * h2) & (width - 1)) + row * width;
        auto& cell = counters[index];

        ++cell.requestCount;
        cell.bytesCount += t_bytesCnt;

        result.requestCount = std::min(result.requestCount, cell.requestCount);
        result.bytesCount = std::min(result.bytesCount, cell.bytesCount);
    }

    return {h1, result};
}

ReqAndBytesCnt CountMinSketch::get(const std::string_view t_key) const
{
    const std::size_t h1 = fnv1a(t_key);
    const std::size_t h2 = mix(h1);

    ReqAndBytesCnt result{std::numeric_limits<std::size_t>::max(), std::numeric_limits<std::size_t>::max()};

    for (std::size_t row{}; row < depth; ++row) {
        const std::size_t index = ((h1 + row * h2) & (width - 1)) + row * width;

        result.requestCount = std::min(result.requestCount, counters[index].requestCount);
        result.bytesCount = std::min(result.bytesCount, counters[index].bytesCount);
    }

    return result;
}

void CountMinSketch::merge(const CountMinSketch& other)
{
    if (width != other.width or depth != other.depth or counters.size() != other.counters.size()) {
        throw std::invalid_argument{"Cannot merge count-min sketches with different dimensions"};
    }

    for (std::size_t i{}; i < counters.size(); ++i) {
        counters[i].requestCount += other.counters[i].requestCount;
        counters[i].bytesCount += other.counters[i].bytesCount;
    }
}

std::size_t CountMinSketch::fnv1a(const std::string_view t_value) {
    std::uint64_t hash = FNV_OFFSET;

    for (const auto byte : t_value) {
        hash ^= static_cast<unsigned char>(byte);
        hash *= FNV_PRIME;
    }

    return hash;
}

std::size_t CountMinSketch::mix(std::uint64_t t_value)
{
    t_value ^= t_value >> 30;
    t_value *= 0xbf58476d1ce4e5b9ULL;
    t_value ^= t_value >> 27;
    t_value *= 0x94d049bb133111ebULL;
    t_value ^= t_value >> 31;

    return t_value | 1ULL;
}
