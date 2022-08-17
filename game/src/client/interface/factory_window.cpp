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
//      client/interface/factory_window.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/factory_window.hpp"
#include "nation.hpp"
#include "pop.hpp"
#include "world.hpp"

#include "eng3d/string_format.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/locale.hpp"

using namespace Interface;

void FactoryWindow::make_building_header(UI::Div& table) {
    auto* row = new UI::Div(0, 0, 800, 35, &table);
    row->flex = UI::Flex::ROW;
    row->flex_justify = UI::FlexJustify::START;

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    auto& tex_man = Eng3D::State::get_instance().tex_man;
    auto border_tex = tex_man.load(Eng3D::State::get_instance().package_man.get_unique("gfx/border2.png"));
    auto border = UI::Border(border_tex, size, texture_size);

    auto name_lab = new UI::Div(0, 0, 150, 35, row);
    name_lab->text(_("Name"));
    name_lab->border = border;

    auto workers_lab = new UI::Div(0, 0, 100, 35, row);
    workers_lab->text(_("Workers"));
    workers_lab->border = border;

    auto province_lab = new UI::Div(0, 0, 200, 35, row);
    province_lab->text(_("Province"));
    province_lab->border = border;

    auto input_lab = new UI::Div(0, 0, 150, 35, row);
    input_lab->text(_("Inputs"));
    input_lab->border = border;

    auto output_lab = new UI::Div(0, 0, 35, 35, row);
    output_lab->text(_("Output"));
    output_lab->border = border;

    auto production_scale_lab = new UI::Div(0, 0, 50, 35, row);
    production_scale_lab->text(_("Scale"));
    production_scale_lab->border = border;
}

void FactoryWindow::make_building_row(UI::Div& table, const Building& building, const BuildingType& type, const Province& province) {
    auto* row = new UI::Div(0, 0, 800, 35, &table);
    row->flex = UI::Flex::ROW;
    row->flex_justify = UI::FlexJustify::START;

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    auto& tex_man = Eng3D::State::get_instance().tex_man;
    auto border_tex = tex_man.load(Eng3D::State::get_instance().package_man.get_unique("gfx/border2.png"));
    auto border = UI::Border(border_tex, size, texture_size);

    auto name_lab = new UI::Div(0, 0, 150, 35, row);
    name_lab->text(type.name.get_string());
    name_lab->border = border;

    auto workers_lab = new UI::Div(0, 0, 100, 35, row);
    workers_lab->text(Eng3D::string_format("%d", building.workers));
    workers_lab->border = border;

    auto province_lab = new UI::Div(0, 0, 200, 35, row);
    province_lab->text(province.name.get_string());
    province_lab->border = border;

    auto input_div = new UI::Div(0, 0, 150, 35, row);
    input_div->border = border;
    input_div->flex = UI::Flex::ROW;
    input_div->flex_justify = UI::FlexJustify::START;

    for(auto good : type.inputs) {
        auto input_good_image = new UI::Image(0, 0, 35, 35, "gfx/good/" + good->ref_name + ".png", true, input_div);
        input_good_image->set_tooltip(good->name.get_string());
    }

    auto output = type.output;
    if(output != nullptr) {
        auto output_div = new UI::Div(0, 0, 35, 35, row);
        output_div->border = border;
        auto output_good_image = new UI::Image(0, 0, 35, 35, "gfx/good/" + output->ref_name + ".png", true, output_div);
        output_good_image->set_tooltip(output->name.get_string());
    }

    auto production_scale_lab = new UI::Div(0, 0, 50, 35, row);
    production_scale_lab->text(Eng3D::string_format("%1.2f", building.production_scale));
    production_scale_lab->border = border;
}

FactoryWindow::FactoryWindow(GameState& gs)
    : UI::Window(-400, -400, 800, 800),
    gs{ gs }
{
    this->origin = UI::Origin::CENTER_SCREEN;
    this->text("Factories");
    this->is_scroll = false;

    this->set_close_btn_function([this](Widget&) {
        this->kill();
    });

    int size = 0;
    for(const auto province_id : gs.curr_nation->owned_provinces) {
        const auto& province = gs.world->provinces[province_id];
        for(Building::Id i = 0; i < province.buildings.size(); i++) {
            auto& building = province.buildings[i];
            if(building.level != 0) size++;
        }
    }

    auto* header_column = new UI::Div(5, 5, 800 - 10, 35, this);
    this->make_building_header(*header_column);

    auto* table = new UI::Div(5, 40, 800 - 10, 700, this);
    table->is_scroll = true;

    auto* flex_column = new UI::Div(0, 0, 800 - 10, size * 35, table);
    flex_column->flex = UI::Flex::COLUMN;
    flex_column->flex_justify = UI::FlexJustify::START;
    for(const auto province_id : gs.curr_nation->owned_provinces) {
        const auto& province = gs.world->provinces[province_id];
        for(Building::Id i = 0; i < province.buildings.size(); i++) {
            const auto& building = province.buildings[i];
            const auto& type = gs.world->building_types[i];
            if(!building.level) continue;
            this->make_building_row(*flex_column, building, type, province);
        }
    }
}
