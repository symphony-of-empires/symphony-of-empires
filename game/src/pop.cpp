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
//      pop.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "pop.hpp"
#include "world.hpp"

bool Pop::operator==(const Pop& rhs) const {
    return (this->culture == rhs.culture && this->religion == rhs.religion && this->type == rhs.type);
}

Ideology* Pop::get_ideology(void) const {
    // Obtain ideology with highest approval
    auto v = g_world->ideologies[std::distance(
        this->ideology_approval.begin(),
        std::max_element(
            this->ideology_approval.begin(),
            this->ideology_approval.end()
        )
    )];
    return v;
}