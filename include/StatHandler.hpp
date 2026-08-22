#pragma once

#include "LineParser.hpp"
#include "CountMinSketch.hpp"

#include <cstddef>
#include <utility>
#include <array>
#include <vector>

struct ReqAndBytesCnt {
    std::size_t requestCount;
    std::size_t bytesCount;

    ReqAndBytesCnt() = delete;
    ReqAndBytesCnt(const std::size_t t_reqCnt, const std::size_t t_bytesCnt): requestCount{t_reqCnt}, bytesCount{t_bytesCnt} {}
    auto operator<=>(const ReqAndBytesCnt& t_other) const {
        return this->requestCount > t_other.requestCount;
    }

    auto operator==(const ReqAndBytesCnt& t_other) const {
        return this->requestCount == t_other.requestCount;
    }
};

struct GroupInfo {
    std::string_view name;
    std::size_t requestCount;
    std::size_t bytesCount;

    auto operator<=>(const GroupInfo& t_other) const {
        return this->requestCount <=> t_other.requestCount;
    }

    auto operator==(const GroupInfo& t_other) const {
        return this->requestCount == t_other.requestCount;
    }
};

struct TempHash {
    std::size_t hash;
    GroupInfo groupInfo;
};

struct Stats {
    Stats(std::size_t);

    std::size_t totalRequestCount;
    std::size_t totalBytesCount;
    std::array<std::size_t, 4> statusCodeCount;
    std::vector<GroupInfo> mostActiveUsers;
    std::vector<GroupInfo> mostActiveIpAddresses;
    std::vector<GroupInfo> mostActiveHours;
};

class StatHandler {
public:
    explicit StatHandler(std::size_t n, double t_epsilon, double t_delta);
    void analyzeLine(const std::optional<LineInfo> &);
    const Stats& retrieveStats(std::size_t n);
    void merge(const StatHandler& other);
private:
    Stats stats;
    std::size_t topLimit;

    CountMinSketch userInfo;
    CountMinSketch ipAddressInfo;
    CountMinSketch hourInfo;

    std::vector<TempHash> topUsers;
    std::vector<TempHash> topIpAddresses;
    std::vector<TempHash> topHours;
};
