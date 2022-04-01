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
//      client/interface/army.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "unified_render/ui/window.hpp"
#include "unified_render/ui/group.hpp"
#include "client/interface/common.hpp"

class World;
class Nation;
class GameState;
class Building;

namespace UI {
    class Chart;
    class Image;
    class Label;
}

namespace Interface {
    class ArmyArmyTab : public UI::Group {
        GameState& gs;
    public:
        ArmyArmyTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class ArmyAirforceTab : public UI::Group {
        GameState& gs;
    public:
        ArmyAirforceTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class ArmyNavyTab : public UI::Group {
        GameState& gs;
    public:
        ArmyNavyTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    // Auto-Production manager
    class ArmyProductionTab : public UI::Group {
        GameState& gs;

        UI::Chart *reqmat_chart;
    public:
        ArmyProductionTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    // Brief about unit being produced
    class ArmyProductionUnitInfo : public UI::Group {
        GameState& gs;
        Province *province = nullptr;
        unsigned int idx;

        UI::Image *unit_icon;
        UI::Label *province_lab;
        UI::Label *name_lab;
    public:
        ArmyProductionUnitInfo(GameState& _gs, int x, int y, Province* _province, unsigned int _idx, UI::Widget* parent);
    };

    class ArmyNewUnitTab : public UI::Group {
        GameState& gs;
    public:
        ArmyNewUnitTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class ArmyView : public UI::Window {
        GameState& gs;
    public:
        ArmyView(GameState& gs);

        ArmyArmyTab* army_tab;
        ArmyAirforceTab* airforce_tab;
        ArmyNavyTab* navy_tab;
        ArmyProductionTab* production_tab;
        ArmyNewUnitTab* new_unit_tab;
    };
};
