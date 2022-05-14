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
//      building.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <unordered_set>
#include "eng3d/entity.hpp"
#include "eng3d/decimal.hpp"

#include "unit.hpp"

class Technology;

// Type for military outposts
class BuildingType: public RefnameEntity<uint8_t> {
public:
    //BuildingType();
    //~BuildingType();

    static constexpr int PLOT_ON_SEA = 0x01;
    static constexpr int PLOT_ON_LAND = 0x02;
    static constexpr int BUILD_LAND_UNITS = 0x04;
    static constexpr int BUILD_NAVAL_UNITS = 0x08;
    static constexpr int BUILD_AIR_UNITS = 0x10;
    // Build any type of military
    static constexpr int BUILD_MILITARY = BUILD_LAND_UNITS | BUILD_NAVAL_UNITS | BUILD_AIR_UNITS;
    std::uint8_t flags;

    // We used to calculate these per each economical tick but now we can just store them
    // and multiply it by the level of the factory - this is the **minimum** amount of employed
    // people we should have at a time
    Eng3D::Number num_req_workers = 0;

    // List of goods that this factory type creates
    Good* output = nullptr;

    // List of goods required to create output
    std::vector<Good*> inputs;
    // Required goods, first describes the id of the good and the second describes how many
    std::vector<std::pair<Good*, Eng3D::Number>> req_goods;
    // Required technologies
    std::vector<Technology*> req_technologies;
};

// A military outpost, on land serves as a "spawn" place for units
// When adjacent to a water tile this serves as a shipyard for spawning naval units
class Building {
public:
    Building();
    ~Building();
    void add_to_stock(const Good& good, size_t add);
    bool can_do_output(void) const;
    bool can_build_unit(void) const;

    // Remaining ticks until the unit is built
    Eng3D::Number build_time;
    // Total money that the factory has
    Eng3D::Decimal budget = 0.f;
    // Days that the factory has not been operational
    Eng3D::Number days_unoperational = 0;
    // Money needed to produce - helps determine the price of the output products
    Eng3D::Decimal production_cost = 0.f;
    // Level of building (all buildings start at 0)
    Eng3D::Number level = 0;
    // Amount of currently working pops
    Eng3D::Number workers = 0;
    // How much of the factory is being used. From 0-1
    Eng3D::Decimal production_scale = 1.f;

    // Unit that is currently being built here (nullptr indicates no unit)
    UnitType* working_unit_type = nullptr;

    // Required goods for building the working unit
    // TODO: change this to a struct instead of a pair for readablity
    std::vector<std::pair<Good*, Eng3D::Number>> req_goods_for_unit;
    // Required goods for building this, or repairing this after a military attack
    std::vector<std::pair<Good*, Eng3D::Number>> req_goods;
    // Stockpile of inputs in the factory
    std::vector<Eng3D::Number> stockpile;
    // The employees needed per output
    std::vector<Eng3D::Number> employees_needed_per_output;
};