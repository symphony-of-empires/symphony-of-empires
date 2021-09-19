#ifndef BUILD_UNIT_WINDOW_H
#define BUILD_UNIT_WINDOW_H

#include "../../unit.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

// TODO: Make it close it's children when closing

class BuildUnitWindow : public UI::Window {
   public:
    BuildUnitWindow(GameState& _gs, Outpost* _outpost, UI::Window* top_win);

    Outpost* outpost;
    GameState& gs;
};

#endif