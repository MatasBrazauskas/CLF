#include "StatHandler.hpp"

#include <algorithm>
#include <iostream>
#include <future>
#include <ranges>

static std::vector<GroupInfo> mostRequestPerData(const std::unordered_map<std::string_view, ReqAndBytesCnt>& map, const std::size_t n);

Stats::Stats() : totalRequestCount{}, totalBytesCount{}, statusCodeCount{{0,0,0,0}} {}

StatHandler::StatHandler(const std::size_t t_reserveSize, const std::size_t t_numberOfHashmaps): stats{} {
    hashMaps.reserve(t_numberOfHashmaps);

    for (int i{}; i < t_numberOfHashmaps; i++) {
        hashMaps.emplace_back(
            std::unordered_map<std::string_view, ReqAndBytesCnt>{},
            std::unordered_map<std::string_view, ReqAndBytesCnt>{},
            std::unordered_map<std::string_view, ReqAndBytesCnt>{}
            );

        hashMaps[i].userInfo.reserve(t_reserveSize);
        hashMaps[i].ipAddressInfo.reserve(t_reserveSize);
        hashMaps[i].hourInfo.reserve(t_reserveSize);

        hashMaps[i].userInfo.max_load_factor(0.5f);
        hashMaps[i].ipAddressInfo.max_load_factor(0.5f);
        hashMaps[i].hourInfo.max_load_factor(0.5f);
    }
}

void StatHandler::analyzeLine(const std::optional<LineInfo> &lineOpt) {
    if (lineOpt.has_value()) {
        const auto& line = lineOpt.value();
        auto& [userInfo, ipAddressInfo, hourInfo] = hashMaps[index];

        this->stats.totalRequestCount++;
        this->stats.totalBytesCount += line.byteCount;

        const int arrayIndex = line.statusCode / 100;
        if (arrayIndex >= 2 and arrayIndex <= 5) {
            this->stats.statusCodeCount[arrayIndex - 2]++;
        }

        const auto& userIt = userInfo.try_emplace(line.userId, ReqAndBytesCnt{0, 0}).first;
        userIt->second.requestCount++;
        userIt->second.bytesCount += line.byteCount;

        const auto& ipAddressIt = ipAddressInfo.try_emplace(line.ipAddress, ReqAndBytesCnt{0, 0}).first;
        ipAddressIt->second.requestCount++;
        ipAddressIt->second.bytesCount += line.byteCount;

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

        const auto& hourIt = hourInfo.try_emplace(hourDate, ReqAndBytesCnt{0,0}).first;
        hourIt->second.requestCount++;
        hourIt->second.bytesCount += line.byteCount;

        index = (index + 1) % hashMaps.size();
    }
}

const Stats& StatHandler::retrieveStats(const std::size_t n, const std::size_t t_reserveSize) {
    std::unordered_map<std::string_view, ReqAndBytesCnt> f_userInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt> f_ipAddressInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt> f_hourInfo;

    f_userInfo.max_load_factor(0.5f);
    f_ipAddressInfo.max_load_factor(0.5f);
    f_hourInfo.max_load_factor(0.5f);

    f_userInfo.reserve(t_reserveSize);
    f_ipAddressInfo.reserve(t_reserveSize);
    f_hourInfo.reserve(t_reserveSize);

    for (const auto& hashMap: hashMaps) {
        const auto& [userInfo, ipAddressInfo, hourInfo] = hashMap;

        for (const auto& [key,value]:userInfo) {
            const auto& userIt = userInfo.try_emplace(key, ReqAndBytesCnt{0, 0}).first;
            userIt->second.requestCount++;
            userIt->second.bytesCount += line.byteCount;
        }
    }

    stats.mostActiveIpAddresses = mostRequestPerData(ipAddressInfo, n);
    stats.mostActiveUsers = mostRequestPerData(userInfo, n);
    stats.mostActiveHours = mostRequestPerData(hourInfo, n);

    return stats;
}

std::vector<GroupInfo> mostRequestPerData(const std::unordered_map<std::string_view, ReqAndBytesCnt>& map, const std::size_t n) {
    const auto sortingPredicate = [](const GroupInfo& a, const GroupInfo& b) {
        return a.requestCount > b.requestCount;
    };

    const auto transformFunction = [](const std::pair<std::string_view, ReqAndBytesCnt>& pair) {
        return GroupInfo {pair.first, pair.second.requestCount, pair.second.bytesCount};
    };

    auto result = map | std::views::take(n) | std::views::transform(transformFunction) | std::ranges::to<std::vector>();
    std::ranges::sort(result, sortingPredicate);

    for (const auto& [data,dataStats] : map | std::views::drop(n)) {
        const auto& [requestCnt, byteCnt] = dataStats;

        if (auto& lastVectorElement = result.back(); lastVectorElement.bytesCount < byteCnt) {
            lastVectorElement = {data, requestCnt, byteCnt};
            std::ranges::sort(result, sortingPredicate);
        }
    }

    return result;
}