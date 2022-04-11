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

#include "unified_render/print.hpp"

#include "event.hpp"
#include "world.hpp"
#include "nation.hpp"

//
// Descision
//
/*
Descision::Descision(void) {

}

Descision::~Descision(void) {

}
//*/

//
// Event
//
/*
Event::Event(void) {

}

Event::~Event(void) {

}
//*/

void Event::take_descision(Nation& sender, Descision& dec) {
    auto receiver = std::find(receivers.begin(), receivers.end(), &sender);

    // Confirm that the sender is in receiver's list
    if(receiver == receivers.end()) {
        // Not in receiver's list so it's not allowed
        return;
    }

    // Tell the world that we took a descision
    g_world->taken_descisions.push_back(std::make_pair(&dec, &sender));

    // Remove from the receivers list so we don't duplicate descisions
    receivers.erase(receiver);

    // Remove from inbox too
    for(auto it = sender.inbox.begin(); it != sender.inbox.end(); it++) {
        if((*it)->ref_name == this->ref_name) {
            sender.inbox.erase(it);
            break;
        }
    }
}
