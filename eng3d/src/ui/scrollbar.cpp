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
//      eng3d/ui/scrollbar.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cmath>
#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/state.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"

UI::ScrollbarThumb::ScrollbarThumb(int _x, int _y, UI::Scrollbar* _parent)
    : UI::Widget(_parent, _x, _y, _parent->width, std::min<int>(_parent->height - 2 * _parent->width, 50), UI::WidgetType::SCROLLBAR_THUMB)
{
    auto& s = Eng3D::State::get_instance();
    this->is_pinned = true;
    this->set_on_drag([this](UI::Widget&, glm::ivec2 diff) {
        assert(this->parent != nullptr);
        const int btn_height = this->parent->width;
        const float track_height = static_cast<float>(this->parent->height - btn_height * 2);
        const auto y_bounds = this->parent->parent->get_y_bounds();
        // Drag force is basically the distance taken by the offset (of drag), divide by track length to get
        // a 0 to 1 number then multiply it with the total parent height to determine the total distance to
        // forcefully scroll with the size of the parent in account
        int drag_force = ((diff.y - (this->height / 2)) / track_height) * (y_bounds.y - y_bounds.x);
        if(this->parent->parent) {
            this->parent->parent->scroll(-drag_force);
            reinterpret_cast<UI::Scrollbar*>(this->parent)->update_thumb();
        }
    });
    
    this->set_on_click([this](UI::Widget&) {

    });
    this->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/scrollbar_drag.png"));
}

UI::Scrollbar::Scrollbar(int _x, int _y, unsigned w, unsigned h, UI::Widget* _parent)
    : UI::Widget(_parent, _x, _y, w, h, UI::WidgetType::SCROLLBAR)
{
    auto& s = Eng3D::State::get_instance();
    this->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/scrollbar.png"));

    this->is_pinned = true;
    this->flex = UI::Flex::COLUMN;
    this->flex_justify = UI::FlexJustify::SPACE_BETWEEN;

    auto* up_btn = new UI::Button(0, 0, this->width, this->width, this);
    up_btn->set_on_click([this](UI::Widget& w) {
        if(this->parent) {
            const auto y_bounds = this->parent->get_y_bounds();
            const float ratio = static_cast<float>(y_bounds.y - y_bounds.x) / static_cast<float>(this->height);
            this->parent->scroll(8 * ratio);
            this->update_thumb();
        }
    });

    up_btn->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/scrollbar_up.png"));

    auto* down_btn = new UI::Button(0, 0, this->width, this->width, this);
    down_btn->set_on_click([this](UI::Widget& w) {
        if(this->parent) {
            const auto y_bounds = this->parent->get_y_bounds();
            const float ratio = static_cast<float>(y_bounds.y - y_bounds.x) / static_cast<float>(this->height);
            this->parent->scroll(-8 * ratio);
            this->update_thumb();
        }
    });
    down_btn->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/scrollbar_down.png"));

    this->thumb_btn = new UI::ScrollbarThumb(0, 16, this);
}

/// @brief Updates the thumb position in respect to the current scroll positioning of the parent
void UI::Scrollbar::update_thumb() {
    const auto y_bounds = this->parent->get_y_bounds();
    const int btn_height = this->width;
    // The height of the track (scrollbar excluding buttons)
    const float track_height = static_cast<float>(this->height - btn_height * 2);
    // 0 to 1 of how much the parent was scrolled relative to it's max scrolling/height
    const float scrolled = static_cast<float>(abs(this->parent->scrolled_y)) / static_cast<float>(y_bounds.y - y_bounds.x);
    this->thumb_btn->set_y(btn_height + scrolled * track_height);
}
