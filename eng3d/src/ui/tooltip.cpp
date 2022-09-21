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
//      eng3d/ui/tooltip.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/text.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/state.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"

using namespace UI;

UI::Tooltip::Tooltip()
    : UI::Widget()
{
    this->type = UI::WidgetType::TOOLTIP;
    this->have_shadow = true;
    this->width = 512;

    this->current_texture = Eng3D::State::get_instance().ui_ctx.tooltip_tex;
    this->text_color = Eng3D::Color(1.f, 1.f, 1.f);

    const glm::ivec2 size{ 4, 4 };
    const glm::ivec2 texture_size{ 10, 10 };
    this->border = UI::Border(g_ui_context->border_tex, size, texture_size);
}

UI::Tooltip::Tooltip(UI::Widget* parent, unsigned w, unsigned h)
    : UI::Widget()
{
    this->parent = parent;
    this->parent->set_tooltip(this);
    this->type = UI::WidgetType::TOOLTIP;
    this->have_shadow = true;
    this->width = w;
    this->height = h;

    this->current_texture = Eng3D::State::get_instance().ui_ctx.tooltip_tex;
    this->text_color = Eng3D::Color(1.f, 1.f, 1.f);

    const glm::ivec2 size{ 4, 4 };
    const glm::ivec2 texture_size{ 10, 10 };
    this->border = UI::Border(g_ui_context->border_tex, size, texture_size);
}

void UI::Tooltip::set_pos(int _x, int _y, int, int _height, int screen_w, int screen_h) {
    int extra_above = _y;
    int extra_below = screen_h - _y - _height;
    if(extra_above > extra_below) {
        y = _y - height - 10;
    } else {
        y = _y + _height + 10;
    }
    x = _x;

    // Make sure the tooltip is visable when it's too long and that it doesn't just disappear
    if(this->x + this->width > screen_w)
        this->x = screen_w - this->width;
}

// Note! Code duplication of Text::text 
void UI::Tooltip::text(const std::string& text) {
    this->kill_children();
    if(text.empty()) return;
    this->width = Eng3D::State::get_instance().width - this->x;
    auto* text_txt = new UI::Text(0, 0, text, *this);
    this->width = text_txt->width;
    this->height = text_txt->height;
}
