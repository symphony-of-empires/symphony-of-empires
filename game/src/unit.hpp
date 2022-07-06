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

class Nation;
class Good;
class Province;

// Included due to ids
#include "province.hpp"

// Defines a type of unit, it can be a tank, garrison, infantry, etc
// this is moddable via a lua script and new unit types can be added
class UnitType : public RefnameEntity<uint8_t> {
public:
    UnitType() {};
    ~UnitType() {};

    Eng3D::StringRef name;
    
    float supply_consumption;
    float speed;
    float max_health;
    float defense;
    float attack;
    float capacity; // Capacity of units that can be carried (transport units)
    float build_time; // Time needed to build
    bool is_ground; // Can go on ground?
    bool is_naval; // Can go on water?
    std::vector<std::pair<Good *, float>> req_goods; // Required goods
};

// A trait for an unit; given randomly per each recruited unit
class UnitTrait : public RefnameEntity<uint8_t> {
public:
    UnitTrait() {};
    ~UnitTrait() {};
    
    float supply_consumption_mod;
    float speed_mod;
    float max_health_mod;
    float defense_mod;
    float attack_mod;
    float morale_mod;
};

// Roughly a batallion, consisting of approximately 500 soldiers each
class Unit : public IdEntity<uint16_t> {
    Unit & operator=(const Unit&) = default;
    friend class Client;
    friend class UnitManager;
public:
    Unit() {};
    ~Unit() {};
    void attack(Unit& enemy);
    glm::vec2 get_pos() const;
    void set_target(const Province& province);
    float get_speed(const Province& province) const;
    float get_speed() const;

    /**
     * @brief Checks if the unit can move (if it can set_province)
     * 
     * @return true 
     * @return false 
     */
    inline bool can_move() const {
        // Unit must not be on a battle
        return !(this->on_battle);
    }

    UnitType* type; // Type of unit
    uint16_t owner_id; // Who owns this unit
    Province::Id target_province_id = Province::invalid();
    // Province::Id province_id = Province::invalid();
    Province::Id province_id() const;

    float size; // Size of the unit (soldiers in unit)
    // Base size of the unit (max size due to anti-attrition)
    float base;
    float move_progress;
    float morale;
    float experience; // For perspective, 0.5 is the normal unit (i.e a soldier POP)
    float supply; // Available supplies, 1.0 is all supplies fullfilled, lower than that and the unit starts shrinking
    float budget; // Money that the unit has
    bool on_battle = false;
};

class UnitManager {
private:
    UnitManager& operator=(const UnitManager&) = default;
public:
    UnitManager() {};
    // Fill in the relationship vectors for each nation
    void init(World& world);

    void add_unit(Unit unit, Province::Id unit_current_province);
    void remove_unit(Unit::Id unit);
    void move_unit(Unit::Id unit, Province::Id target_province);

    template<typename T>
    inline void for_each_unit(T const& lambda) {
        for(Unit::Id id = 0; id < units.size(); id++) {
            if (units[id].is_valid())
                lambda(units[id]);
        }
    }
    
    inline std::vector<Unit::Id> get_province_units(Province::Id province_id) const {
        return province_units[province_id];
    }

    inline Province::Id get_unit_current_province(Unit::Id unit_id) const {
        return unit_province[unit_id];
    }
    
    // The actual units
    std::vector<Unit> units;
    // The unit slots that are free to use
    std::vector<Unit::Id> free_unit_slots;
    // Vector for each unit
    std::vector<Province::Id> unit_province;
    // Vector for each province
    std::vector<std::vector<Unit::Id>> province_units;
};