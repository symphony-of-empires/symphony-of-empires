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
//      eng3d/ui/unit_widget.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/ui/div.hpp"
#include "unit.hpp"

class Map;
class Unit;
namespace UI {
    class Image;
    class ProgressBar;
}

namespace Interface {
    class UnitWidget: public UI::Div {
        Unit::Id unit_id;
        Map& map;
        UI::Image* flag_img;
        UI::Div* size_label;
        UI::ProgressBar* morale_bar;
    public:
        UnitWidget(Unit& unit, Map& map, UI::Widget* parent);
        void set_unit(Unit& _unit);
        void set_size(size_t size);
        virtual ~UnitWidget();
    };
};
