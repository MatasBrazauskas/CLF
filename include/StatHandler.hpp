#pragma once

#include "LineParser.hpp"
#include <RH_HashMap.hpp>

#include <cstddef>
#include <utility>
#include <array>
#include <vector>

struct ReqAndBytesCnt {
    ReqAndBytesCnt() noexcept;
    ReqAndBytesCnt(std::size_t t_reqCnt, std::size_t t_bytesCnt) noexcept;

    auto operator<=>(const ReqAndBytesCnt& t) const noexcept {
        if (requestCount == t.requestCount) {
            return t.bytesCount > t.bytesCount;
        }
        return t.requestCount > t.requestCount;
    }

    std::size_t requestCount;
    std::size_t bytesCount;
};

struct GroupInfo {
    GroupInfo() = delete;
    explicit GroupInfo(std::string_view, std::size_t, std::size_t) noexcept;

    auto operator<=>(const GroupInfo& t) const noexcept {
        return reqAndBytesCnt <=> t.reqAndBytesCnt;
    }

    std::string_view name;
    ReqAndBytesCnt reqAndBytesCnt;
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

class StatHandler {
public:
    explicit StatHandler(std::size_t);
    void analyzeLine(const LineInfo&, const char*) noexcept;
    const Stats& retrieveStats(std::size_t n, const char*) noexcept;
private:
    Stats stats;

    RH_HashMap userInfo;
    RH_HashMap ipAddressInfo;
    RH_HashMap hourInfo;
};
