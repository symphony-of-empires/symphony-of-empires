#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui/window.hpp"

namespace UI {
    class Image;
    class Label;
    class Button;
    class CloseButton;
}
namespace Interface {
    class NationMarketView : public UI::Window {
        GameState& gs;
        Nation* nation;
    public:
        NationMarketView(GameState& gs, Nation* nation);
    };

    class NationView : public UI::Window {
        GameState& gs;
        Nation* nation;

        UI::Image* flag_img;
        UI::Label* name_lab;
        UI::Label* ideology_lab;

        UI::Label* rel_lab;
        UI::Label* interest_lab;
        
        UI::Button* inc_btn;
        UI::Button* dec_btn;

        UI::Button* dow_btn;
        UI::Button* ally_btn;
        UI::Button* defensive_pact_btn;
        UI::Button* embargo_btn;
        UI::Button* allow_market_access_btn;
        UI::Button* allow_military_access_btn;
        UI::Button* propose_truce_btn;
        
        UI::CloseButton* close_btn;
    public:
        NationView(GameState& gs, Nation* nation);
    };
};