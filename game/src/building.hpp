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
#include "unified_render/entity.hpp"
#include "unified_render/decimal.hpp"

#include "unit.hpp"

class Technology;

// Type for military outposts
class BuildingType : public RefnameEntity<uint8_t> {
public:
    //BuildingType();
    //~BuildingType();
    
    bool is_plot_on_sea;
    bool is_plot_on_land;
    bool is_build_land_units;
    bool is_build_naval_units;

    // Defensive bonus given to units on the outpost
    UnifiedRender::Number defense_bonus;

    // Is this a factory?
    bool is_factory;

    // List of goods required to create output
    std::vector<Good*> inputs;
    // List of goods that this factory type creates
    std::vector<Good*> outputs;

    // Required goods, first describes the id of the good and the second describes how many
    std::vector<std::pair<Good*, UnifiedRender::Number>> req_goods;

    // Required technologies
    std::vector<Technology*> req_technologies;
};

// A military outpost, on land serves as a "spawn" place for units
// When adjacent to a water tile this serves as a shipyard for spawning naval units
class Building : public IdEntity<uint16_t> {
public:
    Building();
    ~Building();
    void add_to_stock(const Good& good, size_t add);
    bool can_do_output(void) const;
    bool can_build_unit(void) const;

    // Unit that is currently being built here (nullptr indicates no unit)
    UnitType* working_unit_type = nullptr;

    // Remaining ticks until the unit is built
    UnifiedRender::Number build_time;

    // Required goods for building the working unit
    // TODO: change this to a struct instead of a pair for readablity
    std::vector<std::pair<Good*, UnifiedRender::Number>> req_goods_for_unit;
    // Required goods for building this, or repairing this after a military attack
    std::vector<std::pair<Good*, UnifiedRender::Number>> req_goods;

    // Total money that the factory has
    UnifiedRender::Decimal budget = 0.f;
    
    // Days that the factory has not been operational
    UnifiedRender::Number days_unoperational = 0;
    
    // Money needed to produce - helps determine the price of the output products
    UnifiedRender::Decimal production_cost = 0.f;
    
    // Stockpile of inputs in the factory
    std::vector<UnifiedRender::Number> stockpile;
    
    // The employees needed per output
    std::vector<UnifiedRender::Number> employees_needed_per_output;

    // Level of building (all starts at 0)
    UnifiedRender::Number level = 0;
};