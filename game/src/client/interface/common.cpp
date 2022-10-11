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
//      client/interface/common.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/texture.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/chart.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/checkbox.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/progress_bar.hpp"
#include "eng3d/string.hpp"
#include "eng3d/string.hpp"

#include "client/interface/common.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "unit.hpp"
#include "province.hpp"
#include "product.hpp"
#include "building.hpp"
#include "client/client_network.hpp"
#include "action.hpp"

using namespace Interface;

UnitButton::UnitButton(GameState& _gs, int x, int y, Unit& _unit, UI::Widget* _parent)
    : UI::Button(x, y, _parent->width, 24, _parent),
    gs{ _gs },
    unit{ _unit }
{
    this->set_on_each_tick([this](UI::Widget& w) {
        auto& type = this->gs.world->unit_types[this->unit.type_id];
        w.text(string_format("%zu %s", this->unit.size, type.name.c_str()));
    });
    this->on_each_tick(*this);
}

UnitTypeButton::UnitTypeButton(GameState& _gs, int x, int y, UnitType& _unit_type, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width, 24, _parent),
    gs{ _gs },
    unit_type{ _unit_type }
{
    this->is_scroll = false;

    this->icon_img = new UI::Image(0, 0, 32, 24, this);
    this->icon_img->current_texture = this->gs.tex_man.load(gs.package_man.get_unique(unit_type.get_icon_path()));

    this->name_btn = new UI::Button(0, 0, this->width - 32, 24, this);
    this->name_btn->right_side_of(*this->icon_img);
    this->name_btn->text(this->unit_type.name);
}

NationButton::NationButton(GameState& _gs, int x, int y, Nation& _nation, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width, 24, _parent),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = false;

    this->flag_icon = new UI::Image(0, 0, 32, 24, this);
    this->flag_icon->current_texture = this->gs.get_nation_flag(this->nation);
    this->flag_icon->set_on_each_tick([this](UI::Widget& w) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;
        w.current_texture = this->gs.get_nation_flag(this->nation);
    });

    this->name_btn = new UI::Button(0, 0, this->width - 32, 24, this);
    this->name_btn->right_side_of(*this->flag_icon);
    this->name_btn->text(nation.get_client_hint().alt_name);
    this->name_btn->set_on_each_tick([](UI::Widget& w) {
        auto& o = static_cast<NationButton&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) return;
        w.text(o.nation.get_client_hint().alt_name);
    });
}
