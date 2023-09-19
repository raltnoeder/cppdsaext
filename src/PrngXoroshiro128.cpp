#include <stdexcept>
#include <PrngXoroshiro128.h>

namespace dsaext
{
    const PrngXoroshiro128::state PrngXoroshiro128::NORM_JUMP = {0xDF900294D8F554A5, 0x170865DF4B3201FC};
    const PrngXoroshiro128::state PrngXoroshiro128::LONG_JUMP = {0xD2A98B26625EEE7B, 0xDDDF9B1090AA7AC1};

    // @throws std::invalid_argument, std::bad_alloc
    PrngXoroshiro128::PrngXoroshiro128(const struct PrngXoroshiro128::state init_seed)
    {
        if (init_seed.high_bits != 0 && init_seed.low_bits != 0)
        {
            seed = {init_seed.high_bits, init_seed.low_bits};
        }
        else
        {
            throw std::invalid_argument(
                "Class PrngXoroshiro128 constructor called with "
                "an invalid init_seed value of 0"
            );
        }
    }

    PrngXoroshiro128::~PrngXoroshiro128() noexcept
    {
    }

    uint64_t PrngXoroshiro128::next_uint64()
    {
        return next();
    }

    uint32_t PrngXoroshiro128::next_uint32()
    {
        return static_cast<uint32_t> (next() & 0xFFFFFFFF);
    }

    uint16_t PrngXoroshiro128::next_uint16()
    {
        return static_cast<uint16_t> ((next() & 0xFFFF0000) >> 16);
    }

    uint8_t PrngXoroshiro128::next_uint8()
    {
        return static_cast<uint16_t> ((next() & 0xFF0000) >> 16);
    }

    bool PrngXoroshiro128::next_bool()
    {
        return (next() & 0x100000000) == 0;
    }

    inline uint64_t PrngXoroshiro128::next()
    {
        uint64_t result = seed.high_bits + seed.low_bits;
        uint64_t merged = seed.high_bits ^ seed.low_bits;
        seed.high_bits = ((seed.high_bits << 24) | (seed.high_bits >> 40)) ^
                         merged ^ (merged << 16);
        seed.low_bits = (merged << 37) | (merged >> 27);

        return result;
    }

    void PrngXoroshiro128::jump_impl(const state& jump_params)
    {
        uint64_t hi = 0;
        uint64_t lo = 0;
        for (int shift = 0; shift < 64; ++shift)
        {
            if ((jump_params.high_bits & (static_cast<uint64_t> (1) << shift)) != 0)
            {
                hi ^= seed.high_bits;
                lo ^= seed.low_bits;
            }
            next();
        }
        for (int shift = 0; shift < 64; ++shift)
        {
            if ((jump_params.low_bits & (static_cast<uint64_t> (1) << shift)) != 0)
            {
                hi ^= seed.high_bits;
                lo ^= seed.low_bits;
            }
            next();
        }
        seed.high_bits = hi;
        seed.low_bits = lo;
    }

    void PrngXoroshiro128::jump()
    {
        jump_impl(NORM_JUMP);
    }

    void PrngXoroshiro128::long_jump()
    {
        jump_impl(LONG_JUMP);
    }
}
