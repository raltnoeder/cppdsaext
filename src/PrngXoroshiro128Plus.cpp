#include <stdexcept>
#include <PrngXoroshiro128Plus.h>

namespace dsaext
{
    // @throws std::invalid_argument, std::bad_alloc
    PrngXoroshiro128Plus::PrngXoroshiro128Plus(const PrngXoroshiro128Plus::state& init_seed)
    {
        if (init_seed.high_bits != 0 && init_seed.low_bits != 0)
        {
            seed = init_seed;
        }
        else
        {
            throw std::invalid_argument(
                "Class PrngXoroshiro128Plus constructor called with "
                "an invalid init_seed value of 0"
            );
        }
    }

    PrngXoroshiro128Plus::~PrngXoroshiro128Plus() noexcept
    {
    }

    uint64_t PrngXoroshiro128Plus::next_uint64()
    {
        return next();
    }

    uint32_t PrngXoroshiro128Plus::next_uint32()
    {
        return static_cast<uint32_t> (next() & 0xFFFFFFFF);
    }

    uint16_t PrngXoroshiro128Plus::next_uint16()
    {
        return static_cast<uint16_t> ((next() & 0xFFFF0000) >> 16);
    }

    uint8_t PrngXoroshiro128Plus::next_uint8()
    {
        return static_cast<uint16_t> ((next() & 0xFF0000) >> 16);
    }

    bool PrngXoroshiro128Plus::next_bool()
    {
        return (next() & 0x100000000) == 0;
    }

    inline uint64_t PrngXoroshiro128Plus::next()
    {
        uint64_t result = seed.high_bits + seed.low_bits;
        uint64_t merged = seed.high_bits ^ seed.low_bits;
        seed.high_bits = ((seed.high_bits << 55) | (seed.high_bits >> 9)) ^
                         merged ^ (merged << 14);
        seed.low_bits = (merged << 36) | (merged >> 28);

        return result;
    }
}
