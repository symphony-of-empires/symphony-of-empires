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
//      client/ui/tooltip.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/ui/tooltip.hpp"

#include "client/ui/widget.hpp"
#include "client/ui/label.hpp"
#include "client/ui/ui.hpp"

#include "unified_render/state.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"

using namespace UI;

Tooltip::Tooltip()
    : Widget()
{
    type = UI::WidgetType::TOOLTIP;
    have_shadow = true;
    width = 512;

    current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/window_background.png"));

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    border = new Border(g_ui_context->border_tex, size, texture_size);
}

Tooltip::Tooltip(Widget* parent, unsigned w, unsigned h)
    : Widget()
{
    parent->set_tooltip(this);
    type = UI::WidgetType::TOOLTIP;
    have_shadow = true;
    width = w;
    height = h;

    current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/window_background.png"));

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    border = new Border(g_ui_context->border_tex, size, texture_size);
}

// Tooltip::Tooltip(Widget* parent)
//     : Widget()
// {
//     if(parent != nullptr) {
//         parent->set_tooltip(this);
//     }
//     type = UI::WidgetType::TOOLTIP;
//     have_shadow = true;
//     width = 512;
//     height = 24;

//     current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/window_background.png"));

//     glm::ivec2 size(4, 4);
//     glm::ivec2 texture_size(10, 10);
//     border = new Border(g_ui_context->border_tex, size, texture_size);
// }

void Tooltip::set_pos(int _x, int _y, int, int _height, int screen_w, int screen_h) {
    int extra_above = _y;
    int extra_below = screen_h - _y - _height;
    if(extra_above > extra_below) {
        y = _y - height - 10;
    }
    else {
        y = _y + _height + 10;
    }

    x = _x;
}

// Note! Code duplictation of Text::text 
void Tooltip::text(const std::string& text) {
    // Delete old labels in vector (if any)
    for(auto& lab : labels) {
        lab->kill();
    }
    labels.clear();

    if(text.empty()) {
        return;
    }

    // Separate the text in multiple labels and break on space
    // TODO: only works for monospace fonts width width 12, fix it for all fonts
    size_t pos = 0, y = 0;
    size_t line_width = std::max<size_t>(1, this->width / 12);
    while(pos < text.length()) {
        size_t remaining_chars = text.length() - pos;
        size_t end_pos = text.length();
        if(remaining_chars > line_width) {
            end_pos = pos + line_width;
        }
        bool break_line = false;
        for(int i = pos; i <= end_pos; i++) {
            if(text[i] == '\n') {
                end_pos = i;
                break_line = true;
                break;
            }
        }
        if(!break_line && remaining_chars > line_width) {
            for(int i = end_pos; i > pos; i--) {
                if(text[i] == ' ') {
                    end_pos = i;
                    break;
                }
            }
        }

        std::string buf = text.substr(pos, end_pos - pos);
        pos = end_pos;
        if (break_line) {
            pos++;
        }

        UI::Label* lab = new UI::Label(8, y, buf, this);
        labels.push_back(lab);

        y += 24;
    }
    height = y;
}