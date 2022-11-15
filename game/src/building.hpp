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

struct Technology;
class Unit;
struct UnitType;

/// @brief Type for military outposts
struct BuildingType : RefnameEntity<BuildingTypeId> {
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
struct Eng3D::Deser::Serializer<BuildingType> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, BuildingType>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.flags);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.input_ids);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.output_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_goods);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_technologies);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.num_req_workers);
    }
};

class Province;
/// @brief A military outpost, on land serves as a "spawn" place for units
/// When adjacent to a water tile this serves as a shipyard for spawning naval units
struct Building : Entity<BuildingId> {
    bool can_do_output(const Province& province, const std::vector<GoodId>& inputs) const;

    bool can_build_unit() const {
        for(const auto& [k, v] : req_goods_for_unit)
            if(v) return false;
        return this->level > 0.f;
    }

    float get_upgrade_cost() const {
        constexpr auto base_cost = 1000.f;
        return this->level * base_cost;
    }

    float get_profit() const {
        return this->revenue.get_total() - this->expenses.get_total();
    }

    float get_state_payment(float profit) const {
        return profit * this->state_ownership;
    }

    float get_private_payment(float profit) const {
        return profit * this->private_ownership;
    }

    float get_collective_payment(float profit) const {
        return profit * this->collective_ownership;
    }

    float get_operating_ratio() const {
        const auto total_revenue = this->revenue.get_total();
        if(total_revenue == 0.f) return 0.f;
        const auto total_expenses = this->expenses.get_total();
        if(total_expenses == 0.f) return 0.f;
        return total_revenue / total_expenses;
    }

    static constexpr auto factory_production_rate = 1.f;
    float get_output_amount() const {
        return this->production_scale * this->workers * factory_production_rate;
    }

    float get_max_output_amount(float max_workers) const {
        return this->level * max_workers * factory_production_rate;
    }

    float private_ownership = 0.f;
    float state_ownership = 1.f;
    float collective_ownership = 0.f;
    float individual_ownership = 0.f;
    float foreign_ownership = 0.f;
    NationId foreign_id; // Foreign investor

    float budget = 1000.f; // Total money that the factory has
    float level = 0.f; // Level/Capacity scale of the building
    float workers = 1.f; // Amount of workers
    float production_scale = 1.f; // How much of the factory is being used. From 0-1
    UnitTypeId working_unit_type_id; // Unit that is currently being built here (nullptr indicates no unit)
    // Required goods for building the working unit
    // change this to a struct instead of a pair for readablity
    std::vector<std::pair<GoodId, float>> req_goods_for_unit;
    // Required goods for construction or for repairs
    std::vector<std::pair<GoodId, float>> req_goods;

    // Bookkeeping
    struct {
        float outputs = 0.f;
        float get_total() const {
            return outputs;
        }
    } revenue;
    struct {
        float wages = 0.f;
        float inputs_cost = 0.f;
        float state_taxes = 0.f;
        float state_dividends = 0.f;
        float pop_dividends = 0.f;
        float private_dividends = 0.f;

        float get_dividends() const {
            return state_dividends + private_dividends + pop_dividends;
        }

        float get_total() const {
            return wages + inputs_cost + state_taxes + get_dividends();
        }
    } expenses;
};
template<>
struct Eng3D::Deser::Serializer<Building> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, Building>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.private_ownership);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.state_ownership);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.collective_ownership);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.individual_ownership);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.foreign_ownership);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.foreign_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.budget);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.level);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.production_scale);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.workers);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_goods);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_goods_for_unit);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.revenue.outputs);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.wages);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.inputs_cost);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.state_taxes);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.state_dividends);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.pop_dividends);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.private_dividends);
    }
};
