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
//      province.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "province.hpp"
#include "world.hpp"
#include "product.hpp"
#include "good.hpp"
#include "unit.hpp"
#include "building.hpp"
#include "pop.hpp"

//
// Province
//

// Calculates the total number of POPs in this province (total population)
Eng3D::Number Province::total_pops() const {
    Eng3D::Number total = 0;
    for(const auto& pop : pops)
        total += pop.size;
    return total;
}

Eng3D::Decimal Province::get_attractiveness(const Pop& pop) const {
    Eng3D::Decimal attractive = this->base_attractive;
    if(!this->owner->is_accepted_culture(pop) && !this->owner->is_accepted_religion(pop)) {
        // Linearized version, instead of using if-else trees we just
        // multiply the attractive by the scale; EXTERMINATE = 3, so 3 - 3 is 0 which nullifies the attractivenes
        // and the more open the borders are the more lenient the "scale" becomes
        const Eng3D::Number scale = 3 - this->owner->current_policy.treatment;
        attractive *= scale;
    }

    // A social value between 0 and 1 is for poor people, the value for medium class
    // is between 1 and 2, for the rich is above 2
    if(pop.type->social_value >= 0.f && pop.type->social_value <= 1.f) {
        // For the lower class, lower taxes is good, and so on for other POPs
        attractive += -(this->owner->current_policy.poor_flat_tax);
    } else if(pop.type->social_value >= 1.f && pop.type->social_value <= 2.f) {
        // For the medium class
        attractive += -(this->owner->current_policy.med_flat_tax);
    } else if(pop.type->social_value >= 2.f) {
        // For the high class
        attractive += -(this->owner->current_policy.rich_flat_tax);
    }
    return attractive;
}

void Province::add_building(const BuildingType& building_type) {
    // Now build the building
    this->buildings[g_world->get_id(building_type)].level += 1;
    this->buildings[g_world->get_id(building_type)].req_goods = building_type.req_goods;
}
