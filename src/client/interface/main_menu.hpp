#pragma once

#include "client/game_state.hpp"
#include "client/ui.hpp"

namespace Interface {
    class MainMenu : public UI::Window {
    public:
        MainMenu(GameState& gs);

        GameState& gs;
    };

    class MainMenuConnectServer : public UI::Window {
    public:
        MainMenuConnectServer(GameState& gs, MainMenu* parent);

        GameState& gs;
        bool in_game;

        UI::Input* ip_addr_inp;
        UI::Input* username_inp;
        UI::Button* conn_btn;
    };
};