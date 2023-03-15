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
//      client/interface/selected_units_menu.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/texture.hpp"
#include "eng3d/string.hpp"
#include "eng3d/ui/piechart.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/table.hpp"
#include "eng3d/ui/close_button.hpp"

#include "client/interface/selected_units_menu.hpp"
#include "client/game_state.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"
#include "world.hpp"
#include "action.hpp"
#include "client/client_network.hpp"

using namespace Interface;

void SelectedUnitsMenu::update_unit_list(std::vector<Unit::Id> selected_units, UI::Table<UnitId::Type>& unit_table) {
    unit_table.make_rows_unactive();
    for(auto& unit_id : selected_units) {
        auto& row = unit_table.get_row(unit_id);
        size_t row_index = 0;
        auto& unit = gs.world->unit_manager.units[unit_id];

        auto name = row.get_element(row_index++);
        name->set_text(string_format("%.2f", unit.size));
    }
    unit_table.clear_unactive_rows();
}

SelectedUnitsMenu::SelectedUnitsMenu(GameState& _gs)
    : UI::Window(-200, -300, 200, 300),
    gs{ _gs }
{
    this->origin = UI::Origin::MIDDLE_RIGHT_SCREEN;
    this->is_pinned = true;
    this->is_scroll = false;

    std::vector<int> sizes{ 160 };
    std::vector<std::string> header{ "Size" };
    auto& unit_table = this->make_widget<UI::Table<UnitId::Type>>(0, 0, this->height, 30, sizes, header);

    this->is_render = !this->gs.client_state.get_selected_units().empty();
    gs.client_state.add_listener([this, &unit_table] (std::vector<Unit::Id> selected_units) {
        this->is_render = !selected_units.empty();
        update_unit_list(selected_units, unit_table);
    });
}