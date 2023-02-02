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
//      eng3d/ui/group.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/ui/numeric_input.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/group.hpp"
#include "eng3d/ui/input.hpp"
#include "eng3d/ui/button.hpp"

UI::NumericInput::NumericInput(int _x, int _y, unsigned _w, unsigned _h, Widget* _parent)
    : Group(_x, _y, _w, _h, _parent)
{
    inp = &this->make_widget<UI::Input>(0, 0, width - 32 - 32, height);
    inp->set_buffer("100.0");

    add_btn = &this->make_widget<UI::Button>(0, 0, 32, height);
    add_btn->set_text("+");
    add_btn->right_side_of(*inp);
    add_btn->set_on_click([](UI::Widget& w) {
        auto* p = (UI::NumericInput*)w.parent;
        float val = atof(p->inp->get_buffer().data()) + 1.f;
        char tmpbuf[100];
        snprintf(tmpbuf, 100, "%.2f", val);
        p->set_buffer(tmpbuf);
    });

    sub_btn = &this->make_widget<UI::Button>(0, 0, 32, height);
    sub_btn->set_text("-");
    sub_btn->right_side_of(*add_btn);
    sub_btn->set_on_click([](UI::Widget& w) {
        auto* p = (NumericInput*)w.parent;
        float val = atof(p->get_buffer().data()) + 1.f;
        char tmpbuf[100];
        snprintf(tmpbuf, 100, "%.2f", val);
        p->set_buffer(tmpbuf);
    });
}

void UI::NumericInput::set_buffer(const std::string_view _buffer) {
    inp->set_buffer(_buffer);
}

std::string UI::NumericInput::get_buffer() const {
    return inp->get_buffer();
}
