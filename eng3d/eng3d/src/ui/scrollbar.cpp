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
//      eng3d/ui/scrollbar.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/state.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"

using namespace UI;

Scrollbar::Scrollbar(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::SCROLLBAR)
{
    this->current_texture = Eng3D::State::get_instance().tex_man->load(Eng3D::State::get_instance().package_man->get_unique("gfx/scrollbar.png"));

    this->is_pinned = true;
    this->flex = Flex::COLUMN;
    this->flex_justify = FlexJustify::SPACE_BETWEEN;

    UI::Button* up_btn = new UI::Button(0, 0, w, w, this);
    up_btn->set_on_click([this](Widget&) {
        if(this->parent) {
            this->parent->scroll(6);
        }
    });

    up_btn->current_texture = Eng3D::State::get_instance().tex_man->load(Eng3D::State::get_instance().package_man->get_unique("gfx/scrollbar_up.png"));

    UI::Button* down_btn = new UI::Button(0, 0, w, w, this);
    down_btn->set_on_click([this](Widget&) {
        if(this->parent) {
            this->parent->scroll(-6);
        }
    });
    down_btn->current_texture = Eng3D::State::get_instance().tex_man->load(Eng3D::State::get_instance().package_man->get_unique("gfx/scrollbar_down.png"));

    UI::Button* drag_btn = new UI::Button(0, 100, w, std::min<int>(h - 2 * w, 50), this);
    drag_btn->is_pinned = true;
    drag_btn->set_on_click([this](Widget&) {
        // if(this->parent) this->parent->scroll(-6);
    });
    drag_btn->current_texture = Eng3D::State::get_instance().tex_man->load(Eng3D::State::get_instance().package_man->get_unique("gfx/scrollbar_drag.png"));
}