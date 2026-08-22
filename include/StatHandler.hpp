#pragma once

#include "LineParser.hpp"

#include <cstddef>
#include <utility>
#include <string>
#include <unordered_map>
#include <array>
#include <vector>

struct ReqAndBytesCnt {
    std::size_t requestCount;
    std::size_t bytesCount;

    ReqAndBytesCnt() = delete;
    ReqAndBytesCnt(const std::size_t t_reqCnt, const std::size_t t_bytesCnt): requestCount{t_reqCnt}, bytesCount{t_bytesCnt} {}
};

struct GroupInfo {
    std::string_view name;
    std::size_t requestCount;
    std::size_t bytesCount;
};

struct Stats {
    Stats();

    std::size_t totalRequestCount;
    std::size_t totalBytesCount;
    std::array<std::size_t, 4> statusCodeCount;
    std::vector<GroupInfo> mostActiveUsers;
    std::vector<GroupInfo> mostActiveIpAddresses;
    std::vector<GroupInfo> mostActiveHours;
};

/*struct StringViewHash {
    std::size_t operator()(const std::string_view t_value) const noexcept {
        std::size_t hash{14695981039346656037ULL};

        for (const auto byte : t_value) {
            hash ^= static_cast<unsigned char>(byte);
            hash *= 1099511628211ULL;
        }

        return hash;
    }
};*/

class StatHandler {
public:
    StatHandler(std::size_t);
    void analyzeLine(const std::optional<LineInfo> &);
    const Stats& retrieveStats(std::size_t n);
private:
    Stats stats;

    std::unordered_map<std::string_view, ReqAndBytesCnt> userInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt> ipAddressInfo;
    std::unordered_map<std::string_view, ReqAndBytesCnt> hourInfo;
};
