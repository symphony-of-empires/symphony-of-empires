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
//      client/interface/industry_window.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/string.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/ui/table.hpp"
#include "eng3d/string.hpp"

#include "client/interface/industry_window.hpp"
#include "client/client_network.hpp"
#include "world.hpp"
#include "action.hpp"

UI::Table<uint32_t>* Interface::IndustryWindow::new_table(GameState& gs, int _x, int _y, int _w, int _h, std::vector<ProvinceId> provinces, UI::Widget* parent) {
    std::vector<int> sizes;
    if(provinces.size() > 1) sizes.push_back(150);
    sizes.insert(sizes.end(), { 150, 80, 80, 100, 80, 80, 80, 32 });

    std::vector<std::string> header;
    if(provinces.size() > 1) header.push_back("Province");
    header.insert(header.end(), { "Type", "Workers", "Budget", "Inputs", "Output", "Scale", "Profit", "" });

    auto& table = parent->make_widget<UI::Table<uint32_t>>(_x, _y, _h, 32, sizes, header);
    table.reserve(1);
    table.set_on_each_tick([&gs, &table, provinces](UI::Widget&) {
        size_t row_num = 0;
        for(const auto province_id : provinces) {
            const auto& province = gs.world->provinces[province_id];
            for(size_t i = 0; i < province.buildings.size(); i++) {
                const auto& type = gs.world->building_types[i];
                const auto& building = province.buildings[i];
                auto& row = table.get_row(row_num++);

                size_t row_index = 0;
                if(provinces.size() > 1) {
                    auto* prov_name = row.get_element(row_index++);
                    prov_name->set_text(province.name.data());
                    prov_name->set_key(province.name.data());
                }

                auto* name = row.get_element(row_index++);
                name->set_text(type.name.data());
                name->set_key(type.name.data());

                auto* workers = row.get_element(row_index++);
                workers->set_text(string_format("%.0f", building.workers));
                workers->set_key(building.workers);

                auto* budget = row.get_element(row_index++);
                budget->set_text(string_format("%.0f", building.budget));
                budget->set_key(building.budget);

                row_index++; // Inputs
                row_index++; // Outputs

                auto* scale = row.get_element(row_index++);
                scale->set_text(string_format("%.2f", building.production_scale));
                scale->set_key(building.production_scale);
                scale->set_tooltip(translate_format("Production scale %.2f, level %.2f", building.production_scale, building.level));

                auto* profit = row.get_element(row_index++);
                profit->set_text(string_format("%.2f", building.get_profit()));
                profit->set_key(building.get_profit());
                profit->set_tooltip(translate_format(
                    "Profit: %.2f\n"
                    "[Expenses]\n"
                    "Inputs cost: %.2f\n"
                    "Wages: %.2f\n"
                    "State taxes: %.2f\n"
                    "Dividends: %.2f (%.2f to state, %.2f to pops, %.2f to private investors)\n"
                    "Total expenses: %.2f\n"
                    "[Revenue]\n"
                    "Outputs revenue: %.2f\n"
                    "Total revenue: %.2f\n"
                    "[Investments]\n"
                    "Collective: %.2f\n"
                    "Individual: %.2f\n"
                    "Private: %.2f\n"
                    "State: %.2f",
                    building.get_profit(),
                    building.expenses.inputs_cost,
                    building.expenses.wages,
                    building.expenses.state_taxes,
                    building.expenses.get_dividends(),
                    building.expenses.state_dividends,
                    building.expenses.pop_dividends,
                    building.expenses.private_dividends,
                    building.expenses.get_total(),
                    building.revenue.outputs,
                    building.revenue.get_total(),
                    building.estate_collective.today_funds,
                    building.estate_individual.today_funds,
                    building.estate_private.today_funds,
                    building.estate_state.today_funds
                ));

                const auto total_investment = building.get_total_investment();
                auto* upgrade = row.get_element(row_index++);
                upgrade->set_text("+");
                upgrade->set_tooltip(Eng3D::translate_format("Invest in building (upgrades to level %.2f)\nTotal invested: %.2f\nPrivate shares: %.2f%%\nState shares: %.2f%%\nCollective shares: %.2f%%\nIndividual shares: %.2f%%",
                    building.level,
                    building.get_total_investment(),
                    building.estate_private.get_ownership(total_investment) * 100.f,
                    building.estate_state.get_ownership(total_investment) * 100.f,
                    building.estate_collective.get_ownership(total_investment) * 100.f,
                    building.estate_individual.get_ownership(total_investment) * 100.f
                ));
                upgrade->set_key(0);
                upgrade->set_on_click([&gs, province_id, type_id = type.get_id()](UI::Widget&) {
                    gs.client->send(Action::BuildingAdd::form_packet(gs.world->provinces[province_id], gs.world->building_types[type_id]));
                });
            }
        }
    });
    table.on_each_tick(table);
    
    size_t row_num = 0;
    for(const auto province_id : provinces) {
        const auto& province = gs.world->provinces[province_id];
        for(size_t i = 0; i < province.buildings.size(); i++) {
            const auto& type = gs.world->building_types[i];
            auto& row = table.get_row(row_num++);

            size_t row_index = 0;

            if(provinces.size() > 1) row_index++;

            row_index++;
            row_index++;
            row_index++;

            auto* inputs = row.get_element(row_index++);
            inputs->set_key(type.input_ids.size());
            inputs->flex = UI::Flex::ROW;
            inputs->flex_justify = UI::FlexJustify::START;
            for(auto good_id : type.input_ids) {
                auto& commodity = gs.world->commodities[good_id];
                auto& input_img = inputs->make_widget<UI::Image>(0, 0, 35, 35, commodity.get_icon_path(), true);
                input_img.set_tooltip(commodity.name.data());
            }

            auto* outputs = row.get_element(row_index++);
            outputs->set_key(type.output_id.has_value() ? type.output_id.value() : CommodityId(0));
            outputs->flex = UI::Flex::ROW;
            outputs->flex_justify = UI::FlexJustify::START;

            if(type.output_id.has_value()) {
                auto& output = gs.world->commodities[type.output_id.value()];
                auto& output_img = outputs->make_widget<UI::Image>(0, 0, 35, 35, output.get_icon_path(), true);
                output_img.set_tooltip(output.name.data());
            }
        }
    }
    return &table;
}

Interface::IndustryWindow::IndustryWindow(GameState& _gs)
    : UI::Window(-400, -400, 800, 800),
    gs{ _gs }
{
    this->origin = UI::Origin::CENTER_SCREEN;
    this->set_text(translate("Factories"));
    this->is_scroll = false;
    this->set_close_btn_function([this](UI::Widget&) {
        this->kill();
    });
    auto table = this->new_table(gs, 5, 5, 800 - 10, 800 - 5, this->gs.curr_nation->owned_provinces, this);
    this->width = table->width + 5 + this->padding.x;
}
