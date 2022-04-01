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
//      client/interface/technology.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "unified_render/ui/group.hpp"
#include "unified_render/ui/window.hpp"

#include "client/interface/common.hpp"

class World;
class Nation;
class GameState;
class Technology;

namespace Interface {
    class TechTreeStrategicTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeStrategicTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeMilitaryTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeMilitaryTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeNavyTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeNavyTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeSocialTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeSocialTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeEconomicTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeEconomicTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreePoliticsTab : public UI::Group {
        GameState& gs;
    public:
        TechTreePoliticsTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeView : public UI::Window {
        GameState& gs;
    public:
        TechTreeView(GameState& gs);

        TechTreeStrategicTab* strategic_tab;
        TechTreeMilitaryTab* military_tab;
        TechTreeNavyTab* navy_tab;
        TechTreeSocialTab* social_tab;
        TechTreeEconomicTab* economic_tab;
        TechTreePoliticsTab* politics_tab;
    };
};
