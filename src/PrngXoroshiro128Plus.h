#ifndef RNDXOROSHIRO128PLUS
#define RNDXOROSHIRO128PLUS

#include <cstdint>

#include <RndNrGen.h>

namespace dsaext
{
    class PrngXoroshiro128Plus : public RndNrGen
    {
      public:
        struct state
        {
            uint64_t high_bits;
            uint64_t low_bits;
        };

        // @throws std::invalid_argument, std::bad_alloc
        PrngXoroshiro128Plus(const PrngXoroshiro128Plus::state& init_seed);
        virtual ~PrngXoroshiro128Plus() noexcept override;
        PrngXoroshiro128Plus(const PrngXoroshiro128Plus& orig) = default;
        PrngXoroshiro128Plus& operator=(const PrngXoroshiro128Plus& orig) = default;
        PrngXoroshiro128Plus(PrngXoroshiro128Plus&& orig) = default;
        PrngXoroshiro128Plus& operator=(PrngXoroshiro128Plus&& orig) = default;

        virtual uint64_t next_uint64() override;
        virtual uint32_t next_uint32() override;
        virtual uint16_t next_uint16() override;
        virtual uint8_t next_uint8() override;
        virtual bool next_bool() override;

      private:
        state seed;
        inline uint64_t next();
    };
}

#endif /* RNDXOROSHIRO128PLUS */
