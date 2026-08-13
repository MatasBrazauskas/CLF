#include <algorithm>
#include <functional>
#include <iostream>
#include <ostream>
#include <print>
#include <ranges>
#include <thread>
#include <vector>

#include "FileHandler.hpp"
#include "LineParser.hpp"
#include "InputHandler.hpp"
#include "StatHandler.hpp"

int main(const int argc, char** argv) {
    const uint threadCount = std::max(1u, std::thread::hardware_concurrency());

    const auto inputsOpt = validateInputs(argc, argv);
    if (not inputsOpt.has_value()) {
        return -1;
    }

    const auto inputs = inputsOpt.value();
    const auto topCount = static_cast<std::size_t>(inputs.n);
    const auto candidateCount = topCount * 4;

    std::vector<StatHandler> statHandlers;
    statHandlers.reserve(threadCount);

    for (std::size_t i = 0; i < threadCount; ++i) {
        statHandlers.emplace_back(candidateCount, inputs.epsilon, inputs.delta);
    }

    FileHandler fileHandler{inputs.fileName, threadCount};


    const auto fn = [&fileHandler](StatHandler& stats, const long index) {
        for(const std::string_view line : fileHandler.getLine(index)) {
            const auto lineInfo = parseLine(line);
            stats.analyzeLine(lineInfo);
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    for (auto index : std::views::iota(0u, threadCount)) {
        threads.emplace_back(fn, std::ref(statHandlers[index]), index);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (auto& handler : statHandlers | std::views::drop(1)) {
        statHandlers[0].merge(handler);
    }

    const Stats& stats = statHandlers[0].retrieveStats(topCount);

    std::println("Total requests: {}.", stats.totalRequestCount);

    std::println();

    std::println("Total bytes transferred: {}.", stats.totalBytesCount);

    std::println();
    std::println("Status code statistics:");
    for (const auto& [index, statusCodeCnt] : std::ranges::views::enumerate(stats.statusCodeCount)) {
        std::println("{}. Status {}xx, {} responses", index + 1, index + 2, statusCodeCnt);
    }

    std::println();
    std::println("Users with most bytes transferred:");
    for (const auto& [index, user] : std::ranges::views::enumerate(stats.mostActiveUsers)) {
        std::println("{}. Username: {}, requests: {}, bytes transferred: {}.", index + 1, user.name, user.requestCount, user.bytesCount);
    }

    std::println();
    std::println("IP addresses with most bytes transferred:");
    for (const auto& [index, ip]: std::ranges::views::enumerate(stats.mostActiveIpAddresses)) {
        std::println("{}. IP address: {}, requests: {}, bytes transferred: {}.", index + 1, ip.name, ip.requestCount, ip.bytesCount);
    }

    std::println();
    std::println("\nMost packed hours:");
    for (const auto& [index, hour] : std::ranges::views::enumerate(stats.mostActiveHours)) {
        std::println("{}. Date: {}, requests: {}, bytes transferred: {}.", index + 1, hour.name, hour.requestCount, hour.bytesCount);
    }
}
