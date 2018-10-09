#include <stdexcept>
#include <PrngXorshift128Plus.h>

namespace dsaext
{
    // @throws std::invalid_argument, std::bad_alloc
    PrngXorshift128Plus::PrngXorshift128Plus(const struct PrngXorshift128Plus::state init_seed)
    {
        if (init_seed.high_bits != 0 && init_seed.low_bits != 0)
        {
            seed = {init_seed.high_bits, init_seed.low_bits};
        }
        else
        {
            throw std::invalid_argument(
                "Class PrngXorshift128Plus constructor called with "
                "an invalid init_seed value of 0"
            );
        }
    }

    PrngXorshift128Plus::~PrngXorshift128Plus() noexcept
    {
    }

    uint64_t PrngXorshift128Plus::next_uint64()
    {
        return next();
    }

    uint32_t PrngXorshift128Plus::next_uint32()
    {
        return static_cast<uint32_t> (next() & 0xFFFFFFFF);
    }

    uint16_t PrngXorshift128Plus::next_uint16()
    {
        return static_cast<uint16_t> (next() & 0xFFFF);
    }

    uint8_t PrngXorshift128Plus::next_uint8()
    {
        return static_cast<uint16_t> (next() & 0xFF);
    }

    bool PrngXorshift128Plus::next_bool()
    {
        return (next() & 0x1) == 0;
    }

    inline uint64_t PrngXorshift128Plus::next()
    {
        uint64_t merge = seed.low_bits;
        seed.low_bits = seed.high_bits;
        seed.high_bits = merge;
        seed.low_bits ^= (seed.low_bits << 23) ^ merge ^
                         ((seed.low_bits ^ (seed.low_bits << 23)) >> 17) ^
                         (merge >> 26);
        return seed.low_bits + merge;
    }
}
