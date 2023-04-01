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
#include "world.hpp"
#include "client/game_state.hpp"

using namespace Interface;

ArmyUnitsTab::ArmyUnitsTab(GameState& _gs, int _x, int _y, std::function<bool(Unit& unit)> filter, UI::Widget* _parent)
    : UI::Group(_x, _y, _parent->width - _x, _parent->height - _y, _parent),
    gs{ _gs }
{
    auto& flex_column = this->make_widget<UI::Div>(0, 0, this->width, this->height);
    flex_column.flex = UI::Flex::COLUMN;
    gs.world->unit_manager.units.for_each([&](Unit& unit) {
        if(!filter || !filter(unit)) return;
        auto& btn = flex_column.make_widget<UI::Button>(0, 0, this->width, 24);
        btn.set_on_each_tick([this, unit_id = unit.get_id()](UI::Widget& w) {
            const auto& current_unit = this->gs.world->unit_manager.units[unit_id];
            const auto& current_type = this->gs.world->unit_types[current_unit.type_id];
            w.set_text(string_format("%zu %s", current_unit.size, current_type.name.data()));
        });
        btn.on_each_tick(btn);
    });
}

ArmyProductionTab::ArmyProductionTab(GameState& _gs, int _x, int _y, UI::Widget* _parent)
    : UI::Group(_x, _y, _parent->width - _x, _parent->height, _parent),
    gs{ _gs }
{
    this->is_scroll = true;

    auto& flex_column = this->make_widget<UI::Div>(0, 0, this->width, this->height);
    flex_column.flex = UI::Flex::COLUMN;
    for(const auto& unit_type : gs.world->unit_types) {
        auto& unit_type_grp = flex_column.make_widget<UI::Div>(0, 0, flex_column.width, 24);
        unit_type_grp.is_scroll = false;
        unit_type_grp.flex = UI::Flex::ROW;
        unit_type_grp.set_on_click([this, unit_type_id = unit_type.get_id()](UI::Widget&) {
            this->gs.production_queue.push_back(unit_type_id);
        });

        auto& icon_img = unit_type_grp.make_widget<UI::Image>(0, 0, 24, 24, unit_type.get_icon_path());
        auto& name_btn = unit_type_grp.make_widget<UI::Button>(0, 0, unit_type_grp.width - 24, 24);
        name_btn.right_side_of(icon_img);
        name_btn.set_text(unit_type.name.data());
    }

    // Chart showing total number of required materials
    auto& reqmat_chart = flex_column.make_widget<UI::Chart>(0, 0, this->width, 128);
    reqmat_chart.set_text("Material demand");
    reqmat_chart.set_on_each_tick([this](UI::Widget& w) {
        auto& o = static_cast<UI::Chart&>(w);
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
        //o.data.push_back(total);
    });

    for(const auto province_id : gs.curr_nation->owned_provinces)
        for(const auto& building_type : gs.world->building_types)
            if(building_type.can_build_land_units())
                flex_column.make_widget<ArmyProductionUnitInfo>(gs, 0, 0, gs.world->provinces[province_id], building_type);
}

ArmyProductionUnitInfo::ArmyProductionUnitInfo(GameState& _gs, int _x, int _y, ProvinceId _province_id, size_t _idx, UI::Widget* _parent)
    : UI::Group(_x, _y, _parent->width - _x, 48, _parent),
    gs{ _gs },
    province_id{ _province_id },
    idx{ _idx }
{
    const auto& province = gs.world->provinces[province_id];
    const auto& building = province.get_buildings()[idx];
    this->is_scroll = false;
    this->flex = UI::Flex::ROW;
    this->overflow = UI::Overflow::WRAP;

    auto& unit_icon = this->make_widget<UI::Image>(0, 0, 24, 24);
    unit_icon.current_texture = gs.tex_man.load(gs.package_man.get_unique(gs.world->unit_types[building.working_unit_type_id.value_or(UnitTypeId{})].get_icon_path()));

    auto& province_lab = this->make_widget<UI::Label>(0, 0, "?");
    province_lab.set_on_each_tick([this](UI::Widget& w) {
        const auto& current_province = gs.world->provinces[province_id];
        w.set_text(current_province.name.data());
    });
    province_lab.on_each_tick(province_lab);

    auto& name_lab = this->make_widget<UI::Label>(0, 0, "?");
    name_lab.set_on_each_tick([this](UI::Widget& w) {
        const auto& current_province = gs.world->provinces[province_id];
        auto& current_building = current_province.get_buildings()[this->idx];
        w.set_text(this->gs.world->unit_types[current_building.working_unit_type_id.value_or(UnitTypeId{})].name.data());
    });
    name_lab.on_each_tick(name_lab);

    auto& progress_pgbar = this->make_widget<UI::ProgressBar>(0, 0, 128, 24, 0.f, 1.f);
    progress_pgbar.set_on_each_tick([this](UI::Widget& _w) {
        auto& w = static_cast<UI::ProgressBar&>(_w);
        const auto& c_province = gs.world->provinces[province_id];
        const auto& c_building = c_province.get_buildings()[this->idx];
        if(c_building.working_unit_type_id.has_value()) return;
        auto full = 0.f, needed = 0.f;
        std::string text;
        for(size_t i = 0; i < c_building.req_goods_for_unit.size(); i++) {
            auto need_req = c_building.req_goods_for_unit[i];
            auto full_req = this->gs.world->unit_types[c_building.working_unit_type_id.value()].req_goods[i];
            full += full_req.second;
            needed += need_req.second;
            text += translate_format("Requires %.2f of %s (has %.2f)", need_req.second, this->gs.world->commodities[need_req.first].name.data(), full_req.second);
        }
        w.set_value(full / glm::max(needed, 0.01f));
        w.set_tooltip(text);
    });
    progress_pgbar.on_each_tick(progress_pgbar);
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
    this->set_text(translate("Army management"));

    this->units_tab = new ArmyUnitsTab(gs, 0, 32, nullptr, this);
    this->units_tab->is_render = true;
    auto& army_ibtn = this->make_widget<UI::Image>(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/mili&tary_score.png")));
    army_ibtn.set_on_click([this](UI::Widget&) {
        this->units_tab->is_render = true;
        this->units_tab->kill();
        this->units_tab = &this->make_widget<ArmyUnitsTab>(gs, 0, 32, [this](Unit& unit) {
            auto& current_type = this->gs.world->unit_types[unit.type_id];
            return unit.owner_id == *this->gs.curr_nation && current_type.is_ground && !current_type.is_naval;
        });
        this->production_tab->is_render = false;
    });
    army_ibtn.set_tooltip(translate("Land army"));

    auto& airforce_ibtn = this->make_widget<UI::Image>(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/airforce.png")));
    airforce_ibtn.right_side_of(army_ibtn);
    airforce_ibtn.set_on_click([this](UI::Widget&) {
        this->units_tab->is_render = true;
        this->units_tab->kill();
        this->units_tab = &this->make_widget<ArmyUnitsTab>(gs, 0, 32, [this](Unit& unit) {
            auto& current_type = this->gs.world->unit_types[unit.type_id];
            return unit.owner_id == *this->gs.curr_nation && !current_type.is_ground && !current_type.is_naval;
        });
        this->production_tab->is_render = false;
    });
    airforce_ibtn.set_tooltip(translate("Airforce"));
    
    auto& navy_ibtn = this->make_widget<UI::Image>(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/navy.png")));
    navy_ibtn.right_side_of(airforce_ibtn);
    navy_ibtn.set_on_click([this](UI::Widget&) {
        this->units_tab->is_render = true;
        this->units_tab->kill();
        this->units_tab = &this->make_widget<ArmyUnitsTab>(gs, 0, 32, [this](Unit& unit) {
            auto& current_type = this->gs.world->unit_types[unit.type_id];
            return unit.owner_id == *this->gs.curr_nation && !current_type.is_ground && current_type.is_naval;
        });
        this->production_tab->is_render = false;
    });
    navy_ibtn.set_tooltip(translate("Navy"));

    this->production_tab = new ArmyProductionTab(gs, 0, 32, this);
    this->production_tab->is_render = true;
    auto& production_ibtn = this->make_widget<UI::Image>(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/production.png")));
    production_ibtn.right_side_of(navy_ibtn);
    production_ibtn.set_on_click([this](UI::Widget&) {
        this->units_tab->is_render = false;
        this->production_tab->is_render = true;
    });
    production_ibtn.set_tooltip(translate("Military production"));

    army_ibtn.on_click(army_ibtn);
}
