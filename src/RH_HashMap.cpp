#include <RH_HashMap.hpp>

#include <bit>

KeyValue::KeyValue(): keyData(nullptr), val1{}, val2{}, keyLength{}, hashValue{} {}

KeyValue::KeyValue(const std::string_view t_key, const std::size_t t_val1, const std::size_t t_val2, const std::uint32_t t_hash)
    : keyData{t_key.begin()}, val1{t_val1}, val2{t_val2}, keyLength{static_cast<std::uint32_t>(t_key.size())}, hashValue{t_hash} {}

RH_HashMap::RH_HashMap(const std::size_t t_reserveSize): buckets_{std::bit_ceil(t_reserveSize)}, hashFunction_{} {}

inline std::uint32_t RH_HashMap::modulo(const std::uint32_t t_hash) const noexcept {
    return t_hash & (buckets_.size() - 1);
}

bool RH_HashMap::append(const std::string_view t_key, const std::size_t t_val1, const std::size_t t_val2) {
    const auto hash = static_cast<std::uint32_t>(hashFunction_(t_key));
    auto idx = modulo(hash);

    for (std::size_t probes = 0; probes < buckets_.size(); ++probes) {
        if (buckets_[idx].keyLength == 0) {
            buckets_[idx] = KeyValue(t_key, t_val1, t_val2, hash);
            return true;
        }
        if (buckets_[idx].hashValue == hash) {
            buckets_[idx].val1 += t_val1;
            buckets_[idx].val2 += t_val2;
            return true;
        }
        idx = modulo(idx + 1);
    }

    return false;
}

const KeyValue* RH_HashMap::lookUp(const std::string_view t_key) const {
    const auto hash = static_cast<std::uint32_t>(hashFunction_(t_key));
    auto idx = modulo(hash);

    for (std::size_t probes = 0; probes < buckets_.size(); ++probes) {
        if (buckets_[idx].keyLength == 0) {
            return nullptr;
        }
        if (buckets_[idx].hashValue == hash) {
            return &buckets_[idx];
        }
        idx = modulo(idx + 1);
    }

    return nullptr;
}