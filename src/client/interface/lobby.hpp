#pragma once

#include "client/ui.hpp"

class GameState;
namespace Interface {
    /*class LobbyPlayerView : public UI::Window {
        GameState& gs;
    public:
        LobbyPlayerView(GameState& gs) {};
        ~LobbyPlayerView() {};
    };*/

    class LobbySelectView {
        GameState& gs;
        UI::Button* curr_country_btn;
        UI::Label* select_country_lab;
        UI::Button* next_country_btn;
        UI::Button* prev_country_btn;
    public:
        LobbySelectView(GameState& gs);
        ~LobbySelectView() {};

        void change_nation(size_t id);
        size_t curr_selected_nation;
    };
};