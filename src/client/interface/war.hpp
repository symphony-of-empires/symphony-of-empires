#pragma once

#include "../../nation.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

namespace Interface {
    class WarDeclarePrompt : public UI::Window {
        GameState& gs;
        Nation* nation;

        UI::Text* body_txt;
        UI::Button* approve_btn,* deny_btn;
    public:
        WarDeclarePrompt(GameState& gs, Nation* nation);
    };
};