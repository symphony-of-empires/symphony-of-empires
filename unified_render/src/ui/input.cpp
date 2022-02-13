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
//      unified_render/ui/input.cpp
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

#include "unified_render/ui/widget.hpp"
#include "unified_render/ui/input.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

UI::Input::Input(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::INPUT)
{
    this->on_textinput = ([](UI::Input& w, const char* input) {
        if(input != nullptr) {
            w.buffer += input;
        }

        if(!w.buffer.empty()) {
            if(input == nullptr) {
                w.buffer.resize(w.buffer.length() - 1);
            }

            if(w.buffer.empty()) {
                w.text(" ");
            } else {
                w.text(w.buffer);
            }
        }
    });
    on_click = &UI::Input::on_click_default;
    on_click_outside = &UI::Input::on_click_outside_default;
    on_update = &UI::Input::on_update_default;
}