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
#include "unified_render/ui/label.hpp"
#include "unified_render/ui/image.hpp"

using namespace Interface;

PopWindow::PopWindow(GameState& gs)
	: UI::Window(-400, -400, 0, 800),
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

	std::vector<int> sizes{ 75, 200, 100, 100, 120, 50, 50, 70, 70 };
	std::vector<std::string> header{ "Size", "Province", "Type", "Culture", "Religion", "Mil", "Con", "Lit", "Budget" };
	auto table = new UI::Table<uint64_t>(5, 5, 800-10, 800-5, 35, sizes, header, this);
	this->width = table->width + 5 + this->padding.x;
	table->reserve(size);
	table->on_each_tick = [nation, table](Widget&) {
		for(auto prov : nation.owned_provinces) {
			uint16_t prov_id = prov->cached_id;
			for(auto& pop : prov->pops) {
				u_int64_t id = pop.get_type_id();
				id += (uint64_t)prov_id << 32;
				auto* row = table->get_row(id);
				size_t row_index = 0;

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
                religion->flex = UI::Flex::ROW;
				religion->flex_justify = UI::FlexJustify::END;
				auto religion_str = UnifiedRender::Locale::translate(pop.religion->name.get_string());
				new UI::Label(0, 0, religion_str, religion);
				new UI::Image(0, 0, 35, 35, "gfx/religion/" + pop.religion->ref_name + ".png", true, religion);
				religion->set_key(religion_str);

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