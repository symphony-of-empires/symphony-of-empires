#pragma once

#include "client/ui.hpp"

class GameState;
namespace Interface {
    class TopWindow : public UI::Group {
        GameState& gs;
    public:
        TopWindow(GameState& gs);

        UI::Chart *gdp_chart, *pop_chart, *hdi_chart;
        UI::Label *money_lab, *prestige_lab, *economy_lab, *big_brain_lab, *militancy_lab, *population_lab;
        UI::Image *money_icon, *prestige_icon, *economy_icon, *big_brain_icon, *militancy_icon, *population_icon;
    };
};