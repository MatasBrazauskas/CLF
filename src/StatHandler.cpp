#include "StatHandler.hpp"

#include <algorithm>
#include <iostream>
#include <future>
#include <ranges>

static std::vector<GroupInfo> mostRequestPerData(RH_HashMap& map, const std::size_t n);

Stats::Stats() : totalRequestCount{}, totalBytesCount{}, statusCodeCount{{0,0,0,0}} {}

StatHandler::StatHandler(const std::size_t t_reserveSize)
    : stats{}, userInfo{t_reserveSize}, ipAddressInfo{t_reserveSize}, hourInfo {t_reserveSize} {}

void StatHandler::analyzeLine(const std::optional<LineInfo> &lineOpt) {
    if (lineOpt.has_value()) {
        const auto& line = lineOpt.value();

        this->stats.totalRequestCount++;
        this->stats.totalBytesCount += line.byteCount;

        const int arrayIndex = line.statusCode / 100;
        if (arrayIndex >= 2 and arrayIndex <= 5) {
            this->stats.statusCodeCount[arrayIndex - 2]++;
        }

        userInfo.append(line.userId, 1, line.byteCount);
        ipAddressInfo.append(line.ipAddress, 1, line.byteCount);

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

        hourInfo.append(hourDate, 1, line.byteCount);
    }
}

const Stats& StatHandler::retrieveStats(const std::size_t n) {
    stats.mostActiveIpAddresses = mostRequestPerData(ipAddressInfo, n);
    stats.mostActiveUsers = mostRequestPerData(userInfo, n);
    stats.mostActiveHours = mostRequestPerData(hourInfo, n);

    return stats;
}

std::vector<GroupInfo> mostRequestPerData(RH_HashMap& map, const std::size_t n) {
    const auto sortingPredicate = [](const GroupInfo& a, const GroupInfo& b) {
        return a.requestCount > b.requestCount;
    };

    std::vector<GroupInfo> result;
    result.reserve(n);

    std::size_t count{};
    for (const auto& entry : map.buckets_) {
        const auto& [keyData, requestCnt, bytesCnt, keyLength, hashValue] = entry;

        if (count < n) {
            result.emplace_back(std::string_view{keyData, keyLength}, requestCnt, bytesCnt);
            ++count;
        } else {
            if (auto& lastVectorElement = result.back(); lastVectorElement.requestCount < requestCnt) {
                lastVectorElement = {std::string_view{keyData, keyLength}, requestCnt, bytesCnt};
                std::ranges::sort(result, sortingPredicate);
            }
        }
    }

    return result;
}
