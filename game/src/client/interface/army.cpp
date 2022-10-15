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
        auto* btn = new UI::Button(0, 0, this->width, 24, flex_column);
        btn->set_on_each_tick([this, unit_id = unit.get_id()](UI::Widget& w) {
            const auto& unit = this->gs.world->unit_manager.units[unit_id];
            const auto& type = this->gs.world->unit_types[unit.type_id];
            w.text(string_format("%zu %s", unit.size, type.name.c_str()));
        });
        btn->on_each_tick(*btn);
    });
}

ArmyProductionTab::ArmyProductionTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height, parent),
    gs{ _gs }
{
    this->is_scroll = true;

    auto* flex_column = new UI::Div(0, 0, this->width, this->height, this);
    flex_column->flex = UI::Flex::COLUMN;
    for(const auto& unit_type : gs.world->unit_types) {
        auto* unit_type_grp = new UI::Div(0, 0, flex_column->width, 24, flex_column);
        unit_type_grp->is_scroll = false;
        unit_type_grp->flex = UI::Flex::ROW;
        unit_type_grp->set_on_click([this, unit_type_id = unit_type.get_id()](UI::Widget&) {
            this->gs.production_queue.push_back(unit_type_id);
        });

        auto *icon_img = new UI::Image(0, 0, 24, 24, unit_type.get_icon_path(), unit_type_grp);

        auto *name_btn = new UI::Button(0, 0, unit_type_grp->width - 24, 24, unit_type_grp);
        name_btn->right_side_of(*icon_img);
        name_btn->text(unit_type.name);
    }

    // Chart showing total number of required materials
    this->reqmat_chart = new UI::Chart(0, 0, this->width, 128, flex_column);
    this->reqmat_chart->text("Material demand");
    this->reqmat_chart->set_on_each_tick([this](UI::Widget&) {
        auto total = 0.f;
        for(const auto province_id : this->gs.curr_nation->owned_provinces) {
            const auto& province = gs.world->provinces[province_id];
            for(const auto& building : province.get_buildings()) {
                for(const auto& [_, amount] : building.req_goods_for_unit)
                    total += amount;
                for(const auto& [_, amount] : building.req_goods)
                    total += amount;
            }
        }
        this->reqmat_chart->data.push_back(total);
    });

    for(const auto province_id : gs.curr_nation->owned_provinces) {
        const auto& province = gs.world->provinces[province_id];
        for(const auto& building_type : gs.world->building_types)
            if(building_type.can_build_land_units())
                new ArmyProductionUnitInfo(gs, 0, 0, province, building_type, flex_column);
    }
}

ArmyProductionUnitInfo::ArmyProductionUnitInfo(GameState& _gs, int x, int y, ProvinceId _province_id, size_t _idx, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, 48, parent),
    gs{ _gs },
    province_id{ _province_id },
    idx{ _idx }
{
    const auto& province = gs.world->provinces[province_id];
    const auto& building = province.get_buildings()[idx];
    this->is_scroll = false;
    this->flex = UI::Flex::ROW;
    this->overflow = UI::Overflow::WRAP;

    auto* unit_icon = new UI::Image(0, 0, 24, 24, this);
    if(UnitType::is_valid(building.working_unit_type_id))
        unit_icon->current_texture = gs.tex_man.load(gs.package_man.get_unique(gs.world->unit_types[building.working_unit_type_id].get_icon_path()));

    auto* province_lab = new UI::Label(0, 0, "?", this);
    province_lab->set_on_each_tick([this](UI::Widget& w) {
        const auto& province = gs.world->provinces[province_id];
        w.text(province.name);
    });
    province_lab->on_each_tick(*province_lab);

    auto* name_lab = new UI::Label(0, 0, "?", this);
    name_lab->set_on_each_tick([this](UI::Widget& w) {
        const auto& province = gs.world->provinces[province_id];
        auto& building = province.get_buildings()[this->idx];
        w.text(UnitType::is_valid(building.working_unit_type_id) ? this->gs.world->unit_types[building.working_unit_type_id].name : translate("No unit"));
    });
    name_lab->on_each_tick(*name_lab);

    auto* progress_pgbar = new UI::ProgressBar(0, 0, 128, 24, 0.f, 1.f, this);
    progress_pgbar->set_on_each_tick([this](UI::Widget& _w) {
        auto& w = static_cast<UI::ProgressBar&>(_w);
        const auto& province = gs.world->provinces[province_id];
        auto& building = province.get_buildings()[this->idx];
        if(UnitType::is_invalid(building.working_unit_type_id)) return;
        auto full = 0.f, needed = 0.f;
        std::string text;
        for(size_t i = 0; i < building.req_goods_for_unit.size(); i++) {
            auto need_req = building.req_goods_for_unit[i];
            auto full_req = this->gs.world->unit_types[building.working_unit_type_id].req_goods[i];
            full += full_req.second;
            needed += need_req.second;
            text += translate_format("Requires %.2f of %s (has %.2f)", need_req.second, this->gs.world->goods[need_req.first].name.c_str(), full_req.second);
        }
        w.set_value(full / glm::max(needed, 0.01f));
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
            return unit.owner_id == *this->gs.curr_nation && type.is_ground && !type.is_naval;
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
            return unit.owner_id == *this->gs.curr_nation && !type.is_ground && !type.is_naval;
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
            return unit.owner_id == *this->gs.curr_nation && !type.is_ground && type.is_naval;
        }, this);
        this->production_tab->is_render = false;
    });
    navy_ibtn->set_tooltip(translate("Navy"));

    this->production_tab = new ArmyProductionTab(gs, 0, 32, this);
    this->production_tab->is_render = true;
    auto* production_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/production.png")), this);
    production_ibtn->right_side_of(*navy_ibtn);
    production_ibtn->set_on_click([this](UI::Widget&) {
        this->units_tab->is_render = false;
        this->production_tab->is_render = true;
    });
    production_ibtn->set_tooltip(translate("Military production"));

    army_ibtn->on_click(*army_ibtn);
}
