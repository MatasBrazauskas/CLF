#pragma once

#include "LineParser.hpp"

#include <cstddef>
#include <utility>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>

using ReqAndBytesCnt = std::pair<int, int>;
using GroupInfo = std::tuple<std::string_view, int, int>;

struct Stats {
    Stats();

    std::size_t totalRequestCount;
    std::size_t totalBytesCount;
    std::array<std::size_t, 4> statusCodeCount;
    std::vector<GroupInfo> mostActiveUsers;
    std::vector<GroupInfo> mostActiveIpAddresses;
};

class StatHandler {
public:
    StatHandler();
    void analyzeLine(const std::optional<LineInfo> &);
    const Stats& retrieveStats(std::size_t n);
private:
    Stats stats;

    std::unordered_map<std::string_view, ReqAndBytesCnt> userInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt> ipAddressInfo;

    std::vector<GroupInfo> userWithMostRequest(std::size_t n) const;
    std::vector<GroupInfo> ipAddressesWithMostRequest(std::size_t n) const;
};
