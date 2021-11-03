#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui.hpp"

namespace Interface {
    class NationView : public UI::Window {
        GameState& gs;
        Nation* nation;

        UI::Image* flag_img;
        UI::Label* name_lab;
        UI::Label* ideology_lab;
        
        UI::CloseButton* close_btn;
    public:
        NationView(GameState& gs, Nation* nation);
    };
};