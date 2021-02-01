#ifndef INTUTIL_H
#define INTUTIL_H

#include "CheckUtil.h"
#include "export_lua.h"

#ifndef UNUSED
#define UNUSED(var) (void)((var) = (var))
#endif

export_lua inline uint64_t MakeUINT64(uint32_t left, uint32_t right)
{
    return static_cast<uint64_t>(left) << 32 | static_cast<uint64_t>(right);
}

export_lua inline void SplitUINT64(uint64_t val, uint32_t& left, uint32_t& right)
{
    right = val & 0xFFFFFFFF;
    left  = (val >> 32) & 0xFFFFFFFF;
}

export_lua inline uint32_t MakeUINT32(uint16_t left, uint16_t right)
{
    return static_cast<uint32_t>(left) << 16 | static_cast<uint32_t>(right);
}

export_lua inline void SplitUINT32(uint32_t val, uint16_t& left, uint16_t& right)
{
    right = val & 0xFFFF;
    left  = (val >> 16) & 0xFFF;
}

export_lua inline uint16_t MakeUINT16(uint8_t left, uint8_t right)
{
    return static_cast<uint16_t>(left) << 8 | static_cast<uint16_t>(right);
}

export_lua inline void SplitUINT16(uint16_t val, uint8_t& left, uint8_t& right)
{
    right = val & 0xFF;
    left  = (val >> 8) & 0xFF;
}

export_lua inline uint32_t MAKE32(uint32_t a, uint32_t b)
{
    return (a << 16) | (b & 0xFFFF);
}

export_lua inline uint64_t MAKE64(uint64_t a, uint64_t b)
{
    return (a << 32) | (b & 0xFFFFFFFF);
}

export_lua inline uint32_t GetHighFromU64(uint64_t v)
{
    return (v >> 32) & 0xFFFFFFFF;
}

export_lua inline uint32_t GetLowFromU64(uint64_t v)
{
    return (v)&0xFFFFFFFF;
}
// a*b/c
export_lua inline int32_t MulDivSign(int32_t a, int32_t b, int32_t c)
{
    if(c == 0)
        return 0;
    return ((int32_t)(((int64_t)(a) * (int64_t)(b)) / (int64_t)(c)));
}

export_lua inline uint32_t MulDiv(uint32_t a, uint32_t b, uint32_t c)
{
    if(c == 0)
        return 0;
    return ((uint32_t)(((uint64_t)(a) * (uint64_t)(b)) / (uint64_t)(c)));
}

export_lua inline uint32_t hex_set(uint32_t dwFlag, uint8_t nHex, uint8_t ucVal)
{
    CHECKF_V(nHex < 8, nHex);
    return (dwFlag & ~(0xF << (nHex * 4))) | ((ucVal & 0xF) << (nHex * 4));
}

export_lua inline uint8_t hex_get(uint32_t dwFlag, uint8_t nHex)
{
    CHECKF_V(nHex < 8, nHex);
    return (dwFlag >> (nHex * 4)) & 0xF;
}

export_lua inline bool bit_test(uint32_t dwFlag, uint8_t nBit)
{
    CHECKF_V(nBit < 32, nBit);
    return (dwFlag & ((uint32_t)1 << nBit)) != 0;
}

export_lua inline uint32_t bit_flip(uint32_t dwFlag, uint8_t nBit)
{
    CHECKF_V(nBit < 32, nBit);
    return dwFlag ^ ((uint32_t)1 << nBit);
}

export_lua inline uint32_t bit_set(uint32_t dwFlag, uint8_t nBit, bool bVal)
{
    CHECKF_V(nBit < 32, nBit);
    if(bVal)
        return dwFlag | (uint32_t)1 << nBit;
    else
        return dwFlag & ~((uint32_t)1 << nBit);
}

export_lua inline BYTE toHex(const BYTE& x)
{
    return x > 9 ? x + 55 : x + 48;
}

export_lua inline bool HasFlag(uint32_t flag, uint32_t mask)
{
    return (flag & mask) != 0;
}


export_lua inline uint32_t GetBitCount(uint32_t u)
{
    u = (u & 0x55555555) + ((u >> 1) & 0x55555555);
    u = (u & 0x33333333) + ((u >> 2) & 0x33333333);
    u = (u & 0x0F0F0F0F) + ((u >> 4) & 0x0F0F0F0F);
    u = (u & 0x00FF00FF) + ((u >> 8) & 0x00FF00FF);
    u = (u & 0x0000FFFF) + ((u >> 16) & 0x0000FFFF);
    return u;
    
}

export_lua inline uint32_t GetBitCount64(uint64_t x)
{
    x = (x & 0x5555555555555555ULL) + ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL);
    return (x * 0x0101010101010101ULL) >> 56;
}

template<typename EnumType>
auto enum_to(EnumType e)
{
    return static_cast<std::underlying_type_t<EnumType> >(e);
}

template<typename EnumType>
EnumType to_enum(std::underlying_type_t<EnumType> e)
{
    return static_cast<EnumType>(e);
}

#endif /* INTUTIL_H */
