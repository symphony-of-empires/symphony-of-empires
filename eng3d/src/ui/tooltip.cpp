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
//      eng3d/ui/tooltip.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/state.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"

using namespace UI;

Tooltip::Tooltip()
    : Widget()
{
    this->type = UI::WidgetType::TOOLTIP;
    this->have_shadow = true;
    this->width = 512;

    this->current_texture = Eng3D::State::get_instance().ui_ctx.tooltip_tex;

    const glm::ivec2 size = glm::ivec2{ 4, 4 };
    const glm::ivec2 texture_size = glm::ivec2{ 10, 10 };
    this->border = UI::Border(g_ui_context->border_tex, size, texture_size);
}

Tooltip::Tooltip(Widget* parent, unsigned w, unsigned h)
    : Widget()
{
    this->parent = parent;
    this->parent->set_tooltip(this);
    this->type = UI::WidgetType::TOOLTIP;
    this->have_shadow = true;
    this->width = w;
    this->height = h;

    this->current_texture = Eng3D::State::get_instance().ui_ctx.tooltip_tex;

    const glm::ivec2 size = glm::ivec2{ 4, 4 };
    const glm::ivec2 texture_size = glm::ivec2{ 10, 10 };
    this->border = UI::Border(g_ui_context->border_tex, size, texture_size);
}

Tooltip::~Tooltip() {

}

void Tooltip::set_pos(int _x, int _y, int, int _height, int screen_w, int screen_h) {
    int extra_above = _y;
    int extra_below = screen_h - _y - _height;
    if(extra_above > extra_below) {
        y = _y - height - 10;
    } else {
        y = _y + _height + 10;
    }
    x = _x;
}

// Note! Code duplication of Text::text 
void Tooltip::text(const std::string& text) {
    children.clear();
    if(text.empty()) return;

    // Separate the text in multiple labels and break on space
    /// @todo only works for monospace fonts width width 12, fix it for all fonts
    size_t pos = 0, y = 0;
    size_t line_width = std::max<size_t>(1, this->width / 12);
    while(pos < text.length()) {
        size_t remaining_chars = text.length() - pos;
        size_t end_pos = text.length();
        if(remaining_chars > line_width) end_pos = pos + line_width;

        bool break_line = false;
        for(size_t i = pos; i <= end_pos; i++) {
            if(text[i] == '\n') {
                end_pos = i;
                break_line = true;
                break;
            }
        }

        if(!break_line && remaining_chars > line_width) {
            for(size_t i = end_pos; i > pos; i--) {
                if(text[i] == ' ') {
                    end_pos = i;
                    break;
                }
            }
        }

        auto buf = text.substr(pos, end_pos - pos);
        pos = end_pos;
        if(break_line) pos++;
        
        auto *lab = new UI::Label(8, y, " ", this);
        lab->text_color = Eng3D::Color(1.f, 1.f, 1.f);
        lab->text(buf);
        y += 24;
    }
    height = y;
}