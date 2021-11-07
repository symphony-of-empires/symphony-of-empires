#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui.hpp"
#include "client/interface/common.hpp"

namespace Interface {
    class BuildingSelectCompanyTab : public UI::Group {
        GameState& gs;
    public:
        BuildingSelectCompanyTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

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

    // Set building site for production and placement of a new building
    class BuildingBuildView : public UI::Window {
        GameState& gs;
    public:
        Nation* nation;
        Province* province;
        Company* company;
        
        BuildingSelectCompanyTab* company_tab;
        BuildingSelectProvinceTab* province_tab;
        BuildingSelectNationTab* nation_tab;

        bool in_tile;
        int tx, ty;
        BuildingBuildView(GameState& gs, int tx, int ty, bool in_tile, Nation* nation = nullptr, Province* province = nullptr, Company* company = nullptr);
    };

    class BuildingView : public UI::Window {
        GameState& gs;
    public:

    };
};