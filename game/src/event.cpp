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
//      event.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <cassert>

#include "event.hpp"
#include "world.hpp"
#include "nation.hpp"

//
// Event
//
void Event::take_decision(Nation& sender, Decision& dec) {
    /// @todo Why was this check put in the first place?
    //auto receiver = std::find(receivers.begin(), receivers.end(), &sender);
    // Confirm that the sender is in receiver's list
    //assert(receiver != receivers.end());

    // Tell the world that we took a decision
    g_world.taken_decisions.push_back(std::make_pair(dec, &sender));
    
    // Remove from inbox too
    std::erase_if(sender.inbox, [this](const auto& e) {
        return this->ref_name == e.ref_name;
    });
}
