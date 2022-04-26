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
#include "technology.hpp"

class World;
class Nation;
class GameState;

namespace Interface {
    class TechTreeTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeTab(GameState& gs, int x, int y, int type, UI::Widget* parent);
    };

    class TechTreeView : public UI::Window {
        GameState& gs;
    public:
        TechTreeView(GameState& gs);

        TechTreeTab* strategic_tab;
        TechTreeTab* military_tab;
        TechTreeTab* navy_tab;
        TechTreeTab* social_tab;
        TechTreeTab* economic_tab;
        TechTreeTab* politics_tab;
    };
};
