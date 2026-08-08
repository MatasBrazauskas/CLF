#include "StatHandler.hpp"

#include <algorithm>
#include <iostream>
#include <future>
#include <ranges>
#include <unordered_set>

static void updateTopN(std::vector<TempHash>& top, const std::string_view key, const std::size_t hash, const ReqAndBytesCnt& value);
static std::vector<GroupInfo> buildFinalTopN(const std::vector<TempHash>& candidates, const CountMinSketch& cms, std::size_t n);

Stats::Stats(const std::size_t n): totalRequestCount{}, totalBytesCount{}, statusCodeCount{{0,0,0,0}},
    mostActiveUsers{n, {"", 0, 0}},
    mostActiveIpAddresses{n, {"", 0, 0}},
    mostActiveHours{n, {"", 0, 0}}
    {}

StatHandler::StatHandler(const std::size_t n, const double t_epsilon, const double t_delta)
    : stats{n}, userInfo{t_epsilon, t_delta}, ipAddressInfo {t_epsilon, t_delta}, hourInfo {t_epsilon, t_delta},
    topUsers{n, {0, {"", 0, 0}}},
    topIpAddresses{n, {0, {"", 0, 0}}},
    topHours{n, {0, {"", 0, 0}}} {}

void StatHandler::analyzeLine(const std::optional<LineInfo> &lineOpt) {
    if (lineOpt.has_value()) {
        const auto& line = lineOpt.value();

        this->stats.totalRequestCount++;
        this->stats.totalBytesCount += line.byteCount;

        if (const int arrayIndex = line.statusCode / 100; arrayIndex >= 2 and arrayIndex <= 5) {
            this->stats.statusCodeCount[arrayIndex - 2]++;
        }

        const auto& [userHash, userCms] = userInfo.increment(line.userId, line.byteCount);
        updateTopN(topUsers, line.userId, userHash, userCms);

        const auto& [ipAddressHash, ipAddressCms] = ipAddressInfo.increment(line.ipAddress, line.byteCount);
        updateTopN(topIpAddresses, line.ipAddress, ipAddressHash, ipAddressCms);

        const std::size_t firstColon = line.date.find(':');
        if (firstColon == std::string::npos) {
            return;
        }

        const std::size_t secondColon = line.date.find(':', firstColon + 1);
        if (secondColon == std::string::npos) {
            return;
        }

        const auto hourDate = line.date.substr(1, secondColon - 1);
        if (hourDate.empty()) {
            return;
        }

        const auto& [hourHash, hourCms] = hourInfo.increment(hourDate, line.byteCount);
        updateTopN(topHours, hourDate, hourHash, hourCms);
    }
}

const Stats& StatHandler::retrieveStats(const std::size_t n) {
    /*const auto cmp = [](const TempHash& a, const TempHash& b) {
        return a.groupInfo.requestCount > b.groupInfo.requestCount;
    };

    const auto trs = [](const TempHash& user) {
        return user.groupInfo;
    };

    std::ranges::sort(topUsers, cmp);
    std::ranges::sort(topIpAddresses, cmp);
    std::ranges::sort(topHours, cmp);

    stats.mostActiveUsers = topUsers | std::views::transform(trs) | std::ranges::to<std::vector>();
    stats.mostActiveIpAddresses = topIpAddresses | std::views::transform(trs) | std::ranges::to<std::vector>();
    stats.mostActiveHours = topHours | std::views::transform(trs) | std::ranges::to<std::vector>();*/

    stats.mostActiveUsers = buildFinalTopN(topUsers, userInfo, n);
    stats.mostActiveIpAddresses = buildFinalTopN(topIpAddresses, ipAddressInfo, n);
    stats.mostActiveHours = buildFinalTopN(topHours, hourInfo, n);

    return stats;
}

static void updateTopN(std::vector<TempHash>& top, const std::string_view key, const std::size_t hash, const ReqAndBytesCnt& value) {
    std::size_t minIndex = 0;

    for (int i{}; i < top.size(); ++i) {
        if (top[i].hash == hash) {
            top[i].groupInfo.requestCount = value.requestCount;
            top[i].groupInfo.bytesCount = value.bytesCount;
            return;
        }

        if (top[i].groupInfo.requestCount < top[minIndex].groupInfo.requestCount) {
            minIndex = i;
        }
    }

    if (value.requestCount <= top[minIndex].groupInfo.requestCount) {
        return;
    }

    top[minIndex] = {hash, {key,value.requestCount,value.bytesCount}};
}

static std::vector<GroupInfo> buildFinalTopN(const std::vector<TempHash>& candidates, const CountMinSketch& cms, std::size_t n) {
    std::unordered_set<std::string_view> seen;
    seen.reserve(candidates.size());

    std::vector<GroupInfo> result;
    result.reserve(candidates.size());

    for (const auto& candidate : candidates) {
        const auto key = candidate.groupInfo.name;

        if (key.empty()) {
            continue;
        }

        if (!seen.insert(key).second) {
            continue;
        }

        const auto value = cms.get(key);

        result.push_back({
            key,
            value.requestCount,
            value.bytesCount
        });
    }

    std::ranges::sort(
        result,
        std::greater{},
        &GroupInfo::requestCount
    );

    if (result.size() > n) {
        result.resize(n);
    }

    return result;
}

void StatHandler::merge(const StatHandler& other)
{
    // Scalar statistics
    stats.totalRequestCount += other.stats.totalRequestCount;
    stats.totalBytesCount   += other.stats.totalBytesCount;

    for (std::size_t i = 0; i < stats.statusCodeCount.size(); ++i) {
        stats.statusCodeCount[i] += other.stats.statusCodeCount[i];
    }

    userInfo.merge(other.userInfo);
    ipAddressInfo.merge(other.ipAddressInfo);
    hourInfo.merge(other.hourInfo);

    // Collect candidates.
    topUsers.insert(
        topUsers.end(),
        other.topUsers.begin(),
        other.topUsers.end()
    );

    topIpAddresses.insert(
        topIpAddresses.end(),
        other.topIpAddresses.begin(),
        other.topIpAddresses.end()
    );

    topHours.insert(
        topHours.end(),
        other.topHours.begin(),
        other.topHours.end()
    );
}