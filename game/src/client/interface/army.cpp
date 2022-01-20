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
//      client/interface/army.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/army.hpp"
#include "client/map.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui/button.hpp"
#include "client/ui/chart.hpp"
#include "client/ui/image.hpp"
#include "client/ui/label.hpp"
#include "client/ui/progress_bar.hpp"
#include "client/ui/tooltip.hpp"
#include "client/ui/close_button.hpp"
#include "unified_render/locale.hpp"

using namespace Interface;

ArmyArmyTab::ArmyArmyTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& unit : gs.world->units) {
        if(unit->owner != gs.curr_nation) {
            continue;
        }
        if(!(unit->type->is_ground == true && unit->type->is_naval == false)) {
            continue;
        }

        auto* btn = new UnitButton(gs, 0, 24 * i, unit, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ArmyArmyTab&>(*w.parent);
            //static_cast<UnitButton&>(w).unit;
        });
        i++;
    }
}

ArmyAirforceTab::ArmyAirforceTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& unit : gs.world->units) {
        if(unit->owner != gs.curr_nation) {
            continue;
        }
        if(!(unit->type->is_ground == true && unit->type->is_naval == true)) {
            continue;
        }

        auto* btn = new UnitButton(gs, 0, 24 * i, unit, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ArmyAirforceTab&>(*w.parent);
            //static_cast<UnitButton&>(w).unit;
        });
        i++;
    }
}

ArmyNavyTab::ArmyNavyTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& unit : gs.world->units) {
        if(unit->owner != gs.curr_nation) {
            continue;
        }
        if(!(unit->type->is_ground == false && unit->type->is_naval == true)) {
            continue;
        }

        auto* btn = new UnitButton(gs, 0, 24 * i, unit, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ArmyNavyTab&>(*w.parent);
            //static_cast<UnitButton&>(w).unit;
        });

        i++;
    }
}

ArmyProductionTab::ArmyProductionTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height, parent),
    gs{ _gs }
{
    // Chart showing total number of required materials
    this->reqmat_chart = new UI::Chart(0, 0, 128, 128, this);
    this->reqmat_chart->text("Material demand");
    this->reqmat_chart->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyProductionTab&>(*w.parent);

        float reqtotal = 0.f;
        for(const auto& building : o.gs.world->buildings) {
            if(building->get_owner() != o.gs.curr_nation) {
                continue;
            }

            for(const auto& req : building->req_goods_for_unit) {
                reqtotal += req.second;
            }

            for(const auto& req : building->req_goods) {
                reqtotal += req.second;
            }
        }

        o.reqmat_chart->data.push_back(reqtotal);
        if(o.reqmat_chart->data.size() >= 30) {
            o.reqmat_chart->data.pop_back();
        }
    });

    unsigned int i = 0;
    for(const auto& building : gs.world->buildings) {
        if(building->get_owner() != gs.curr_nation) {
            continue;
        }

        new ArmyProductionUnitInfo(gs, 0, 128 + (48 * i), building, this);
        i++;
    }
}

ArmyProductionUnitInfo::ArmyProductionUnitInfo(GameState& _gs, int x, int y, Building* _building, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, 48, parent),
    gs{ _gs },
    building{ _building }
{
    this->is_scroll = false;

    this->unit_icon = new UI::Image(0, 0, 24, 24, nullptr, this);
    if(building->working_unit_type != nullptr) {
        this->unit_icon->current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/" + building->working_unit_type->ref_name + ".png"));
    }

    this->province_lab = new UI::Label(0, 0, "?", this);
    this->province_lab->right_side_of(*this->unit_icon);
    this->province_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyProductionUnitInfo&>(*w.parent);
        if(o.building->get_province() != nullptr) {
            w.text(UnifiedRender::Locale::translate(o.building->get_province()->name));
        }
    });
    this->province_lab->on_each_tick(*this->province_lab, nullptr);

    this->name_lab = new UI::Label(0, 0, "?", this);
    this->name_lab->right_side_of(*this->province_lab);
    this->name_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyProductionUnitInfo&>(*w.parent);
        w.text((o.building->working_unit_type != nullptr) ? UnifiedRender::Locale::translate(o.building->working_unit_type->name) : "No unit");
    });
    this->name_lab->on_each_tick(*this->name_lab, nullptr);

    auto* progress_pgbar = new UI::ProgressBar(0, 0, 128, 24, 0.f, 1.f, this);
    progress_pgbar->below_of(*this->name_lab);
    progress_pgbar->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyProductionUnitInfo&>(*w.parent);

        std::string text = "";

        size_t full = 0, needed = 0;
        text = "Needs ";
        for(size_t i = 0; i < o.building->req_goods_for_unit.size(); i++) {
            auto need_req = o.building->req_goods_for_unit[i];
            auto full_req = o.building->working_unit_type->req_goods[i];

            full_req.second -= need_req.second;

            full += full_req.second;
            needed += need_req.second;

            text += std::to_string(need_req.second) + " of " + UnifiedRender::Locale::translate(need_req.first->name) + " (has " + std::to_string(full_req.second) + "), ";
        }

        ((UI::ProgressBar&)w).value = (float)full / (float)needed;
        w.tooltip->text(text);
    });
    progress_pgbar->tooltip = new UI::Tooltip(progress_pgbar, 512, 24);
    progress_pgbar->on_each_tick(*progress_pgbar, nullptr);
}

ArmySelectUnitTab::ArmySelectUnitTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& unit_type : gs.world->unit_types) {
        auto* btn = new UnitTypeButton(gs, 0, 24 * i, unit_type, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ArmyView&>(*w.parent->parent);
            o.new_unit_tab->unit_type = ((UnitTypeButton&)w).unit_type;

            // This tab gets hidden and "pass control" to the new_unit_tab
            o.new_unit_tab->is_render = true;
            o.select_unit_tab->is_render = false;
        });
        i++;
    }
}

ArmyNewUnitTab::ArmyNewUnitTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    auto* select_btn = new UI::Button(0, 0, 128, 24, this);
    select_btn->text(UnifiedRender::Locale::translate("Select type"));
    select_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent->parent);

        // "Give control" to the selection tab - they will return us control after selecting an unit type
        w.parent->is_render = false;
        o.select_unit_tab->is_render = true;
    });

    auto* create_btn = new UI::Button(0, 0, 128, 24, this);
    create_btn->text(UnifiedRender::Locale::translate("Create"));
    create_btn->below_of(*select_btn);
    create_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyNewUnitTab&>(*w.parent);

        if(o.unit_type == nullptr) {
            o.gs.ui_ctx->prompt("Error", "No unit type is selected");
            return;
        }

        o.gs.production_queue.push_back(o.unit_type);
    });
}

ArmyView::ArmyView(GameState& _gs)
    : UI::Window(-400, 0, 400, _gs.height),
    gs{ _gs }
{
    if(gs.right_side_panel != nullptr) {
        gs.right_side_panel->kill();
    }
    gs.right_side_panel = this;

    this->origin = UI::Origin::UPPER_RIGHT_SCREEN;
    this->is_scroll = false;
    this->text(UnifiedRender::Locale::translate("Army manager"));

    this->army_tab = new ArmyArmyTab(gs, 0, 32, this);
    this->army_tab->is_render = true;
    auto* army_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/military_score.png")), this);
    army_ibtn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = true;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.production_tab->is_render = false;
        o.new_unit_tab->is_render = false;
        o.select_unit_tab->is_render = false;
    });
    army_ibtn->tooltip = new UI::Tooltip(army_ibtn, 512, 24);
    army_ibtn->tooltip->text(UnifiedRender::Locale::translate("Army"));

    this->airforce_tab = new ArmyAirforceTab(gs, 0, 32, this);
    this->airforce_tab->is_render = false;
    auto* airforce_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/airforce.png")), this);
    airforce_ibtn->right_side_of(*army_ibtn);
    airforce_ibtn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = true;
        o.navy_tab->is_render = false;
        o.production_tab->is_render = false;
        o.new_unit_tab->is_render = false;
        o.select_unit_tab->is_render = false;
    });
    airforce_ibtn->tooltip = new UI::Tooltip(airforce_ibtn, 512, 24);
    airforce_ibtn->tooltip->text(UnifiedRender::Locale::translate("Airforce"));

    this->navy_tab = new ArmyNavyTab(gs, 0, 32, this);
    this->navy_tab->is_render = false;
    auto* navy_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/navy.png")), this);
    navy_ibtn->right_side_of(*airforce_ibtn);
    navy_ibtn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = true;
        o.production_tab->is_render = false;
        o.new_unit_tab->is_render = false;
        o.select_unit_tab->is_render = false;
    });
    navy_ibtn->tooltip = new UI::Tooltip(navy_ibtn, 512, 24);
    navy_ibtn->tooltip->text(UnifiedRender::Locale::translate("Navy"));

    this->production_tab = new ArmyProductionTab(gs, 0, 32, this);
    this->production_tab->is_render = false;
    auto* production_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/production.png")), this);
    production_ibtn->right_side_of(*navy_ibtn);
    production_ibtn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.production_tab->is_render = true;
        o.new_unit_tab->is_render = false;
        o.select_unit_tab->is_render = false;
    });
    production_ibtn->tooltip = new UI::Tooltip(production_ibtn, 512, 24);
    production_ibtn->tooltip->text(UnifiedRender::Locale::translate("Production"));

    this->new_unit_tab = new ArmyNewUnitTab(gs, 0, 32, this);
    this->new_unit_tab->is_render = false;
    auto* new_unit_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/new_unit.png")), this);
    new_unit_ibtn->right_side_of(*production_ibtn);
    new_unit_ibtn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.production_tab->is_render = false;
        o.new_unit_tab->is_render = true;
        o.select_unit_tab->is_render = false;
    });
    new_unit_ibtn->tooltip = new UI::Tooltip(new_unit_ibtn, 512, 24);
    new_unit_ibtn->tooltip->text(UnifiedRender::Locale::translate("New unit"));

    // Hidden - only rendered on invokation and does not store state on the View directly
    // rather it stores it on the new_unit_tab
    this->select_unit_tab = new ArmySelectUnitTab(gs, 0, 32, this);
    this->select_unit_tab->is_render = false;

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->right_side_of(*new_unit_ibtn);
    close_btn->text(UnifiedRender::Locale::translate("Close"));
    close_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);
        o.gs.right_side_panel->kill();
        o.gs.right_side_panel = nullptr;
    });
}