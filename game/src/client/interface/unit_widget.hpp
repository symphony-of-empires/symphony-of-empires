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
//      eng3d/ui/unit_widget.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/ui/div.hpp"
#include "eng3d/ui/window.hpp"
#include "world.hpp"

class Map;
class GameState;
class Unit;
namespace UI {
    class Image;
    class ProgressBar;
}

namespace Interface {
    class UnitWidget: public UI::Div {
        UnitId unit_id;
        Map& map;
        GameState& gs;
        std::shared_ptr<Eng3D::Texture> select_border_texture;
        UI::Image* flag_img;
        UI::Div* size_label;
        UI::ProgressBar* experience_bar;
    public:
        UnitWidget(Map& map, GameState& gamestate, UI::Widget* parent);
        void set_unit(Unit& _unit);
        void set_size(size_t size);
        virtual ~UnitWidget() = default;
    };

    class UnitView: public UI::Window {
        GameState& gs;
        UnitId unit_id;
    public:
        UnitView(GameState& _gs, Unit& unit);
        virtual ~UnitView() = default;
    };
}
