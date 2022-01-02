#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui/window.hpp"

#include "client/interface/common.hpp"

namespace UI {
    class PieChart;
    class Chart;
    class Button;
}

namespace Interface {
    class PoliciesScreen : public UI::Window {
        GameState& gs;
        Policies new_policy;
    public:
        PoliciesScreen(GameState& gs);
    };
};