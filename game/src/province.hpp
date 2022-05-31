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
#include <set>
#include <string>
#include <memory>

#include "eng3d/entity.hpp"
#include "eng3d/decimal.hpp"
#include "eng3d/rectangle.hpp"

class World;
class Nation;
class Industry;
class Product;
class TerrainType;
class Unit;
class Building;

#include "pop.hpp"
#include "building.hpp"
#include "product.hpp"

// A single province, which is used to simulate economy in a "bulk-tiles" way
// instead of doing economical operations on every single tile
class Province : public RefnameEntity<uint16_t> {
public:
    Province() {};
    ~Province() {};
    Eng3D::Number total_pops() const;
    Eng3D::Decimal get_attractiveness(const Pop& pop) const;
    std::pair<Eng3D::Decimal, Eng3D::Decimal> get_pos() const;
    const std::vector<Unit*> get_units() const;
    const std::vector<Building>& get_buildings() const;
    std::vector<Building>& get_buildings();
    bool is_neighbour(Province& province) const;
    void add_building(const BuildingType& building_type);

    Eng3D::StringRef name;

    // Color of the province, used for mapping the province's shape from the map_div.png file
    std::uint32_t color = 0;

    bool is_coastal = false;

    // The (military) supply limit of the province, the max number of supplies there can be per tick
    Eng3D::Decimal supply_limit = 0.f;
    // The number of remaining (military) supplies in the province
    Eng3D::Decimal supply_rem = 0.f;
    // Attractiveness of province
    Eng3D::Decimal base_attractive = 0.f;
    
    // Rectangle coordinates (x,y - x,y) for "area" scanning a province when needed
    // (for example, when changing owners)
    Eng3D::Rect box_area;

    Nation* owner = nullptr; // The owner of this province
    Nation* controller = nullptr;
    TerrainType* terrain_type = nullptr;

    std::set<Nation*> nuclei; // Nations who have a nuclei in this province
    std::vector<uint32_t> rgo_size; // How much of each rgo that can be extracted
    std::set<Province*> neighbours; // Neighbouring provinces
    std::vector<Pop> pops; // List of pops in this province
    std::vector<Product> products;
    std::vector<Unit*> units;
    std::vector<Building> buildings;
};