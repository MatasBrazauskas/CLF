#include "StatHandler.hpp"

#include <algorithm>
#include <iostream>
#include <future>
#include <ranges>

Stats::Stats(const std::size_t n): totalRequestCount{}, totalBytesCount{}, statusCodeCount{{0,0,0,0}},
    mostActiveUsers{n, {"", 0, 0}},
    mostActiveIpAddresses{n, {"", 0, 0}},
    mostActiveHours{n, {"", 0, 0}} {}

StatHandler::StatHandler(const std::size_t n, const double t_epsilon, const double t_delta)
    : stats{n}, userInfo{t_epsilon, t_delta}, ipAddressInfo {t_epsilon, t_delta}, hourInfo {t_epsilon, t_delta} {}

void StatHandler::analyzeLine(const std::optional<LineInfo> &lineOpt) {
    if (lineOpt.has_value()) {
        const auto& line = lineOpt.value();

        this->stats.totalRequestCount++;
        this->stats.totalBytesCount += line.byteCount;

        if (const int arrayIndex = line.statusCode / 100; arrayIndex >= 2 and arrayIndex <= 5) {
            this->stats.statusCodeCount[arrayIndex - 2]++;
        }

        const auto userCms = userInfo.increment(line.userId, line.byteCount);
        if (userCms.bytesCount > stats.mostActiveUsers.back().bytesCount and std::ranges::find_if(stats.mostActiveUsers, [line](const GroupInfo& i){ return i.name == line.userId; }) != stats.mostActiveUsers.end();) {
            this->stats.mostActiveUsers.back() = {line.userId, userCms.requestCount, userCms.bytesCount};
            std::ranges::sort(stats.mostActiveUsers, [](const auto& a, const auto& b){ return a.bytesCount > b.bytesCount; });
        }

        const auto ipAddressCms = ipAddressInfo.increment(line.ipAddress, line.byteCount);
        if (ipAddressCms.bytesCount > stats.mostActiveIpAddresses.back().bytesCount) {
            this->stats.mostActiveIpAddresses.back() = {line.ipAddress, ipAddressCms.requestCount, ipAddressCms.bytesCount};
            std::ranges::sort(stats.mostActiveIpAddresses, [](const auto& a, const auto& b){ return a.bytesCount > b.bytesCount; });
        }

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

        const auto hourCms = hourInfo.increment(hourDate, line.byteCount);
        if (hourCms.bytesCount > stats.mostActiveHours.back().bytesCount) {
            this->stats.mostActiveHours.back() = {hourDate, hourCms.requestCount, hourCms.bytesCount};
            std::ranges::sort(stats.mostActiveHours, [](const auto& a, const auto& b){ return a.bytesCount > b.bytesCount; });
        }
    }
}

const Stats& StatHandler::retrieveStats(const std::size_t n) {
    return stats;
}