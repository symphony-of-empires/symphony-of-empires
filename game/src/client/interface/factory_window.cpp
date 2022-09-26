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

#include "client/interface/factory_window.hpp"
#include "nation.hpp"
#include "pop.hpp"
#include "world.hpp"

#include "eng3d/string.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/ui/table.hpp"
#include "eng3d/string.hpp"

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

    std::vector<int> sizes{ 75, 100, 100, 180, 32, 50 };
    std::vector<std::string> header{ "Province", "Type", "Workers", "Inputs", "Output", "Scale" };
    auto table = new UI::Table<uint64_t>(5, 5, 800 - 10, 800 - 5, 35, sizes, header, this);
    this->width = table->width + 5 + this->padding.x;
    table->reserve(1);
    table->set_on_each_tick([this, table](UI::Widget&) {
        size_t row_num = 0;
        for(const auto province_id : this->gs.curr_nation->owned_provinces) {
            const auto& province = this->gs.world->provinces[province_id];
            for(size_t i = 0; i < province.buildings.size(); i++) {
                const auto& type = this->gs.world->building_types[i];
                const auto& building = province.buildings[i];
                auto* row = table->get_row(row_num++);

                size_t row_index = 0;
                auto prov_name = row->get_element(row_index++);
                prov_name->text(province.name.get_string());
                prov_name->set_key(province.name.get_string());

                auto name = row->get_element(row_index++);
                name->text(type.name.get_string());
                name->set_key(type.name.get_string());

                auto workers = row->get_element(row_index++);
                workers->text(string_format("%.0f", building.workers));
                workers->set_key(building.workers);

                auto inputs = row->get_element(row_index++);
                inputs->set_key(type.input_ids.size());
                inputs->flex_justify = UI::FlexJustify::START;
                for(auto good_id : type.input_ids) {
                    auto& good = g_world.goods[good_id];
                    auto input_img = new UI::Image(0, 0, 35, 35, "gfx/good/" + good.ref_name + ".png", true, inputs);
                    input_img->set_tooltip(good.name.get_string());
                }

                auto outputs = row->get_element(row_index++);
                outputs->set_key(type.output_id);
                outputs->flex_justify = UI::FlexJustify::START;
                if(Good::is_valid(type.output_id)) {
                    auto& output = this->gs.world->goods[type.output_id];
                    outputs->current_texture = this->gs.tex_man.load(this->gs.package_man.get_unique("gfx/good/" + output.ref_name + ".png"));
                    outputs->set_tooltip(output.name.get_string());
                }

                auto scale = row->get_element(row_index++);
                scale->text(string_format("%.0f", building.production_scale));
                scale->set_key(building.production_scale);
            }
        }
    });
    table->on_each_tick(*table);
}
