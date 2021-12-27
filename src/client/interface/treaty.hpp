#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui/window.hpp"

namespace UI {
    class Text;
    class Button;
}

namespace Interface {
    class TreatyDraftView : public UI::Window {
        GameState& gs;
        Treaty* treaty;
    public:
        TreatyDraftView(GameState& gs);
    };

    class TreatyChooseWindow : public UI::Window {
        GameState& gs;
        Treaty* treaty;

        UI::Text* body_txt;
        UI::Button* approve_btn,* deny_btn;
    public:
        TreatyChooseWindow(GameState& gs, Treaty* treaty);
    };
};