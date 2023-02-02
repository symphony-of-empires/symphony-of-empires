// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      eng3d/ui/checkbox.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/checkbox.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/state.hpp"

using namespace UI;

//
// Checkbox
//
Checkbox::Checkbox(Widget* _parent)
    : Widget(_parent, 0, 0, 24, 24, UI::WidgetType::CHECKBOX)
{
    init_checkbox(24);
}

Checkbox::Checkbox(int size, Widget* _parent)
    : Widget(_parent, 0, 0, size, size, UI::WidgetType::CHECKBOX)
{
    init_checkbox(size);
}

Checkbox::Checkbox(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::CHECKBOX)
{
    auto min_size = glm::min<int>(w, h);
    init_checkbox(min_size);
}

void Checkbox::init_checkbox(int size) {
    this->unchecked_texture = Eng3D::State::get_instance().tex_man.load(Eng3D::State::get_instance().package_man.get_unique("gfx/checkbox_false.png"));
    this->checked_texture = Eng3D::State::get_instance().tex_man.load(Eng3D::State::get_instance().package_man.get_unique("gfx/checkbox_true.png"));

    this->box = new Div(0, 0, size, size, this);
    this->box->current_texture = this->unchecked_texture;
    this->on_click = ([this](UI::Widget& w) {
        this->set_value(!this->value);
        if(this->outside_on_click)
            this->outside_on_click(w);
    });
    this->text_offset_x = size + 4;
    this->clickable_effect = false;
}

bool Checkbox::get_value() const {
    return this->value;
}

void Checkbox::set_value(bool checked) {
    this->value = checked;
    if(this->value) {
        this->box->current_texture = this->checked_texture;
    } else {
        this->box->current_texture = this->unchecked_texture;
    }
}

void Checkbox::set_on_click(std::function<void(Widget&)> _on_click) {
    this->outside_on_click = _on_click;
}

void Checkbox::set_text(const std::string_view _text) {
    UI::Widget::set_text(_text);
    this->width = this->get_text_texture()->width + this->text_offset_x;
    this->height = glm::max(this->get_text_texture()->height, this->box->height);
}