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
//      eng3d/ui/text.cpp
//
// Abstract:
//      A text widget with support for multiple lines.
// ----------------------------------------------------------------------------

#include "eng3d/ui/text.hpp"
#include "eng3d/ui/label.hpp"

using namespace UI;

UI::Text::Text(int _x, int _y, unsigned w, unsigned h, UI::Widget* _parent)
    : UI::Widget(_parent, _x, _y, w, h, UI::WidgetType::GROUP)
{

}

/// @brief Create a new text object from a text string
/// @param _x X coordinate
/// @param _y Y coordinate
/// @param _text Text to generate from
/// @param _parent Parent of the object (required to properly auto adjust)
UI::Text::Text(int _x, int _y, const std::string& _text, UI::Widget& _parent)
    : UI::Widget(&_parent, _x, _y, 0, 0, UI::WidgetType::GROUP)
{
    this->auto_adjust = true;
    this->width = this->parent->width;
    this->text(_text);
}

void UI::Text::on_render(Context&, Eng3D::Rect) {

}

void UI::Text::text(const std::string& text) {
    this->kill_children();
    if(text.empty()) return;

    // Separate the text in multiple labels and break on space
    /// @todo only works for monospace fonts width width 12, fix it for all fonts
    size_t pos = 0, y = 0;
    size_t max_width = 0;
    size_t line_width = std::max<size_t>(1, this->width / 12);
    while(pos < text.length()) {
        size_t remaining_chars = text.length() - pos;
        size_t end_pos = text.length();
        if(remaining_chars > line_width)
            end_pos = pos + line_width;

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

        std::string buf = text.substr(pos, end_pos - pos);
        pos = end_pos;
        if(break_line) pos++;
        auto* lab = new UI::Label(0, y, buf, this);
        y += lab->height;
        max_width = glm::max<size_t>(max_width, lab->width);
    }

    if(this->auto_adjust) {
        this->width = max_width + 8;
        this->height = y + 8;
    }
}
