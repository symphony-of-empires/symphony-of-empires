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
//      building.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <bitset>
#include <unordered_set>
#include "eng3d/entity.hpp"

class Technology;
class Unit;
class UnitType;
class Good;

// Type for military outposts
class BuildingType : public RefnameEntity<uint8_t> {
public:
    BuildingType() = default;
    ~BuildingType() = default;

    inline bool can_plot_on_sea() const { return flags[0]; }
    inline bool can_plot_on_land() const { return flags[1]; }
    inline bool can_build_land_units() const { return flags[2]; }
    inline bool can_build_naval_units() const { return flags[3]; }
    inline bool can_build_air_units() const { return flags[4]; }

    /// @brief Can this building type build a military unit
    inline bool can_build_military() const {
        return can_build_land_units() | can_build_air_units() | can_build_naval_units();
    }

    inline void can_plot_on_sea(bool b) { flags[0] = b; }
    inline void can_plot_on_land(bool b) { flags[1] = b; }
    inline void can_build_land_units(bool b) { flags[2] = b; }
    inline void can_build_naval_units(bool b) { flags[3] = b; }
    inline void can_build_air_units(bool b) { flags[4] = b; }

    Eng3D::StringRef name;
    std::bitset<4> flags;
    // We used to calculate these per each economical tick but now we can just store them
    // and multiply it by the level of the factory - this is the **minimum** amount of employed
    // people we should have at a time
    float num_req_workers = 0;
    Good* output = nullptr; // Good that this building creates
    std::vector<Good*> inputs; // Goods required to create output
    // Required goods, first describes the id of the good and the second describes how many
    std::vector<std::pair<Good*, float>> req_goods;
    std::vector<Technology*> req_technologies; // Required technologies to build
};

// A military outpost, on land serves as a "spawn" place for units
// When adjacent to a water tile this serves as a shipyard for spawning naval units
class Building {
public:
    using Id = BuildingType::Id;
    Building() = default;
    ~Building() = default;
    void add_to_stock(const Good& good, size_t add);

    /// @brief Checks if the building can produce output (if it has enough input)
    inline bool can_do_output() const {
        // Check that we have enough stockpile
        for(const auto& stock : this->stockpile)
            if(!stock) return false;
        return true;
    }

    inline bool can_build_unit() const {
        for(const auto& req : req_goods_for_unit)
            if(req.second) return false;
        return true;
    }

    float build_time; // Remaining ticks until the unit is built
    float budget = 0; // Total money that the factory has
    float days_unoperational = 0; // Days that the factory has not been operational
    float level = 0; // Level/Capacity scale of the building
    float workers = 0; // Amount of workers
    float production_scale = 1.f; // How much of the factory is being used. From 0-1
    UnitType* working_unit_type = nullptr; // Unit that is currently being built here (nullptr indicates no unit)
    /// @brief Required goods for building the working unit
    /// @todo change this to a struct instead of a pair for readablity
    std::vector<std::pair<Good*, float>> req_goods_for_unit;
    /// @brief Required goods for construction or for repairs
    std::vector<std::pair<Good*, float>> req_goods;
    /// @brief Stockpile of inputs in the factory
    std::vector<float> stockpile;
};
