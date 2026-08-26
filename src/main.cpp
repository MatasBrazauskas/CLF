#include "InputHandler.hpp"
#include "StatHandler.hpp"
#include "FileHandler.hpp"
#include "LineParser.hpp"

#include <string>
#include <print>
#include <thread>

using namespace std::string_literals;

int main(const int argc, char** argv) {
    const auto inputsOpt = validateInputs(argc, argv);
    if (not inputsOpt.has_value()) {
        return -1;
    }

    const auto inputs = inputsOpt.value();
    const std::size_t threadCount = std::thread::hardware_concurrency();

    FileHandler fileHandler{inputs.fileName, threadCount};
    std::vector<Simd> simd{threadCount};

    std::vector<StatHandler> statHandlers;
    statHandlers.reserve(threadCount);

    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    const auto fn = [&fileHandler, &simd](StatHandler& stats, const long index) {
        for(const std::string_view line : fileHandler.getLine(index)) {
            const auto lineInfo = parseLine(simd[index], line);
            stats.analyzeLine(lineInfo);
        }
    };

    for (std::size_t i{}; i < threadCount; ++i) {
        statHandlers.emplace_back(static_cast<std::size_t>(inputs.reserveCount));
        threads.emplace_back(fn, std::ref(statHandlers[i]), i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    const auto stats = statHandlers[0].retrieveStats(inputs.n);

    std::println("Total requests: {}.", stats.totalRequestCount);
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
    std::println("Most packed hours:");
    for (const auto& [index, hour] : std::ranges::views::enumerate(stats.mostActiveHours)) {
        std::println("{}. Date: {}, requests: {}, bytes transferred: {}.", index + 1, hour.name, hour.requestCount, hour.bytesCount);
    }
}