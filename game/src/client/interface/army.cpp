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

#include "eng3d/locale.hpp"
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

ArmyArmyTab::ArmyArmyTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    auto* flex_column = new UI::Div(0, 0, this->width, this->height, this);
    flex_column->flex = UI::Flex::COLUMN;
    gs.world->unit_manager.for_each_unit([this, flex_column](Unit& unit) {
        if(unit.owner_id != this->gs.curr_nation->get_id()) return;
        else if(!(unit.type->is_ground == true && unit.type->is_naval == false)) return;

        auto* btn = new UnitButton(this->gs, 0, 0, unit, flex_column);
        btn->set_on_click([](UI::Widget&) {

        });
    });
}

ArmyAirforceTab::ArmyAirforceTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    auto* flex_column = new UI::Div(0, 0, this->width, this->height, this);
    flex_column->flex = UI::Flex::COLUMN;
    gs.world->unit_manager.for_each_unit([this, flex_column](Unit& unit) {
        if(unit.owner_id != this->gs.curr_nation->get_id()) return;
        else if(!(unit.type->is_ground == true && unit.type->is_naval == true)) return;

        auto* btn = new UnitButton(this->gs, 0, 0, unit, flex_column);
        btn->set_on_click([](UI::Widget&) {

        });
    });
}

ArmyNavyTab::ArmyNavyTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    auto* flex_column = new UI::Div(0, 0, this->width, this->height, this);
    flex_column->flex = UI::Flex::COLUMN;
    gs.world->unit_manager.for_each_unit([this, flex_column](Unit& unit) {
        if(unit.owner_id != this->gs.curr_nation->get_id()) return;
        else if(!(unit.type->is_ground == false && unit.type->is_naval == true)) return;

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

    // Chart showing total number of required materials
    this->reqmat_chart = new UI::Chart(0, 0, 128, 128, this);
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

    auto* flex_column = new UI::Div(0, 0, this->width, this->height, this);
    flex_column->flex = UI::Flex::COLUMN;
    for(const auto province_id : gs.curr_nation->owned_provinces) {
        const auto& province = gs.world->provinces[province_id];
        for(const auto& building_type : gs.world->building_types) {
            if(!building_type.can_build_land_units())
                continue;
            new ArmyProductionUnitInfo(gs, 0, 0, province, gs.world->get_id(building_type), flex_column);
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
    if(building.working_unit_type != nullptr)
        this->unit_icon->current_texture = gs.tex_man.load(gs.package_man.get_unique("gfx/" + building.working_unit_type->ref_name + ".png"));

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
        w.text((building.working_unit_type != nullptr) ? building.working_unit_type->name.get_string() : translate("No unit"));
    });
    this->name_lab->on_each_tick(*this->name_lab);

    auto* progress_pgbar = new UI::ProgressBar(0, 0, 128, 24, 0.f, 1.f, this);
    progress_pgbar->below_of(*this->name_lab);
    progress_pgbar->set_on_each_tick([this](UI::Widget& w) {
        auto& building = this->province.get_buildings()[this->idx];
        if(!building.working_unit_type) return;
        auto full = 0.f, needed = 0.f;
        std::string text = "Needs ";
        for(Good::Id i = 0; i < building.req_goods_for_unit.size(); i++) {
            auto need_req = building.req_goods_for_unit[i];
            auto full_req = building.working_unit_type->req_goods[i];
            full_req.second -= need_req.second;
            full += full_req.second;
            needed += need_req.second;
            text += string_format("%.2f of %s (has %.2f)", need_req.second, need_req.first->name.get_string().c_str(), full_req.second);
        }

        ((UI::ProgressBar&)w).set_value((float)full / (float)needed);
        w.set_tooltip(text);
    });
    progress_pgbar->on_each_tick(*progress_pgbar);
}

ArmyNewUnitTab::ArmyNewUnitTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    auto* flex_column = new UI::Div(0, 0, this->width, this->height, this);
    flex_column->flex = UI::Flex::COLUMN;
    for(auto& unit_type : gs.world->unit_types) {
        auto* btn = new UnitTypeButton(gs, 0, 0, unit_type, flex_column);
        btn->set_on_click([this, btn](UI::Widget&) {
            this->gs.production_queue.push_back(&btn->unit_type);
        });
    }
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

    this->army_tab = new ArmyArmyTab(gs, 0, 32, this);
    this->army_tab->is_render = true;
    auto* army_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/military_score.png")), this);
    army_ibtn->set_on_click([this](UI::Widget&) {
        this->army_tab->is_render = true;
        this->airforce_tab->is_render = false;
        this->navy_tab->is_render = false;
        this->production_tab->is_render = false;
        this->new_unit_tab->is_render = false;
    });
    army_ibtn->set_tooltip(translate("Land army"));

    this->airforce_tab = new ArmyAirforceTab(gs, 0, 32, this);
    this->airforce_tab->is_render = false;
    auto* airforce_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/airforce.png")), this);
    airforce_ibtn->right_side_of(*army_ibtn);
    airforce_ibtn->set_on_click([this](UI::Widget&) {
        this->army_tab->is_render = false;
        this->airforce_tab->is_render = true;
        this->navy_tab->is_render = false;
        this->production_tab->is_render = false;
        this->new_unit_tab->is_render = false;
    });
    airforce_ibtn->set_tooltip(translate("Airforce"));

    this->navy_tab = new ArmyNavyTab(gs, 0, 32, this);
    this->navy_tab->is_render = false;
    auto* navy_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/navy.png")), this);
    navy_ibtn->right_side_of(*airforce_ibtn);
    navy_ibtn->set_on_click([this](UI::Widget&) {
        this->army_tab->is_render = false;
        this->airforce_tab->is_render = false;
        this->navy_tab->is_render = true;
        this->production_tab->is_render = false;
        this->new_unit_tab->is_render = false;
    });
    navy_ibtn->set_tooltip(translate("Navy"));

    this->production_tab = new ArmyProductionTab(gs, 0, 32, this);
    this->production_tab->is_render = false;
    auto* production_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/production.png")), this);
    production_ibtn->right_side_of(*navy_ibtn);
    production_ibtn->set_on_click([this](UI::Widget&) {
        this->army_tab->is_render = false;
        this->airforce_tab->is_render = false;
        this->navy_tab->is_render = false;
        this->production_tab->is_render = true;
        this->new_unit_tab->is_render = false;
    });
    production_ibtn->set_tooltip(translate("Militar production"));

    this->new_unit_tab = new ArmyNewUnitTab(gs, 0, 32, this);
    this->new_unit_tab->is_render = false;
    auto* new_unit_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/new_unit.png")), this);
    new_unit_ibtn->right_side_of(*production_ibtn);
    new_unit_ibtn->set_on_click([this](UI::Widget&) {
        this->army_tab->is_render = false;
        this->airforce_tab->is_render = false;
        this->navy_tab->is_render = false;
        this->production_tab->is_render = false;
        this->new_unit_tab->is_render = true;
    });
    new_unit_ibtn->set_tooltip(translate("TODO: Megre this with production"));
}
