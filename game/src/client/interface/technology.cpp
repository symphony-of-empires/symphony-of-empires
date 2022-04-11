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
//      client/interface/technology.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/technology.hpp"
#include "client/map.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "unified_render/ui/image.hpp"
#include "unified_render/ui/close_button.hpp"
#include "unified_render/ui/button.hpp"

using namespace Interface;

TechTreeStrategicTab::TechTreeStrategicTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::STRATEGIC) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeMilitaryTab::TechTreeMilitaryTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::MILITARY) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeNavyTab::TechTreeNavyTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::NAVY) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeSocialTab::TechTreeSocialTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::SOCIAL) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeEconomicTab::TechTreeEconomicTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::ECONOMIC) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreePoliticsTab::TechTreePoliticsTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::POLITICS) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeView::TechTreeView(GameState& _gs)
    : UI::Window(-400, 0, 400, _gs.height),
    gs{ _gs }
{
    if(gs.right_side_panel != nullptr) {
        gs.right_side_panel->kill();
    }
    gs.right_side_panel = this;

    this->origin = UI::Origin::UPPER_RIGHT_SCREEN;
    this->is_scroll = false;
    this->text("Research");

    this->strategic_tab = new TechTreeStrategicTab(gs, 0, 24, this);
    this->strategic_tab->is_render = true;
    auto* strategic_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/strategic_tech.png")), this);
    strategic_ibtn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = true;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = false;
    });

    this->military_tab = new TechTreeMilitaryTab(gs, 0, 24, this);
    this->military_tab->is_render = false;
    auto* military_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/military_tech.png")), this);
    military_ibtn->right_side_of(*strategic_ibtn);
    military_ibtn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = true;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = false;
    });

    this->navy_tab = new TechTreeNavyTab(gs, 0, 24, this);
    this->navy_tab->is_render = false;
    auto* navy_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/navy_tech.png")), this);
    navy_ibtn->right_side_of(*military_ibtn);
    navy_ibtn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = true;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = false;
    });

    this->social_tab = new TechTreeSocialTab(gs, 0, 24, this);
    this->social_tab->is_render = false;
    auto* social_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/social_tech.png")), this);
    social_ibtn->right_side_of(*navy_ibtn);
    social_ibtn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = true;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = false;
    });

    this->economic_tab = new TechTreeEconomicTab(gs, 0, 24, this);
    this->economic_tab->is_render = false;
    auto* economic_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/economic_tech.png")), this);
    economic_ibtn->right_side_of(*social_ibtn);
    economic_ibtn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = true;
        o.politics_tab->is_render = false;
    });

    this->politics_tab = new TechTreePoliticsTab(gs, 0, 0, this);
    this->politics_tab->is_render = false;
    auto* politics_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/politics_tech.png")), this);
    politics_ibtn->right_side_of(*economic_ibtn);
    politics_ibtn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = true;
    });

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->right_side_of(*politics_ibtn);
    close_btn->text("Close");
    close_btn->on_click = ([](UI::Widget& w) {
        (static_cast<TechTreeView&>(*w.parent)).kill();
    });
}