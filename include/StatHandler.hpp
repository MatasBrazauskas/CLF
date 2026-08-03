#pragma once

#include "LineParser.hpp"

#include <cstddef>
#include <utility>
#include <string>
#include <unordered_map>
#include <vector>

using ReqAndBytesCnt = std::pair<int, int>;
using GroupInfo = std::tuple<std::string, int, int>;

class StatHandler {
public:
    StatHandler();
    void analyzeLine(std::optional<LineInfo>);

    float errorRate() const;
    std::vector<GroupInfo> userWithMostRequest(std::size_t n) const;
    std::vector<GroupInfo> ipAddressesWithMostRequest(std::size_t n) const;
private:
    std::size_t errorCount;
    std::size_t totalCount;

    std::unordered_map<std::string, ReqAndBytesCnt> userInfo;
    std::unordered_map<std::string, ReqAndBytesCnt> ipAddressInfo;
};
