#include <algorithm>

#include "InputHandler.hpp"
#include "StatHandler.hpp"
#include "FileHandler.hpp"
#include "LineParser.hpp"

#include <string>
#include <thread>
#include <print>

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

void mergeMultipleVector(std::vector<GroupInfo>& t_result, const std::vector<GroupInfo>& t_mergeVector) {
    for (const auto& entry : t_mergeVector) {
        if (auto& lastVectorElement = t_result.back(); lastVectorElement.requestCount < entry.requestCount) {
            lastVectorElement = entry;
            std::ranges::sort(t_result, sortingPredicate);
        }
    }
}


int main(const int argc, char** argv) {
    std::vector<std::thread> firstLine;
    firstLine.reserve(8);

    std::vector<std::thread> secondLine;
    secondLine.reserve(4);

    std::vector<std::thread> thirdLine;
    thirdLine.reserve(2);

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

    std::ranges::sort(stats[0].mostActiveUsers, sortingPredicate);
    stats[0].mostActiveUsers.resize(5);

    for (auto i{1}; i < CThreadCount; ++i) {
        mergeMultipleVector(stats[0].mostActiveUsers, stats[i].mostActiveUsers);
    }

    std::ranges::sort(stats[0].mostActiveIpAddresses, sortingPredicate);
    stats[0].mostActiveIpAddresses.resize(5);

    for (auto i{1}; i < CThreadCount; ++i) {
        mergeMultipleVector(stats[0].mostActiveIpAddresses, stats[i].mostActiveIpAddresses);
    }

    std::ranges::sort(stats[0].mostActiveHours, sortingPredicate);
    stats[0].mostActiveHours.resize(5);

    for (auto i{1}; i < CThreadCount; ++i) {
        mergeMultipleVector(stats[0].mostActiveHours, stats[i].mostActiveHours);
    }

    printInfo(stats[0]);
}