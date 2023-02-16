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
//      client/interface/policies.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include "world.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "eng3d/ui/window.hpp"

namespace UI {
    class PieChart;
    class Chart;
    class Button;
}

namespace Interface {
    class PoliciesView : public UI::Window {
        GameState& gs;
        Policies new_policy;
        std::vector<float> commodity_production;
    public:
        PoliciesView(GameState& gs);
    };
}
