#include <RH_HashMap.hpp>

#include <bit>

RH_HashMap::RH_HashMap(const std::size_t t_reserveSize): buckets_{std::bit_ceil(t_reserveSize)}, hashFunction_{} {}

inline std::uint32_t RH_HashMap::modulo(const std::uint32_t t_hash) const noexcept {
    return t_hash & (buckets_.size() - 1);
}

bool RH_HashMap::append(const std::string_view t_key, const char* t_mmapAddress, const std::size_t t_bytesCnt) {
    const auto hash = static_cast<std::uint32_t>(hashFunction_(t_key));
    const auto i= modulo(hash); // Keep initial index fixed

    for (std::size_t probe{}; probe < buckets_.size(); ++probe) {
        const auto idx = modulo(i + probe * probe);

        if (buckets_[idx].comp.keyLength == 0) {
            buckets_[idx] = KeyValue{t_mmapAddress, t_key.data(), hash, static_cast<std::uint16_t>(t_key.length()), 1, static_cast<std::uint32_t>(t_bytesCnt)};
            return true;
        }

        if (buckets_[idx].hashValue == hash) {
            buckets_[idx].comp.requestCnt++;
            buckets_[idx].bytesCnt += t_bytesCnt;
            return true;
        }
    }

    return false;
}

const KeyValue* RH_HashMap::lookUp(const std::string_view t_key) const {
    const auto hash = static_cast<std::uint32_t>(hashFunction_(t_key));
    const auto i = modulo(hash);

    for (std::size_t probes{}; probes < buckets_.size(); ++probes) {
        const auto idx = modulo(i + probes * probes);
        if (buckets_[idx].comp.keyLength == 0) {
            return nullptr;
        }

        if (buckets_[idx].hashValue == hash) {
            return &buckets_[idx];
        }
    }

    return nullptr;
}