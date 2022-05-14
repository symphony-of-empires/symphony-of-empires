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
//      unit.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <type_traits>

#include "eng3d/entity.hpp"
#include "eng3d/decimal.hpp"

class Nation;
class Good;
class Province;

// Defines a type of unit, it can be a tank, garrison, infantry, etc
// this is moddable via a lua script and new unit types can be added
class UnitType : public RefnameEntity<uint8_t> {
public:
    //UnitType();
    //~UnitType();

    Eng3D::Decimal supply_consumption;
    Eng3D::Decimal speed;
    Eng3D::Decimal max_health;
    Eng3D::Decimal defense;
    Eng3D::Decimal attack;

    // Capacity of units that can be carried (transport units)
    Eng3D::Number capacity;

    // Time needed to build
    Eng3D::Number build_time;

    // Can go on ground?
    bool is_ground;
    
    // Can go on water?
    bool is_naval;

    // Required goods, first describes the id of the good and the second describes how many
    std::vector<std::pair<Good *, Eng3D::Number>> req_goods;
};

// A trait for an unit; given randomly per each recruited unit
class UnitTrait : public RefnameEntity<uint8_t> {
public:
    //UnitTrait();
    //~UnitTrait();
    
    Eng3D::Decimal supply_consumption_mod;
    Eng3D::Decimal speed_mod;
    Eng3D::Decimal max_health_mod;
    Eng3D::Decimal defense_mod;
    Eng3D::Decimal attack_mod;
    Eng3D::Decimal morale_mod;
};

// Roughly a batallion, consisting of approximately 500 soldiers each
class Unit : public IdEntity<uint16_t> {
public:
    void attack(Unit& enemy);
    std::pair<Eng3D::Number, Eng3D::Number> get_pos(void) const;
    void set_target(Province& province);
    Eng3D::Decimal get_speed(const Province& province) const;
    Eng3D::Decimal get_speed(void) const;
    void set_province(Province& province);
    bool can_move(void) const;
    
    // Type of unit
    UnitType* type;
    // Who owns this unit
    Nation* owner;
    Province* target = nullptr;
    Province* province = nullptr;

    // Size of the unit (soldiers in unit)
    Eng3D::Number size;
    // Base size of the unit (max size due to anti-attrition)
    Eng3D::Number base;
    Eng3D::Decimal move_progress;
    Eng3D::Decimal morale;
    // For perspective, 0.5 is the normal unit (i.e a soldier POP)
    Eng3D::Decimal experience;
    // Available supplies, 1.0 is all supplies fullfilled, lower than that and the unit starts shrinking
    Eng3D::Decimal supply;
    // Money that the unit has
    Eng3D::Decimal budget;
    // TODO: ser/deser
    bool on_battle = false;
};