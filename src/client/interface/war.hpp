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
    class WarDeclarePrompt : public UI::Window {
        GameState& gs;
        Nation* nation;

        UI::Text* body_txt;
        UI::Button* approve_btn,* deny_btn;
    public:
        WarDeclarePrompt(GameState& gs, Nation* nation);
    };
};