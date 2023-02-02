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
//      eng3d/ui/label.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>

#include <glm/vec2.hpp>

#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"

using namespace UI;

UI::Label::Label(int _x, int _y, const std::string_view _text, Widget* _parent)
    : Widget(_parent, _x, _y, 0, 0, UI::WidgetType::LABEL)
{
    this->text_color = this->parent->text_color;
    this->set_text(_text);
}

UI::Label::Label(int _x, int _y, const std::u32string& _text, Widget* _parent)
    : Widget(_parent, _x, _y, 0, 0, UI::WidgetType::LABEL)
{
    this->text_color = this->parent->text_color;
    this->set_text(_text);
}

UI::Label::~Label() {

}

void UI::Label::set_text(const std::string_view _text) {
    UI::Widget::set_text(_text);
    if(this->get_text_texture().get() != nullptr) {
        this->text_offset_x = this->text_offset_y = 0;
        this->width = this->get_text_texture()->width;
        this->height = this->get_text_texture()->height;
    }
}

void UI::Label::set_text(const std::u32string& _text) {
    UI::Widget::set_text(_text);
    if(this->get_text_texture().get() != nullptr) {
        this->text_offset_x = this->text_offset_y = 0;
        this->width = this->get_text_texture()->width;
        this->height = this->get_text_texture()->height;
    }
}
