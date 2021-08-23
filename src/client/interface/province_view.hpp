#ifndef PROVINCE_VIEW_H
#define PROVINCE_VIEW_H

#include "../ui.hpp"
#include "../game_state.hpp"
#include "industry.hpp"
#include "nation.hpp"
#include "world.hpp"

class GameState;
class ProvinceView {
   public:
    ProvinceView(GameState& gs, UI::Window* top_win, const Tile& tile);

    GameState& gs;
    Province* selected_province;
    static void colonize_province_cb(UI::Widget& w, ProvinceView* data);
};

#endif