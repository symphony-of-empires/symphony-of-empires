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
#include <cassert>
#include <type_traits>
#include <glm/vec2.hpp>
#include "objects.hpp"
#include "eng3d/freelist.hpp"

class Nation;
struct Commodity;
class Province;
class World;

/// @brief Defines a type of unit, it can be a tank, garrison, infantry, etc
/// this is moddable via a lua script and new unit types can be added
struct UnitType : RefnameEntity<UnitTypeId> {
    Eng3D::StringRef name;
    float supply_consumption;
    float speed;
    float max_health;
    float defense;
    float attack;
    float capacity; // Capacity of units that can be carried (transport units)
    bool is_ground; // Can go on ground?
    bool is_naval; // Can go on water?
    std::vector<std::pair<CommodityId, float>> req_goods; // Required commodities
    std::string get_icon_path() const {
        return string_format("gfx/unittype/%s.png", ref_name.data());
    }
};
template<>
struct Eng3D::Deser::Serializer<UnitType> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, UnitType>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.supply_consumption);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.speed);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.max_health);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.defense);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.attack);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.is_ground);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.is_naval);
    }
};

class UnitManager;
/// @brief Roughly a batallion, consisting of approximately 500 soldiers each
class Unit : public Entity<UnitId> {
    Unit& operator=(const Unit&) = default;
    friend class Client;
    friend class UnitManager;
    friend struct Eng3D::Deser::Serializer<Unit>;
    std::vector<ProvinceId> path;
    float days_left_until_move = 0;
    bool has_target = false;
    ProvinceId target_province_id;
public:
    float attack(Unit& enemy);
    glm::vec2 get_pos() const;

    void set_target(const Province& province);

    void stop_movement() {
        this->has_target = false;
        this->days_left_until_move = 0;
    }

    float days_to_move_to(const Province& province) const;
    bool update_movement(UnitManager& unit_manager); // Returns true if unit moved
    void set_owner(const Nation& nation);

    /// @brief Checks if the unit can move (if it can set_province)
    /// @return true 
    /// @return false 
    bool can_move() const {
        return !(this->on_battle); // Unit must not be on a battle
    }

    const std::vector<ProvinceId> get_path() const {
        return path;
    }

    void set_path(const Province& target);

    bool has_target_province() const {
        return this->has_target;
    }

    ProvinceId get_target_province_id() const {
        return this->target_province_id;
    }

    float get_strength() const;

    UnitTypeId type_id;
    NationId owner_id;
    PopId pop_id;
    ProvinceId province_id() const;

    float size = 0.f;
    float base = 0.f;
    float experience = 1.f;
    bool on_battle = false;
};
template<>
struct Eng3D::Deser::Serializer<Unit> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Unit>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.type_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.size);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.base);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.experience);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.target_province_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.has_target);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.owner_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.days_left_until_move);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.path);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.on_battle);
    }
};

class UnitManager {
    UnitManager& operator=(const UnitManager&) = default;
public:
    void init(World& world);
    void add_unit(Unit unit, ProvinceId unit_current_province);
    void remove_unit(UnitId unit);
    void move_unit(UnitId unit, ProvinceId target_province);

    std::vector<UnitId> get_province_units(ProvinceId province_id) const {
        return province_units[province_id];
    }

    ProvinceId get_unit_current_province(UnitId unit_id) const {
        return unit_province[unit_id];
    }

    Eng3D::Freelist<Unit> units;

    std::vector<ProvinceId> unit_province;
    std::vector<std::vector<UnitId>> province_units;
};
template<>
struct Eng3D::Deser::Serializer<UnitManager> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, UnitManager>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.units);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.unit_province);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.province_units);
    }
};
