#ifndef PROVINCE_VIEW_H
#define PROVINCE_VIEW_H

#include "../../nation.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

class GameState;
class ProvinceView : public UI::Window {
public:
    ProvinceView(GameState& gs, const Tile& tile);

    GameState& gs;
    Province* province;

    static void colonize_province_cb(UI::Widget& w, ProvinceView* data);
};

class ProvinceViewEconomic : public UI::Window {
public:
    ProvinceViewEconomic(GameState& gs, Province* province);

    GameState& gs;
    Province* province;
};

#endif