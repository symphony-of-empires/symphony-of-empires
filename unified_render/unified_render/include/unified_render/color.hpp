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
//      color.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>

// Maybe make it a template so it could be either float or uint8_t
namespace UnifiedRender {
    class Color {
    public:
        float r, g, b;
        float a = 1.f;
    public:
        Color();
        Color(float red, float green, float blue, float alpha = 1.f);
        ~Color();

        uint32_t get_value() const;
        static Color rgb8(uint8_t red, uint8_t green, uint8_t blue);
        static Color rgba8(uint8_t red, uint8_t green, uint8_t blue,uint8_t alpha);
        static Color rgba32(uint32_t argb);
        static Color lerp(Color color1, Color color2, float lamda);
    };
};
