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
//      client/interface/pop_window.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/string.hpp"
#include "eng3d/ui/table.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/image.hpp"

#include "client/interface/pop_window.hpp"
#include "nation.hpp"
#include "pop.hpp"
#include "world.hpp"

Interface::PopWindow::PopWindow(GameState& gs)
    : UI::Window(-400, -400, 0, 800),
    gs{ gs }
{
    this->origin = UI::Origin::CENTER_SCREEN;
    this->text("Population");
    this->is_scroll = false;

    this->set_close_btn_function([this](UI::Widget&) {
        this->kill();
    });

    const auto& nation = *gs.curr_nation;

    int size = 0;
    for(const auto province_id : nation.owned_provinces)
        size += gs.world->provinces[province_id].pops.size();

    std::vector<int> sizes{ 75, 200, 100, 80, 80, 50 };
    std::vector<std::string> header{ "Size", "Province", "Type", "Militancy", "Literacy", "Budget" };
    auto table = new UI::Table<uint64_t>(5, 5, 800-10, 800-5, 35, sizes, header, this);
    this->width = table->width + 5 + this->padding.x;
    table->reserve(size);
    table->set_on_each_tick([this, &nation, table](UI::Widget&) {
        for(const auto province_id : nation.owned_provinces) {
            const auto& province = this->gs.world->provinces[province_id];
            for(const auto& pop : province.pops) {
                const auto id = pop.get_type_id() + (static_cast<uint64_t>(province) << 32);
                auto* row = table->get_row(id);
                size_t row_index = 0;

                auto size = row->get_element(row_index++);
                size->text(string_format("%.0f", pop.size));
                size->set_key(pop.size);

                auto prov_name = row->get_element(row_index++);
                prov_name->text(province.name.get_string());
                prov_name->set_key(province.name.get_string());

                auto type = row->get_element(row_index++);
                type->text(this->gs.world->pop_types[pop.type_id].name.get_string());
                type->set_key(this->gs.world->pop_types[pop.type_id].name.get_string());

                auto militancy = row->get_element(row_index++);
                militancy->text(string_format("%1.2f", pop.militancy));
                militancy->set_key(pop.militancy);

                auto literacy = row->get_element(row_index++);
                literacy->text(string_format("%2.0f%%", pop.literacy * 100));
                literacy->set_key(pop.literacy);

                auto budget = row->get_element(row_index++);
                budget->text(string_format("%.0f", pop.budget / pop.size));
                budget->set_key(pop.budget / pop.size);
                budget->set_tooltip(Eng3D::translate_format("Total budget: %.2f", pop.budget));
            }
        }
    });
    table->on_each_tick(*table);
}