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

#include <string>
#include <algorithm>
#include <cmath>
#include <cassert>
#ifndef M_PI
#   define M_PI 3.1415f
#endif

#include "eng3d/log.hpp"
#include "eng3d/common.hpp"

#include "unit.hpp"
#include "province.hpp"
#include "world.hpp"

//
// Unit
//
void Unit::attack(Unit& enemy) {
    /// @todo Better attack algorithm

    // It's important that a size of zero nullifies the attack, this prevents the edge case
    // of 1 v 1 units that kill each other
    const float damage = (type->attack * (float)this->size) * 0.025f;
    enemy.size -= std::min<size_t>(enemy.size, damage);
}

std::pair<float, float> Unit::get_pos() const {
    const World& world = World::get_instance();
    auto prov_id = this->province_id();
    auto& province = world.provinces[prov_id];
    return province.get_pos();
}

Province::Id Unit::province_id() const {
    // Don't know if this is cleaner than getting it from unit manager :thinking:
    const World& world = World::get_instance();
    return world.unit_manager.unit_province[cached_id];
}

void Unit::set_target(const Province& _province) {
    assert(this->target_province_id != this->province_id());

    const World& world = World::get_instance();
    assert(this->can_move());
    this->target_province_id = world.get_id(_province);

    // Calculate the required movement before it can reach the target
    const auto& cur_pos = world.provinces[this->province_id()].get_pos();
    const auto& target_pos = world.provinces[this->target_province_id].get_pos();
    this->move_progress = std::sqrt(std::abs(cur_pos.first - target_pos.first) + std::abs(cur_pos.second - target_pos.second));
}

float Unit::get_speed(const Province& _province) const {
    const World& world = World::get_instance();
    auto start_pos = world.provinces[this->province_id()].get_pos();
    auto end_pos = _province.get_pos();

    // Get the linear distance from the current deduced position of the unit and the target
    // the current position of the unit is relative to the move progress it has done (so if it's
    // halfway thru a province it will then be placed at half of the distance)
    const float x_dist = (end_pos.first - start_pos.first);
    const float y_dist = (end_pos.second - start_pos.second);
    const float angle = std::atan2(x_dist, y_dist);

    /// @todo The comment above makes no sense since we don't do (max_move_progress / move_progress)
    const float dist_div = move_progress;

    //const float linear_dist = std::fabs(std::sqrt(x_dist * x_dist + y_dist * y_dist) / dist_div);
    const float speed = (type->speed) / _province.terrain_type->movement_penalty;
    float radius_scale = std::cos(M_PI / (2 * World::get_instance().height) * (2 * (y_dist / dist_div) - World::get_instance().height));
    float x_scale = 1 / (std::fabs(radius_scale) + 0.001f);
    float speed_scale = std::sqrt(std::pow(std::sin(angle), 2) + std::pow(std::cos(angle) * x_scale, 2));
    return (speed * speed_scale) / 100.f;
}

float Unit::get_speed() const {
    return this->get_speed(World::get_instance().provinces[this->target_province_id]);
}

void UnitManager::init(World& world) {
    province_units.resize(world.provinces.size());
}

void UnitManager::add_unit(Unit unit, Province::Id unit_current_province) {
    Unit::Id id;
    if(free_unit_slots.size() > 0) {
        id = free_unit_slots.back();
        free_unit_slots.pop_back();
        units[id] = unit;
        unit_province[id] = unit_current_province;
    } else {
        id = units.size();
        units.emplace_back(unit);
        unit_province.push_back(unit_current_province);
    }
    units[id].cached_id = id;
    if(unit_current_province >= province_units.size()) {
        province_units.resize(unit_current_province + 1);
    }
    province_units[unit_current_province].push_back(id);
}

void UnitManager::remove_unit(Unit::Id unit_id) {
    const Province::Id current_province_id = unit_province[unit_id];
    Eng3D::fast_erase(province_units[current_province_id], unit_id);
    units[unit_id].cached_id = Province::invalid();
}

void UnitManager::move_unit(Unit::Id unit_id, Province::Id target_province_id) {
    assert(units[unit_id].can_move()); // Must be able to move to perform this...
    assert(unit_province[unit_id] != target_province_id); // Not setting to same province

    const Province::Id current_province_id = unit_province[unit_id];
    Eng3D::fast_erase(province_units[current_province_id], unit_id);
    unit_province[unit_id] = target_province_id;
    province_units[current_province_id].push_back(unit_id);
}

/*
- Hi, greetings!
- Symphony Of Empires is a game were you rule a country in the victorian era
- You can do whatever you want! But you must keep your nation together by 1935
- We're in the Netherlands, we have a colony on the Indonesian archipielago
*/
