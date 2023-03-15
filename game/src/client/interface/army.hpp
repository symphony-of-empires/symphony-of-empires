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
//      client/interface/army.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <functional>
#include "eng3d/ui/window.hpp"
#include "eng3d/ui/group.hpp"
#include "eng3d/ui/image.hpp"
#include "world.hpp"

class World;
class Nation;
class GameState;
struct Building;
class Unit;
namespace UI {
    class Chart;
    class Image;
    class Label;
}

namespace Interface {
    class ArmyUnitsTab : public UI::Group {
        GameState& gs;
    public:
        ArmyUnitsTab(GameState& gs, int x, int y, std::function<bool(Unit& unit)> filter, UI::Widget* parent);
    };

    // Auto-Production manager
    class ArmyProductionTab : public UI::Group {
        GameState& gs;
    public:
        ArmyProductionTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    // Brief about unit being produced
    class ArmyProductionUnitInfo : public UI::Group {
        GameState& gs;
        ProvinceId province_id;
        size_t idx;
    public:
        ArmyProductionUnitInfo(GameState& _gs, int x, int y, ProvinceId _province_id, size_t _idx, UI::Widget* parent);
    };

    class ArmyView : public UI::Window {
        GameState& gs;
    public:
        ArmyView(GameState& gs);

        ArmyUnitsTab* units_tab;
        ArmyProductionTab* production_tab;
    };
}
