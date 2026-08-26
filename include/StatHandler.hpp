#pragma once

#include "LineParser.hpp"

#include <cstddef>
#include <utility>
#include <array>
#include <vector>
#include <Cache.hpp>

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
    StatHandler(std::size_t, std::size_t);
    void analyzeLine(const std::optional<LineInfo> &);
    const Stats& retrieveStats(std::size_t n);
private:
    Stats stats;

    Cache userInfo;
    Cache ipAddressInfo;
    Cache hourInfo;
};
