#pragma once

#include "../../nation.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

namespace Interface {
    class TreatyChooseWindow : public UI::Window {
        GameState& gs;
        Treaty* treaty;

        UI::Text* body_txt;
        UI::Button* approve_btn,* deny_btn;
    public:
        TreatyChooseWindow(GameState& gs, Treaty* treaty);
    };
};