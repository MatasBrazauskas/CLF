#pragma once

#include "LineParser.hpp"
#include <HashMap.h>

#include <cstddef>
#include <utility>
#include <string>
#include <unordered_map>
#include <array>
#include <vector>

#define XXH_INLINE_ALL
#include "xxhash.h"

struct ReqAndBytesCnt {
    std::size_t requestCount;
    std::size_t bytesCount;

    ReqAndBytesCnt(): requestCount{}, bytesCount{} {};
    ReqAndBytesCnt(const std::size_t t_reqCnt, const std::size_t t_bytesCnt): requestCount{t_reqCnt}, bytesCount{t_bytesCnt} {}
};

struct GroupInfo {
    std::string_view name;
    std::size_t requestCount;
    std::size_t bytesCount;
};

struct Stats {
    Stats();

    std::vector<GroupInfo> mostActiveUsers;
    std::vector<GroupInfo> mostActiveIpAddresses;
    std::vector<GroupInfo> mostActiveHours;
    std::array<std::size_t, 4> statusCodeCount;
    std::size_t totalRequestCount;
    std::size_t totalBytesCount;
};

/*struct StringViewHash {
    std::size_t operator()(const std::string_view t_value) const noexcept {
        return XXH64(t_value.data(), t_value.size(), 0);
    }
};*/

class StatHandler {
public:
    StatHandler(std::size_t);
    void analyzeLine(const std::optional<LineInfo> &);
    const Stats& retrieveStats(std::size_t n);
private:
    Stats stats;

    rigtorp::HashMap<std::string_view, ReqAndBytesCnt/*, StringViewHash*/> userInfo;
    rigtorp::HashMap<std::string_view, ReqAndBytesCnt/*, StringViewHash*/> ipAddressInfo;
    rigtorp::HashMap<std::string_view, ReqAndBytesCnt/*, StringViewHash*/> hourInfo;
};
