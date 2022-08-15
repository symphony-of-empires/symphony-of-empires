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
//      eng3d/ui/battle_widget.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/ui/div.hpp"

class Province;
class Map;
namespace UI {
    class Image;
    class ProgressBar;
}

namespace Interface {
    class BattleWidget: public UI::Div {
        Province* province;
        size_t idx;
        Map& map;

        UI::Image* left_flag_img;
        UI::Div* left_size_label;
        
        UI::Image* right_flag_img;
        UI::Div* right_size_label;
    public:
        BattleWidget(Map& map, UI::Widget* parent);
        void set_battle(Province& province, size_t idx);
        virtual ~BattleWidget() {};
    };
};
