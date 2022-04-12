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

#include "unified_render/string_format.hpp"
#include "unified_render/ui/div.hpp"
#include "unified_render/ui/label.hpp"
#include "unified_render/ui/image.hpp"
#include "unified_render/ui/scrollbar.hpp"

using namespace Interface;

void make_building_header(UI::Div* table) {
	auto* row = new UI::Div(0, 0, 800, 35, table);
	row->flex = UI::Flex::ROW;
	row->flex_justify = UI::FlexJustify::START;

	glm::ivec2 size(4, 4);
	glm::ivec2 texture_size(10, 10);
	auto tex_man = UnifiedRender::State::get_instance().tex_man;
	auto border_tex = tex_man->load(Path::get("gfx/border2.png"));
	auto border = UI::Border(border_tex, size, texture_size);

	auto name_lab = new UI::Div(0, 0, 150, 35, row);
	name_lab->text("Name");
	name_lab->border = border;

	auto workers_lab = new UI::Div(0, 0, 100, 35, row);
	workers_lab->text("Workers");
	workers_lab->border = border;

	auto province_lab = new UI::Div(0, 0, 200, 35, row);
	province_lab->text("Province");
	province_lab->border = border;

	auto input_lab = new UI::Div(0, 0, 150, 35, row);
	input_lab->text("Inputs");
	input_lab->border = border;

	auto output_lab = new UI::Div(0, 0, 35, 35, row);
	output_lab->text("Output");
	output_lab->border = border;

	auto production_scale_lab = new UI::Div(0, 0, 50, 35, row);
	production_scale_lab->text("Scale");
	production_scale_lab->border = border;
}

void make_building_row(UI::Div* table, Building& building, BuildingType* type, Province* province) {
	auto* row = new UI::Div(0, 0, 800, 35, table);
	row->flex = UI::Flex::ROW;
	row->flex_justify = UI::FlexJustify::START;

	glm::ivec2 size(4, 4);
	glm::ivec2 texture_size(10, 10);
	auto tex_man = UnifiedRender::State::get_instance().tex_man;
	auto border_tex = tex_man->load(Path::get("gfx/border2.png"));
	auto border = UI::Border(border_tex, size, texture_size);

	auto name_lab = new UI::Div(0, 0, 150, 35, row);
	name_lab->text(type->name);
	name_lab->border = border;

	auto workers_lab = new UI::Div(0, 0, 100, 35, row);
	workers_lab->text(UnifiedRender::string_format("%d", building.workers));
	workers_lab->border = border;

	auto province_lab = new UI::Div(0, 0, 200, 35, row);
	province_lab->text(province->name);
	province_lab->border = border;

	auto input_div = new UI::Div(0, 0, 150, 35, row);
	input_div->border = border;
	input_div->flex = UI::Flex::ROW;
	input_div->flex_justify = UI::FlexJustify::START;

	for(auto good : type->inputs) {
		auto input_good_image = new UI::Image(0, 0, 35, 35, "gfx/good/" + good->ref_name + ".png", true, input_div);
		input_good_image->set_tooltip(good->name);
	}

	auto output = type->output;
	auto output_div = new UI::Div(0, 0, 35, 35, row);
	output_div->border = border;
	auto output_good_image = new UI::Image(0, 0, 35, 35, "gfx/good/" + output->ref_name + ".png", true, output_div);
	output_good_image->set_tooltip(output->name);

	auto production_scale_lab = new UI::Div(0, 0, 50, 35, row);
	production_scale_lab->text(UnifiedRender::string_format("%1.2f", building.production_scale));
	production_scale_lab->border = border;
}

FactoryWindow::FactoryWindow(GameState& gs)
	: UI::Window(-400, -400, 800, 800),
	gs{ gs }
{
	this->origin = UI::Origin::CENTER_SCREEN;
	this->text("Factories");
	this->is_scroll = false;

	Nation* nation = gs.curr_nation;

	int size = 0;
	for(auto prov : nation->owned_provinces) {
		for(size_t i = 0; i < prov->buildings.size(); i++) {
			auto& building = prov->buildings[i];
			if(building.level != 0)
				size++;
		}
	}

	auto* header_column = new UI::Div(5, 5, 800 - 10, 35, this);
	make_building_header(header_column);

	auto* table = new UI::Div(5, 40, 800 - 10, 700, this);
	table->is_scroll = true;
	new UI::Scrollbar(700, 0, 20, 700, table);

	auto* flex_column = new UI::Div(0, 0, 800 - 10, size * 35, table);
	flex_column->flex = UI::Flex::COLUMN;
	flex_column->flex_justify = UI::FlexJustify::START;

	for(auto prov : nation->owned_provinces) {
		for(size_t i = 0; i < prov->buildings.size(); i++) {
			auto& building = prov->buildings[i];
			auto type = gs.world->building_types[i];
			if(building.level == 0)
				continue;

			make_building_row(flex_column, building, type, prov);
		}
	}
}