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
//      client/ui/label.cpp
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

#include "client/ui/widget.hpp"
#include "client/ui/label.hpp"
#include "client/ui/ui.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

using namespace UI;

Label::Label(int _x, int _y, const std::string& _text, Widget* _parent)
    : Widget(_parent, _x, _y, 0, 0, UI::WidgetType::LABEL)
{
    text(_text);
    width = text_texture->width + text_offset_x;
    height = text_texture->height;
}

void Label::on_render(Context&, UnifiedRender::Rect viewport) {
    if(text_texture != nullptr) {
        if(!text_texture->gl_tex_num) {
            text_texture->to_opengl();
        }

        glColor3f(text_color.r, text_color.g, text_color.b);
        draw_rectangle(4, 0, text_texture->width, text_texture->height, viewport, text_texture->gl_tex_num);
    }
}

void Label::text(const std::string& _text) {
    UI::Widget::text(_text);

    width = text_texture->width + text_offset_x;
    height = text_texture->height;
}