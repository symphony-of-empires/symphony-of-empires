#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui.hpp"

#include "client/interface/common.hpp"

namespace Interface {
    class PoliciesScreen : public UI::Window {
        GameState& gs;
        UI::PieChart* ideology_pie;
        UI::Chart* militancy_chart;
        UI::Chart* con_chart;

        UI::Button* enact_btn;
        Policies new_policy;
    public:
        PoliciesScreen(GameState& gs);
    };
};