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
//      client/interface/province_view.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/ui/group.hpp"
#include "eng3d/ui/window.hpp"
#include "eng3d/ui/slider.hpp"
#include "eng3d/ui/input.hpp"
#include "eng3d/ui/image.hpp"

#include "world.hpp"
#include "world.hpp"
#include "client/game_state.hpp"

namespace UI {
    class PieChart;
    class Chart;
}

namespace Interface {
    // View information related to the population and diversity of languages and religions
    // on the specified province
    class ProvincePopulationTab : public UI::Group {
        GameState& gs;
        const Province& province;
        UI::PieChart* languages_pie;
        UI::PieChart* religions_pie;
        UI::PieChart* pop_types_pie;
        UI::Widget& create_pop_table();
        UI::Widget& create_stock_table();
    public:
        ProvincePopulationTab(GameState& gs, int x, int y, Province& province, UI::Widget* _parent);
        void update_piecharts();
    };

    // View information related to the economic activity of the province
    struct ProvinceEconomyTab : UI::Group {
        ProvinceEconomyTab(GameState& gs, int x, int y, Province& province, UI::Widget* _parent);
        GameState& gs;
        Province& province;
    };

    struct ProvinceBuildingTab : UI::Group {
        ProvinceBuildingTab(GameState& gs, int x, int y, Province& province, UI::Widget* _parent);
        GameState& gs;
        Province& province;
    };

    struct ProvinceEditLanguageTab : UI::Group {
        ProvinceEditLanguageTab(GameState& gs, int x, int y, Province& province, UI::Widget* _parent);
        GameState& gs;
        Province& province;
        Language& language;
        Religion& religion;
    };

    struct ProvinceEditTerrainTab : UI::Group {
        ProvinceEditTerrainTab(GameState& gs, int x, int y, Province& province, UI::Widget* _parent);
        GameState& gs;
        Province& province;
        TerrainType& terrain_type;
    };

    class ProvinceView : public UI::Window {
        GameState& gs;
        Province& province;
        ProvincePopulationTab* pop_tab;
        ProvinceEconomyTab* econ_tab;
        ProvinceBuildingTab* build_tab;
        ProvinceEditLanguageTab* edit_language_tab;
        ProvinceEditTerrainTab* edit_terrain_tab;
    public:
        ProvinceView(GameState& gs, Province& province);
        ~ProvinceView() override;
        friend struct ProvinceBuildingTab;
    };

    // Brief
    class ProvinceBrief : public UI::Window {
        UI::PieChart* languages_pie;
        UI::Chart* economy_chart;
    public:
        ProvinceBrief(GameState& gs, const Province& province);
    };
}
