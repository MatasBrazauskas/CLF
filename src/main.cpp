#include <print>

#include "FileHandler.hpp"
#include "LineParser.hpp"

#include <string>

#include "InputHandler.hpp"
#include "StatHandler.hpp"

using namespace std::string_literals;

int main(const int argc, char** argv) {
    const auto inputsOpt = validateInputs(argc, argv);
    if (not inputsOpt.has_value()) {
        return -1;
    }

    const auto inputs = inputsOpt.value();

    StatHandler statHandler{static_cast<std::size_t>(inputs.reserveCount)};
    FileHandler fileHandler{inputs.fileName};
    Simd simd;

    for(const std::string_view line : fileHandler.getLine()) {
        const auto lineInfo = parseLine(simd, line);
        statHandler.analyzeLine(lineInfo);
    }

    const auto stats = statHandler.retrieveStats(inputs.n);

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