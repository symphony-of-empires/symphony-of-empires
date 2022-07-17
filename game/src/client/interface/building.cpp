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
//      client/interface/building.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/serializer.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/close_button.hpp"
#include "eng3d/locale.hpp"

#include "client/interface/building.hpp"
#include "client/client_network.hpp"
#include "action.hpp"
#include "io_impl.hpp"

using namespace Interface;

BuildingBuildView::BuildingBuildView(GameState& _gs, int _tx, int _ty, bool _in_tile, Province& _province)
    : UI::Window(0, 0, 256, 256),
    gs{ _gs },
    province{ _province },
    in_tile{ _in_tile },
    tx{ _tx },
    ty{ _ty }
{
    this->is_scroll = true;
    this->text(Eng3D::Locale::translate("Construction of buildings"));
    this->set_close_btn_function([this](UI::Widget&) {
        this->kill();
    });

    auto* flex_column = new UI::Div(0, 0, this->width, this->height, this);
    flex_column->flex = UI::Flex::COLUMN;
    for(auto& building_type : gs.world->building_types) {
        auto* btn = new BuildingTypeButton(gs, 0, 0, building_type, flex_column);
        btn->set_on_click([this](UI::Widget& w) {
            const auto& building_type = ((BuildingTypeButton&)w).building_type;
            const_cast<Province&>(this->province).add_building(building_type);
            g_client->send(Action::BuildingAdd::form_packet(this->province, building_type));
            this->gs.ui_ctx.prompt("Production", "Building " + building_type.name + " in " + this->province.ref_name + " by " + this->province.controller->name);
        });
    }
}
