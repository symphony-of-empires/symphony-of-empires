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
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "eng3d/log.hpp"
#include "eng3d/common.hpp"

#include "server/server_network.hpp"
#include "action.hpp"
#include "unit.hpp"
#include "province.hpp"
#include "world.hpp"

//
// Unit
//
void Unit::attack(Unit& enemy) {
    assert(this->is_valid());
    assert(enemy.is_valid());
    /// @todo Better attack algorithm

    // It's important that a size of zero nullifies the attack, this prevents the edge case
    // of 1 v 1 units that kill each other
    const float damage = (type->attack * static_cast<float>(this->size)) * this->morale * 0.08f;
    enemy.size -= std::min<size_t>(enemy.size, damage);
}

glm::vec2 Unit::get_pos() const {
    assert(this->is_valid());
    const auto& world = World::get_instance();
    auto prov_id = this->province_id();
    auto& province = world.provinces[prov_id];
    return province.get_pos();
}

Province::Id Unit::province_id() const {
    assert(this->is_valid());
    // Don't know if this is cleaner than getting it from unit manager :thinking:
    const auto& world = World::get_instance();
    return world.unit_manager.unit_province[cached_id];
}

void Unit::set_target(const Province& province) {
    assert(this->is_valid());
    assert(this->target_province_id != this->province_id());
    assert(province.get_id() != this->province_id());
    assert(this->can_move());
    this->target_province_id = province.get_id();
    this->days_left_until_move = this->days_to_move_to(province);
}

void Unit::stop_movement() {
    this->target_province_id = Province::invalid();
    this->days_left_until_move = 0;
}

float Unit::days_to_move_to(const Province& _province) const {
    assert(this->is_valid());
    const auto& world = World::get_instance();
    auto start_province = world.provinces[this->province_id()];
    auto end_province = _province;

    const glm::vec2 world_size{ world.width, world.height };
    auto distance = start_province.euclidean_distance(end_province, world_size, 100);
    return distance;
}

bool Unit::update_movement(UnitManager& unit_manager) {
    if(Province::is_invalid(target_province_id)) return false;

    days_left_until_move--;
    if(days_left_until_move <= 0) {
        days_left_until_move = 0;

        unit_manager.move_unit(this->get_id(), target_province_id);
        target_province_id = Province::invalid();
        return true;
    }
    return false;
}

/// @brief Fill in the relationship vectors for each nation
void UnitManager::init(World& world) {
    province_units.resize(world.provinces.size());
}

void UnitManager::add_unit(Unit unit, Province::Id unit_current_province) {
    Unit::Id id;
    if(free_unit_slots.empty()) {
        id = units.size();
        units.emplace_back(unit);
        unit_province.push_back(unit_current_province);
    } else {
        id = free_unit_slots.back();
        free_unit_slots.pop_back();
        units[id] = unit;
        unit_province[id] = unit_current_province;
    }
    units[id].cached_id = id;
    if(unit_current_province >= province_units.size())
        province_units.resize(unit_current_province + 1);
    province_units[unit_current_province].push_back(id);

    if(g_server != nullptr)
        /// @todo Obtain the cached ID of the newly added unit
        g_server->broadcast(Action::UnitAdd::form_packet(units[id]));
}

void UnitManager::remove_unit(Unit::Id unit_id) {
    if(g_server != nullptr)
        g_server->broadcast(Action::UnitRemove::form_packet(units[unit_id]));

    const auto current_province_id = unit_province[unit_id];
    Eng3D::fast_erase(province_units[current_province_id], unit_id);
    units[unit_id].cached_id = Unit::invalid();
    free_unit_slots.push_back(unit_id);
}

void UnitManager::move_unit(Unit::Id unit_id, Province::Id target_province_id) {
    assert(units[unit_id].can_move()); // Must be able to move to perform this...
    assert(unit_province[unit_id] != target_province_id); // Not setting to same province

    const auto current_province_id = unit_province[unit_id];
    Eng3D::fast_erase(province_units[current_province_id], unit_id);
    unit_province[unit_id] = target_province_id;
    province_units[target_province_id].push_back(unit_id);

    if(g_server != nullptr)
        g_server->broadcast(Action::UnitMove::form_packet(units[unit_id], World::get_instance().provinces[target_province_id]));
}
