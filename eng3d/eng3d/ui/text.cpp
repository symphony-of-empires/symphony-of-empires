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
//      eng3d/ui/text.cpp
//
// Abstract:
//      A text widget with support for multiple lines.
// ----------------------------------------------------------------------------

#include <string>
#include <codecvt>
#include "eng3d/ui/text.hpp"
#include "eng3d/ui/label.hpp"

using namespace UI;

UI::Text::Text(int _x, int _y, unsigned w, unsigned h, UI::Widget* _parent)
    : UI::Widget(_parent, _x, _y, w, h, UI::WidgetType::GROUP)
{
    this->text_color = this->parent->text_color;
    this->flex = UI::Flex::COLUMN;
}

/// @brief Create a new text object from a text string
/// @param _x X coordinate
/// @param _y Y coordinate
/// @param _text Text to generate from
/// @param _parent Parent of the object (required to properly auto adjust)
UI::Text::Text(int _x, int _y, const std::string_view _text, UI::Widget* _parent)
    : UI::Widget(_parent, _x, _y, 0, 0, UI::WidgetType::GROUP)
{
    this->text_color = this->parent->text_color;
    this->auto_adjust = true;
    this->width = this->parent->width;
    this->flex = UI::Flex::COLUMN;
    this->set_text(_text);
}

void UI::Text::on_render(Context&, Eng3D::Rect) {

}

void UI::Text::set_text(const std::string_view text) {
    this->kill_children();
    if(text.empty()) return;

    // Separate the text in multiple labels and break on space
    /// @todo only works for monospace fonts width width 12, fix it for all fonts
    size_t pos = 0;
    size_t max_width = 0, max_height = 0;
    size_t line_width = glm::max(this->width / 12, static_cast<size_t>(1));
    
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv_utf8_utf32;
    std::u32string unicode_text = conv_utf8_utf32.from_bytes(text.data());
    while(pos < unicode_text.length()) {
        size_t remaining_chars = unicode_text.length() - pos;
        size_t end_pos = unicode_text.length();
        if(remaining_chars > line_width)
            end_pos = pos + line_width;

        bool break_line = false;
        for(size_t i = pos; i <= end_pos; i++) {
            if(unicode_text[i] == U'\n') {
                end_pos = i;
                break_line = true;
                break;
            }
        }

        if(!break_line && remaining_chars > line_width) {
            for(size_t i = end_pos; i > pos; i--) {
                if(unicode_text[i] == U' ') {
                    end_pos = i;
                    break;
                }
            }
        }

        auto buf = unicode_text.substr(pos, end_pos - pos);
        pos = end_pos;
        if(break_line)
		    pos++;
        auto& lab = this->make_widget<UI::Label>(4, 4, buf);
        max_width = glm::max(max_width, lab.width);
        max_height += lab.height;
    }

    if(this->auto_adjust) {
        this->width = max_width + 4;
        this->height = max_height + 4;
    }
}
