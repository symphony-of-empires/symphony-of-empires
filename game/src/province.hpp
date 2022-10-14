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
//      province.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <vector>
#include <unordered_set>
#include <string>
#include <array>
#include <memory>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "eng3d/rectangle.hpp"

#include "objects.hpp"
#include "pop.hpp"
#include "product.hpp"
#include "building.hpp"
#include "diplomacy.hpp"

class World;
class Nation;
class TerrainType;

/// @brief A single province, which is used to simulate economy in a "bulk-tiles" way
/// instead of doing economical operations on every single tile
class Province : public RefnameEntity<ProvinceId> {
private:
    Province& operator=(const Province&) = default;
public:
    float total_pops() const {
        auto total = 0.f;
        for(const auto& pop : pops)
            total += pop.size;
        return total;
    }

    float get_attractiveness(const Pop& pop) const;
    void add_building(const BuildingType& building_type);
    void cancel_construction_project();
    bool is_neighbour(const Province& province) const;
    Pop& get_soldier_pop();
    
    inline glm::vec2 get_pos() const {
        return glm::vec2(box_area.left + ((box_area.right - box_area.left) / 2.f), box_area.top + ((box_area.bottom - box_area.top) / 2.f));
    }

    glm::vec3 get_sphere_coord(glm::vec2 world_size, float radius) const;

    float euclidean_distance(const Province& other_province, glm::vec2 world_size, float radius) const;

    inline const std::vector<Building>& get_buildings() const {
        return buildings;
    }

    inline std::vector<Building>& get_buildings() {
        return buildings;
    }

    bool is_populated() const {
        for(const auto& pop : pops)
            if(pop.size > 0.f)
                return true;
        return false;
    }

    void unpopulate() {
        for(auto& pop : pops)
            pop.size = 0.f;
    }

    Eng3D::StringRef name;
    std::uint32_t color = 0; // Color of the province, used for mapping the province's shape from the map_div.png file
    bool is_coastal = false;
    float base_attractive = 0.f; // Attractiveness of province
    // Rectangle coordinates (x,y - x,y) for "area" scanning a province when needed
    // (for example, when changing owners)
    Eng3D::Rect box_area = Eng3D::Rect(0.f, 0.f, 0.f, 0.f);
    NationId owner_id; // The owner of this province
    NationId controller_id;
    TerrainTypeId terrain_type_id;
    std::vector<uint32_t> rgo_size; // How much of each rgo that can be extracted
    std::array<Pop, 8> pops; // List of pops in this province
    std::vector<Product> products;
    std::vector<Building> buildings;
    struct Battle : public Entity<BattleId> {
        Battle() = default;
        Battle(const War& war)
            : war_id{ war.get_id() }
        {

        }
        
        WarId war_id;
        float attacker_casualties = 0.f;
        float defender_casualties = 0.f;
        std::vector<uint16_t> attackers_ids;
        std::vector<uint16_t> defenders_ids;
        bool active = false;
    } battle;
    std::vector<NationId> nuclei; // Nations who have a nuclei in this province
    std::vector<ProvinceId> neighbour_ids; // Neighbouring provinces
    /// @brief Percentage of each languages from 0 to 1
    std::vector<float> languages;
    /// @brief Percentage of each religion prescence on the pops, from 0 to 1
    std::vector<float> religions;
};
template<>
struct Serializer<Province*> : SerializerReference<World, Province> {};
template<>
struct Serializer<const Province*> : SerializerReference<World, const Province> {};
template<>
struct Serializer<Province> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Province& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.color);
        ::deser_dynamic<is_serialize>(ar, obj.box_area);
        ::deser_dynamic<is_serialize>(ar, obj.owner_id);
        ::deser_dynamic<is_serialize>(ar, obj.nuclei);
        ::deser_dynamic<is_serialize>(ar, obj.rgo_size);
        ::deser_dynamic<is_serialize>(ar, obj.neighbour_ids);
        ::deser_dynamic<is_serialize>(ar, obj.products);
        ::deser_dynamic<is_serialize>(ar, obj.pops);
        ::deser_dynamic<is_serialize>(ar, obj.buildings);
        ::deser_dynamic<is_serialize>(ar, obj.controller_id);
        ::deser_dynamic<is_serialize>(ar, obj.base_attractive);
        ::deser_dynamic<is_serialize>(ar, obj.languages);
        ::deser_dynamic<is_serialize>(ar, obj.religions);
        ::deser_dynamic<is_serialize>(ar, obj.terrain_type_id);
    }
};

class ProvinceManager {
    ProvinceManager& operator=(const ProvinceManager&) = default;
    std::vector<ProvinceId> recently_changed_owner;
    std::vector<ProvinceId> recently_changed_control;
    bool changed;
public:
    ProvinceManager() = default;

    inline void mark_province_owner_changed(ProvinceId province_id) {
        recently_changed_owner.push_back(province_id);
    }

    inline void mark_province_control_changed(ProvinceId province_id) {
        recently_changed_control.push_back(province_id);
    }

    inline void clear() {
        recently_changed_owner.clear();
        recently_changed_control.clear();
    }

    inline const std::vector<ProvinceId>& get_changed_owner_provinces() const {
        return recently_changed_owner;
    }

    inline const std::vector<ProvinceId>& get_changed_control_provinces() const {
        return recently_changed_control;
    }

    inline bool is_provinces_changed() const {
        return !recently_changed_owner.empty() || !recently_changed_control.empty();
    }
};
