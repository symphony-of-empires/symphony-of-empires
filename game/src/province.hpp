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

#include "unified_render/entity.hpp"
#include "unified_render/decimal.hpp"

class World;
class Nation;
class Industry;
class Product;
class TerrainType;
class Unit;
class Building;

#include "pop.hpp"
#include "building.hpp"

// A single province, which is used to simulate economy in a "bulk-tiles" way
// instead of doing economical operations on every single tile
class Province : public RefnameEntity<uint16_t> {
public:
    //Province();
    //~Province();
    UnifiedRender::Number total_pops(void) const;
    UnifiedRender::Decimal get_attractiveness(const Pop& pop) const;
    std::pair<UnifiedRender::Decimal, UnifiedRender::Decimal> get_pos(void) const;
    std::vector<Unit*> get_units(void) const;
    const std::vector<Building>& get_buildings(void) const;
    std::vector<Building>& get_buildings(void);
    bool is_neighbour(Province& province) const;

    // Color of the province, used for mapping the province's shape from the map_div.png file
    uint32_t color;

    bool is_coastal = false;

    // Budget of the province, money stored from taxes, to later be sent into
    // the nation's main wealth ark (or in case of multiple owners - a % of wealth)
    UnifiedRender::Decimal budget;

    // The (military) supply limit of the province, the max number of supplies there can be per tick
    UnifiedRender::Decimal supply_limit;

    // The number of remaining (military) supplies in the province
    UnifiedRender::Decimal supply_rem;

    // Attractiveness of province
    UnifiedRender::Decimal base_attractive = 0.f;

    // Number of tiles this province takes
    size_t n_tiles = 0;

    // Rectangle coordinates (x,y - x,y) for "area" scanning a province when needed
    // (for example, when changing owners)
    size_t min_x = 65532, min_y = 65532, max_x = 0, max_y = 0;

    // The owner of this province
    Nation* owner = nullptr;
    Nation* controller = nullptr;
    TerrainType* terrain_type;

    // List containing all nations who have a nucleus in this province
    std::set<Nation*> nuclei;

    // List of all neighbouring provinces (*should* be used for pathfinding)
    std::set<Province*> neighbours;

    // Products of this province (size == goods.size()!)
    std::vector<Product> products;

    // List of pops in this province
    std::vector<Pop> pops;
    std::vector<Building> buildings;
    std::vector<Unit*> units;
};