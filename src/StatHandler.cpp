#include "StatHandler.hpp"

#include <algorithm>
#include <iostream>

StatHandler::StatHandler(): errorCount{}, totalCount{} {}

void StatHandler::analyzeLine(std::optional<LineInfo> lineOpt) {
    totalCount++;

    if (lineOpt.has_value()) {
        const auto line = lineOpt.value();

        if (const auto it = userInfo.find(line.userId); it != userInfo.end()) {
            auto& [requestCount, byteCount] = it->second;
            requestCount++;
            byteCount += line.byteCount;
        } else {
            userInfo.insert({line.userId, {1, line.byteCount}});
        }

        if (const auto it = ipAddressInfo.find(line.ipAddress); it != ipAddressInfo.end()) {
            auto& [requestCount, byteCount] = it->second;
            requestCount++;
            byteCount += line.byteCount;
        } else {
            ipAddressInfo.insert({line.userId, {1, line.byteCount}});
        }
    } else {
        errorCount++;
    }
}

float StatHandler::errorRate() const {
    return this->errorCount / static_cast<float>(this->totalCount) * 100.0f;
}

std::vector<GroupInfo> StatHandler::userWithMostRequest(const std::size_t n) const {
    const auto sortingPredicate = [](const GroupInfo& a, const GroupInfo& b) {
        return get<2>(a) > get<2>(b);
    };
    std::vector<GroupInfo> result;
    result.reserve(n);

    for (const auto& [userId, userStat] : userInfo) {
        const auto& [requestCnt, byteCnt] = userStat;

        if (result.size() < n) {
            const auto lastVectorElement = std::make_tuple(userId, requestCnt, byteCnt);
            result.push_back(lastVectorElement);
            std::ranges::sort(result, sortingPredicate);
        } else if (auto& lastVectorElement = result.back(); get<2>(lastVectorElement) < byteCnt) {
            lastVectorElement = std::make_tuple(userId, requestCnt, byteCnt);
            std::ranges::sort(result, sortingPredicate);
        }
    }

    return result;
}

std::vector<GroupInfo> StatHandler::ipAddressesWithMostRequest(const std::size_t n) const {
    const auto sortingPredicate = [](const GroupInfo& a, const GroupInfo& b) {
        return get<2>(a) > get<2>(b);
    };
    std::vector<GroupInfo> result;
    result.reserve(n);

    for (const auto& [ipAddress, ipAddressStat] : ipAddressInfo) {
        const auto& [requestCnt, byteCnt] = ipAddressStat;

        if (result.size() < n) {
            const auto lastVectorElement = std::make_tuple(ipAddress, requestCnt, byteCnt);
            result.push_back(lastVectorElement);
            std::ranges::sort(result, sortingPredicate);
        } else if (auto& lastVectorElement = result.back(); get<2>(lastVectorElement) < byteCnt) {
            lastVectorElement = std::make_tuple(ipAddress, requestCnt, byteCnt);
            std::ranges::sort(result, sortingPredicate);
        }
    }

    return result;
}
