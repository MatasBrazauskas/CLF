#include <RH_HashMap.hpp>

#include <bit>

KeyValue::KeyValue(): keyData(nullptr), requestCnt{}, bytesCnt{}, keyLength{}, hashValue{} {}

KeyValue::KeyValue(const std::string_view t_key, const std::size_t t_val1, const std::size_t t_val2, const std::uint32_t t_hash)
    : keyData{t_key.begin()}, requestCnt{t_val1}, bytesCnt{t_val2}, keyLength{static_cast<std::uint32_t>(t_key.size())}, hashValue{t_hash} {}

RH_HashMap::RH_HashMap(const std::size_t t_reserveSize): buckets_{std::bit_ceil(t_reserveSize)}, hashFunction_{} {}

inline std::uint32_t RH_HashMap::modulo(const std::uint32_t t_hash) const noexcept {
    return t_hash & (buckets_.size() - 1);
}


void RH_HashMap::tryPromote(const std::size_t t_index, const std::size_t t_probeDistance) {
    if (t_probeDistance == 0) {
        return;
    }

    auto& curr = buckets_[t_index];
    auto& prev = buckets_[t_index - 1];

    if (curr.requestCnt > prev.requestCnt) {
        std::swap(prev, curr);
    }
}

bool RH_HashMap::append(const std::string_view t_key, const std::size_t t_val1, const std::size_t t_val2) {
    const auto hash = static_cast<std::uint32_t>(hashFunction_(t_key));
    auto idx = modulo(hash);

    for (std::size_t probes{}; probes < buckets_.size(); ++probes) {
        if (buckets_[idx].keyLength == 0) {
            buckets_[idx] = KeyValue(t_key, t_val1, t_val2, hash);
            return true;
        }

        if (buckets_[idx].hashValue == hash) {
            buckets_[idx].requestCnt += t_val1;
            buckets_[idx].bytesCnt += t_val2;
            tryPromote(idx, probes);
            return true;
        }

        idx = modulo(idx + 1);
    }

    return false;
}

const KeyValue* RH_HashMap::lookUp(const std::string_view t_key) const {
    const auto hash = static_cast<std::uint32_t>(hashFunction_(t_key));
    auto idx = modulo(hash);

    for (std::size_t probes{}; probes < buckets_.size(); ++probes) {
        if (buckets_[idx].keyData == nullptr) {
            return nullptr;
        }

        if (buckets_[idx].hashValue == hash) {
            return &buckets_[idx];
        }

        idx = modulo(idx + 1);
    }

    return nullptr;
}