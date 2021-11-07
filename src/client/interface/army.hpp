#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui.hpp"
#include "client/interface/common.hpp"

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

    class ArmyView : public UI::Window {
        GameState& gs;

        ArmyArmyTab* army_tab;
        ArmyAirforceTab* airforce_tab;
        ArmyNavyTab* navy_tab;
    public:
        ArmyView(GameState& gs);
    };
};
