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
//      color.hpp
//
// Abstract:
//      Primitive color type used through the engine
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <cstddef>

namespace Eng3D {
    /// @brief Primitive color type used through the engine
    struct Color {
        float r = 0.f, g = 0.f, b = 0.f;
        float a = 1.f;

        constexpr Color()
            : r{ 0.f },
            g{ 0.f },
            b{ 0.f },
            a{ 0.f }
        {

        }

        constexpr Color(float red, float green, float blue, float alpha = 1.f)
            : r{ red },
            g{ green },
            b{ blue },
            a{ alpha }
        {

        }

        ~Color() = default;

        /// @brief Create a color from RGBA components
        /// @param red Red component
        /// @param green Green component
        /// @param blue Blue component
        /// @param alpha Alpha component
        /// @return Color Resulting color
        constexpr static Color rgba8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
            return Color(red / 255.f, green / 255.f, blue / 255.f, alpha / 255.f);
        }

        /// @brief Create a color from RGB components
        /// @param red Red component
        /// @param green Green component
        /// @param blue Blue component
        /// @return Color Resulting color
        constexpr static Color rgb8(uint8_t red, uint8_t green, uint8_t blue) {
            return rgba8(red, green, blue, 255);
        }

        /// @brief Create a color from RGBA32 components
        /// @param argb The ARGB32 color
        /// @return Color Resulting color
        constexpr static Color argb32(uint32_t argb) {
            const auto a = ((argb >> 24) & 0xFF) / 255.f;
            const auto r = ((argb >> 16) & 0xFF) / 255.f;
            const auto g = ((argb >> 8) & 0xFF) / 255.f;
            const auto b = (argb & 0xFF) / 255.f;
            return Color(r, g, b, a);
        }

        /// @brief Create a color from RGB32 components
        /// @param argb The ARGB32 color
        /// @return Color Resulting color
        constexpr static Color rgb32(uint32_t argb) {
            return Color::argb32(argb | (0xff << 24));
        }

        constexpr static Color abgr32(uint32_t abgr) {
            const auto a = ((abgr >> 24) & 0xFF) / 255.f;
            const auto b = ((abgr >> 16) & 0xFF) / 255.f;
            const auto g = ((abgr >> 8) & 0xFF) / 255.f;
            const auto r = (abgr & 0xFF) / 255.f;
            return Color(r, g, b, a);
        }

        constexpr static Color bgr32(uint32_t abgr) {
            return Color::abgr32(abgr | (0xff << 24));
        }

        /// @brief Get the raw value of the color
        /// @return uint32_t The raw value
        constexpr uint32_t get_value() const noexcept {
            const auto alpha = static_cast<uint8_t>(a * 255);
            const auto red = static_cast<uint8_t>(r * 255);
            const auto green = static_cast<uint8_t>(g * 255);
            const auto blue = static_cast<uint8_t>(b * 255);
            uint32_t color = (alpha << 24) | (blue << 16) | (green << 8) | (red);
            return color;
        }

        /// @brief Combine two colors with LERP
        /// @param color1 Color 1
        /// @param color2 Color 2
        /// @param lamda Intensity of merge in respect to Color 2
        /// @return Color Resulting color
        constexpr static Color lerp(Color color1, Color color2, float lamda) {
            const auto r = color1.r * (1 - lamda) + color2.r * lamda;
            const auto g = color1.g * (1 - lamda) + color2.g * lamda;
            const auto b = color1.b * (1 - lamda) + color2.b * lamda;
            return Color(r, g, b);
        }

        constexpr static Color get_random(size_t mod) {
            const uint32_t colors[] = {
                0xFF000000, 0xFF00FF00, 0xFF0000FF, 0xFFFF0000, 0xFF01FFFE, 0xFFFFA6FE,
                0xFFFFDB66, 0xFF006401, 0xFF010067, 0xFF95003A, 0xFF007DB5, 0xFFFF00F6,
                0xFFFFEEE8, 0xFF774D00, 0xFF90FB92, 0xFF0076FF, 0xFFD5FF00, 0xFFFF937E,
                0xFF6A826C, 0xFFFF029D, 0xFFFE8900, 0xFF7A4782, 0xFF7E2DD2, 0xFF85A900,
                0xFFFF0056, 0xFFA42400, 0xFF00AE7E, 0xFF683D3B, 0xFFBDC6FF, 0xFF263400,
                0xFFBDD393, 0xFF00B917, 0xFF9E008E, 0xFF001544, 0xFFC28C9F, 0xFFFF74A3,
                0xFF01D0FF, 0xFF004754, 0xFFE56FFE, 0xFF788231, 0xFF0E4CA1, 0xFF91D0CB,
                0xFFBE9970, 0xFF968AE8, 0xFFBB8800, 0xFF43002C, 0xFFDEFF74, 0xFF00FFC6,
                0xFFFFE502, 0xFF620E00, 0xFF008F9C, 0xFF98FF52, 0xFF7544B1, 0xFFB500FF,
                0xFF00FF78, 0xFFFF6E41, 0xFF005F39, 0xFF6B6882, 0xFF5FAD4E, 0xFFA75740,
                0xFFA5FFD2, 0xFFFFB167, 0xFF009BFF, 0xFFE85EBE
            };
            return Eng3D::Color::rgb32(colors[mod % 64]);
        }

        constexpr bool operator==(const Color& x) const noexcept {
            return this->get_value() == x.get_value();
        }
    };
}
