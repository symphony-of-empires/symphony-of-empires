#pragma once

#include "client/game_state.hpp"
#include "client/ui.hpp"

class MainMenuConnectServer: public UI::Window {
public:
    MainMenuConnectServer(GameState& gs);

    GameState& gs;
    bool in_game;

    UI::Input* ip_addr_inp;
    UI::Input* username_inp;
    UI::Button* conn_btn;
};