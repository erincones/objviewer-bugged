#include "fnv.hpp"

// Fowler�Noll�Vo hash
std::uint64_t FNV::hash(const unsigned char *const data, const std::uint64_t &size) {
    std::int64_t ans = FNV::OFFSET_BASIS;

    for (std::uint64_t i = 0; i < size; i++) {
        ans ^= data[i];
        ans *= FNV::PRIME;
    }

    return ans;
}