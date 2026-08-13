#include "StatHandler.hpp"

#include <algorithm>

static bool isBetter(const GroupInfo& lhs, const GroupInfo& rhs);
static void updateTopN(std::vector<TempHash>& top, std::size_t limit, std::string_view key, std::size_t hash, const ReqAndBytesCnt& value);
static std::vector<GroupInfo> buildSortedGroups(const std::vector<TempHash>& candidates);
static std::vector<GroupInfo> buildFinalTopN(const std::vector<TempHash>& candidates, std::size_t limit);
static std::vector<GroupInfo> buildFinalTopN(const std::vector<TempHash>& candidates, const CountMinSketch& cms, std::size_t limit);
static std::vector<GroupInfo> mergeTopN(const std::vector<GroupInfo>& vecA, const std::vector<GroupInfo>& vecB);
static std::vector<TempHash> toTempHashVector(const std::vector<GroupInfo>& groups);

Stats::Stats(const std::size_t n): totalRequestCount{}, totalBytesCount{}, statusCodeCount{{0,0,0,0}},
    mostActiveUsers{},
    mostActiveIpAddresses{},
    mostActiveHours{}
{
    mostActiveUsers.reserve(n);
    mostActiveIpAddresses.reserve(n);
    mostActiveHours.reserve(n);
}

StatHandler::StatHandler(const std::size_t n, const double t_epsilon, const double t_delta)
    : stats{n}, topLimit{n}, userInfo{t_epsilon, t_delta}, ipAddressInfo {t_epsilon, t_delta}, hourInfo {t_epsilon, t_delta}
{
    topUsers.reserve(n);
    topIpAddresses.reserve(n);
    topHours.reserve(n);
}

void StatHandler::analyzeLine(const std::optional<LineInfo> &lineOpt) {
    if (lineOpt.has_value()) {
        const auto& line = lineOpt.value();

        this->stats.totalRequestCount++;
        this->stats.totalBytesCount += line.byteCount;

        if (const int arrayIndex = line.statusCode / 100; arrayIndex >= 2 and arrayIndex <= 5) {
            this->stats.statusCodeCount[arrayIndex - 2]++;
        }

        const auto& [userHash, userCms] = userInfo.increment(line.userId, line.byteCount);
        updateTopN(topUsers, topLimit, line.userId, userHash, userCms);

        const auto& [ipAddressHash, ipAddressCms] = ipAddressInfo.increment(line.ipAddress, line.byteCount);
        updateTopN(topIpAddresses, topLimit, line.ipAddress, ipAddressHash, ipAddressCms);

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

        const auto& [hourHash, hourCms] = hourInfo.increment(hourDate, line.byteCount);
        updateTopN(topHours, topLimit, hourDate, hourHash, hourCms);
    }
}

const Stats& StatHandler::retrieveStats(const std::size_t n) {
    stats.mostActiveUsers = buildFinalTopN(topUsers, userInfo, n);
    stats.mostActiveIpAddresses = buildFinalTopN(topIpAddresses, ipAddressInfo, n);
    stats.mostActiveHours = buildFinalTopN(topHours, hourInfo, n);

    return stats;
}

void StatHandler::merge(const StatHandler& other)
{
    stats.totalRequestCount += other.stats.totalRequestCount;
    stats.totalBytesCount += other.stats.totalBytesCount;

    for (std::size_t i = 0; i < stats.statusCodeCount.size(); ++i) {
        stats.statusCodeCount[i] += other.stats.statusCodeCount[i];
    }

    userInfo.merge(other.userInfo);
    ipAddressInfo.merge(other.ipAddressInfo);
    hourInfo.merge(other.hourInfo);

    topUsers = toTempHashVector(mergeTopN(buildSortedGroups(topUsers), buildFinalTopN(other.topUsers, other.topLimit)));
    topIpAddresses = toTempHashVector(mergeTopN(buildSortedGroups(topIpAddresses), buildFinalTopN(other.topIpAddresses, other.topLimit)));
    topHours = toTempHashVector(mergeTopN(buildSortedGroups(topHours), buildFinalTopN(other.topHours, other.topLimit)));

    stats.mostActiveUsers = buildFinalTopN(topUsers, userInfo, topLimit);
    stats.mostActiveIpAddresses = buildFinalTopN(topIpAddresses, ipAddressInfo, topLimit);
    stats.mostActiveHours = buildFinalTopN(topHours, hourInfo, topLimit);
}

static bool isBetter(const GroupInfo& lhs, const GroupInfo& rhs) {
    if (lhs.bytesCount != rhs.bytesCount) {
        return lhs.bytesCount > rhs.bytesCount;
    }

    if (lhs.requestCount != rhs.requestCount) {
        return lhs.requestCount > rhs.requestCount;
    }

    return lhs.name < rhs.name;
}

static void updateTopN(std::vector<TempHash>& top, const std::size_t limit, const std::string_view key, const std::size_t hash, const ReqAndBytesCnt& value) {
    const GroupInfo candidate{key, value.requestCount, value.bytesCount};

    if (top.size() < limit) {
        top.push_back({hash, candidate});
        return;
    }

    std::size_t minIndex{};
    for (int i{}; i < top.size(); i++) {
        if (top[i].groupInfo.name == key) {
            top[i].groupInfo = candidate;
            return;
        }

        if (isBetter(top[minIndex].groupInfo, top[i].groupInfo)) {
            minIndex = i;
        }

    }

    if (not isBetter(candidate, top[minIndex].groupInfo)) {
        return;
    }

    top[minIndex] = {hash, candidate};
}

static std::vector<GroupInfo> buildSortedGroups(const std::vector<TempHash>& candidates) {
    std::vector<GroupInfo> result;
    result.reserve(candidates.size());

    for (const auto& candidate : candidates) {
        result.push_back(candidate.groupInfo);
    }

    std::ranges::sort(result, isBetter);

    return result;
}

static std::vector<GroupInfo> buildFinalTopN(const std::vector<TempHash>& candidates, const std::size_t limit) {
    auto result = buildSortedGroups(candidates);

    if (result.size() > limit) {
        result.resize(limit);
    }

    return result;
}

static std::vector<GroupInfo> buildFinalTopN(const std::vector<TempHash>& candidates, const CountMinSketch& cms, const std::size_t limit) {
    std::vector<GroupInfo> result;
    result.reserve(candidates.size());

    for (const auto& candidate : candidates) {
        const auto key = candidate.groupInfo.name;
        const auto existing = std::ranges::find(result, key, &GroupInfo::name);
        if (existing != result.end()) {
            continue;
        }

        const auto value = cms.get(key);
        result.push_back({key, value.requestCount, value.bytesCount});
    }

    std::ranges::sort(result, isBetter);

    if (result.size() > limit) {
        result.resize(limit);
    }

    return result;
}

static std::vector<GroupInfo> mergeTopN(const std::vector<GroupInfo>& vecA, const std::vector<GroupInfo>& vecB) {
    std::vector<GroupInfo> merged;
    merged.reserve(vecA.size() + vecB.size());

    const auto appendOrMerge = [&merged](const GroupInfo& group) {
        const auto iter = std::ranges::find(merged, group.name, &GroupInfo::name);
        if (iter == merged.end()) {
            merged.push_back(group);
            return;
        }

        iter->requestCount += group.requestCount;
        iter->bytesCount += group.bytesCount;
    };

    std::size_t aIndex{};
    std::size_t bIndex{};

    while (aIndex < vecA.size() or bIndex < vecB.size()) {
        if (bIndex == vecB.size() or (aIndex < vecA.size() and isBetter(vecA[aIndex], vecB[bIndex]))) {
            appendOrMerge(vecA[aIndex]);
            ++aIndex;
        } else {
            appendOrMerge(vecB[bIndex]);
            ++bIndex;
        }
    }

    std::ranges::sort(merged, isBetter);

    return merged;
}

static std::vector<TempHash> toTempHashVector(const std::vector<GroupInfo>& groups) {
    std::vector<TempHash> result;
    result.reserve(groups.size());

    for (const auto& group : groups) {
        result.push_back({0, group});
    }

    return result;
}
