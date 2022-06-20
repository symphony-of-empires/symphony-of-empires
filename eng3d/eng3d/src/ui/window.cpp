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
//      eng3d/ui/window.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/window.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/state.hpp"
#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"

using namespace UI;

Window::Window(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::WINDOW),
    is_movable{ true }
{
    auto& s = Eng3D::State::get_instance();
    this->padding = glm::ivec2(8, 24 + 8);
    this->current_texture = s.tex_man.load(s.package_man.get_unique("gfx/window_background.png"));
    this->is_scroll = true;
    this->text_color = Eng3D::Color(1.f, 1.f, 1.f);

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    glm::ivec2 offset(0, 24);
    this->border = Border(g_ui_context->border_tex, size, texture_size, offset);

    // set_have_close_btn(true);
}

// There are a number of improvement to be made here
// * This places the button on in the end of the children vector, 
//      would be better to have it in the beginning
// * This function is not general, would be nice to set your own exit btn
// * We should add an on_close event that is called when exiting
void Window::set_close_btn_function(std::function<void(Widget&)> _on_click) {
    if(_on_click) {
        if(!this->close_btn) {
            const int size = 24;
            auto padding = this->padding;
            auto* btn_wrapper = new Div(-size - padding.x, -padding.y, size, size, this);
            btn_wrapper->origin = UI::Origin::UPPER_RIGHT;

            new Image(0, 0, size, size, "gfx/ui/button/exit_btn_shadow.png", true, btn_wrapper);
            const int btn_size = (int)(size * 0.75f);
            const int offset = (size - btn_size) / 2;
            auto* btn = Image::make_transparent(offset, offset, btn_size, btn_size, "gfx/ui/button/exit_btn.png", true, btn_wrapper);
            btn->set_on_click(_on_click);
            this->close_btn = btn_wrapper;
        }
    } else {
        if(this->close_btn) {
            this->close_btn->kill();
            delete this->close_btn;
            this->close_btn = nullptr;
        }
    }
}