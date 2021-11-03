#pragma once

// GNU C does not - so we have to define them by hand
#if defined __GNUC__ && !defined __MINGW32__
#   define bswap32(x) __bswap_32(x)
#   define bswap64(x) __bswap_64(x)
#endif

// Mingw is a bit more "special" - Instead of numbers we got AT&T-like specification of sizes
#if defined __MINGW32__
#   include <cstdlib>
#   define bswap32(x) ({\
    uint8_t b[4];\
    b[0] = x & 0xff;\
    b[1] = (x >> 8) & 0xff;\
    b[2] = (x >> 16) & 0xff;\
    b[3] = (x >> 24) & 0xff;\
    x = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3]);\
})

#   define bswap64(x) _byteswap_uint64(x) ({\
    uint8_t b[4];\
    b[0] = x & 0xff;\
    b[1] = (x >> 8) & 0xff;\
    b[2] = (x >> 16) & 0xff;\
    b[3] = (x >> 24) & 0xff;\
    b[4] = (x >> 32) & 0xff;\
    b[5] = (x >> 40) & 0xff;\
    b[6] = (x >> 48) & 0xff;\
    b[7] = (x >> 56) & 0xff;\
    x = (b[0] << 56) | (b[1] << 48) | (b[2] << 40) | (b[3] << 32) | (b[4] << 24) | (b[5] << 16) | (b[6] << 8) | (b[7]);\
})

#endif
