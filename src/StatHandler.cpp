#include "StatHandler.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>

ReqAndBytesCnt::ReqAndBytesCnt() noexcept: requestCount{}, bytesCount{} {}

ReqAndBytesCnt::ReqAndBytesCnt(const std::size_t t_reqCnt, const std::size_t t_bytesCnt) noexcept: requestCount{t_reqCnt}, bytesCount{t_bytesCnt} {}

GroupInfo::GroupInfo(const std::string_view t_name, const std::size_t t_reqCnt, const std::size_t t_bytesCnt) noexcept
    :name{t_name}, reqAndBytesCnt{t_reqCnt, t_bytesCnt} {}

static std::vector<GroupInfo> mostRequestPerData(RH_HashMap& map, const std::size_t n, const char*) noexcept;

static bool cmp(const KeyValue& l, const KeyValue& r) noexcept {
    if (l.comp.requestCnt == r.comp.requestCnt) {
        return l.bytesCnt > r.bytesCnt;
    }

    return l.comp.requestCnt > r.comp.requestCnt;
};


Stats::Stats(): statusCodeCount{{0,0,0,0}}, totalRequestCount{}, totalBytesCount{} {}

StatHandler::StatHandler(const std::size_t t_reserveSize)
    : stats{}, userInfo{t_reserveSize}, ipAddressInfo{t_reserveSize}, hourInfo {t_reserveSize} {}

void StatHandler::analyzeLine(const LineInfo& t_line, const char* t_mmapStart) noexcept {
    [[assume(t_mmapStart != nullptr)]];

    this->stats.totalRequestCount++;
    this->stats.totalBytesCount += t_line.byteCount;

    //const int arrayIndex = t_line.statusCode / 100;
    //if (arrayIndex >= 2 and arrayIndex <= 5) {
        this->stats.statusCodeCount[t_line.statusCode / 100 - 2]++;
    //}

    userInfo.append(t_line.userId, t_mmapStart, t_line.byteCount);
    ipAddressInfo.append(t_line.ipAddress, t_mmapStart, t_line.byteCount);

    const auto hourDate = t_line.date.substr(0, 14);
    hourInfo.append(hourDate, t_mmapStart, t_line.byteCount);
}

const Stats& StatHandler::retrieveStats(const std::size_t n, const char* t_mmapStart) noexcept {
    stats.mostActiveUsers = mostRequestPerData(userInfo, n, t_mmapStart);
    stats.mostActiveIpAddresses = mostRequestPerData(ipAddressInfo, n, t_mmapStart);
    stats.mostActiveHours = mostRequestPerData(hourInfo, n, t_mmapStart);

    return stats;
}

std::vector<GroupInfo> mostRequestPerData(RH_HashMap& map, const std::size_t n, const char* t_mmapStart) noexcept {
    [[assume(t_mmapStart != nullptr)]];

    std::ranges::partial_sort(map.buckets_, map.buckets_.begin() + n, cmp);

    std::vector<GroupInfo> result;
    result.reserve(map.buckets_.size());

    for (auto i{0}; i < n; i++) {
        const auto& kv = map.buckets_[i];
        result.emplace_back(std::string_view{t_mmapStart + kv.offset, kv.comp.keyLength}, kv.comp.requestCnt, kv.bytesCnt);
    }

    return result;
}