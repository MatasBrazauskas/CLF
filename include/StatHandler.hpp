#pragma once

#include "LineParser.hpp"

#include <cstddef>
#include <utility>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <array>

using ReqAndBytesCnt = std::pair<int, int>;
using GroupInfo = std::tuple<std::string_view, int, int>;

struct StringViewHash {
    std::size_t operator()(const std::string_view& strView) const {
        /*std::size_t hash = strView.length();
        for (const char c : strView) {
            hash = (hash ^ static_cast<unsigned char>(c)) * 0xbf58476d1ce4e5b9ULL;
            hash = (hash ^ (hash >> 30)) * 0x94d049bb133111ebULL;
        }
        return hash ^ (hash >> 31);*/

        std::size_t hash{5381};
        for(const auto & c : strView)
        {
            hash = (33*hash) ^ c;
        }
        return hash;
    }
};

struct Stats {
    Stats();

    std::size_t totalRequestCount;
    std::size_t totalBytesCount;
    std::array<std::size_t, 4> statusCodeCount;
    std::vector<GroupInfo> mostActiveUsers;
    std::vector<GroupInfo> mostActiveIpAddresses;
};

class StatHandler {
public:
    StatHandler();
    void analyzeLine(const std::optional<LineInfo> &);
    const Stats& retrieveStats(std::size_t n);
private:
    Stats stats;

    std::unordered_map<std::string_view, ReqAndBytesCnt, StringViewHash> userInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt, StringViewHash> ipAddressInfo;

    std::vector<GroupInfo> userWithMostRequest(std::size_t n) const;
    std::vector<GroupInfo> ipAddressesWithMostRequest(std::size_t n) const;
};
