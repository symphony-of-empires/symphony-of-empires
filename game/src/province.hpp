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
#include <memory>
#include "eng3d/entity.hpp"
#include "eng3d/decimal.hpp"
#include "eng3d/rectangle.hpp"

#include "pop.hpp"
#include "product.hpp"
#include "building.hpp"
#include "diplomacy.hpp"

class World;
class Nation;
class TerrainType;
class War;

class Battle : public IdEntity<uint16_t> {
public:
    Battle(War& war)
        : war{ &war }
    {

    }
    ~Battle() {};

    Eng3D::StringRef name;
    War* war = nullptr;
    float attacker_casualties = 0;
    float defender_casualties = 0;
    std::vector<uint16_t> attackers_ids;
    std::vector<uint16_t> defenders_ids;
};

// A single province, which is used to simulate economy in a "bulk-tiles" way
// instead of doing economical operations on every single tile
class Province : public RefnameEntity<uint16_t> {
private:
    Province & operator=(const Province&) = default;
public:
    Province() {};
    ~Province() {};
    float total_pops() const;
    float get_attractiveness(const Pop& pop) const;
    void add_building(const BuildingType& building_type);
    bool is_neighbour(Province& province) const;

    inline std::pair<float, float> get_pos() const {
        return std::make_pair(box_area.left + ((box_area.right - box_area.left) / 2.f), box_area.top + ((box_area.bottom - box_area.top) / 2.f));
    }

    inline const std::vector<uint16_t>& get_units() const {
        return units_ids;
    }

    inline const std::vector<Building>& get_buildings() const {
        return buildings;
    }

    inline std::vector<Building>& get_buildings() {
        return buildings;
    }

    Eng3D::StringRef name;
    std::uint32_t color = 0; // Color of the province, used for mapping the province's shape from the map_div.png file
    bool is_coastal = false;
    float supply_limit = 0.f; // The (military) supply limit of the province, the max number of supplies there can be per tick
    float supply_rem = 0.f; // The number of remaining (military) supplies in the province
    float base_attractive = 0.f; // Attractiveness of province
    // Rectangle coordinates (x,y - x,y) for "area" scanning a province when needed
    // (for example, when changing owners)
    Eng3D::Rect box_area;
    uint16_t owner_id = (uint16_t)-1; // The owner of this province
    Nation* controller = nullptr;
    TerrainType* terrain_type = nullptr;
    std::vector<uint32_t> rgo_size; // How much of each rgo that can be extracted
    std::vector<Pop> pops; // List of pops in this province
    std::vector<Product> products;
    std::vector<Building> buildings;
    std::vector<Battle> battles;
    std::vector<uint16_t> units_ids;
    std::unordered_set<uint16_t> nuclei; // Nations who have a nuclei in this province
    std::unordered_set<Province::Id> neighbours; // Neighbouring provinces
};