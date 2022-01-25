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

void Text::on_render(Context&, UnifiedRender::Rect) {
    // Do nothing!
}

void Text::text(const std::string& text) {
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
        size_t end_pos = text.length();
        size_t remaining_chars = text.length() - pos;
        if(remaining_chars > line_width) {
            end_pos = pos + line_width;
            for(int i = pos + line_width; i > pos; i--) {
                if(text[i] == ' ') {
                    end_pos = i;
                    break;
                }
            }
        }

        std::string buf = text.substr(pos, end_pos - pos);
        pos = end_pos;

        UI::Label* lab = new UI::Label(8, y, buf, this);
        labels.push_back(lab);

        y += 24;
    }
    height = y;
}