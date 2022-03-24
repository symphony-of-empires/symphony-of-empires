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

#include "unified_render/serializer.hpp"
#include "unified_render/ui/close_button.hpp"
#include "unified_render/locale.hpp"

#include "client/interface/building.hpp"
#include "client/client_network.hpp"
#include "action.hpp"
#include "io_impl.hpp"

using namespace Interface;

BuildingSelectProvinceTab::BuildingSelectProvinceTab(GameState& _gs, int x, int y, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& province : gs.world->provinces) {
        if(province->owner != gs.curr_nation) {
            continue;
        }

        auto* btn = new ProvinceButton(gs, 0, 24 * i, province, this);
        btn->on_click = ([](UI::Widget& w) {
            auto& o = static_cast<BuildingBuildView&>(*w.parent->parent);
            o.province = static_cast<ProvinceButton&>(w).province;
        });
        i++;
    }
}

BuildingSelectNationTab::BuildingSelectNationTab(GameState& _gs, int x, int y, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& nation : gs.world->nations) {
        if(!nation->exists()) {
            continue;
        }
        
        auto* btn = new NationButton(gs, 0, 24 * i, nation, this);
        btn->on_click = ([](UI::Widget& w) {
            auto& o = static_cast<BuildingBuildView&>(*w.parent->parent);
            o.nation = static_cast<NationButton&>(w).nation;
        });
        i++;
    }
}

BuildingSelectTypeTab::BuildingSelectTypeTab(GameState& _gs, int x, int y, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& building_type : gs.world->building_types) {
        auto* btn = new BuildingTypeButton(gs, 0, 24 * i, building_type, this);
        btn->on_click = ([](UI::Widget& w) {
            auto& o = static_cast<BuildingBuildView&>(*w.parent->parent);
            if(o.nation == nullptr) {
                o.gs.ui_ctx->prompt("Error", "No nation selected");
                return;
            }
            
            if(o.province == nullptr) {
                o.gs.ui_ctx->prompt("Error", "Select a province to build it on");
                return;
            }

            if(o.gs.curr_nation->owned_provinces.empty()) {
                o.gs.ui_ctx->prompt("Error", "You do not own any provinces");
                return;
            }
            
            const BuildingType* building_type = ((BuildingTypeButton&)w).building_type;
            o.province->buildings[o.gs.world->get_id(*building_type)].level += 1;
            g_client->send(Action::BuildingAdd::form_packet(o.province, ((BuildingTypeButton&)w).building_type));
            o.gs.ui_ctx->prompt("Production", "Building a " + building_type->name + " in " + o.province->ref_name + "; owned by " + o.province->controller->name);
        });
        i++;
    }
}

BuildingBuildView::BuildingBuildView(GameState& _gs, int _tx, int _ty, bool _in_tile, Nation* _nation, Province* _province)
    : UI::Window(0, 0, 512, 512),
    gs{ _gs },
    tx{ _tx },
    ty{ _ty },
    in_tile{ _in_tile },
    nation{ _nation },
    province{ _province }
{
    this->is_scroll = false;
    this->text(UnifiedRender::Locale::translate("Build a new building"));

    this->province_tab = new BuildingSelectProvinceTab(gs, 128, 24, this);
    this->province_tab->is_render = false;

    this->nation_tab = new BuildingSelectNationTab(gs, 128, 24, this);
    this->nation_tab->is_render = false;

    this->type_tab = new BuildingSelectTypeTab(gs, 128, 24, this);
    this->type_tab->is_render = false;

    auto* province_btn = new UI::Button(0, 0, 128, 24, this);
    province_btn->text(UnifiedRender::Locale::translate("Province"));
    province_btn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<BuildingBuildView&>(*w.parent);
        o.province_tab->is_render = true;
        o.nation_tab->is_render = false;
        o.type_tab->is_render = false;
    });

    auto* nation_btn = new UI::Button(0, 0, 128, 24, this);
    nation_btn->below_of(*province_btn);
    nation_btn->text(UnifiedRender::Locale::translate("Nation"));
    nation_btn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<BuildingBuildView&>(*w.parent);
        o.province_tab->is_render = false;
        o.nation_tab->is_render = true;
        o.type_tab->is_render = false;
    });

    auto* build_btn = new UI::Button(0, 0, 128, 24, this);
    build_btn->below_of(*nation_btn);
    build_btn->text(UnifiedRender::Locale::translate("Build"));
    build_btn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<BuildingBuildView&>(*w.parent);
        o.province_tab->is_render = false;
        o.nation_tab->is_render = false;
        o.type_tab->is_render = true;
    });

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*build_btn);
    close_btn->text(UnifiedRender::Locale::translate("Close"));
}

BuildingView::BuildingView(GameState& _gs, Building* _building)
    : UI::Window(0, 0, 512, 512),
    gs{ _gs },
    building{ _building }
{
    this->is_scroll = false;
    this->text(UnifiedRender::Locale::translate("Information for this building"));

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    //close_btn->below_of(*build_btn);
    close_btn->text(UnifiedRender::Locale::translate("Close"));
}