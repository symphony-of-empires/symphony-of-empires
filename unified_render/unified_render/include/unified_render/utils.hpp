// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      utils.hpp
//
// Abstract:
//      General purpouse utility macros and other often-used stuff.
// ----------------------------------------------------------------------------

#pragma once

// GNU C does not - so we have to define them by hand
#if defined windows
#   define bswap32(x) _byteswap_ulong(x)
#   define bswap64(x) _byteswap_uint64(x)
#elif defined _MSC_VER
#   include <cstdlib>
#   define bswap32(x) _byteswap_ulong(x)
#   define bswap64(x) _byteswap_uint64(x)
#elif defined __GNUC__ && !defined __MINGW32__
#   define bswap32(x) __bswap_32(x)
#   define bswap64(x) __bswap_64(x)
// Mingw and MSVC is a bit more "special" - Instead of numbers we got AT&T-like specification of sizes
#elif defined __MINGW32__
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
#else
#   include <byteswap.h>
#endif

#if !defined(UR_EXCEPTIONS)
#   define CXX_THROW(class, ...) \
    throw class(__VA_ARGS__);
#else
#include <cstdio>
#   define CXX_THROW(class, ...) \
    fprintf(stderr, class(__VA_ARGS__).what()); \
    abort();
#endif
