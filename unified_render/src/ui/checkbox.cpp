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
    UnifiedRender::TextureOptions mipmap_options{};
    mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    auto tex_man = UnifiedRender::State::get_instance().tex_man;
    unchecked_texture = &tex_man->load(Path::get("gfx/checkbox_false.png"), mipmap_options);
    checked_texture = &tex_man->load(Path::get("gfx/checkbox_true.png"), mipmap_options);

    box = new Div(0, 0, size, size, this);
    box->current_texture = unchecked_texture;
    on_click = on_click_default;
    text_offset_x = size + 4;
    clickable_effect = false;
}

bool Checkbox::get_value() {
    return value;
}

void Checkbox::set_value(bool checked) {
    value = checked;
    if(value) {
        box->current_texture = checked_texture;
    }
    else {
        box->current_texture = unchecked_texture;
    }
}

void Checkbox::set_on_click(std::function<void(Widget&)> on_click) {
    outside_on_click = on_click;
}

void Checkbox::on_click_default(Widget& w, void*) {
    Checkbox& checkbox = static_cast<Checkbox&>(w);
    checkbox.set_value(!checkbox.value);
    if(checkbox.outside_on_click) {
        checkbox.outside_on_click(w);
    }
}

void Checkbox::text(const std::string& _text) {
    UI::Widget::text(_text);

    width = text_texture->width + text_offset_x;
    height = std::max(text_texture->height, box->height);
}