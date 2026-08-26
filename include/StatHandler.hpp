#pragma once

#include "LineParser.hpp"
#include <RH_HashMap.hpp>

#include <cstddef>
#include <utility>
#include <array>
#include <vector>

struct ReqAndBytesCnt {
    std::size_t requestCount;
    std::size_t bytesCount;

    ReqAndBytesCnt(): requestCount{}, bytesCount{} {};
    ReqAndBytesCnt(const std::size_t t_reqCnt, const std::size_t t_bytesCnt): requestCount{t_reqCnt}, bytesCount{t_bytesCnt} {}
};

struct GroupInfo {
    std::string_view name;
    std::size_t requestCount;
    std::size_t bytesCount;
};

struct Stats {
    Stats();

    std::vector<GroupInfo> mostActiveUsers;
    std::vector<GroupInfo> mostActiveIpAddresses;
    std::vector<GroupInfo> mostActiveHours;
    std::array<std::size_t, 4> statusCodeCount;
    std::size_t totalRequestCount;
    std::size_t totalBytesCount;
};

class StatHandler {
public:
    StatHandler(std::size_t);
    void analyzeLine(const std::optional<LineInfo> &);
    const Stats& retrieveStats(std::size_t n);
private:
    Stats stats;

    RH_HashMap userInfo;
    RH_HashMap ipAddressInfo;
    RH_HashMap hourInfo;
};
