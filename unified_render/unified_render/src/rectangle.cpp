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
//      rectangle.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/rectangle.hpp"

using namespace UnifiedRender;

Rectangle::Rectangle(float x, float y, float width, float height)
    : left{ x },
    top{ y },
    right{ x + width },
    bottom{ y + height }
{

}

Rectangle::~Rectangle(void) {

}

Rectangle Rectangle::intersection(const UnifiedRender::Rectangle& rect) const {
    float i_left = glm::max(this->left, rect.left);
    float i_top = glm::max(this->top, rect.top);
    float i_right = glm::min(this->right, rect.right);
    float i_bottom = glm::min(this->bottom, rect.bottom);
    return UnifiedRender::Rectangle{ i_left, i_top, i_right - i_left, i_bottom - i_top };
}
