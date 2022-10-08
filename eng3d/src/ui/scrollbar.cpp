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
//      eng3d/ui/scrollbar.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/state.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/log.hpp"

UI::ScrollbarThumb::ScrollbarThumb(int _x, int _y, UI::Scrollbar* _parent)
    : UI::Widget(_parent, _x, _y, _parent->width, 0, UI::WidgetType::SCROLLBAR_THUMB)
{
    auto& s = Eng3D::State::get_instance();
    //this->is_pinned = true;
    this->set_on_drag([this](UI::Widget&, glm::ivec2 diff) {
        assert(this->parent != nullptr);
        if(this->parent->parent != nullptr) {
            /// @todo Ratio is not taken in account!
            this->parent->parent->scroll(-diff.y / 2);
            static_cast<UI::Scrollbar*>(this->parent)->update_thumb();
        }
    });

    // Only set so we get the free UI hints
    this->set_on_click([this](UI::Widget&) { });
    this->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/scrollbar_drag.png"));
}

UI::Scrollbar::Scrollbar(int _x, int _y, unsigned h, UI::Widget* _parent)
    : UI::Widget(_parent, _x, _y, 20, h, UI::WidgetType::SCROLLBAR)
{
    auto& s = Eng3D::State::get_instance();
    this->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/scrollbar.png"));

    this->is_pinned = true;
    //this->flex = UI::Flex::COLUMN;
    //this->flex_justify = UI::FlexJustify::SPACE_BETWEEN;

    auto* up_btn = new UI::Button(0, 0, 20, 20, this);
    up_btn->set_on_click([this](UI::Widget&) {
        if(this->parent) {
            const auto y_bounds = this->parent->get_y_bounds();
            const float ratio = static_cast<float>(y_bounds.y - y_bounds.x) / static_cast<float>(this->height);
            this->parent->scroll(16 * ratio);
            this->update_thumb();
        }
    });
    up_btn->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/scrollbar_up.png"));

    auto* down_btn = new UI::Button(0, this->height - 20, 20, 20, this);
    down_btn->set_on_click([this](UI::Widget&) {
        if(this->parent) {
            const auto y_bounds = this->parent->get_y_bounds();
            const float ratio = static_cast<float>(y_bounds.y - y_bounds.x) / static_cast<float>(this->height);
            this->parent->scroll(-16 * ratio);
            this->update_thumb();
        }
    });
    down_btn->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/scrollbar_down.png"));

    this->thumb_btn = new UI::ScrollbarThumb(0, 0, this);
    this->update_thumb();
}

/// @brief Updates the thumb position in respect to the current scroll positioning of the parent
void UI::Scrollbar::update_thumb() {
    const auto y_bounds = this->parent->get_y_bounds();
    const float parent_height = static_cast<float>(y_bounds.y - y_bounds.x);
    const auto btn_height = 20;
    // The height of the track (scrollbar excluding buttons)
    const float track_height = static_cast<float>(this->height - this->thumb_btn->height - btn_height * 2);
    // 0 to 1 of how much the parent was scrolled relative to it's max scrolling/height
    const auto scrolled = glm::abs<float>(this->parent->scrolled_y) / parent_height;
    // Ratio of pixels in track v. pixels in widget
    const auto ratio = track_height / parent_height;
    this->thumb_btn->set_y(btn_height + scrolled * track_height);
    this->thumb_btn->height = glm::clamp<size_t>(ratio * track_height, 20.f, track_height);
}
