#ifndef PROVINCE_VIEW_HPP
#define PROVINCE_VIEW_HPP

#include "../../nation.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

#include "common.hpp"

namespace Interface {
    class PoliciesScreen : public UI::Window {
        GameState& gs;
        UI::PieChart* ideology_pie;
        UI::Chart* militancy_chart;
        UI::Chart* con_chart;
    public:
        PoliciesScreen(GameState& gs);
    };
};

#endif