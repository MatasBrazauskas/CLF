#pragma once

#include "LineParser.hpp"

#include <cstddef>
#include <utility>
#include <string>
#include <unordered_map>
#include <array>
#include <vector>

struct StringViewHash {
    std::size_t operator()(const std::string_view& view) const {

        std::size_t hash{FNVOffsetBasis};
        for (const char c: view)
        {
            hash ^= c;
            hash *= FNVPrime;
        }
        return hash;
    }
private:
    const std::size_t FNVPrime = 0x01000193;
    const std::size_t FNVOffsetBasis = 0x811C9DC5;
};

struct ReqAndBytesCnt {
    std::size_t requestCount;
    std::size_t bytesCount;

    ReqAndBytesCnt() = delete;
    ReqAndBytesCnt(const std::size_t t_reqCnt, const std::size_t t_bytesCnt): requestCount{t_reqCnt}, bytesCount{t_bytesCnt} {}
};

struct GroupInfo {
    std::string_view name;
    std::size_t requestCount;
    std::size_t bytesCount;
};

struct Stats {
    Stats();

    std::size_t totalRequestCount;
    std::size_t totalBytesCount;
    std::array<std::size_t, 4> statusCodeCount;
    std::vector<GroupInfo> mostActiveUsers;
    std::vector<GroupInfo> mostActiveIpAddresses;
    std::vector<GroupInfo> mostActiveHours;
};

class StatHandler {
public:
    StatHandler(std::size_t);
    void analyzeLine(const std::optional<LineInfo> &);
    const Stats& retrieveStats(std::size_t n);
private:
    Stats stats;

    std::unordered_map<std::string_view, ReqAndBytesCnt, StringViewHash> userInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt, StringViewHash> ipAddressInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt, StringViewHash> hourInfo;
};
