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
//      client/interface/province_view.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "unified_render/ui/group.hpp"
#include "unified_render/ui/window.hpp"
#include "unified_render/ui/slider.hpp"
#include "unified_render/ui/input.hpp"
#include "client/interface/common.hpp"

namespace UI {
    class PieChart;
    class Chart;
}
namespace Interface {
    // View information related to the population and diversity of cultures and religions
    // on the specified province
    class ProvincePopulationTab: public UI::Group {
        GameState& gs;
        Province* province;

        UI::PieChart* cultures_pie;
        UI::PieChart* religions_pie;
        UI::PieChart* pop_types_pie;

        std::vector<PopInfo*> pop_infos;
    public:
        ProvincePopulationTab(GameState& gs, int x, int y, Province* province, UI::Widget* _parent);

        void update_piecharts();
    };

    // View information related to the economic activity of the province
    class ProvinceEconomyTab: public UI::Group {
        UI::PieChart* products_pie;
        UI::PieChart* goods_pie;

        std::vector<ProductInfo*> product_infos;
    public:
        ProvinceEconomyTab(GameState& gs, int x, int y, Province* province, UI::Widget* _parent);

        GameState& gs;
        Province* province;
    };

    class ProvinceBuildingTab: public UI::Group {
        std::vector<BuildingInfo*> building_infos;
    public:
        ProvinceBuildingTab(GameState& gs, int x, int y, Province* province, UI::Widget* _parent);
        GameState& gs;
        Province* province;
    };

    class ProvinceEditCultureTab : public UI::Group {
    public:
        ProvinceEditCultureTab(GameState& gs, int x, int y, Province* province, UI::Widget* _parent);
        GameState& gs;
        Province* province;
        Culture* culture;
    };

    class ProvinceEditTerrainTab : public UI::Group {
    public:
        ProvinceEditTerrainTab(GameState& gs, int x, int y, Province* province, UI::Widget* _parent);
        GameState& gs;
        Province* province;
        TerrainType* terrain_type;
    };

    class ProvinceView: public UI::Window {
        GameState& gs;
        Province* province;
		
        UI::Input* rename_inp;
        UI::Slider* density_sld;

        ProvincePopulationTab* pop_tab;
        ProvinceEconomyTab* econ_tab;
        ProvinceBuildingTab* build_tab;
        ProvinceEditCultureTab* edit_culture_tab;
        ProvinceEditTerrainTab* edit_terrain_tab;
    public:
        ProvinceView(GameState& gs, Province* province);
        friend class ProvinceBuildingTab;
    };

    // Brief
    class ProvinceBrief: public UI::Window {
        UI::PieChart* cultures_pie;
        UI::Chart* economy_chart;
    public:
        ProvinceBrief(GameState& gs, Province* province);
    };
};