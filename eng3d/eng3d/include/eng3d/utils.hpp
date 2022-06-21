// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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

#include <cstdint>

static inline uint32_t bswap32(const uint32_t x) {
    return
        ((x << 24) & 0xff000000ULL) |
        ((x << 8) & 0x00ff0000ULL) |
        ((x >> 8) & 0x0000ff00ULL) |
        ((x >> 24) & 0x000000ffULL);
}

static inline uint64_t bswap64(const uint64_t x) {
    return
        ((x << 56) & 0xff00000000000000ULL) |
        ((x << 40) & 0x00ff000000000000ULL) |
        ((x << 24) & 0x0000ff0000000000ULL) |
        ((x << 8) & 0x000000ff00000000ULL) |
        ((x >> 8) & 0x00000000ff000000ULL) |
        ((x >> 24) & 0x0000000000ff0000ULL) |
        ((x >> 40) & 0x000000000000ff00ULL) |
        ((x >> 56) & 0x00000000000000ffULL);
}

#if !defined(E3D_EXCEPTIONS)
#   define CXX_THROW(class, ...) throw class(__VA_ARGS__);
#else
#include <cstdio>
#   define CXX_THROW(class, ...) { fprintf(stderr, class(__VA_ARGS__).what()); abort(); }
#endif
