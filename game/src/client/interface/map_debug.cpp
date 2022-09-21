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
//      client/interface/event.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/serializer.hpp"
#include "eng3d/ui/text.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/group.hpp"
#include "eng3d/network.hpp"

#include "client/interface/map_debug.hpp"
#include "client/game_state.hpp"
#include "client/map.hpp"

using namespace Interface;

MapDebugMenu::MapDebugMenu(GameState& _gs)
    : UI::Window(0, 0, 200, 200),
    gs{ _gs }
{
    this->is_scroll = false;
    
    auto* reload_shader_btn = new UI::Button(20, 20, 100, 24, this);
    reload_shader_btn->text("Reload shades");
    reload_shader_btn->set_on_click([this](UI::Widget&) {
        this->gs.map->reload_shaders();
    });
}