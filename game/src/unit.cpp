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
//      unit.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cmath>
#ifndef M_PI
#   define M_PI 3.1415f
#endif
#include "eng3d/assert.hpp"

#include "unit.hpp"
#include "province.hpp"
#include "world.hpp"

//
// Unit type
//

/*
UnitType::UnitType(void) {

}

UnitType::~UnitType(void) {

}
//*/

//
// Unit trait
//

/*
UnitTrait::UnitTrait(void) {

}

UnitTrait::~UnitTrait(void) {
    
}
//*/

//
// Unit
//
#include "eng3d/log.hpp"
#include <string>
#include <algorithm>
Unit::Unit(void) {
    Eng3D::Log::debug("unit", "Construct unit " + std::to_string((std::uintptr_t)this));
}

Unit::~Unit(void) {
    Eng3D::Log::debug("unit", "Destruct unit " + std::to_string((std::uintptr_t)this));
    if(province != nullptr) {
        const auto it = std::find(province->units.cbegin(), province->units.cend(), this);
        assert(it != province->units.cend());
        province->units.erase(it);
    }
}
//*/

void Unit::attack(Unit& enemy) {
    /// @todo Better attack algorithm
    const Eng3D::Decimal damage = type->attack;
    enemy.size -= std::min<size_t>(enemy.size, damage);
}

std::pair<Eng3D::Number, Eng3D::Number> Unit::get_pos(void) const {
    return province->get_pos();
}

void Unit::set_target(Province& _province) {
    assert(this->can_move());
    target = &_province;
    move_progress = std::sqrt(std::abs((province->max_x + ((province->max_x - province->min_x) / 2.f)) - (target->max_x + ((target->max_x - target->min_x) / 2.f))) + std::abs((province->max_y + ((province->max_y - province->min_y) / 2.f)) - (target->max_y + ((target->max_y - target->min_y) / 2.f))));
}

Eng3D::Decimal Unit::get_speed(const Province& _province) const {
    auto start_pos = province->get_pos();
    auto end_pos = _province.get_pos();

    // Get the linear distance from the current deduced position of the unit and the target
    // the current position of the unit is relative to the move progress it has done (so if it's
    // halfway thru a province it will then be placed at half of the distance)
    const Eng3D::Decimal x_dist = (end_pos.first - start_pos.first);
    const Eng3D::Decimal y_dist = (end_pos.second - start_pos.second);
    const Eng3D::Decimal angle = std::atan2(x_dist, y_dist);

    /// @todo The comment above makes no sense since we don't do (max_move_progress / move_progress)
    const Eng3D::Decimal dist_div = move_progress;

    //const float linear_dist = std::fabs(std::sqrt(x_dist * x_dist + y_dist * y_dist) / dist_div);
    
    const Eng3D::Decimal speed = (type->speed) / _province.terrain_type->movement_penalty;
    Eng3D::Decimal radius_scale = std::cos(M_PI / (2 * World::get_instance().height) * (2 * (y_dist / dist_div) - World::get_instance().height));
    Eng3D::Decimal x_scale = 1 / (std::fabs(radius_scale) + 0.001f);
    Eng3D::Decimal speed_scale = std::sqrt(std::pow(std::sin(angle), 2) + std::pow(std::cos(angle) * x_scale, 2));
    return (speed * speed_scale) / 100.f;
}

Eng3D::Decimal Unit::get_speed(void) const {
    return get_speed(*target);
}

void Unit::set_province(Province& _province) {
    assert(this->can_move()); // Must be able to move to perform this...

    // Delete the unit from the previous cache list of units
    if(province != nullptr) {
        std::erase_if(province->units, [this](const auto& e) {
            return e == this;
        });
    }

    province = &_province;
    // Add unit to "cache list" of units
    province->units.push_back(this);
}
