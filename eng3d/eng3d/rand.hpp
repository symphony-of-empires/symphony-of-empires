// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
    /// @brief Thread safe random number generator
    class Rand {
        uint32_t a = 0, b = 0, c = 0, d = 0;
    public:
        using result_type = uint32_t;
        constexpr static uint32_t initial_seed = 0xf1ea5eed;

        constexpr Rand() = default;

        Rand(uint32_t seed) {
            a = Rand::initial_seed;
            b = c = d = seed;
            for(size_t i = 0; i < 20; ++i)
                (*this)();
        }

        constexpr Rand(Rand const& o) noexcept
            : a{ o.a },
            b{ o.b },
            c{ o.c },
            d{ o.d }
        {

        }

        constexpr Rand(Rand&& o) noexcept
            : a{ o.a },
            b{ o.b },
            c{ o.c },
            d{ o.d }
        {

        }

        /// @brief Obtains the maximum generable number
        /// @return constexpr uint32_t 
        constexpr static uint32_t max() {
            return std::numeric_limits<uint32_t>::max();
        }
        
        /// @brief Obtains the minimum generable number
        /// @return constexpr uint32_t 
        constexpr static uint32_t min() {
            return std::numeric_limits<uint32_t>::min();
        }

        constexpr uint32_t operator()() {
            uint32_t e = a - rot32(b, 27);
            a = b ^ rot32(c, 17);
            b = c + d;
            c = d + e;
            d = e + a;
            return d;
        }

        template<int32_t n>
        constexpr void advance_n() {
            for(int32_t i = n; i--; )
                this->operator()();
        }

        Rand& operator=(Rand const&) noexcept = default;
        Rand& operator=(Rand&&) noexcept = default;
    };

    inline Rand& get_local_generator() {
        static thread_local Rand local_generator(std::random_device{}());
        return local_generator;
    }
}
#undef rot32
