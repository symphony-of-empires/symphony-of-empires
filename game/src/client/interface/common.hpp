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
//      client/interface/common.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/ui/button.hpp"
#include "eng3d/ui/group.hpp"
#include "eng3d/ui/chart.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/label.hpp"

class World;
class Nation;
class GameState;
class Unit;
class UnitType;
class Province;
class Product;
class Technology;
class Good;
class Building;
class BuildingType;

namespace Interface {
    // Pretty brief summaries (for example to click oon a button from a list)
    class UnitButton : public UI::Button {
        GameState& gs;
    public:
        Unit& unit;
        UnitButton(GameState& gs, int x, int y, Unit& unit, UI::Widget* parent);
    };

    class UnitTypeButton : public UI::Group {
        GameState& gs;
        UI::Image* icon_img;
        UI::Button* name_btn;
    public:
        UnitType& unit_type;
        UnitTypeButton(GameState& gs, int x, int y, UnitType& unit_type, UI::Widget* parent);
    };

    class NationButton : public UI::Group {
        GameState& gs;
        UI::Image* flag_icon;
        UI::Button* name_btn;
    public:
        Nation& nation;
        NationButton(GameState& gs, int x, int y, Nation& nation, UI::Widget* parent);
    };
    
    class ProductInfo : public UI::Group {
        GameState& gs;
        Province& province;
        Good& good;

        UI::Image* good_ibtn;
        UI::Button* name_btn,* province_btn;
        UI::Button* price_btn,* price_rate_btn;
        UI::Chart* price_chart,* demand_chart,* supply_chart;

        std::deque<float> price_history, supply_history, demand_history;
    public:
        ProductInfo(GameState& gs, int x, int y, Province& province, Good& good, UI::Widget* parent);
    };
};