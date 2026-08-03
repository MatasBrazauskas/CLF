#include <iostream>

#include "FileHandler.hpp"
#include "LineParser.hpp"

#include <string>

#include "StatHandler.hpp"

using namespace std::string_literals;

constexpr auto filePath = "data/log100k";
constexpr auto n = 5;

int main() {
    StatHandler statHandler{};
    FileHandler fileHandler{filePath};

    for(const std::string line : fileHandler.getLine()) {
        const auto lineInfo = parseLine(line);
        statHandler.analyzeLine(lineInfo);
    }

    std::cout << "Error parsing lines: " << statHandler.errorRate() << '%' << std::endl;

    const auto userWithMostReq = statHandler.userWithMostRequest(n);
    std::cout << "Users with most bytes transferred: " << std::endl;

    for (const auto& user : userWithMostReq) {
        std::cout << "Username: " << get<0>(user) << ", requests: " << get<1>(user) << ", bytes transferred: " << get<2>(user)  << "." << std::endl;
    }

    const auto ipAddressWithMostReq = statHandler.ipAddressesWithMostRequest(n);
    std::cout << "IP addresses with most bytes transferred: " << std::endl;

    for (const auto& ip: ipAddressWithMostReq) {
        std::cout << "IP address: " << get<0>(ip) << ", requests: " << get<1>(ip) << ", bytes transferred: " << get<2>(ip)  << "." << std::endl;
    }
}