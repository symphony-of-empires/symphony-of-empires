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
#include "unified_render/locale.hpp"
#include "unified_render/ui/table.hpp"

using namespace Interface;

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

	std::vector<int> sizes{ 75, 200, 100, 100, 35, 50, 50, 50, 70 };
	std::vector<std::string> header{ "Size", "Province", "Type", "Culture", "Religion", "Mil", "Con", "Lit", "Budget" };
	auto table = new UI::Table<uint64_t>(5, 5, 800-10, 500, 35, sizes, header, this);
	table->reserve(size);
	table->on_each_tick = [nation, table](Widget&) {
		for(auto prov : nation.owned_provinces) {
			uint16_t prov_id = prov->cached_id;
			for(auto& pop : prov->pops) {
				u_int64_t id = pop.get_type_id();
				id += prov_id << 32;
				auto* row = table->get_row(id);
				size_t row_index = 0;

				auto tex_man = UnifiedRender::State::get_instance().tex_man;

				auto size = row->get_element(row_index++);
				auto size_str = UnifiedRender::string_format("%.0f", pop.size);
				size->text(size_str);
				size->set_key(pop.size);

				auto prov_name = row->get_element(row_index++);
				prov_name->text(prov->name.get_string());
				prov_name->set_key(prov->name.get_string());

				auto type = row->get_element(row_index++);
				type->text(pop.type->name.get_string());
				type->set_key(pop.type->name.get_string());

				auto culture = row->get_element(row_index++);
				auto culture_str = UnifiedRender::Locale::translate(pop.culture->name.get_string());
				culture->text(culture_str);
				culture->set_key(culture_str);

				auto religion = row->get_element(row_index++);
				auto religion_icon = tex_man->load(Path::get("gfx/religion/" + pop.religion->ref_name + ".png"));
				religion->current_texture = religion_icon;
				auto religion_tip = UnifiedRender::Locale::translate(pop.religion->name.get_string());
				religion->set_tooltip(religion_tip);
				religion->set_key(religion_tip);

				auto militancy = row->get_element(row_index++);
				militancy->text(UnifiedRender::string_format("%1.2f", pop.militancy));
				militancy->set_key(pop.militancy);

				auto consciousness = row->get_element(row_index++);
				consciousness->text(UnifiedRender::string_format("%1.2f", pop.con));
				consciousness->set_key(pop.con);

				auto literacy = row->get_element(row_index++);
				literacy->text(UnifiedRender::string_format("%2.0f%%", pop.literacy * 100));
				literacy->set_key(pop.literacy);

				auto budget = row->get_element(row_index++);
				auto budget_str = UnifiedRender::string_format("%.0f", pop.budget / pop.size);
				budget->text(budget_str);
				auto budget_tip = UnifiedRender::Locale::translate("A total budget of") + " " + UnifiedRender::string_format("%.0f", pop.budget);
				budget->set_tooltip(budget_tip);
				budget->set_key(pop.budget / pop.size);
			}
		}
	};
	table->on_each_tick(*table);
}