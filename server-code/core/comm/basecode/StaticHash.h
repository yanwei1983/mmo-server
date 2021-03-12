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


namespace hash
{

    struct xxh32
    {
        static constexpr uint32_t hash(const char* input, uint32_t len, uint32_t seed)
        {
            return finalize((len >= 16 ? h16bytes(input, len, seed) : seed + PRIME5) + len, (input) + (len & ~0xF), len & 0xF);
        }

    private:
        static constexpr uint32_t PRIME1 = 0x9E3779B1U;
        static constexpr uint32_t PRIME2 = 0x85EBCA77U;
        static constexpr uint32_t PRIME3 = 0xC2B2AE3DU;
        static constexpr uint32_t PRIME4 = 0x27D4EB2FU;
        static constexpr uint32_t PRIME5 = 0x165667B1U;

        // 32-bit rotate left.
        static constexpr uint32_t rotl(uint32_t x, int r) { return ((x << r) | (x >> (32 - r))); }
        // Normal stripe processing routine.
        static constexpr uint32_t round(uint32_t acc, const uint32_t input) { return rotl(acc + (input * PRIME2), 13) * PRIME1; }

        static constexpr uint32_t avalanche_step(const uint32_t h, const int rshift, const uint32_t prime) { return (h ^ (h >> rshift)) * prime; }
        // Mixes all bits to finalize the hash.
        static constexpr uint32_t avalanche(const uint32_t h)
        {
            return avalanche_step(avalanche_step(avalanche_step(h, 15, PRIME2), 13, PRIME3), 16, 1);
        }

#ifdef XXH32_BIG_ENDIAN
        static constexpr uint32_t endian32(const char* v)
        {
            return uint32_t(uint8_t(v[3])) | (uint32_t(uint8_t(v[2])) << 8) | (uint32_t(uint8_t(v[1])) << 16) | (uint32_t(uint8_t(v[0])) << 24);
        }
#else
        static constexpr uint32_t endian32(const char* v)
        {
            return uint32_t(uint8_t(v[0])) | (uint32_t(uint8_t(v[1])) << 8) | (uint32_t(uint8_t(v[2])) << 16) | (uint32_t(uint8_t(v[3])) << 24);
        }
#endif // XXH32_BIG_ENDIAN

        static constexpr uint32_t fetch32(const char* p, const uint32_t v) { return round(v, endian32(p)); }

        // Processes the last 0-15 bytes of p.
        static constexpr uint32_t finalize(const uint32_t h, const char* p, uint32_t len)
        {
            return (len >= 4)  ? finalize(rotl(h + (endian32(p) * PRIME3), 17) * PRIME4, p + 4, len - 4)
                   : (len > 0) ? finalize(rotl(h + (uint8_t(*p) * PRIME5), 11) * PRIME1, p + 1, len - 1)
                               : avalanche(h);
        }

        static constexpr uint32_t h16bytes(const char* p, uint32_t len, const uint32_t v1, const uint32_t v2, const uint32_t v3, const uint32_t v4)
        {
            return (len >= 16) ? h16bytes(p + 16, len - 16, fetch32(p, v1), fetch32(p + 4, v2), fetch32(p + 8, v3), fetch32(p + 12, v4))
                               : rotl(v1, 1) + rotl(v2, 7) + rotl(v3, 12) + rotl(v4, 18);
        }
        static constexpr uint32_t h16bytes(const char* p, uint32_t len, const uint32_t seed)
        {
            return h16bytes(p, len, seed + PRIME1 + PRIME2, seed + PRIME2, seed, seed - PRIME1);
        }
    };
}; // namespace hash

namespace hash
{
    struct xxh64
    {
        static constexpr uint64_t hash(const char* p, uint64_t len, uint64_t seed)
        {
            return finalize((len >= 32 ? h32bytes(p, len, seed) : seed + PRIME5) + len, p + (len & ~0x1F), len & 0x1F);
        }

    private:
        static constexpr uint64_t PRIME1 = 11400714785074694791ULL;
        static constexpr uint64_t PRIME2 = 14029467366897019727ULL;
        static constexpr uint64_t PRIME3 = 1609587929392839161ULL;
        static constexpr uint64_t PRIME4 = 9650029242287828579ULL;
        static constexpr uint64_t PRIME5 = 2870177450012600261ULL;

        static constexpr uint64_t rotl(uint64_t x, int r) { return ((x << r) | (x >> (64 - r))); }
        static constexpr uint64_t mix1(const uint64_t h, const uint64_t prime, int rshift) { return (h ^ (h >> rshift)) * prime; }
        static constexpr uint64_t mix2(const uint64_t p, const uint64_t v = 0) { return rotl(v + p * PRIME2, 31) * PRIME1; }
        static constexpr uint64_t mix3(const uint64_t h, const uint64_t v) { return (h ^ mix2(v)) * PRIME1 + PRIME4; }
#ifdef XXH64_BIG_ENDIAN
        static constexpr uint32_t endian32(const char* v)
        {
            return uint32_t(uint8_t(v[3])) | (uint32_t(uint8_t(v[2])) << 8) | (uint32_t(uint8_t(v[1])) << 16) | (uint32_t(uint8_t(v[0])) << 24);
        }
        static constexpr uint64_t endian64(const char* v)
        {
            return uint64_t(uint8_t(v[7])) | (uint64_t(uint8_t(v[6])) << 8) | (uint64_t(uint8_t(v[5])) << 16) | (uint64_t(uint8_t(v[4])) << 24) |
                   (uint64_t(uint8_t(v[3])) << 32) | (uint64_t(uint8_t(v[2])) << 40) | (uint64_t(uint8_t(v[1])) << 48) |
                   (uint64_t(uint8_t(v[0])) << 56);
        }
#else
        static constexpr uint32_t endian32(const char* v)
        {
            return uint32_t(uint8_t(v[0])) | (uint32_t(uint8_t(v[1])) << 8) | (uint32_t(uint8_t(v[2])) << 16) | (uint32_t(uint8_t(v[3])) << 24);
        }
        static constexpr uint64_t endian64(const char* v)
        {
            return uint64_t(uint8_t(v[0])) | (uint64_t(uint8_t(v[1])) << 8) | (uint64_t(uint8_t(v[2])) << 16) | (uint64_t(uint8_t(v[3])) << 24) |
                   (uint64_t(uint8_t(v[4])) << 32) | (uint64_t(uint8_t(v[5])) << 40) | (uint64_t(uint8_t(v[6])) << 48) |
                   (uint64_t(uint8_t(v[7])) << 56);
        }
#endif
        static constexpr uint64_t fetch64(const char* p, const uint64_t v = 0) { return mix2(endian64(p), v); }
        static constexpr uint64_t fetch32(const char* p) { return uint64_t(endian32(p)) * PRIME1; }
        static constexpr uint64_t fetch8(const char* p) { return uint8_t(*p) * PRIME5; }
        static constexpr uint64_t finalize(const uint64_t h, const char* p, uint64_t len)
        {
            return (len >= 8) ? (finalize(rotl(h ^ fetch64(p), 27) * PRIME1 + PRIME4, p + 8, len - 8))
                              : ((len >= 4) ? (finalize(rotl(h ^ fetch32(p), 23) * PRIME2 + PRIME3, p + 4, len - 4))
                                            : ((len > 0) ? (finalize(rotl(h ^ fetch8(p), 11) * PRIME1, p + 1, len - 1))
                                                         : (mix1(mix1(mix1(h, PRIME2, 33), PRIME3, 29), 1, 32))));
        }
        static constexpr uint64_t h32bytes(const char* p, uint64_t len, const uint64_t v1, const uint64_t v2, const uint64_t v3, const uint64_t v4)
        {
            return (len >= 32) ? h32bytes(p + 32, len - 32, fetch64(p, v1), fetch64(p + 8, v2), fetch64(p + 16, v3), fetch64(p + 24, v4))
                               : mix3(mix3(mix3(mix3(rotl(v1, 1) + rotl(v2, 7) + rotl(v3, 12) + rotl(v4, 18), v1), v2), v3), v4);
        }
        static constexpr uint64_t h32bytes(const char* p, uint64_t len, const uint64_t seed)
        {
            return h32bytes(p, len, seed + PRIME1 + PRIME2, seed + PRIME2, seed, seed - PRIME1);
        }
    };
}; // namespace hash

#endif /* STATICHASH_H */
