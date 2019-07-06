#ifndef __FNV_HPP_
#define __FNV_HPP_

#include <cstdint>

class FNV {
    private:
        static constexpr std::uint64_t OFFSET_BASIS = 0xCBF29CE484222325;
        static constexpr std::uint64_t PRIME = 0x100000001B3;

    public:
        static std::uint64_t hash(const unsigned char *const data, const std::uint64_t &size);
};

#endif // __FNV_HPP