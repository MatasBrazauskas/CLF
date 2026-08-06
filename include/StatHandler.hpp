#pragma once

#include "LineParser.hpp"

#include <cstddef>
#include <utility>
#include <string>
#include <unordered_map>
#include <array>
#include <vector>

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

struct HashMaps {
    std::unordered_map<std::string_view, ReqAndBytesCnt> userInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt> ipAddressInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt> hourInfo;
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
    StatHandler(std::size_t t_reserveSize, std::size_t t_numberOfHashmap);
    void analyzeLine(const std::optional<LineInfo> &);
    const Stats& retrieveStats(std::size_t n, std::size_t t_reserveSize);
private:
    Stats stats;
    std::vector<HashMaps> hashMaps;
    int index{};
};
