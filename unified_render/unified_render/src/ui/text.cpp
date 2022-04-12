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
//      unified_render/ui/text.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/ui/text.hpp"
#include "unified_render/ui/label.hpp"

using namespace UI;

Text::Text(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::GROUP)
{

}

Text::~Text(void) {
    
}

void Text::on_render(Context&, UnifiedRender::Rect) {

}

void Text::text(const std::string& text) {
    children.clear();
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
        if(break_line) {
            pos++;
        }
        
        new UI::Label(0, y, buf, this);
        y += 24;
    }
    height = y;
}
