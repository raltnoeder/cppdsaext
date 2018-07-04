#ifndef RNDXORSHIFT128PLUS_H
#define RNDXORSHIFT128PLUS_H

#include <cstdint>

#include <RndNrGen.h>

namespace dsaext
{
    class PrngXorshift128Plus : public RndNrGen
    {
      public:
        struct state
        {
            uint64_t high_bits;
            uint64_t low_bits;
        };

        // @throws std::invalid_argument, std::bad_alloc
        PrngXorshift128Plus(const PrngXorshift128Plus::state& init_seed);
        virtual ~PrngXorshift128Plus() noexcept override;
        PrngXorshift128Plus(const PrngXorshift128Plus& orig) = default;
        PrngXorshift128Plus& operator=(const PrngXorshift128Plus& orig) = default;
        PrngXorshift128Plus(PrngXorshift128Plus&& orig) = default;
        PrngXorshift128Plus& operator=(PrngXorshift128Plus&& orig) = default;

        virtual uint64_t next_uint64() override;
        virtual uint32_t next_uint32() override;
        virtual uint16_t next_uint16() override;
        virtual uint8_t next_uint8() override;
        virtual bool next_bool() override;

      private:
        struct state seed;
        inline uint64_t next();
    };
}

#endif /* RNDXORSHIFT128PLUS_H */
