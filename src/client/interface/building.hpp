#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui/window.hpp"
#include "client/ui/group.hpp"
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