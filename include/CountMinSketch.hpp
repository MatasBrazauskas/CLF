#pragma once

#include <cstddef>
#include <vector>
#include <string_view>

struct ReqAndBytesCnt {
    std::size_t requestCount;
    std::size_t bytesCount;

    ReqAndBytesCnt() = delete;
    ReqAndBytesCnt(std::size_t t_reqCnt, std::size_t t_bytesCnt)
        : requestCount{t_reqCnt}, bytesCount{t_bytesCnt} {}
};

struct Temp {
    std::size_t hash;
    ReqAndBytesCnt value;
};

class CountMinSketch {
public:
    CountMinSketch() = delete;
    CountMinSketch(double t_epsilon, double t_delta);
    ~CountMinSketch() = default;

    [[nodiscard]] Temp increment(std::string_view, std::size_t);
private:
    static constexpr std::size_t FNV_OFFSET = 14695981039346656037ULL;
    static constexpr std::size_t FNV_PRIME = 1099511628211ULL;

    std::size_t width;
    std::size_t depth;

    std::vector<ReqAndBytesCnt> counters;

    static std::size_t fnv1a(std::string_view);
    static std::size_t mix(std::size_t);
};
