// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
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
//      utils.hpp
//
// Abstract:
//      General purpouse utility macros and other often-used stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>

#if !defined(__cpp_lib_byteswap) || __cpp_lib_byteswap < 202110L
#   include <bit>
#   include <concepts>
#   include <iomanip>
#   include <ranges>
#   include <algorithm>
namespace std {
    template<std::integral T>
    constexpr T byteswap(T value) noexcept {
        static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits");
        auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
        std::ranges::reverse(value_representation);
        return std::bit_cast<T>(value_representation);
    }
}
#endif

#ifndef __cpp_lib_endian
namespace std {
    enum class endian {
#   if defined E3D_TARGET_WINDOWS
        little = 0,
        big    = 1,
        native = little
#   else
        little = __ORDER_LITTLE_ENDIAN__,
        big    = __ORDER_BIG_ENDIAN__,
        native = __BYTE_ORDER__
#   endif
    };
}
#endif

#if !defined(E3D_EXCEPTIONS)
#   define CXX_THROW(class, ...) throw class(__VA_ARGS__);
#else
#include <cstdio>
#   define CXX_THROW(class, ...) { fprintf(stderr, class(__VA_ARGS__).what()); abort(); }
#endif

namespace Eng3D {
    // Does the same as std::erase but doesn't keep the order
    template <typename C, typename T>
    inline void fast_erase(C& c, T value) noexcept {
        for(auto i = c.size(); i-- > 0; ) {
            if(c[i] == value) {
                c[i] = c.back();
                c.pop_back();
                return;
            }
        }
    }

    // Does the same as std::erase_all but doesn't keep the order
    template <typename C, typename T>
    inline void fast_erase_all(C& c, T value) noexcept {
        for(auto i = c.size(); i-- > 0; ) {
            if(c[i] == value) {
                c[i] = c.back();
                c.pop_back();
            }
        }
    }

    template<typename T>
    class LazyGlobalWrapper {
        char data[sizeof(T)];
    public:
        const T* get() const {
            return reinterpret_cast<const T*>(data);
        }

        T* get() {
            return reinterpret_cast<T*>(data);
        }
    };
}
