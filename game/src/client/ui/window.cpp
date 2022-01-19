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
//      client/ui/window.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/ui/window.hpp"
#include "client/ui/ui.hpp"
#include "unified_render/state.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"

using namespace UI;

Window::Window(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::WINDOW),
    is_movable{ true }
{
    padding = glm::ivec2(8, 24 + 8);
    current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/window_background.png"));
    is_scroll = true;

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    glm::ivec2 offset(0, 24);
    border = new Border(g_ui_context->border_tex, size, texture_size, offset);
}