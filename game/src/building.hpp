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
#include "objects.hpp"
#include "product.hpp"

class Technology;
class Unit;
class UnitType;

/// @brief Type for military outposts
struct BuildingType : public RefnameEntity<BuildingTypeId> {
    bool can_plot_on_sea() const { return flags[0]; }
    bool can_plot_on_land() const { return flags[1]; }
    bool can_build_land_units() const { return flags[2]; }
    bool can_build_naval_units() const { return flags[3]; }
    bool can_build_air_units() const { return flags[4]; }

    /// @brief Can this building type build a military unit
    bool can_build_military() const {
        return can_build_land_units() | can_build_air_units() | can_build_naval_units();
    }

    void can_plot_on_sea(bool b) { flags[0] = b; }
    void can_plot_on_land(bool b) { flags[1] = b; }
    void can_build_land_units(bool b) { flags[2] = b; }
    void can_build_naval_units(bool b) { flags[3] = b; }
    void can_build_air_units(bool b) { flags[4] = b; }

    Eng3D::StringRef name;
    std::bitset<4> flags;
    // We used to calculate these per each economical tick but now we can just store them
    // and multiply it by the level of the factory - this is the **minimum** amount of employed
    // people we should have at a time
    float num_req_workers = 0.f;
    GoodId output_id; // Good that this building creates
    std::vector<GoodId> input_ids; // Goods required to create output
    // Required goods, first describes the id of the good and the second describes how many
    std::vector<std::pair<GoodId, float>> req_goods;
    std::vector<TechnologyId> req_technologies; // Required technologies to build
};
template<>
struct Serializer<BuildingType*>: public SerializerReferenceLocal<World, BuildingType> {};
template<>
struct Serializer<const BuildingType*>: public SerializerReferenceLocal<World, const BuildingType> {};
template<>
struct Serializer<BuildingType> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, BuildingType& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.flags);
        ::deser_dynamic<is_serialize>(ar, obj.input_ids);
        ::deser_dynamic<is_serialize>(ar, obj.output_id);
        ::deser_dynamic<is_serialize>(ar, obj.req_goods);
        ::deser_dynamic<is_serialize>(ar, obj.req_technologies);
        ::deser_dynamic<is_serialize>(ar, obj.num_req_workers);
    }
};

/// @brief A military outpost, on land serves as a "spawn" place for units
/// When adjacent to a water tile this serves as a shipyard for spawning naval units
struct Building : public Entity<BuildingId> {
    /// @brief Adds a good by id to a building stockpile
    void add_to_stock(const Good& good, const size_t add) {
        stockpile[good] += add;
    }

    /// @brief Checks if the building can produce output (if it has enough input)
    inline bool can_do_output() const {
        // Check that we have enough stockpile
        for(const auto& stock : this->stockpile)
            if(!stock) return false;
        return this->level > 0;
    }

    inline bool can_build_unit() const {
        for(const auto& [k, v] : req_goods_for_unit)
            if(v) return false;
        return this->level > 0;
    }
    
    float budget = 0.f; // Total money that the factory has
    float level = 0.f; // Level/Capacity scale of the building
    float workers = 1.f; // Amount of workers
    float production_scale = 1.f; // How much of the factory is being used. From 0-1
    UnitTypeId working_unit_type_id; // Unit that is currently being built here (nullptr indicates no unit)
    // Required goods for building the working unit
    // change this to a struct instead of a pair for readablity
    std::vector<std::pair<GoodId, float>> req_goods_for_unit;
    // Required goods for construction or for repairs
    std::vector<std::pair<GoodId, float>> req_goods;
    // Stockpile of inputs in the factory
    std::vector<float> stockpile;
};
template<>
struct Serializer<Building> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Building& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.working_unit_type_id);
        ::deser_dynamic<is_serialize>(ar, obj.budget);
        ::deser_dynamic<is_serialize>(ar, obj.level);
        ::deser_dynamic<is_serialize>(ar, obj.production_scale);
        ::deser_dynamic<is_serialize>(ar, obj.workers);
        ::deser_dynamic<is_serialize>(ar, obj.stockpile);
        ::deser_dynamic<is_serialize>(ar, obj.req_goods);
        ::deser_dynamic<is_serialize>(ar, obj.req_goods_for_unit);
    }
};
