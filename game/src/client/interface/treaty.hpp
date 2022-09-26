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
//      client/interface/treaty.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "eng3d/ui/window.hpp"

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"

namespace UI {
    class Text;
    class Button;
}

namespace Interface {
    class TreatyDraftView : public UI::Window {
        GameState& gs;
        Treaty treaty;
        Nation& nation;
    public:
        TreatyDraftView(GameState& gs, Nation& nation);
    };

    class TreatyChooseWindow : public UI::Window {
        GameState& gs;
        const TreatyId treaty_id;

        UI::Text* body_txt;
        UI::Button* approve_btn,* deny_btn;
    public:
        TreatyChooseWindow(GameState& gs, const TreatyId treaty_id);
    };
};