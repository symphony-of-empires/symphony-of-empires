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
//      rectangle.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <glm/vec2.hpp>
#include <glm/common.hpp>

namespace UnifiedRender {
    class Rectangle {
    public:
        float left, top, right, bottom;
        Rectangle(float x, float y, float width, float height);
        ~Rectangle();

        Rectangle(glm::vec2 position, glm::vec2 size)
            : left{ position.x },
            top{ position.y },
            right{ position.x + size.x },
            bottom{ position.y + size.y }
        {

        }

        inline glm::vec2 size(void) const {
            return glm::vec2{ right - left, bottom - top };
        }

        inline void size(glm::vec2 size) {
            right = left + size.x;
            bottom = top + size.y;
        }

        inline glm::vec2 position(void) const {
            return glm::vec2{ left, top };
        }

        inline void position(glm::vec2 position) {
            left = position.x;
            top = position.y;
        }

        inline void offset(glm::vec2 offset) {
            left += offset.x;
            top += offset.y;
            right += offset.x;
            bottom += offset.y;
        }

        inline bool in_bounds(glm::vec2 pos) const {
            return (pos.x >= left && pos.x <= right && pos.y >= top && pos.y <= bottom);
        }

        template<typename T>
        inline bool in_bounds(T x, T y) const {
            return in_bounds(glm::vec2(x, y));
        }

        Rectangle intersection(const Rectangle& rect) const;
    };

    typedef class Rectangle Rect;
}