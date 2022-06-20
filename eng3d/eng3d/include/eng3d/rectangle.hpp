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
//      rectangle.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <glm/vec2.hpp>
#include <glm/common.hpp>

namespace Eng3D {
    class Rectangle {
    public:
        float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;

        constexpr Rectangle() = default;

        constexpr Rectangle(float x, float y, float width, float height)
            : left{ x },
            top{ y },
            right{ x + width },
            bottom{ y + height }
        {

        }

        constexpr Rectangle(glm::vec2 position, glm::vec2 size)
            : left{ position.x },
            top{ position.y },
            right{ position.x + size.x },
            bottom{ position.y + size.y }
        {

        }

        ~Rectangle() = default;

        /**
         * @brief Obtains the width
         * 
         * @return float Width of the rectangle
         */
        constexpr float width() const {
            return right - left;
        }

        /**
         * @brief Obtains the height
         * 
         * @return float Height of the rectangle
         */
        constexpr float height() const {
            return bottom - top;
        }

        /**
         * @brief Obtains the current size of the rectangle
         * 
         * @return glm::vec2 The size of the rectangle
         */
        constexpr glm::vec2 size() const {
            return glm::vec2{ right - left, bottom - top };
        }

        /**
         * @brief Sets the size of the rectangle
         * 
         * @param size Size to set it to
         */
        constexpr void size(glm::vec2 size) {
            right = left + size.x;
            bottom = top + size.y;
        }

        template<typename T>
        constexpr void size(T x, T y) {
            size(glm::vec2(x, y));
        }

        /**
         * @brief Obtains the current position of the rectangle
         * 
         * @return glm::vec2 The base position
         */
        constexpr glm::vec2 position() const {
            return glm::vec2{ left, top };
        }

        /**
         * @brief Sets the base position of the rectangle, modifying it's size
         * 
         * @param position Position to set
         */
        constexpr void position(glm::vec2 position) {
            left = position.x;
            top = position.y;
        }

        template<typename T>
        constexpr void position(T x, T y) {
            position(glm::vec2(x, y));
        }

        /**
         * @brief Scales the rectangle by factor
         * 
         * @param factor Factor to scale rectangle by
         */
        constexpr void scale(glm::vec2 factor) {
            left *= factor.x;
            top *= factor.y;
            right *= factor.x;
            bottom *= factor.y;
        }

        template<typename T>
        constexpr void scale(T x, T y) {
            scale(glm::vec2(x, y));
        }

        /**
         * @brief Offset the rectangle by the given parameter
         * 
         * @param offset Offset to apply to the rectangle
         */
        constexpr void offset(glm::vec2 offset) {
            left += offset.x;
            top += offset.y;
            right += offset.x;
            bottom += offset.y;
        }

        template<typename T>
        constexpr void offset(T x, T y) {
            offset(glm::vec2(x, y));
        }

        /**
         * @brief Checks if the rectangle is in bounds
         * 
         * @param pos Position to check (rectangle must cover this point)
         * @return true Rectangle not in bounds
         * @return false Rectangle is in bounds
         */
        constexpr bool in_bounds(glm::vec2 pos) const {
            return (pos.x >= left && pos.x <= right && pos.y >= top && pos.y <= bottom);
        }

        template<typename T>
        constexpr bool in_bounds(T x, T y) const {
            return in_bounds(glm::vec2(x, y));
        }

        /**
         * @brief Obtains the intersection rectangle from two other rectangles R1 and R2
         * 
         * @param rect Second rectangle to perform the intersection for
         * @return Rectangle Intersection area rectangle
         */
        constexpr Rectangle intersection(const Rectangle& rect) const {
            const float i_left = glm::max(this->left, rect.left);
            const float i_top = glm::max(this->top, rect.top);
            const float i_right = glm::min(this->right, rect.right);
            const float i_bottom = glm::min(this->bottom, rect.bottom);
            return Eng3D::Rectangle{ i_left, i_top, i_right - i_left, i_bottom - i_top };
        }

        /**
         * @brief Obtains the intersection rectangle from two other rectangles R1 and R2
         * 
         * @param rect Second rectangle to perform the intersection for
         * @return Rectangle Intersection area rectangle
         */
        constexpr Rectangle join(const Rectangle& rect) const {
            const float i_left = glm::min(this->left, rect.left);
            const float i_top = glm::min(this->top, rect.top);
            const float i_right = glm::max(this->right, rect.right);
            const float i_bottom = glm::max(this->bottom, rect.bottom);
            return Eng3D::Rectangle{ i_left, i_top, i_right - i_left, i_bottom - i_top };
        }
    };

    typedef class Rectangle Rect;
}