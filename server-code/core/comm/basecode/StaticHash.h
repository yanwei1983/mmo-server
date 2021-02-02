#ifndef STATICHASH_H
#define STATICHASH_H

#include <cstdint>

namespace hash
{
    constexpr uint32_t djb2a(const char* s, uint32_t h = 5381) { return !*s ? h : djb2a(s + 1, 33 * h ^ (uint8_t)*s); }

    constexpr uint32_t fnv1a(const char* s, uint32_t h = 0x811C9DC5) { return !*s ? h : fnv1a(s + 1, (h ^ (uint8_t)*s) * 0x01000193); }

    constexpr uint32_t CRC32_TABLE[] = {0x00000000,
                                        0x1DB71064,
                                        0x3B6E20C8,
                                        0x26D930AC,
                                        0x76DC4190,
                                        0x6B6B51F4,
                                        0x4DB26158,
                                        0x5005713C,
                                        0xEDB88320,
                                        0xF00F9344,
                                        0xD6D6A3E8,
                                        0xCB61B38C,
                                        0x9B64C2B0,
                                        0x86D3D2D4,
                                        0xA00AE278,
                                        0xBDBDF21C};
    constexpr uint32_t crc32(const char* s, uint32_t h = ~0)
    {
#define CRC4(c, h) (CRC32_TABLE[((h)&0xF) ^ (c)] ^ ((h) >> 4))
        return !*s ? ~h : crc32(s + 1, CRC4((uint8_t)*s >> 4, CRC4((uint8_t)*s & 0xF, h)));
#undef CRC4
    }

    namespace MurmurHash3
    {
        constexpr uint32_t rotl(uint32_t x, int8_t r) { return (x << r) | (x >> (32 - r)); }
        constexpr uint32_t kmix(uint32_t k) { return rotl(k * 0xCC9E2D51, 15) * 0x1B873593; }
        constexpr uint32_t hmix(uint32_t h, uint32_t k) { return rotl(h ^ kmix(k), 13) * 5 + 0xE6546B64; }
        constexpr uint32_t shlxor(uint32_t x, int8_t l) { return (x >> l) ^ x; }
        constexpr uint32_t fmix(uint32_t h) { return shlxor(shlxor(shlxor(h, 16) * 0x85EBCA6B, 13) * 0xC2B2AE35, 16); }
        constexpr uint32_t body(const char* s, size_t n, uint32_t h)
        {
            return n < 4 ? h : body(s + 4, n - 4, hmix(h, s[0] | (s[1] << 8) | (s[2] << 16) | (s[3] << 24)));
        }
        constexpr uint32_t tail(const char* s, size_t n, uint32_t h)
        {
            return h ^ kmix(n == 3 ? s[0] | (s[1] << 8) | (s[2] << 16) : n == 2 ? s[0] | (s[1] << 8) : n == 1 ? s[0] : 0);
        }
        constexpr uint32_t shash(const char* s, size_t n, uint32_t seed) { return fmix(tail(s + (n & ~3), n & 3, body(s, n, seed)) ^ n); }
    } // namespace MurmurHash3



    // Tomas Wang
    constexpr uint32_t hash32shift(uint32_t key)
    {
        key = ~key + (key << 15); // key = (key << 15) - key - 1;
        key = key ^ (key >> 12);
        key = key + (key << 2);
        key = key ^ (key >> 4);
        key = key * 2057; // key = (key + (key << 3)) + (key << 11);
        key = key ^ (key >> 16);
        return key;
    }

    // Bob Jenkins' 32 bit integer hash function
    // 这六个数是随机数， 通过设置合理的6个数，你可以找到对应的perfect hash.
    constexpr uint32_t hash32( uint32_t a)
    {
        a = (a+0x7ed55d16) + (a<<12);
        a = (a^0xc761c23c) ^ (a>>19);
        a = (a+0x165667b1) + (a<<5);
        a = (a+0xd3a2646c) ^ (a<<9);
        a =(a+0xfd7046c5) + (a<<3); // <<和 +的组合是可逆的
        a = (a^0xb55a4f09) ^ (a>>16);
        return a;
    }

    // 64 bit Mix Functions
    constexpr uint64_t hash64shift(uint64_t key)
    {
        key = (~key) + (key << 21); // key = (key << 21) - key - 1;
        key = key ^ (key >> 24);
        key = (key + (key << 3)) + (key << 8); // key * 265
        key = key ^ (key >> 14);
        key = (key + (key << 2)) + (key << 4); // key * 21
        key = key ^ (key >> 28);
        key = key + (key << 31);
        return key;
    }
    // 64 bit to 32 bit Mix Functions
    constexpr uint32_t hash64_32shift(uint64_t key)
    {
        key = (~key) + (key << 18); // key = (key << 18) - key - 1;
        key = key ^ (key >> 31);
        key = key * 21; // key = (key + (key << 2)) + (key << 4);
        key = key ^ (key >> 11);
        key = key + (key << 6);
        key = key ^ (key >> 22);
        return (int) key;
    }

    // Bob Jenkins' 96 bit Mix Function
    constexpr uint32_t hash96(uint32_t a, uint32_t b, uint32_t c)
    {
        a=a-b; a=a-c; a=a^(c >> 13);
        b=b-c; b=b-a; b=b^(a << 8);
        c=c-a; c=c-b; c=c^(b >> 13);
        a=a-b; a=a-c; a=a^(c >> 12);
        b=b-c; b=b-a; b=b^(a << 16);
        c=c-a; c=c-b; c=c^(b >> 5);
        a=a-b; a=a-c; a=a^(c >> 3);
        b=b-c; b=b-a; b=b^(a << 10);
        c=c-a; c=c-b; c=c^(b >> 15);
        return c;

    }
}; // namespace hash

constexpr uint32_t operator"" _HASH(const char* s, size_t size)
{
    return hash::MurmurHash3::shash(s, size, 0);
}

#endif /* STATICHASH_H */
