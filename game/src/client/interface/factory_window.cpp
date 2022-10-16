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
//      client/interface/factory_window.cpp
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

#include "client/interface/factory_window.hpp"
#include "client/client_network.hpp"
#include "nation.hpp"
#include "action.hpp"
#include "pop.hpp"
#include "world.hpp"

UI::Table<uint32_t>* Interface::FactoryWindow::new_table(GameState& gs, int _x, int _y, int _w, int _h, std::vector<ProvinceId> provinces, UI::Widget* parent) {
    std::vector<int> sizes;
    if(provinces.size() > 1) sizes.push_back(75);
    sizes.insert(sizes.end(), { 100, 50, 40, 32, 50, 32 });

    std::vector<std::string> header;
    if(provinces.size() > 1) header.push_back("Province");
    header.insert(header.end(), { "Type", "Workers", "Inputs", "Output", "Scale", "" });

    auto table = new UI::Table<uint32_t>(_x, _y, _w, _h, 32, sizes, header, parent);
    table->reserve(1);
    table->set_on_each_tick([&gs, table, provinces](UI::Widget&) {
        size_t row_num = 0;
        for(const auto province_id : provinces) {
            const auto& province = gs.world->provinces[province_id];
            for(size_t i = 0; i < province.buildings.size(); i++) {
                const auto& type = gs.world->building_types[i];
                const auto& building = province.buildings[i];
                auto* row = table->get_row(row_num++);

                size_t row_index = 0;

                if(provinces.size() > 1) {
                    auto prov_name = row->get_element(row_index++);
                    prov_name->text(province.name);
                    prov_name->set_key(province.name);
                }

                auto name = row->get_element(row_index++);
                name->text(type.name);
                name->set_key(type.name);

                auto workers = row->get_element(row_index++);
                workers->text(string_format("%.0f", building.workers));
                workers->set_key(building.workers);

                auto inputs = row->get_element(row_index++);
                inputs->set_key(type.input_ids.size());
                inputs->flex_justify = UI::FlexJustify::START;
                for(auto good_id : type.input_ids) {
                    auto& good = gs.world->goods[good_id];
                    auto input_img = new UI::Image(0, 0, 35, 35, good.get_icon_path(), true, inputs);
                    input_img->set_tooltip(good.name);
                }

                auto outputs = row->get_element(row_index++);
                outputs->set_key(type.output_id);
                outputs->flex_justify = UI::FlexJustify::START;
                if(Good::is_valid(type.output_id)) {
                    auto& output = gs.world->goods[type.output_id];
                    outputs->current_texture = gs.tex_man.load(output.get_icon_path());
                    outputs->set_tooltip(output.name);
                }

                auto scale = row->get_element(row_index++);
                scale->text(string_format("%.0f", building.level * building.production_scale));
                scale->set_key(building.level * building.production_scale);

                auto upgrade = row->get_element(row_index++);
                upgrade->text("+");
                upgrade->set_tooltip(translate("Upgrade building"));
                upgrade->set_key(0);
                upgrade->set_on_click([&gs, province_id, type_id = type.get_id()](UI::Widget&) {
                    gs.client->send(Action::BuildingAdd::form_packet(gs.world->provinces[province_id], gs.world->building_types[type_id]));
                });
            }
        }
    });
    table->on_each_tick(*table);
    return table;
}

Interface::FactoryWindow::FactoryWindow(GameState& gs)
    : UI::Window(-400, -400, 800, 800),
    gs{ gs }
{
    this->origin = UI::Origin::CENTER_SCREEN;
    this->text(translate("Factories"));
    this->is_scroll = false;
    this->set_close_btn_function([this](UI::Widget&) {
        this->kill();
    });
    auto table = this->new_table(gs, 5, 5, 800 - 10, 800 - 5, this->gs.curr_nation->owned_provinces, this);
    this->width = table->width + 5 + this->padding.x;
}
