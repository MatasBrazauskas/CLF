#include "StatHandler.hpp"

#include <algorithm>
#include <iostream>
#include <future>
#include <ranges>

static std::vector<GroupInfo> mostRequestPerData(RH_HashMap& map, const std::size_t n, const char*);

Stats::Stats(): statusCodeCount{{0,0,0,0}}, totalRequestCount{}, totalBytesCount{} {}

static bool sortingPredicate(const GroupInfo& a, const GroupInfo& b) {
    if (a.requestCount == b.requestCount) {
        return a.bytesCount > b.bytesCount;
    }
    return a.requestCount > b.requestCount;
};

StatHandler::StatHandler(const std::size_t t_reserveSize)
    : stats{}, userInfo{t_reserveSize}, ipAddressInfo{t_reserveSize}, hourInfo {t_reserveSize} {
}

StatHandler::StatHandler(const std::size_t t_reserveSize, bool)
: stats{}, userInfo{t_reserveSize}, ipAddressInfo{t_reserveSize}, hourInfo {t_reserveSize} {}

void StatHandler::analyzeLine(const LineInfo& t_line, const char* t_mmapStart) {
    this->stats.totalRequestCount++;
    this->stats.totalBytesCount += t_line.byteCount;

    const int arrayIndex = t_line.statusCode / 100;
    if (arrayIndex >= 2 and arrayIndex <= 5) {
        this->stats.statusCodeCount[arrayIndex - 2]++;
    }

    userInfo.append(t_line.userId, t_mmapStart, t_line.byteCount);
    ipAddressInfo.append(t_line.ipAddress, t_mmapStart, t_line.byteCount);

    const auto hourDate = t_line.date.substr(0, 14);
    hourInfo.append(hourDate, t_mmapStart, t_line.byteCount);
}

void StatHandler::merge(const StatHandler& t_otherStats, const char* t_mmapStart) {
    for (auto i{0}; i < 4; ++i) {
        this->stats.statusCodeCount[i] += t_otherStats.stats.statusCodeCount[i];
    }

    this->stats.totalRequestCount += t_otherStats.stats.totalRequestCount;
    this->stats.totalBytesCount += t_otherStats.stats.totalBytesCount;

    this->userInfo.merge(t_otherStats.userInfo, t_mmapStart);
    this->ipAddressInfo.merge(t_otherStats.ipAddressInfo, t_mmapStart);
    this->hourInfo.merge(t_otherStats.hourInfo, t_mmapStart);
}

const Stats& StatHandler::retrieveStats(const std::size_t n, const char* t_mmapStart) {
    stats.mostActiveUsers = mostRequestPerData(userInfo, n, t_mmapStart);
    stats.mostActiveIpAddresses = mostRequestPerData(ipAddressInfo, n, t_mmapStart);
    stats.mostActiveHours = mostRequestPerData(hourInfo, n, t_mmapStart);

    return stats;
}

std::vector<GroupInfo> mostRequestPerData(RH_HashMap& map, const std::size_t n, const char* t_mmapStart) {
    std::vector<GroupInfo> result;
    result.reserve(n);

    std::size_t count{};
    for (const auto& entry : map.buckets_) {
        if (entry.offset == 0) {
            continue;
        }

        const auto& [offset, hash, comp, bytes] = entry;

        if (count < n) {
            result.emplace_back(std::string_view{t_mmapStart + offset, comp.keyLength}, comp.requestCnt, bytes);
            ++count;
        } else {
            if (auto& lastVectorElement = result.back(); lastVectorElement.requestCount < comp.requestCnt) {
                lastVectorElement = {std::string_view{t_mmapStart + offset, comp.keyLength}, comp.requestCnt, bytes};
                std::ranges::sort(result, sortingPredicate);
            }
        }
    }

    return result;
}
