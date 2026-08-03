#include <iostream>

#include "FileHandler.hpp"
#include "LineParser.hpp"

#include <string>

#include "StatHandler.hpp"

using namespace std::string_literals;

constexpr auto filePath = "data/log1M";
constexpr auto n = 5;

int main() {
    StatHandler statHandler{};
    FileHandler fileHandler{filePath};

    for(const std::string_view line : fileHandler.getLine()) {
        const auto lineInfo = parseLine(line);
        statHandler.analyzeLine(lineInfo);
    }

    const auto stats = statHandler.retrieveStats(n);

    std::cout << "Total requests: " << stats.totalRequestCount << std::endl << std::endl;
    std::cout << "Total bytes transferred: " << stats.totalBytesCount << std::endl << std::endl;

    std::cout << "Status code statistics:" << std::endl;
    for (const auto& [index, statusCodeCnt] : std::ranges::views::enumerate(stats.statusCodeCount)) {
        std::cout << "Status " << index + 2 << "xx: " << statusCodeCnt << " responses" << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Users with most bytes transferred: " << std::endl;
    for (const auto& user : stats.mostActiveUsers) {
        std::cout << "Username: " << get<0>(user) << ", requests: " << get<1>(user) << ", bytes transferred: " << get<2>(user)  << "." << std::endl;
    }

    std::cout << std::endl;

    std::cout << "IP addresses with most bytes transferred: " << std::endl;
    for (const auto& ip: stats.mostActiveIpAddresses) {
        std::cout << "IP address: " << get<0>(ip) << ", requests: " << get<1>(ip) << ", bytes transferred: " << get<2>(ip)  << "." << std::endl;
    }
}