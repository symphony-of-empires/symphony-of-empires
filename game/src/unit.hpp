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
#include "unified_render/entity.hpp"
#include "unified_render/decimal.hpp"

class Nation;
class Good;
class Province;

// Defines a type of unit, it can be a tank, garrison, infantry, etc
// this is moddable via a lua script and new unit types can be added
class UnitType : public RefnameEntity<uint8_t> {
public:
    //UnitType();
    //~UnitType();

    UnifiedRender::Decimal supply_consumption;
    UnifiedRender::Decimal speed;
    UnifiedRender::Decimal max_health;
    UnifiedRender::Decimal defense;
    UnifiedRender::Decimal attack;

    // Max ticks allowed for defensive bonus - this basically prevents an unit from staying 200
    // years on the same spot and be fucking indestructible
    UnifiedRender::Number max_defensive_ticks;

    // The defense provided by the unit each tick for staying on the same position
    // aka. digging trenches
    UnifiedRender::Number position_defense;

    // Capacity of units that can be carried (transport units)
    UnifiedRender::Number capacity;

    // Time needed to build
    UnifiedRender::Number build_time;

    // Can go on ground?
    bool is_ground;
    
    // Can go on water?
    bool is_naval;

    // Required goods, first describes the id of the good and the second describes how many
    std::vector<std::pair<Good *, UnifiedRender::Number>> req_goods;
};

// A trait for an unit; given randomly per each recruited unit
class UnitTrait : public RefnameEntity<uint8_t> {
public:
    //UnitTrait();
    //~UnitTrait();
    
    UnifiedRender::Decimal supply_consumption_mod;
    UnifiedRender::Decimal speed_mod;
    UnifiedRender::Decimal max_health_mod;
    UnifiedRender::Decimal defense_mod;
    UnifiedRender::Decimal attack_mod;
    UnifiedRender::Decimal morale_mod;
};

// Roughly a batallion, consisting of approximately 500 soldiers each
class Unit : public IdEntity<uint16_t> {
public:
    //Unit();
    //~Unit();

    void attack(Unit& enemy);
    std::pair<UnifiedRender::Number, UnifiedRender::Number> get_pos(void) const;
    void set_target(Province& province);
    UnifiedRender::Decimal get_speed(const Province& province) const;
    UnifiedRender::Decimal get_speed(void) const;
    void set_province(Province& province);
    bool can_move(void) const;
    
    // Type of unit
    UnitType* type;
    // Who owns this unit
    Nation* owner;
    Province* target = nullptr;
    Province* province = nullptr;

    // Size of the unit (soldiers in unit)
    UnifiedRender::Number size;
    // Base size of the unit (max size due to anti-attrition)
    UnifiedRender::Number base;
    UnifiedRender::Decimal move_progress;
    UnifiedRender::Decimal morale;
    // For perspective, 0.5 is the normal unit (i.e a soldier POP)
    UnifiedRender::Decimal experience;
    // Available supplies, 1.0 is all supplies fullfilled, lower than that and the unit starts shrinking
    UnifiedRender::Decimal supply;
    // Money that the unit has
    UnifiedRender::Decimal budget;
    // TODO: ser/deser
    bool on_battle = false;
};