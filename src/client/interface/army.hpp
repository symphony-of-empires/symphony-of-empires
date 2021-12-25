#pragma once

#include "client/ui.hpp"
#include "client/interface/common.hpp"

class World;
class Nation;
class GameState;
class Building;

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
        Building *building = nullptr;

        UI::Image *unit_icon;
        UI::Label *province_lab;
        UI::Label *name_lab;
    public:
        ArmyProductionUnitInfo(GameState& gs, int x, int y, Building *building, UI::Widget* parent);
    };

    class ArmyNewUnitTab : public UI::Group {
        GameState& gs;
    public:
        ArmyNewUnitTab(GameState& gs, int x, int y, UI::Widget* parent);
        UnitType* unit_type = nullptr;
    };

    class ArmySelectUnitTab : public UI::Group {
        GameState& gs;
    public:
        ArmySelectUnitTab(GameState& gs, int x, int y, UI::Widget* parent);
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
        ArmySelectUnitTab* select_unit_tab;
    };
};
