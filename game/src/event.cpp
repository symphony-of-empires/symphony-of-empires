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
//      event.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <cassert>

#include "world.hpp"
#include "world.hpp"
#include "world.hpp"

//
// Event
//
void Event::take_decision(Nation& sender, Decision& dec) {
    g_world.taken_decisions.emplace_back(dec, sender); // Tell the world that we took a decision
    std::erase_if(sender.inbox, [this](const auto& e) { // Remove from inbox too
        return this->ref_name == e.ref_name;
    });
}
