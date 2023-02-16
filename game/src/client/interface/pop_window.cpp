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
#include "world.hpp"
#include "world.hpp"

std::string pop_qol_tooltip_text(const Pop& pop, const World* world) {
    std::string text = Eng3D::translate_format("Quality of life: %.3f\n\nNeed for commodities, ", pop.life_needs_met);
    const auto& needs_amounts = world->pop_types[pop.type_id].basic_needs_amount;
    auto total_factor = std::reduce(needs_amounts.begin(), needs_amounts.end());
    for (const auto& commodity : world->commodities) {
        if (needs_amounts[commodity] == 0) continue;
        const auto need_factor = needs_amounts[commodity] / total_factor;
        text += Eng3D::translate_format("\n%s: %.3f", commodity.name.data(), need_factor);
    }
    return text;
}

Interface::PopWindow::PopWindow(GameState& _gs)
    : UI::Window(-400, -400, 0, 800),
    gs{ _gs }
{
    this->origin = UI::Origin::CENTER_SCREEN;
    this->set_text("Population");
    this->is_scroll = false;

    this->set_close_btn_function([this](UI::Widget&) {
        this->kill();
    });

    const auto& nation = *gs.curr_nation;

    auto size = 0.f;
    for(const auto province_id : nation.owned_provinces)
        size += gs.world->provinces[province_id].pops.size();

    std::vector<int> sizes{ 75, 200, 100, 80, 80, 80, 50};
    std::vector<std::string> header{ "Size", "Province", "Type", "Militancy", "Literacy", "Budget", "QOL" };
    auto& table = this->make_widget<UI::Table<uint64_t>>(5, 5, 800 - 5, 35, sizes, header);
    this->width = table.width + 5 + this->padding.x;
    table.reserve(size);
    table.set_on_each_tick([this, &nation, &table](UI::Widget&) {
        for(const auto province_id : nation.owned_provinces) {
            const auto& province = this->gs.world->provinces[province_id];
            for(const auto& pop : province.pops) {
                const auto id = static_cast<size_t>(pop.type_id) + (static_cast<uint64_t>(province) << 32);
                auto& row = table.get_row(id);
                size_t row_index = 0;

                auto size_label = row.get_element(row_index++);
                size_label->set_text(string_format("%.0f", pop.size));
                size_label->set_key(pop.size);

                auto prov_name = row.get_element(row_index++);
                prov_name->set_text(province.name.data());
                prov_name->set_key(province.name.data());

                auto type_lable = row.get_element(row_index++);
                type_lable->set_text(this->gs.world->pop_types[pop.type_id].name.data());
                type_lable->set_key(this->gs.world->pop_types[pop.type_id].name.data());

                auto militancy = row.get_element(row_index++);
                militancy->set_text(string_format("%1.2f", pop.militancy));
                militancy->set_key(pop.militancy);

                auto literacy = row.get_element(row_index++);
                literacy->set_text(string_format("%2.0f%%", pop.literacy * 100));
                literacy->set_key(pop.literacy);

                auto budget = row.get_element(row_index++);
                budget->set_text(string_format("%.2f", pop.budget / pop.size));
                budget->set_key(pop.budget / pop.size);
                budget->set_tooltip(Eng3D::translate_format("Total budget: %.2f\nPublic debt: %.2f\nPrivate debt: %.2f", pop.budget, pop.public_debt, pop.private_debt));

                auto quality_of_life = row.get_element(row_index++);
                quality_of_life->set_text(string_format("%.2f", pop.life_needs_met));
                quality_of_life->set_key(pop.life_needs_met);
                quality_of_life->set_tooltip([pop, this]{
                    return pop_qol_tooltip_text(pop, this->gs.world);
                });
            }
        }
    });
    table.on_each_tick(table);
}
