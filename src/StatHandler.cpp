#include "StatHandler.hpp"

#include <algorithm>
#include <iostream>
#include <future>
#include <ranges>

static std::vector<GroupInfo> mostRequestPerData(const Cache& cache, std::size_t n);
static void collectMostRequestPerData(const rigtorp::HashMap<std::string_view, ReqAndBytesCnt>& map, std::vector<GroupInfo>& result, std::size_t n);

Stats::Stats() : totalRequestCount{}, totalBytesCount{}, statusCodeCount{{0,0,0,0}} {}

StatHandler::StatHandler(const std::size_t t_reserveSize, const std::size_t t_n)
    : stats{}, userInfo{t_reserveSize, t_n}, ipAddressInfo{t_reserveSize, t_n}, hourInfo {t_reserveSize, t_n} {}

void StatHandler::analyzeLine(const std::optional<LineInfo> &lineOpt) {
    if (lineOpt.has_value()) {
        const auto& line = lineOpt.value();

        this->stats.totalRequestCount++;
        this->stats.totalBytesCount += line.byteCount;

        const int arrayIndex = line.statusCode / 100;
        if (arrayIndex >= 2 and arrayIndex <= 5) {
            this->stats.statusCodeCount[arrayIndex - 2]++;
        }

        userInfo.try_emplace(line.userId, line.byteCount);
        ipAddressInfo.try_emplace(line.ipAddress, line.byteCount);

        const std::size_t firstColon = line.date.find(':');
        if (firstColon == std::string::npos) {
            return;
        }

        const std::size_t secondColon = line.date.find(':', firstColon + 1);
        if (secondColon == std::string::npos) {
            return;
        }

        const auto hourDate = line.date.substr(0, secondColon);
        if (hourDate.empty()) {
            return;
        }

        hourInfo.try_emplace(hourDate, line.byteCount);
    }
}

const Stats& StatHandler::retrieveStats(const std::size_t n) {
    stats.mostActiveIpAddresses = mostRequestPerData(ipAddressInfo, n);
    stats.mostActiveUsers = mostRequestPerData(userInfo, n);
    stats.mostActiveHours = mostRequestPerData(hourInfo, n);

    return stats;
}

std::vector<GroupInfo> mostRequestPerData(const Cache& cache, const std::size_t n) {
    std::vector<GroupInfo> result;
    result.reserve(n);

    const auto& map = cache.cachedEntries();
    collectMostRequestPerData(map, result, n);

    return result;
}

void collectMostRequestPerData(const rigtorp::HashMap<std::string_view, ReqAndBytesCnt>& map, std::vector<GroupInfo>& result, const std::size_t n) {
    if (n == 0) {
        return;
    }

    const auto sortingPredicate = [](const GroupInfo& a, const GroupInfo& b) {
        return a.requestCount > b.requestCount;
    };

    for (const auto& [data, dataStats] : map) {
        const auto& [requestCnt, byteCnt] = dataStats;
        const GroupInfo group{data, requestCnt, byteCnt};

        if (result.size() < n) {
            result.push_back(group);
            std::ranges::sort(result, sortingPredicate);
        } else if (auto& lastVectorElement = result.back(); lastVectorElement.requestCount < requestCnt) {
            lastVectorElement = group;
            std::ranges::sort(result, sortingPredicate);
        }
    }
}
