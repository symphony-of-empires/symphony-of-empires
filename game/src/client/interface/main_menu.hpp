// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/main_menu.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "client/game_state.hpp"
#include "unified_render/ui/window.hpp"
#include "unified_render/ui/div.hpp"

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

    class Settings;
    class MainMenu: public UI::Div {
    public:
        MainMenu(GameState& gs);
        ~MainMenu() override;

        GameState& gs;
        MainMenuConnectServer* connect_window = nullptr;
        Settings* settings_window = nullptr;
    };
};