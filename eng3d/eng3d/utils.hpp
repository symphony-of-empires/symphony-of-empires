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
#include <cstring>

#if !defined(__cpp_lib_byteswap) || __cpp_lib_byteswap < 202110L
#   include <bit>
#   include <concepts>
#   include <iomanip>
#   include <algorithm>
#   include <array>
namespace std {
#if !defined(__cpp_lib_bit_cast)
    template <class To, class From>
    std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>, To> bit_cast(const From& src) noexcept {
        static_assert(std::is_trivially_constructible_v<To>, "This implementation additionally requires destination type to be trivially constructible");
        To dst;
        ::memcpy(&dst, &src, sizeof(To));
        return dst;
    }
#endif
#ifndef __cpp_lib_byteswap
    template<typename T>
    constexpr T byteswap(T value) noexcept {
#ifdef __cpp_lib_ranges
        static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits");
        auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
        std::ranges::reverse(value_representation);
        return std::bit_cast<T>(value_representation);
#endif
        if(sizeof(T) == sizeof(uint64_t)) {
            uint64_t x = value;
            return ((x << 56) & 0xff00000000000000ULL) |
                ((x << 40) & 0x00ff000000000000ULL) |
                ((x << 24) & 0x0000ff0000000000ULL) |
                ((x << 8) & 0x000000ff00000000ULL) |
                ((x >> 8) & 0x00000000ff000000ULL) |
                ((x >> 24) & 0x0000000000ff0000ULL) |
                ((x >> 40) & 0x000000000000ff00ULL) |
                ((x >> 56) & 0x00000000000000ffULL);
        } else if(sizeof(T) == sizeof(uint32_t)) {
            uint32_t x = value;
            return
                ((x << 24) & 0xff000000UL) |
                ((x << 8) & 0x00ff0000UL) |
                ((x >> 8) & 0x0000ff00UL) |
                ((x >> 24) & 0x000000ffUL);
        } else {
            uint16_t x = value;
            return
                ((x << 8) & 0xff00U) |
                ((x >> 8) & 0x00ffU);
        }
        return value;
    }
#endif

#if (defined(__llvm__) || defined(__clang__)) && defined(__mingw__)
    template<class T>
    concept destructible = std::is_nothrow_destructible_v<T>;
#endif
}
#endif

#ifndef __cpp_lib_endian
namespace std {
    enum class endian {
#   if defined _WIN32
        little = 0,
        big = 1,
        native = little
#   else
        little = __ORDER_LITTLE_ENDIAN__,
        big = __ORDER_BIG_ENDIAN__,
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

template<typename It>
class Range
{
    It _b, _e;
public:
    Range(It b, It e): _b(b), _e(e) {}
    It begin() const { return _b; }
    It end() const { return _e; }
};

template<typename ORange, typename OIt = decltype(std::begin(std::declval<ORange>())), typename It = std::reverse_iterator<OIt>>
Range<It> reverse(ORange&& originalRange) {
    return Range<It>(It(std::end(originalRange)), It(std::begin(originalRange)));
}

#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

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

    inline glm::vec3 get_sphere_coord(glm::vec2 size, glm::vec2 pos, float radius) {
        const auto normalized_pos = pos / size;
        glm::vec2 radiance_pos{
            normalized_pos.x * 2.f * glm::pi<float>(),
            normalized_pos.y * glm::pi<float>()
        };
        glm::vec3 sphere_position{
            glm::cos(radiance_pos.x) * glm::sin(radiance_pos.y),
            glm::sin(radiance_pos.x) * glm::sin(radiance_pos.y),
            glm::cos(radiance_pos.y)
        };
        return sphere_position * radius;
    }

    /// @brief Obtain the euclidean distance from p0 to p1
    /// @param p0 Point A
    /// @param p1 Point B
    inline float euclidean_distance(glm::vec2 p0, glm::vec2 p1, glm::vec2 world_size, float radius) {
        const auto sphere_coord1 = Eng3D::get_sphere_coord(world_size, p0, radius);
        const auto sphere_coord2 = Eng3D::get_sphere_coord(world_size, p1, radius);
        const auto cos_angle = glm::dot(sphere_coord1, sphere_coord2) / (radius * radius);
        const auto angle = glm::acos(glm::clamp(cos_angle, -1.f, 1.f));
        return angle * radius;
    }
}
