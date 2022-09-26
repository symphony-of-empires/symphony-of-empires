// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
#include <glm/vec2.hpp>
#include "objects.hpp"

class Nation;
class Good;
class Province;
class World;

/// @brief Defines a type of unit, it can be a tank, garrison, infantry, etc
/// this is moddable via a lua script and new unit types can be added
class UnitType : public RefnameEntity<UnitTypeId> {
public:
    Eng3D::StringRef name;

    float supply_consumption;
    float speed;
    float max_health;
    float defense;
    float attack;
    float capacity; // Capacity of units that can be carried (transport units)
    bool is_ground; // Can go on ground?
    bool is_naval; // Can go on water?
    std::vector<std::pair<GoodId, float>> req_goods; // Required goods
};
template<>
class Serializer<UnitType*>: public SerializerReferenceLocal<World, UnitType> {};
template<>
class Serializer<const UnitType*>: public SerializerReferenceLocal<World, const UnitType> {};
template<>
class Serializer<UnitType> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, UnitType& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.supply_consumption);
        ::deser_dynamic<is_serialize>(ar, obj.speed);
        ::deser_dynamic<is_serialize>(ar, obj.max_health);
        ::deser_dynamic<is_serialize>(ar, obj.defense);
        ::deser_dynamic<is_serialize>(ar, obj.attack);
        ::deser_dynamic<is_serialize>(ar, obj.is_ground);
        ::deser_dynamic<is_serialize>(ar, obj.is_naval);
    }
};

class UnitManager;
template <class T>
class Serializer;
/// @brief Roughly a batallion, consisting of approximately 500 soldiers each
class Unit : public Entity<UnitId> {
    Unit& operator=(const Unit&) = default;
    friend class Client;
    friend class UnitManager;
    friend class Serializer<Unit>;

    std::vector<ProvinceId> path;
    float days_left_until_move = 0;
    ProvinceId target_province_id;
public:
    void attack(Unit& enemy);
    glm::vec2 get_pos() const;
    void set_target(const Province& province);
    void stop_movement();
    float days_to_move_to(const Province& province) const;
    // Returns true if unit moved
    bool update_movement(UnitManager& unit_manager);
    float get_speed() const;
    void set_owner(const Nation& nation);
    bool can_move() const;
    const std::vector<ProvinceId> get_path() const;
    void set_path(const Province& target);
    ProvinceId get_target_province_id() const;

    UnitTypeId type_id; // Type of unit
    NationId owner_id; // Who owns this unit
    ProvinceId province_id() const;

    float size; // Size of the unit (soldiers in unit)
    float base; // Base size of the unit (max size due to anti-attrition)
    float morale = 1.f;
    float experience = 1.f; // For perspective, 0.5 is the normal unit (i.e a soldier POP)
    float supply = 1.f; // Available supplies, 1.0 is all supplies fullfilled, lower than that and the unit starts shrinking
    float budget; // Money that the unit has
    bool on_battle = false;
};
template<>
class Serializer<Unit> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Unit& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.type_id);
        ::deser_dynamic<is_serialize>(ar, obj.size);
        ::deser_dynamic<is_serialize>(ar, obj.target_province_id);
        ::deser_dynamic<is_serialize>(ar, obj.owner_id);
        ::deser_dynamic<is_serialize>(ar, obj.days_left_until_move);
        ::deser_dynamic<is_serialize>(ar, obj.path);
        ::deser_dynamic<is_serialize>(ar, obj.on_battle);
    }
};

class UnitManager {
private:
    UnitManager& operator=(const UnitManager&) = default;
public:
    void init(World& world);
    void add_unit(Unit unit, ProvinceId unit_current_province);
    void remove_unit(UnitId unit);
    void move_unit(UnitId unit, ProvinceId target_province);

    template<typename T>
    inline void for_each_unit(const T& lambda) {
        for(size_t i = 0; i < units.size(); i++)
            if(units[i].is_valid())
                lambda(units[i]);
    }

    inline std::vector<UnitId> get_province_units(ProvinceId province_id) const {
        return province_units[province_id];
    }

    inline ProvinceId get_unit_current_province(UnitId unit_id) const {
        return unit_province[unit_id];
    }

    std::vector<Unit> units;
    std::vector<UnitId> free_unit_slots;
    std::vector<ProvinceId> unit_province;
    std::vector<std::vector<UnitId>> province_units;
};
template<>
class Serializer<UnitManager> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, UnitManager& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.units);
        ::deser_dynamic<is_serialize>(ar, obj.free_unit_slots);
        ::deser_dynamic<is_serialize>(ar, obj.unit_province);
        ::deser_dynamic<is_serialize>(ar, obj.province_units);
    }
};
