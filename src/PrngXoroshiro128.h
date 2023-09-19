#ifndef RNDXOROSHIRO128
#define RNDXOROSHIRO128

#include <cstdint>

#include <RndNrGen.h>

namespace dsaext
{
    class PrngXoroshiro128 : public RndNrGen
    {
      public:
        struct state
        {
            uint64_t high_bits;
            uint64_t low_bits;
        };

        // @throws std::invalid_argument, std::bad_alloc
        PrngXoroshiro128(const struct PrngXoroshiro128::state init_seed);
        virtual ~PrngXoroshiro128() noexcept override;
        PrngXoroshiro128(const PrngXoroshiro128& orig) = default;
        PrngXoroshiro128& operator=(const PrngXoroshiro128& orig) = default;
        PrngXoroshiro128(PrngXoroshiro128&& orig) = default;
        PrngXoroshiro128& operator=(PrngXoroshiro128&& orig) = default;

        virtual uint64_t next_uint64() override;
        virtual uint32_t next_uint32() override;
        virtual uint16_t next_uint16() override;
        virtual uint8_t next_uint8() override;
        virtual bool next_bool() override;
        virtual void jump();
        virtual void long_jump();

      private:
        state seed;
        static const state NORM_JUMP;
        static const state LONG_JUMP;
        inline uint64_t next();
        inline void jump_impl(const state& jump_params);
    };
}

#endif /* RNDXOROSHIRO128 */
