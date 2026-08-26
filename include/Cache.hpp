#pragma once

#include <HashMap.h>

#include <cstddef>
#include <string_view>

struct ReqAndBytesCnt {
    std::size_t requestCount;
    std::size_t bytesCount;

    ReqAndBytesCnt() : requestCount{}, bytesCount{} {}
    ReqAndBytesCnt(const std::size_t t_reqCnt, const std::size_t t_bytesCnt) : requestCount{t_reqCnt}, bytesCount{t_bytesCnt} {}
};

class Cache {
public:
    Cache() = delete;
    explicit Cache(std::size_t, std::size_t);
    void try_emplace(std::string_view, std::size_t);

    const rigtorp::HashMap<std::string_view, ReqAndBytesCnt>& cachedEntries() const;
private:
    void tryPromote(rigtorp::HashMap<std::string_view, ReqAndBytesCnt>::iterator);

    rigtorp::HashMap<std::string_view, ReqAndBytesCnt> map_;
    rigtorp::HashMap<std::string_view, ReqAndBytesCnt> cache_;

    std::size_t cacheSize_;
};
