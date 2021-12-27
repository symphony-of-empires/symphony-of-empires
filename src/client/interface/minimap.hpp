#pragma once

#include "client/ui/window.hpp"
#include "client/game_state.hpp"

namespace Interface {
    class Minimap: public UI::Window {
        GameState& gs;
    public:
        Minimap(GameState& gs, int x, int y, UI::Origin origin);
    };
};