#include "StatHandler.hpp"

#include <algorithm>
#include <ranges>

enum class TopNMetric {
    Requests,
    Bytes,
};

static void updateTopN(std::vector<TempHash>& top, std::string_view key, std::size_t hash, const ReqAndBytesCnt& value, TopNMetric metric);
static bool ranksAbove(const GroupInfo& a, const GroupInfo& b, TopNMetric metric);

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

        const auto [userHash, userCms] = userInfo.increment(line.userId, line.byteCount);
        updateTopN(topUsers, line.userId, userHash, userCms, TopNMetric::Bytes);

        const auto [ipAddressHash, ipAddressCms] = ipAddressInfo.increment(line.ipAddress, line.byteCount);
        updateTopN(topIpAddresses, line.ipAddress, ipAddressHash, ipAddressCms, TopNMetric::Bytes);

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

        const auto [hourHash, hourCms] = hourInfo.increment(hourDate, line.byteCount);
        updateTopN(topHours, hourDate, hourHash, hourCms, TopNMetric::Requests);
    }
}

const Stats& StatHandler::retrieveStats(const std::size_t) {
    const auto trs = [](const TempHash& user) {
        return user.groupInfo;
    };

    const auto bytesCmp = [](const TempHash& a, const TempHash& b) {
        return ranksAbove(a.groupInfo, b.groupInfo, TopNMetric::Bytes);
    };

    const auto requestsCmp = [](const TempHash& a, const TempHash& b) {
        return ranksAbove(a.groupInfo, b.groupInfo, TopNMetric::Requests);
    };

    std::ranges::sort(topUsers, bytesCmp);
    std::ranges::sort(topIpAddresses, bytesCmp);
    std::ranges::sort(topHours, requestsCmp);

    stats.mostActiveUsers = topUsers | std::views::transform(trs) | std::ranges::to<std::vector>();
    stats.mostActiveIpAddresses = topIpAddresses | std::views::transform(trs) | std::ranges::to<std::vector>();
    stats.mostActiveHours = topHours | std::views::transform(trs) | std::ranges::to<std::vector>();

    return stats;
}

static void updateTopN(std::vector<TempHash>& top, const std::string_view key, const std::size_t hash, const ReqAndBytesCnt& value, const TopNMetric metric) {
    const GroupInfo candidate{key, value.requestCount, value.bytesCount};
    std::size_t minIndex{};

    for (std::size_t i{}; i < top.size(); ++i) {
        if (top[i].hash == hash and top[i].groupInfo.name == key) {
            top[i].groupInfo = candidate;
            return;
        }

        if (ranksAbove(top[minIndex].groupInfo, top[i].groupInfo, metric)) {
            minIndex = i;
        }
    }

    if (not ranksAbove(candidate, top[minIndex].groupInfo, metric)) {
        return;
    }

    top[minIndex] = {hash, candidate};
}

static bool ranksAbove(const GroupInfo& a, const GroupInfo& b, const TopNMetric metric) {
    if (metric == TopNMetric::Bytes) {
        if (a.bytesCount != b.bytesCount) {
            return a.bytesCount > b.bytesCount;
        }

        if (a.requestCount != b.requestCount) {
            return a.requestCount > b.requestCount;
        }
    } else {
        if (a.requestCount != b.requestCount) {
            return a.requestCount > b.requestCount;
        }

        if (a.bytesCount != b.bytesCount) {
            return a.bytesCount > b.bytesCount;
        }
    }

    return a.name < b.name;
}
