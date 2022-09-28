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
//      client/interface/army.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/string.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/chart.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/progress_bar.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/close_button.hpp"
#include "eng3d/ui/div.hpp"

#include "client/interface/army.hpp"
#include "client/map.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"

using namespace Interface;

ArmyUnitsTab::ArmyUnitsTab(GameState& _gs, int x, int y, std::function<bool(Unit& unit)> filter, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    auto* flex_column = new UI::Div(0, 0, this->width, this->height, this);
    flex_column->flex = UI::Flex::COLUMN;
    gs.world->unit_manager.for_each_unit([this, filter, flex_column](Unit& unit) {
        auto& type = this->gs.world->unit_types[unit.type_id];
        if(!filter || !filter(unit)) return;
        auto* btn = new UnitButton(this->gs, 0, 0, unit, flex_column);
        btn->set_on_click([](UI::Widget&) {

        });
    });
}

ArmyProductionTab::ArmyProductionTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height, parent),
    gs{ _gs }
{
    this->is_scroll = true;

    auto* flex_column = new UI::Div(0, 0, this->width, this->height, this);
    flex_column->flex = UI::Flex::COLUMN;
    for(auto& unit_type : gs.world->unit_types) {
        auto* btn = new UnitTypeButton(gs, 0, 0, unit_type, flex_column);
        btn->set_on_click([this, btn](UI::Widget&) {
            this->gs.production_queue.push_back(&btn->unit_type);
        });
    }

    // Chart showing total number of required materials
    this->reqmat_chart = new UI::Chart(0, 0, this->width, 128, flex_column);
    this->reqmat_chart->text("Material demand");
    this->reqmat_chart->set_on_each_tick([this](UI::Widget&) {
        float reqtotal = 0.f;
        for(const auto province_id : this->gs.curr_nation->owned_provinces) {
            const auto& province = gs.world->provinces[province_id];
            for(const auto& building : province.get_buildings()) {
                for(const auto& req : building.req_goods_for_unit)
                    reqtotal += req.second;
                for(const auto& req : building.req_goods)
                    reqtotal += req.second;
            }
        }

        this->reqmat_chart->data.push_back(reqtotal);
        if(this->reqmat_chart->data.size() >= 30)
            this->reqmat_chart->data.pop_back();
    });

    for(const auto province_id : gs.curr_nation->owned_provinces) {
        const auto& province = gs.world->provinces[province_id];
        for(const auto& building_type : gs.world->building_types) {
            if(!building_type.can_build_land_units())
                continue;
            new ArmyProductionUnitInfo(gs, 0, 0, province, building_type, flex_column);
        }
    }
}

ArmyProductionUnitInfo::ArmyProductionUnitInfo(GameState& _gs, int x, int y, const Province& _province, unsigned int _idx, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, 48, parent),
    gs{ _gs },
    province{ _province },
    idx{ _idx }
{
    const auto& building = province.get_buildings()[idx];
    this->is_scroll = false;

    this->unit_icon = new UI::Image(0, 0, 24, 24, this);
    if(UnitType::is_valid(building.working_unit_type_id))
        this->unit_icon->current_texture = gs.tex_man.load(gs.package_man.get_unique("gfx/" + this->gs.world->unit_types[building.working_unit_type_id].ref_name + ".png"));

    this->province_lab = new UI::Label(0, 0, "?", this);
    this->province_lab->right_side_of(*this->unit_icon);
    this->province_lab->set_on_each_tick([this](UI::Widget& w) {
        w.text(this->province.name.get_string());
    });
    this->province_lab->on_each_tick(*this->province_lab);

    this->name_lab = new UI::Label(0, 0, "?", this);
    this->name_lab->right_side_of(*this->province_lab);
    this->name_lab->set_on_each_tick([this](UI::Widget& w) {
        auto& building = this->province.get_buildings()[this->idx];
        w.text(UnitType::is_valid(building.working_unit_type_id) ? this->gs.world->unit_types[building.working_unit_type_id].name.get_string() : translate("No unit"));
    });
    this->name_lab->on_each_tick(*this->name_lab);

    auto* progress_pgbar = new UI::ProgressBar(0, 0, 128, 24, 0.f, 1.f, this);
    progress_pgbar->below_of(*this->name_lab);
    progress_pgbar->set_on_each_tick([this](UI::Widget& w) {
        auto& building = this->province.get_buildings()[this->idx];
        if(UnitType::is_invalid(building.working_unit_type_id)) return;
        auto full = 0.f, needed = 0.f;
        std::string text = "Needs ";
        for(size_t i = 0; i < building.req_goods_for_unit.size(); i++) {
            auto need_req = building.req_goods_for_unit[i];
            auto full_req = this->gs.world->unit_types[building.working_unit_type_id].req_goods[i];
            full += need_req.second - full_req.second;
            needed += need_req.second;
            text += translate_format("%.2f of %s (has %.2f)", need_req.second, this->gs.world->goods[need_req.first].name.c_str(), full_req.second);
        }
        needed = glm::min(needed, full);
        ((UI::ProgressBar&)w).set_value(full / needed);
        w.set_tooltip(text);
    });
    progress_pgbar->on_each_tick(*progress_pgbar);
}

ArmyView::ArmyView(GameState& _gs)
    : UI::Window(-400, 0, 400, _gs.height),
    gs{ _gs }
{
    if(this->gs.right_side_panel != nullptr)
        this->gs.right_side_panel->kill();
    this->gs.right_side_panel = this;
    this->set_close_btn_function([this](Widget&) {
        this->kill();
        this->gs.right_side_panel = nullptr;
    });

    this->origin = UI::Origin::UPPER_RIGHT_SCREEN;
    this->is_scroll = false;
    this->text(translate("Army management"));

    this->units_tab = new ArmyUnitsTab(gs, 0, 32, nullptr, this);
    this->units_tab->is_render = true;
    auto* army_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/military_score.png")), this);
    army_ibtn->set_on_click([this](UI::Widget&) {
        this->units_tab->is_render = true;
        this->units_tab->kill();
        this->units_tab = new ArmyUnitsTab(gs, 0, 32, [this](Unit& unit) {
            auto& type = this->gs.world->unit_types[unit.type_id];
            if(unit.owner_id != *this->gs.curr_nation)
                return false;
            else if(type.is_ground && !type.is_naval)
                return true;
            return false;
        }, this);
        this->production_tab->is_render = false;
    });
    army_ibtn->set_tooltip(translate("Land army"));

    auto* airforce_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/airforce.png")), this);
    airforce_ibtn->right_side_of(*army_ibtn);
    airforce_ibtn->set_on_click([this](UI::Widget&) {
        this->units_tab->is_render = true;
        this->units_tab->kill();
        this->units_tab = new ArmyUnitsTab(gs, 0, 32, [this](Unit& unit) {
            auto& type = this->gs.world->unit_types[unit.type_id];
            if(unit.owner_id != *this->gs.curr_nation)
                return false;
            else if(!type.is_ground && !type.is_naval)
                return true;
            return false;
        }, this);
        this->production_tab->is_render = false;
    });
    airforce_ibtn->set_tooltip(translate("Airforce"));
    
    auto* navy_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/navy.png")), this);
    navy_ibtn->right_side_of(*airforce_ibtn);
    navy_ibtn->set_on_click([this](UI::Widget&) {
        this->units_tab->is_render = true;
        this->units_tab->kill();
        this->units_tab = new ArmyUnitsTab(gs, 0, 32, [this](Unit& unit) {
            auto& type = this->gs.world->unit_types[unit.type_id];
            if(unit.owner_id != *this->gs.curr_nation)
                return false;
            else if(!type.is_ground && type.is_naval)
                return true;
            return false;
        }, this);
        this->production_tab->is_render = false;
    });
    navy_ibtn->set_tooltip(translate("Navy"));

    this->production_tab = new ArmyProductionTab(gs, 0, 32, this);
    this->production_tab->is_render = false;
    auto* production_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/production.png")), this);
    production_ibtn->right_side_of(*navy_ibtn);
    production_ibtn->set_on_click([this](UI::Widget&) {
        this->units_tab->is_render = false;
        this->production_tab->is_render = true;
    });
    production_ibtn->set_tooltip(translate("Military production"));

    army_ibtn->on_click(*army_ibtn);
}
