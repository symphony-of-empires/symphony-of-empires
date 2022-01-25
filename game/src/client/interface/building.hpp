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
//      client/interface/building.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "unified_render/ui/window.hpp"
#include "unified_render/ui/group.hpp"
#include "client/interface/common.hpp"

namespace Interface {
    class BuildingSelectProvinceTab : public UI::Group {
        GameState& gs;
    public:
        BuildingSelectProvinceTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class BuildingSelectNationTab : public UI::Group {
        GameState& gs;
    public:
        BuildingSelectNationTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class BuildingSelectTypeTab : public UI::Group {
        GameState& gs;
    public:
        BuildingSelectTypeTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    // Set building site for production and placement of a new building
    class BuildingBuildView : public UI::Window {
        GameState& gs;
    public:
        Nation* nation = nullptr;
        Province* province = nullptr;
        BuildingType* building_type = nullptr;

        BuildingSelectProvinceTab* province_tab;
        BuildingSelectNationTab* nation_tab;
        BuildingSelectTypeTab* type_tab;

        bool in_tile;
        int tx, ty;
        BuildingBuildView(GameState& gs, int tx, int ty, bool in_tile, Nation* nation = nullptr, Province* province = nullptr);

        /* The tab holds buttons which sends the packets to the server - this requires access to the parent which holds private data :) */
        friend class BuildingSelectTypeTab;
    };

    class BuildingView : public UI::Window {
        GameState& gs;
        Building* building;
    public:
        BuildingView(GameState& gs, Building* building);
    };
};