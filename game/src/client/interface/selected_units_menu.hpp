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
//      client/interface/selected_units_menu.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/ui/group.hpp"
#include "eng3d/ui/window.hpp"
#include "eng3d/ui/slider.hpp"
#include "eng3d/ui/input.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/table.hpp"

#include "world.hpp"
#include "world.hpp"
#include "client/game_state.hpp"

namespace UI {
    class PieChart;
    class Chart;
}

namespace Interface {
    // View information related to the population and diversity of languages and religions
    // on the specified province
    class SelectedUnitsMenu : public UI::Window {
        GameState& gs;
        void update_unit_list(std::vector<Unit::Id> selected_units, UI::Table<UnitId::Type>& unit_table);
    public:
        SelectedUnitsMenu(GameState& gs);
    };

}
