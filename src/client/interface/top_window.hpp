#ifndef TOP_WINDOW
#define TOP_WINDOW

#include "../game_state.hpp"
#include "../ui.hpp"

class GameState;

class TopWindow {
   public:
    TopWindow(GameState& gs);
    GameState& gs;
    UI::Window* top_win;
    UI::Chart *gdp_chart, *pop_chart, *hdi_chart;
    UI::Label *money_lab, *prestige_lab, *economy_lab, *big_brain_lab, *militancy_lab, *population_lab;
    UI::Image *money_icon, *prestige_icon, *economy_icon, *big_brain_icon, *militancy_icon, *population_icon;

    static void show_industry_cb(UI::Widget& w, TopWindow* data);
    static void show_pops_cb(UI::Widget& w, TopWindow* data);
    static void show_reforms_cb(UI::Widget& w, TopWindow* data);
};

#endif