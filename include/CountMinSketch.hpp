#pragma once


#include <vector>
#include <string_view>


struct ReqAndBytesCnt;

struct Temp {
    std::size_t hash;
    ReqAndBytesCnt& value;
};

class CountMinSketch {
public:
    CountMinSketch() = delete;
    CountMinSketch(double t_epsilon, double t_delta);
    ~CountMinSketch() = default;

    [[nodiscard]] Temp increment(std::string_view, std::size_t);
    [[nodiscard]] ReqAndBytesCnt get(std::string_view key) const;
    void merge(const CountMinSketch& other);
private:
    static constexpr std::size_t FNV_OFFSET = 14695981039346656037ULL;
    static constexpr std::size_t FNV_PRIME = 1099511628211ULL;

    std::size_t width;
    std::size_t depth;

    std::vector<ReqAndBytesCnt> counters;

    static std::size_t fnv1a(std::string_view);
    static std::size_t mix(std::size_t);
};
