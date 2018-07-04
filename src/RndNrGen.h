#ifndef RNDNRGEN_H
#define RNDNRGEN_H

namespace dsaext
{
    class RndNrGen
    {
      public:
        virtual ~RndNrGen() noexcept
        {
        }
        virtual uint64_t next_uint64() = 0;
        virtual uint32_t next_uint32() = 0;
        virtual uint16_t next_uint16() = 0;
        virtual uint8_t  next_uint8() = 0;
        virtual bool     next_bool() = 0;
    };
}

#endif /* RNDNRGEN_H */
