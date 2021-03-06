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
#include "ideology.hpp"
#include "culture.hpp"
#include "religion.hpp"
#include "world.hpp"

Ideology& Pop::get_ideology() const {
    // Obtain ideology with highest approval
    auto* v = &g_world.ideologies[std::distance(
        this->ideology_approval.begin(),
        std::max_element(
            this->ideology_approval.begin(),
            this->ideology_approval.end()
        )
    )];
    return *v;
}

uint32_t Pop::get_type_id() const {
    uint32_t id = this->type_id;
    id |= this->culture_id << 8;
    id |= this->religion_id << 24;
    return id;
}
