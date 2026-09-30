#pragma once

#include <string_view>
#include <vector>
#include <cstdint>

struct Compressed {
    Compressed() = default;
    Compressed(const int t_keyLength, const int t_requestCnt)
        : keyLength{static_cast<std::uint16_t>(t_keyLength)}, requestCnt{static_cast<std::uint16_t>(t_requestCnt)} {}

    std::uint16_t keyLength;
    std::uint16_t requestCnt;
};

struct KeyValue {
    KeyValue() = default;
    KeyValue(const char* t_mmapStart, const char* t_mmapEnd, const std::uint32_t t_hashValue, const std::uint16_t t_keyLength,
        const std::uint16_t t_requestCnt, const std::uint32_t t_bytesCnt)
        :offset{static_cast<std::uint32_t>(t_mmapEnd - t_mmapStart)}, hashValue{t_hashValue}, comp{t_keyLength, t_requestCnt}, bytesCnt{t_bytesCnt} {}

    std::uint32_t offset;
    std::uint32_t hashValue;
    Compressed comp;
    std::uint32_t bytesCnt;
};

class RH_HashMap {
public:
    RH_HashMap() = delete;
    explicit RH_HashMap(std::size_t);

    bool append(std::string_view, const char*, std::size_t);
    const KeyValue* lookUp(std::string_view) const;

    std::uint32_t modulo(const std::uint32_t t_hash) const noexcept;
    std::vector<KeyValue> buckets_;
    std::hash<std::string_view> hashFunction_;
};