#include <algorithm>

#include "InputHandler.hpp"
#include "StatHandler.hpp"
#include "FileHandler.hpp"
#include "LineParser.hpp"

#include <string>
#include <thread>
#include <print>
#include <unordered_map>

using namespace std::string_literals;

constexpr auto CThreadCount{16zu};
std::array<Stats, CThreadCount> stats;

Simd fileSimd{};

void workerFunction(StatHandler& t_statHandler, const FileHandler& t_fileHandler, Simd& t_simd, const std::size_t t_index) {
    const auto chunkStartAddress = t_fileHandler.getChunkStartAddress(t_index);
    for (const std::string_view line : t_fileHandler.getLine(t_index)) {
        const auto lineInfo = parseLine(t_simd, line);

        if (lineInfo.has_value()) {
            const auto lineValue = lineInfo.value();
            t_statHandler.analyzeLine(lineValue, chunkStartAddress);
        }
    }

    stats[t_index] = t_statHandler.retrieveStats(40, chunkStartAddress);
};

static bool sortingPredicate(const GroupInfo& a, const GroupInfo& b) {
    if (a.requestCount == b.requestCount) {
        return a.bytesCount > b.bytesCount;
    }
    return a.requestCount > b.requestCount;
};

void mergeVectors(std::unordered_map<std::string_view, GroupInfo>& t_map, const std::vector<GroupInfo>& t_result) {
    for (const auto& entry : t_result) {
        const auto& userIt = t_map.try_emplace(entry.name, GroupInfo{entry.name, 0, 0}).first;
        userIt->second.requestCount += entry.requestCount;
        userIt->second.bytesCount += entry.bytesCount;
    }
}

std::vector<GroupInfo> topN(const std::unordered_map<std::string_view, GroupInfo>& t_map) {
    std::vector<GroupInfo> result;
    result.reserve(5);

    std::size_t count{};
    for (const auto &value: t_map | std::views::values) {

        if (count < 5) {
            result.emplace_back(value);
            ++count;
        } else {
            if (auto& lastVectorElement = result.back(); lastVectorElement.requestCount < value.requestCount) {
                lastVectorElement = value;
                std::ranges::sort(result, sortingPredicate);
            }
        }
    }

    return result;
}

int main(const int argc, char** argv) {
    const auto inputsOpt = validateInputs(argc, argv);
    if (not inputsOpt.has_value()) {
        return -1;
    }

    const auto inputs = inputsOpt.value();

    std::array<Simd, CThreadCount> simd{};

    std::vector<StatHandler> statHandlers;
    statHandlers.reserve(CThreadCount);
    statHandlers.emplace_back(inputs.reserveCount * CThreadCount, true);
    for (auto i{1}; i < CThreadCount; ++i) {
        statHandlers.emplace_back(static_cast<std::size_t>(inputs.reserveCount));
    }

    FileHandler fileHandler{fileSimd, inputs.fileName, CThreadCount};

    std::vector<std::thread> workerThreads;
    workerThreads.reserve(CThreadCount);
    for (auto i{0}; i < CThreadCount; ++i) {
        workerThreads.emplace_back(workerFunction, std::ref(statHandlers[i]), std::ref(fileHandler) , std::ref(simd[i]), i);
    }

    for (auto& thread : workerThreads) {
        thread.join();
    }

    std::unordered_map<std::string_view, GroupInfo> userInfo;
    for (auto i{0}; i < CThreadCount; ++i) {
        mergeVectors(userInfo, stats[i].mostActiveUsers);
    }

    std::unordered_map<std::string_view, GroupInfo> ipAddressesInfo;
    for (auto i{0}; i < CThreadCount; ++i) {
        mergeVectors(ipAddressesInfo, stats[i].mostActiveIpAddresses);
    }

    std::unordered_map<std::string_view, GroupInfo> hourInfo;
    for (auto i{0}; i < CThreadCount; ++i) {
        mergeVectors(hourInfo, stats[i].mostActiveHours);
    }

    for (auto i{1}; i < CThreadCount; ++i) {
        stats[0].totalBytesCount += stats[i].totalBytesCount;
        stats[0].totalRequestCount += stats[i].totalRequestCount;

        for (auto j{0}; j < 4; j++) {
            stats[0].statusCodeCount[j] = stats[i].statusCodeCount[j];
        }
    }

    stats[0].mostActiveUsers = topN(userInfo);
    stats[0].mostActiveIpAddresses = topN(ipAddressesInfo);
    stats[0].mostActiveHours = topN(hourInfo);

    printInfo(stats[0]);
}