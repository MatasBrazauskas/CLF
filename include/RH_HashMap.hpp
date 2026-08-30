#pragma once

#include <string_view>
#include <vector>
#include <cstdint>

struct KeyValue {
    KeyValue();
    KeyValue(std::string_view, std::size_t, std::size_t, std::uint32_t);

    const char *keyData;
    std::size_t requestCnt;
    std::size_t bytesCnt;
    std::uint32_t keyLength;
    std::uint32_t hashValue;
};

class RH_HashMap {
public:
    RH_HashMap() = delete;
    explicit RH_HashMap(std::size_t);

    bool append(std::string_view, std::size_t, std::size_t);
    const KeyValue* lookUp(std::string_view) const;

    std::uint32_t modulo(const std::uint32_t t_hash) const noexcept;
    std::vector<KeyValue> buckets_;
    std::hash<std::string_view> hashFunction_;
private:
    void tryPromote(std::size_t, std::size_t);
};