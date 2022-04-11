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
//      client/interface/good_view.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "unified_render/ui/window.hpp"

#include "good.hpp"
#include "world.hpp"
#include "client/game_state.hpp"

namespace UI {
    class Label;
    class Image;
    class PieChart;
    class CloseButton;
    class Chart;
    class Group;
}

namespace Interface {
    class ProductView : public UI::Window {
        GameState& gs;
        Product* product;

        UI::Image* icon_img;
        UI::PieChart *supply_pie;
        UI::Chart* price_chart,* supply_chart,* demand_chart;
        UI::CloseButton* close_btn;
    public:
        ProductView(GameState& gs, Product* product);
    };

    class GoodView : public UI::Window {
        GameState& gs;
        Good* good;

        UI::Image* icon_img;
        UI::PieChart *sellers_pie;
        UI::Chart* avg_price_chart;
        UI::CloseButton* close_btn;
    public:
        GoodView(GameState& gs, Good* good);
    };
};