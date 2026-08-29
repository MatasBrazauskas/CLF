#include "InputHandler.hpp"
#include "StatHandler.hpp"
#include "FileHandler.hpp"
#include "LineParser.hpp"

#include <print>
#include <thread>
#include <vector>
#include <ranges>
#include <unordered_map>

Stats mergeStatsParallel(const std::vector<Stats*>& t_stats, const std::size_t t_n) {
    Stats s{};

    const auto sortingPredicate = [](const GroupInfo& a, const GroupInfo& b) {
        return a.requestCount > b.requestCount;
    };

    const auto mergeCategory = [t_n, sortingPredicate](
        const std::vector<Stats*>& stats,
        auto Stats::* memberPtr
    ) -> std::vector<GroupInfo> {
        std::unordered_map<std::string_view, GroupInfo> map;
        map.reserve(t_n * 2 * stats.size());

        for (const auto* stat : stats) {
            for (const auto& entry : stat->*memberPtr) {
                auto [it, inserted] = map.try_emplace(entry.name, entry);
                if (!inserted) {
                    it->second.requestCount += entry.requestCount;
                    it->second.bytesCount += entry.bytesCount;
                }
            }
        }

        const std::size_t outputSize = std::min(t_n, map.size());
        std::vector<GroupInfo> result(outputSize);

        auto valuesView = map | std::views::values;
        std::partial_sort_copy(
            valuesView.begin(), valuesView.end(),
            result.begin(), result.end(),
            sortingPredicate
        );

        return result;
    };

    std::thread t1([&] { s.mostActiveUsers = mergeCategory(t_stats, &Stats::mostActiveUsers); });
    std::thread t2([&] { s.mostActiveIpAddresses = mergeCategory(t_stats, &Stats::mostActiveIpAddresses); });
    std::thread t3([&] { s.mostActiveHours = mergeCategory(t_stats, &Stats::mostActiveHours); });
    std::thread t4([&] {
        for (const auto* stat : t_stats) {
            s.totalRequestCount += stat->totalRequestCount;
            s.totalBytesCount += stat->totalBytesCount;

            for (std::size_t i = 0; i < stat->statusCodeCount.size(); ++i) {
                s.statusCodeCount[i] += stat->statusCodeCount[i];
            }
        }
    });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return s;
}

int main(const int argc, char** argv) {
    const auto inputsOpt = validateInputs(argc, argv);
    if (not inputsOpt.has_value()) {
        return -1;
    }

    const auto inputs = inputsOpt.value();
    constexpr std::size_t threadCount = 16;

    std::vector<Simd> simd{threadCount};
    FileHandler fileHandler{inputs.fileName, threadCount, simd[0]};

    std::vector<StatHandler> statHandlers;
    statHandlers.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i) {
        statHandlers.emplace_back(static_cast<std::size_t>(inputs.reserveCount));
    }

    std::vector<std::thread> workerThreads;
    workerThreads.reserve(threadCount);

    std::vector<Stats*> stats(threadCount);

    const auto fn = [&fileHandler, &simd, &stats](StatHandler& stat, const std::size_t index, const std::size_t n) {
        for (const std::string_view line : fileHandler.getLine(index)) {
            const auto lineInfo = parseLine(simd[index], line);
            stat.analyzeLine(lineInfo);
        }
        stats[index] = &stat.retrieveStats(n * 4);
    };

    for (std::size_t i = 0; i < threadCount; ++i) {
        workerThreads.emplace_back(fn, std::ref(statHandlers[i]), i, inputs.n);
    }

    for (auto& thread : workerThreads) {
        thread.join();
    }

    mergeStatsParallel(stats, inputs.n);

    const Stats& s = *stats.front();

    std::println("Total requests: {}.", s.totalRequestCount);
    std::println("Total bytes transferred: {}.", s.totalBytesCount);

    std::println();
    std::println("Status code statistics:");
    for (const auto& [index, statusCodeCnt] : std::ranges::views::enumerate(s.statusCodeCount)) {
        std::println("{}. Status {}xx, {} responses", index + 1, index + 2, statusCodeCnt);
    }

    std::println();
    std::println("Users with most bytes transferred:");
    for (const auto& [index, user] : std::ranges::views::enumerate(s.mostActiveUsers)) {
        std::println("{}. Username: {}, requests: {}, bytes transferred: {}.", index + 1, user.name, user.requestCount, user.bytesCount);
    }

    std::println();
    std::println("IP addresses with most bytes transferred:");
    for (const auto& [index, ip] : std::ranges::views::enumerate(s.mostActiveIpAddresses)) {
        std::println("{}. IP address: {}, requests: {}, bytes transferred: {}.", index + 1, ip.name, ip.requestCount, ip.bytesCount);
    }

    std::println();
    std::println("Most packed hours:");
    for (const auto& [index, hour] : std::ranges::views::enumerate(s.mostActiveHours)) {
        std::println("{}. Date: {}, requests: {}, bytes transferred: {}.", index + 1, hour.name, hour.requestCount, hour.bytesCount);
    }
}
