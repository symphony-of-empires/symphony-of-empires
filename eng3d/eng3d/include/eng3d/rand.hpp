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
//      rand.hpp
//
// Abstract:
//      Thread safe random number generator.
// ----------------------------------------------------------------------------

#pragma once

#include <random>

#undef rot32
#define rot32(x, k) (((x) << (k)) | ((x) >> (32 - (k))))

namespace Eng3D {
    /**
     * @brief Thread safe random number generator
     * 
     */
    class Rand {
    private:
        uint32_t a;
        uint32_t b;
        uint32_t c;
        uint32_t d;
    public:
        using result_type = uint32_t;
        constexpr static uint32_t initial_seed = 0xf1ea5eed;

        Rand(void) {

        }

        Rand(uint32_t seed) {
            a = Rand::initial_seed;
            b = c = d = seed;
            for(size_t i = 0; i < 20; ++i) {
                (*this)();
            }
        }

        Rand(Rand const& o) noexcept
            : a{ o.a },
            b{ o.b },
            c{ o.c },
            d{ o.d }
        {

        }

        Rand(Rand&& o) noexcept
            : a{ o.a },
            b{ o.b },
            c{ o.c },
            d{ o.d }
        {

        }

        /**
         * @brief Obtains the maximum generable number
         * 
         * @return constexpr uint32_t 
         */
        constexpr static uint32_t max(void) {
            return std::numeric_limits<uint32_t>::max();
        }
        
        /**
         * @brief Obtains the minimum generable number
         * 
         * @return constexpr uint32_t 
         */
        constexpr static uint32_t min(void) {
            return std::numeric_limits<uint32_t>::min();
        }

        inline uint32_t operator()(void) {
            uint32_t e = a - rot32(b, 27);
            a = b ^ rot32(c, 17);
            b = c + d;
            c = d + e;
            d = e + a;
            return d;
        }

        template<int32_t n>
        inline void advance_n(void) {
            for(int32_t i = n; i--; ) {
                this->operator()();
            }
        }

        Rand& operator=(Rand const&) noexcept = default;
        Rand& operator=(Rand&&) noexcept = default;
    };

    inline Rand& get_local_generator(void) {
        static thread_local Rand local_generator(std::random_device{}());
        return local_generator;
    }
}
#undef rot32