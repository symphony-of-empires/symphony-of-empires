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
//      unified_render/ui/scrollbar.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/ui/scrollbar.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/ui/button.hpp"
#include "unified_render/state.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"

using namespace UI;

Scrollbar::Scrollbar(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::SCROLLBAR)
{
    auto tex_man = UnifiedRender::State::get_instance().tex_man;
    current_texture = &tex_man->load(Path::get("gfx/scrollbar.png"));

    is_pinned = true;
    flex = Flex::COLUMN;
    flex_justify = FlexJustify::SPACE_BETWEEN;

    auto up_btn = new Button(0, 0, w, w, this);
    up_btn->set_on_click([this](Widget&) {
        if(this->parent) this->parent->scroll(6);
    });
    up_btn->current_texture = &tex_man->load(Path::get("gfx/scrollbar_up.png"));

    auto down_btn = new Button(0, 0, w, w, this);
    down_btn->set_on_click([this](Widget&) {
        if(this->parent) this->parent->scroll(-6);
    });
    down_btn->current_texture = &tex_man->load(Path::get("gfx/scrollbar_down.png"));

    auto drag_btn = new Button(0, 100, w, std::min<int>(h - 2 * w, 50), this);
    drag_btn->is_pinned = true;
    drag_btn->set_on_click([this](Widget&) {
        // if(this->parent) this->parent->scroll(-6);
    });
    drag_btn->current_texture = &tex_man->load(Path::get("gfx/scrollbar_drag.png"));
}