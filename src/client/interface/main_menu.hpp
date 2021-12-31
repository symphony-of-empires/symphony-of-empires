#pragma once

#include "client/game_state.hpp"
#include "client/ui/window.hpp"

namespace UI {
    class Input;
    class Button;
}

namespace Interface {
    class MainMenuConnectServer: public UI::Window {
        GameState& gs;
    public:
        MainMenuConnectServer(GameState& gs);

        UI::Input* ip_addr_inp;
        UI::Input* username_inp;
        UI::Button* conn_btn;
    };

    class MainMenuSettings : public UI::Window {
        GameState& gs;
    public:
        MainMenuSettings(GameState& gs);
    };

    class MainMenu: public UI::Window {
    public:
        MainMenu(GameState& gs);
        ~MainMenu() override;

        GameState& gs;
        MainMenuConnectServer* connect_window = nullptr;
        MainMenuSettings* settings_window = nullptr;
    };
};