// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/lobby.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <cstdlib>

class GameState;
namespace UI {
    class Button;
    class Image;
    class Window;
}

namespace Interface {
    /*class LobbyPlayerView : public UI::Window {
        GameState& gs;
    public:
        LobbyPlayerView(GameState& gs) {};
        ~LobbyPlayerView() {};
    };*/

    class LoadGameBtnData {
    public:
        GameState& gs;
        std::string file;
    };

    class LobbySelectView {
        GameState& gs;
        UI::Window* ctrl_window;
        UI::Image* curr_country_flag_img;
        UI::Button* curr_country_btn;
        std::vector<class LoadGameBtnData> ldgame_data;
    public:
        LobbySelectView(GameState& gs);
        ~LobbySelectView() {};

        void change_nation(size_t id);
        size_t curr_selected_nation;
    };
};