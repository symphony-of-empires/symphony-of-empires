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
#include <optional>
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
        return can_build_land_units() || can_build_air_units() || can_build_naval_units();
    }

    void can_plot_on_sea(bool b) { flags[0] = b; }
    void can_plot_on_land(bool b) { flags[1] = b; }
    void can_build_land_units(bool b) { flags[2] = b; }
    void can_build_naval_units(bool b) { flags[3] = b; }
    void can_build_air_units(bool b) { flags[4] = b; }

    Eng3D::StringRef name;
    std::bitset<4> flags;
    // We used to calculate these per each economical tick but now we can just store them
    // and multiply it by the level of the industry - this is the **minimum** amount of employed
    // people we should have at a time
    float num_req_workers = 0.f;
    std::optional<CommodityId> output_id; // Commodity that this building creates
    std::vector<CommodityId> input_ids; // Goods required to create output
    // Required commodities, first describes the id of the commodity and the second describes how many
    std::vector<std::pair<CommodityId, float>> req_goods;
    std::vector<TechnologyId> req_technologies; // Required technologies to build
};
template<>
struct Eng3D::Deser::Serializer<BuildingType> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, BuildingType>::type;

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
    bool can_do_output(const Province& province, const std::vector<CommodityId>& inputs) const;

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

    float get_operating_ratio() const {
        const auto total_revenue = this->revenue.get_total();
        const auto total_expenses = this->expenses.get_total();
        if(total_expenses == 0.f) return 0.f;
        assert(total_expenses >= 0.f && total_revenue >= 0.f);
        return total_revenue / total_expenses;
    }

    static constexpr auto industry_production_rate = 2.5f;
    float get_output_amount() const {
        return this->production_scale * this->workers * industry_production_rate;
    }

    float get_max_output_amount(float max_workers) const {
        return this->level * max_workers * industry_production_rate;
    }

    bool is_working_on_unit() const {
        return this->_is_wrking_on_unit;
    }

    void work_on_unit(const UnitType& unit_type);

    void stop_working_on_unit() {
        this->_is_wrking_on_unit = false;
    }

    struct Investment {
        float total = 0.f;
        float today_funds = 0.f;

        void invest(float amount) {
            total += amount;
            today_funds += amount;
        }

        float get_ownership(float total_investments) const {
            if(total == 0.f || total_investments == 0.f) return 0.f;
            return total_investments / total;
        }

        float get_dividends(float profit, float ownership) const {
            assert(ownership >= 0.f && ownership <= 1.f);
            return profit * ownership;
        }
    };
    Investment estate_private;
    Investment estate_state;
    Investment estate_collective;
    Investment estate_individual;
    std::vector<Investment> estate_foreign;
    float get_total_investment() const {
        auto sum = estate_private.total;
        sum += estate_state.total;
        sum += estate_collective.total;
        sum += estate_individual.total;
        for(const auto& foreign : estate_foreign)
            sum += foreign.total;
        return sum;
    }

    float budget = 1000.f; // Total money that the industry has
    float level = 0.f; // Level/Capacity scale of the building
    float workers = 1.f; // Amount of workers
    float production_scale = 1.f; // How much of the industry is being used. From 0-1
    bool _is_wrking_on_unit = false;
    UnitTypeId working_unit_type_id; // Unit that is currently being built here (nullptr indicates no unit)
    // Required commodities for building the working unit
    // change this to a struct instead of a pair for readablity
    std::vector<std::pair<CommodityId, float>> req_goods_for_unit;
    // Required commodities for construction or for repairs
    std::vector<std::pair<CommodityId, float>> req_goods;

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
struct Eng3D::Deser::Serializer<Building::Investment> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Building::Investment>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.total);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.today_funds);
    }
};

template<>
struct Eng3D::Deser::Serializer<Building> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Building>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_private);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_state);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_collective);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_individual);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_foreign);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.budget);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.level);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.production_scale);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.workers);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_goods);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_goods_for_unit);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj._is_wrking_on_unit);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.revenue.outputs);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.wages);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.inputs_cost);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.state_taxes);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.state_dividends);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.pop_dividends);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.private_dividends);
    }
};
