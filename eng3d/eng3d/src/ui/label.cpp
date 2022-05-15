// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
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
//      eng3d/ui/label.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec2.hpp>

#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"

using namespace UI;

Label::Label(int _x, int _y, const std::string& _text, Widget* _parent)
    : Widget(_parent, _x, _y, 0, 0, UI::WidgetType::LABEL)
{
    text(_text);
    width = text_texture->width + text_offset_x;
    height = text_texture->height;
}

Label::~Label(void) {
    
}

void Label::text(const std::string& _text) {
    UI::Widget::text(_text);
    if(text_texture != nullptr) {
        width = text_texture->width + text_offset_x;
        height = text_texture->height;
    }
}