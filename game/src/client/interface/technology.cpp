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

#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/close_button.hpp"
#include "eng3d/ui/button.hpp"

#include "client/interface/technology.hpp"
#include "client/map.hpp"
#include "client/game_state.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "technology.hpp"

using namespace Interface;

TechTreeTab::TechTreeTab(GameState& _gs, int x, int y, int type, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(auto& technology : gs.world->technologies) {
        if(static_cast<int>(technology.type) & static_cast<int>(type)) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology.ref_name.get_string());
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
    this->set_close_btn_function([this](Widget&) {
        this->kill();
        this->gs.right_side_panel = nullptr;
    });

    this->origin = UI::Origin::UPPER_RIGHT_SCREEN;
    this->is_scroll = false;
    this->text("Research");

    this->strategic_tab = new TechTreeTab(gs, 0, 24, TechnologyType::STRATEGIC, this);
    this->strategic_tab->is_render = true;
    auto* strategic_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man->load(Path::get("gfx/strategic_tech.png")), this);
    strategic_ibtn->set_on_click([this](UI::Widget&) {
        this->strategic_tab->is_render = true;
        this->military_tab->is_render = false;
        this->navy_tab->is_render = false;
        this->social_tab->is_render = false;
        this->economic_tab->is_render = false;
        this->politics_tab->is_render = false;
    });

    this->military_tab = new TechTreeTab(gs, 0, 24, TechnologyType::MILITARY, this);
    this->military_tab->is_render = false;
    auto* military_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man->load(Path::get("gfx/military_tech.png")), this);
    military_ibtn->right_side_of(*strategic_ibtn);
    military_ibtn->set_on_click([this](UI::Widget&) {
        this->strategic_tab->is_render = false;
        this->military_tab->is_render = true;
        this->navy_tab->is_render = false;
        this->social_tab->is_render = false;
        this->economic_tab->is_render = false;
        this->politics_tab->is_render = false;
    });

    this->navy_tab = new TechTreeTab(gs, 0, 24, TechnologyType::NAVY, this);
    this->navy_tab->is_render = false;
    auto* navy_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man->load(Path::get("gfx/navy_tech.png")), this);
    navy_ibtn->right_side_of(*military_ibtn);
    navy_ibtn->set_on_click([this](UI::Widget&) {
        this->strategic_tab->is_render = false;
        this->military_tab->is_render = false;
        this->navy_tab->is_render = true;
        this->social_tab->is_render = false;
        this->economic_tab->is_render = false;
        this->politics_tab->is_render = false;
    });

    this->social_tab = new TechTreeTab(gs, 0, 24, TechnologyType::SOCIAL, this);
    this->social_tab->is_render = false;
    auto* social_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man->load(Path::get("gfx/social_tech.png")), this);
    social_ibtn->right_side_of(*navy_ibtn);
    social_ibtn->set_on_click([this](UI::Widget&) {
        this->strategic_tab->is_render = false;
        this->military_tab->is_render = false;
        this->navy_tab->is_render = false;
        this->social_tab->is_render = true;
        this->economic_tab->is_render = false;
        this->politics_tab->is_render = false;
    });

    this->economic_tab = new TechTreeTab(gs, 0, 24, TechnologyType::ECONOMIC, this);
    this->economic_tab->is_render = false;
    auto* economic_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man->load(Path::get("gfx/money.png")), this);
    economic_ibtn->right_side_of(*social_ibtn);
    economic_ibtn->set_on_click([this](UI::Widget&) {
        this->strategic_tab->is_render = false;
        this->military_tab->is_render = false;
        this->navy_tab->is_render = false;
        this->social_tab->is_render = false;
        this->economic_tab->is_render = true;
        this->politics_tab->is_render = false;
    });

    this->politics_tab = new TechTreeTab(gs, 0, 0, TechnologyType::POLITICS, this);
    this->politics_tab->is_render = false;
    auto* politics_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man->load(Path::get("gfx/politics_tech.png")), this);
    politics_ibtn->right_side_of(*economic_ibtn);
    politics_ibtn->set_on_click([this](UI::Widget&) {
        this->strategic_tab->is_render = false;
        this->military_tab->is_render = false;
        this->navy_tab->is_render = false;
        this->social_tab->is_render = false;
        this->economic_tab->is_render = false;
        this->politics_tab->is_render = true;
    });
}