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
//      unified_render/ui/group.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/ui/numeric_input.hpp"
#include "unified_render/ui/widget.hpp"
#include "unified_render/ui/group.hpp"
#include "unified_render/ui/input.hpp"
#include "unified_render/ui/button.hpp"

UI::NumericInput::NumericInput(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Group(_x, _y, w, h, _parent)
{
    inp = new UI::Input(0, 0, w - 32 - 32, h, this);
    inp->set_buffer("100.0");

    add_btn = new UI::Button(0, 0, 32, h, this);
    add_btn->text("+");
    add_btn->right_side_of(*inp);
    add_btn->set_on_click([](UI::Widget& w) {
        auto* p = (UI::NumericInput*)w.parent;
        float val = atof(p->inp->get_buffer().c_str()) + 1.f;
        char tmpbuf[100];
        snprintf(tmpbuf, 100, "%.2f", val);
        p->set_buffer(tmpbuf);
    });

    sub_btn = new UI::Button(0, 0, 32, h, this);
    sub_btn->text("-");
    sub_btn->right_side_of(*add_btn);
    sub_btn->set_on_click([](UI::Widget& w) {
        auto* p = (NumericInput*)w.parent;
        float val = atof(p->get_buffer().c_str()) + 1.f;
        char tmpbuf[100];
        snprintf(tmpbuf, 100, "%.2f", val);
        p->set_buffer(tmpbuf);
    });
}

void UI::NumericInput::set_buffer(const std::string& _buffer) {
    inp->set_buffer(_buffer);
}

std::string UI::NumericInput::get_buffer(void) const {
    return inp->get_buffer();
}
