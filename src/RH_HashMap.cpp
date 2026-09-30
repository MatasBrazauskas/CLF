#include <RH_HashMap.hpp>

#include <bit>

RH_HashMap::RH_HashMap(const std::size_t t_reserveSize): buckets_{std::bit_ceil(t_reserveSize)}, hashFunction_{} {}

inline std::uint32_t RH_HashMap::modulo(const std::uint32_t t_hash) const noexcept {
    return t_hash & (buckets_.size() - 1);
}

bool RH_HashMap::append(const std::string_view t_key, const char* t_mmapAddress, const std::size_t t_bytesCnt) {
    const auto hash = static_cast<std::uint32_t>(hashFunction_(t_key));
    auto idx = modulo(hash);

    for (std::size_t probes{}; probes < buckets_.size(); ++probes) {
        if (buckets_[idx].comp.keyLength == 0) {
            buckets_[idx] = KeyValue{t_mmapAddress, t_key.data(), hash, static_cast<std::uint16_t>(t_key.length()), 1, static_cast<std::uint32_t>(t_bytesCnt)};
            return true;
        }

        if (buckets_[idx].hashValue == hash) {
            buckets_[idx].comp.requestCnt++;
            buckets_[idx].bytesCnt += t_bytesCnt;
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
        if (buckets_[idx].comp.keyLength == 0) {
            return nullptr;
        }

        if (buckets_[idx].hashValue == hash) {
            return &buckets_[idx];
        }

        idx = modulo(idx + 1);
    }

    return nullptr;
}

void RH_HashMap::merge(const RH_HashMap& t_map, const char* t_mmapAddress) {
    for (const auto& value : t_map.buckets_) {
        const auto [t_offset, t_hashValue, t_comp, t_bytesCnt] = value;
        const auto [t_keyLength, t_requestCnt] = t_comp;

        if (t_comp.keyLength != 0) {
            const auto key = std::string_view{t_mmapAddress + t_offset, t_keyLength};
            this->append(key, t_mmapAddress, t_bytesCnt);
        }
    }
}
