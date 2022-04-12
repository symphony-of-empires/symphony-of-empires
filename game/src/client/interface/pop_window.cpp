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
//      client/interface/pop_window.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/pop_window.hpp"
#include "nation.hpp"
#include "pop.hpp"
#include "world.hpp"

#include "unified_render/string_format.hpp"
#include "unified_render/ui/div.hpp"
#include "unified_render/ui/label.hpp"
#include "unified_render/ui/image.hpp"

using namespace Interface;

void make_pop_header(UI::Div* table, const World* world) {
	auto* row = new UI::Div(0, 0, 800, 35, table);
	row->flex = UI::Flex::ROW;
	row->flex_justify = UI::FlexJustify::START;

	glm::ivec2 size(4, 4);
	glm::ivec2 texture_size(10, 10);
	auto tex_man = UnifiedRender::State::get_instance().tex_man;
	auto border_tex = tex_man->load(Path::get("gfx/border2.png"));
	auto border = UI::Border(border_tex, size, texture_size);

	auto size_lab = new UI::Div(0, 0, 75, 35, row);
	size_lab->text("Size");
	size_lab->border = border;

	auto prov_lab = new UI::Div(0, 0, 200, 35, row);
	prov_lab->text("Province");
	prov_lab->border = border;

	auto type_lab = new UI::Div(0, 0, 100, 35, row);
	type_lab->text("Type");
	type_lab->border = border;

	auto culture_lab = new UI::Div(0, 0, 100, 35, row);
	culture_lab->text("Culture");
	culture_lab->border = border;

	auto religion_icon = new UI::Image(0, 0, 35, 35, "gfx/religion/" + world->religions[0]->ref_name + ".png", row);
	religion_icon->set_tooltip("Religion");
	religion_icon->border = border;

	auto militancy_lab = new UI::Div(0, 0, 50, 35, row);
	militancy_lab->text("Mil");
	militancy_lab->border = border;

	auto consciousness_lab = new UI::Div(0, 0, 50, 35, row);
	consciousness_lab->text("Con");
	consciousness_lab->border = border;

	auto literacy_lab = new UI::Div(0, 0, 50, 35, row);
	literacy_lab->text("Lit");
	literacy_lab->border = border;

	auto budget_lab = new UI::Div(0, 0, 70, 35, row);
	budget_lab->text("Budget");
	budget_lab->border = border;
}

void make_pop_row(UI::Div* table, Pop& pop, Province* province) {
	auto* row = new UI::Div(0, 0, 800, 35, table);
	row->flex = UI::Flex::ROW;
	row->flex_justify = UI::FlexJustify::START;

	glm::ivec2 size(4, 4);
	glm::ivec2 texture_size(10, 10);
	auto tex_man = UnifiedRender::State::get_instance().tex_man;
	auto border_tex = tex_man->load(Path::get("gfx/border2.png"));
	auto border = UI::Border(border_tex, size, texture_size);

	auto size_lab = new UI::Div(0, 0, 75, 35, row);
	size_lab->text(std::to_string(pop.size));
	size_lab->border = border;

	auto prov_lab = new UI::Div(0, 0, 200, 35, row);
	prov_lab->text(province->name);
	prov_lab->border = border;

	auto type_lab = new UI::Div(0, 0, 100, 35, row);
	type_lab->text(pop.type->name);
	type_lab->border = border;

	auto culture_lab = new UI::Div(0, 0, 100, 35, row);
	culture_lab->text(pop.culture->name);
	culture_lab->border = border;

	auto religion_icon = new UI::Image(0, 0, 35, 35, "gfx/religion/" + pop.religion->ref_name + ".png", row);
	religion_icon->set_tooltip(pop.religion->name);
	religion_icon->border = border;

	auto militancy_lab = new UI::Div(0, 0, 50, 35, row);
	militancy_lab->text(UnifiedRender::string_format("%1.2f", pop.militancy));
	militancy_lab->border = border;

	auto consciousness_lab = new UI::Div(0, 0, 50, 35, row);
	consciousness_lab->text(UnifiedRender::string_format("%1.2f", pop.con));
	consciousness_lab->border = border;

	auto literacy_lab = new UI::Div(0, 0, 50, 35, row);
	literacy_lab->text(UnifiedRender::string_format("%2.0f%%", pop.literacy * 100));
	literacy_lab->border = border;

	auto budget_lab = new UI::Div(0, 0, 70, 35, row);
	budget_lab->text(UnifiedRender::string_format("%f", pop.budget));
	budget_lab->border = border;
}

PopWindow::PopWindow(GameState& gs)
	: UI::Window(-400, -400, 800, 800),
	gs{ gs }
{
	this->origin = UI::Origin::CENTER_SCREEN;
	this->text("Population");
	this->is_scroll = false;

	const Nation& nation = *gs.curr_nation;

	int size = 0;
	for(auto prov : nation.owned_provinces) {
		size += prov->pops.size();
	}

	auto* header_column = new UI::Div(5, 5, 800 - 10, 35, this);
	header_column->flex = UI::Flex::COLUMN;
	header_column->flex_justify = UI::FlexJustify::START;
	make_pop_header(header_column, gs.world);

	auto* table = new UI::Div(5, 40, 800 - 10, 800-40, this);
	table->is_scroll = true;

	auto* flex_column = new UI::Div(0, 0, 800 - 10, size * 35, table);
	flex_column->flex = UI::Flex::COLUMN;
	flex_column->flex_justify = UI::FlexJustify::START;

	for(auto prov : nation.owned_provinces) {
		for(auto& pop : prov->pops) {
			make_pop_row(flex_column, pop, prov);
		}
	}
}