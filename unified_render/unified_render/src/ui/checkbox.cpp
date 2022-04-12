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
//      unified_render/ui/checkbox.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/ui/checkbox.hpp"
#include "unified_render/ui/div.hpp"

#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/state.hpp"

using namespace UI;

const int UNLIMITED_SIZE = 65536;
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
    int min_size = std::min(w, h);
    init_checkbox(min_size);
}

void Checkbox::init_checkbox(int size) {
    UnifiedRender::TextureManager* tex_man = UnifiedRender::State::get_instance().tex_man;

    UnifiedRender::TextureOptions mipmap_options{};
    mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    this->unchecked_texture = tex_man->load(Path::get("gfx/checkbox_false.png"), mipmap_options);
    this->checked_texture = tex_man->load(Path::get("gfx/checkbox_true.png"), mipmap_options);

    this->box = new Div(0, 0, size, size, this);
    this->box->current_texture = this->unchecked_texture;
    this->on_click = ([this](UI::Widget& w) {
        this->set_value(!this->value);
        if(this->outside_on_click) {
            this->outside_on_click(w);
        }
    });
    this->text_offset_x = size + 4;
    this->clickable_effect = false;
}

bool Checkbox::get_value(void) const {
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
    this->outside_on_click = on_click;
}

void Checkbox::text(const std::string& _text) {
    UI::Widget::text(_text);
    this->width = this->text_texture->width + this->text_offset_x;
    this->height = std::max<size_t>(this->text_texture->height, this->box->height);
}