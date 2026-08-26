#include "Cache.hpp"

Cache::Cache(const std::size_t t_reserveMap, const std::size_t t_reserveCache)
    : map_{t_reserveMap, ""}, cache_{t_reserveCache, ""}, cacheSize_{t_reserveCache} {
}

void Cache::try_emplace(const std::string_view t_key, const std::size_t t_bytes) {
    if (auto it = cache_.find(t_key); it != cache_.end()) {
        ++it->second.requestCount;
        it->second.bytesCount += t_bytes;
        return;
    }

    if (auto it = map_.find(t_key); it != map_.end()) {
        ++it->second.requestCount;
        it->second.bytesCount += t_bytes;

        tryPromote(it);
        return;
    }

    if (cache_.size() < cacheSize_) {
        cache_.emplace(t_key, ReqAndBytesCnt{1, t_bytes});
    } else {
        map_.emplace(t_key, ReqAndBytesCnt{1, t_bytes});
    }
}

void Cache::tryPromote(const rigtorp::HashMap<std::string_view, ReqAndBytesCnt>::iterator candidate) {
    const auto coldest = std::min_element(
        cache_.begin(),
        cache_.end(),
        [](const auto& a, const auto& b) {
            return a.second.requestCount < b.second.requestCount;
        }
    );

    if (candidate->second.requestCount <= coldest->second.requestCount) {
        return;
    }

    auto hotKey   = candidate->first;
    auto hotValue = candidate->second;

    auto coldKey   = coldest->first;
    auto coldValue = coldest->second;

    map_.erase(candidate);
    cache_.erase(coldest);

    cache_.emplace(hotKey, hotValue);
    map_.emplace(coldKey, coldValue);
}

const rigtorp::HashMap<std::string_view, ReqAndBytesCnt>& Cache::cachedEntries() const {
    return cache_;
}
