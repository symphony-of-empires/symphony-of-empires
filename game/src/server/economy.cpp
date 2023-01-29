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
//      server/economy.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <cstdio>
#include <map>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/combinable.h>
#include <glm/gtx/compatibility.hpp>

#include "eng3d/log.hpp"
#include "eng3d/serializer.hpp"
#include "eng3d/rand.hpp"

#include "action.hpp"
#include "server/economy.hpp"
#include "world.hpp"
#include "server/server_network.hpp"
#include "product.hpp"
#include "emigration.hpp"

#undef min
#undef max

// Visual Studio does not define ssize_t because it's a POSIX-only type
#ifdef _MSC_VER
typedef signed int ssize_t;
#endif

struct PopNeed {
    float life_needs_met = 0.f;
    float budget = 0.f;
    float debt = 0.f;
};

struct NewUnit {
    UnitTypeId type_id;
    float size;
    ProvinceId province_id;
    PopTypeId pop_id;
    NewUnit(UnitTypeId _type_id, float _size, ProvinceId _province_id, PopTypeId _pop_id)
        : type_id{ _type_id },
        size{ _size },
        province_id{ _province_id },
        pop_id{ _pop_id }
    {

    }
};

constexpr auto scale_speed(auto c, auto target) {
    constexpr auto friction = 0.1f;
    return (c * (1.f - friction)) + friction * target;
}

struct ProvinceEconomyInfo {
    // Incomes
    float laborers_payment = 0.f;
    float artisans_payment = 0.f;
    float state_payment = 0.f;
    float private_payment = 0.f;
    float bureaucrats_payment = 0.f;
    float pops_payment = 0.f;
    float bureaucracy_eff = 0.f;

    // Costs
    float admin_funds = 0.f;
    float military_funds = 0.f;

    // Contributions
    /// @brief Produced amount by artisans and factories to calculate final payment to both
    std::vector<std::pair<float, float>> produced;
};

// Updates supply, demand, and set wages for workers
static void update_industry_production(World& world, Building& building, const BuildingType& building_type, Province& province, ProvinceEconomyInfo& info)
{
    if(!building_type.output_id.has_value())
        return;

    constexpr auto artisan_production_rate = 2.3f;
    auto& output = world.commodities[building_type.output_id.value()];
    auto& output_product = province.products[output];

    // Artisans take place of industry or co-exist with it
    if(output_product.demand > 0.f) { // Artisans only produce iff suitable so
        /// @todo Artisans will produce FOR EACH industry type, so if there is
        /// two wheat farm types there will be double the wheat produced??
        const auto artisans_amount = province.pops[(int)PopGroup::ARTISAN].size;
        const auto output_amount = artisans_amount * artisan_production_rate;
        info.produced[output].first += output_amount;
    }
    
    if(building.level != 0.f) {
        /// @todo add input modifier
        building.expenses.inputs_cost = 0.f; // Buy the inputs for the industry
        for(const auto& [product_id, required_amount] : building_type.req_goods) {
            assert(required_amount >= 0.f && building.production_scale >= 0.f);
            auto& product = province.products[product_id];
            const auto wanted_amount = required_amount * building.production_scale;
            auto amount = 0.f;
            building.expenses.inputs_cost += product.buy(wanted_amount, amount);
        }
        if(building.can_do_output(province, building_type.input_ids))
            info.produced[output].second += building.get_output_amount();
    }

    // Produce the product itself by the combined efforts of the artisans and the industries
    const auto [artisan_production, industry_production] = info.produced[output];
    output_product.produce(artisan_production + industry_production);
}

static void update_industry_accounting(World& world, Building& building, const BuildingType& building_type, Province& province, ProvinceEconomyInfo& info)
{
    const auto& nation = world.nations[province.controller_id];

    // TODO: Make building inoperate for the legnth of the upgrade (need to acquire materials)
    const auto upgrade_cost = building.get_upgrade_cost();
    if(building.budget >= upgrade_cost) {
        building.budget -= upgrade_cost;
        building.level += 1.f;
    } else if(building.budget <= -(upgrade_cost * 0.5f)) {
        building.budget += upgrade_cost;
        building.level -= 1.f;
        if(building.level <= 0.f)
            building.level = 0.f;
    }

    // TODO: Make it so burgeoise aren't duplicating money out of thin air
    const auto private_investment = info.private_payment * 0.8f * nation.current_policy.private_ownership;
    building.estate_private.invest(private_investment);

    // Pay the new funds to the building as a form of the new investment
    building.budget += building.estate_collective.today_funds;
    for(const auto& estate_foreign : building.estate_foreign)
        building.budget += estate_foreign.today_funds;
    building.budget += building.estate_individual.today_funds;
    building.budget += building.estate_private.today_funds;
    building.budget += building.estate_state.today_funds;
    // Reset today new funds given to the building
    building.estate_collective.today_funds = 0.f;
    for(auto& estate_foreign : building.estate_foreign)
        estate_foreign.today_funds = 0.f;
    building.estate_individual.today_funds = 0.f;
    building.estate_private.today_funds = 0.f;
    building.estate_state.today_funds = 0.f;

    // Pay the combined value of production
    building.revenue.outputs = 0.f;
    if(building_type.output_id.has_value()) {
        const auto& output = world.commodities[building_type.output_id.value()];
        auto& output_product = province.products[output];

        // Obtain production ratios to divide payments for the amount of goods produced either by artisans or
        // by industries which produce a lot of stuff
        const auto [artisan_production, industry_production] = info.produced[output];
        assert(artisan_production >= 0.f && industry_production >= 0.f);
        const auto artisan_production_ratio = artisan_production / glm::max(artisan_production + industry_production, 1.f);
        assert(artisan_production_ratio >= 0.f && artisan_production_ratio <= 1.f);
        const auto industry_production_ratio = industry_production / glm::max(artisan_production + industry_production, 1.f);
        assert(industry_production_ratio >= 0.f && industry_production_ratio <= 1.f);

        // Obtain revenue from the products on this province & from how many were bought
        info.artisans_payment += output_product.bought * output_product.price * artisan_production_ratio;
        building.revenue.outputs += output_product.bought * output_product.price * industry_production_ratio;
    }

    // Below code can only be relevant if the building exists in the first place
    if(building.level == 0.f)
        return;

    // TODO add output modifier
    // Calculate outputs

    // TODO set min wages
    float min_wage = glm::max(nation.current_policy.min_wage, glm::epsilon<float>());

    building.expenses.wages = min_wage * building.workers;
    info.laborers_payment += building.expenses.wages;
    auto profit = building.get_profit();

    // Taxation occurs even without a surplus
    building.expenses.state_taxes = 0.f;
    if(profit > 0.f) {
        building.expenses.state_taxes = profit * nation.current_policy.industry_profit_tax;
        info.state_payment += building.expenses.state_taxes;
        profit -= building.expenses.state_taxes;
    }

    // Dividends that will be paid to the shareholders
    building.expenses.state_dividends = building.expenses.pop_dividends = building.expenses.private_dividends = 0.f; // Dividends are paid after calculating the surplus
    auto surplus = profit - building.expenses.get_total();
    if(surplus > 0.f) {
        // Disperse profits to holders
        // TODO: Should surplus be decremented between each payout?
        const auto state_dividends = building.estate_state.get_dividends(surplus,
            building.estate_state.get_ownership(building.get_total_investment()));
        info.state_payment += state_dividends;
        building.expenses.state_dividends += state_dividends;
        surplus -= state_dividends;

        const auto private_dividends = building.estate_private.get_dividends(surplus,
            building.estate_private.get_ownership(building.get_total_investment()));
        info.private_payment += private_dividends;
        building.expenses.private_dividends += private_dividends;
        surplus -= private_dividends;

        const auto collective_dividends = building.estate_collective.get_dividends(surplus,
            building.estate_collective.get_ownership(building.get_total_investment()));
        info.pops_payment += collective_dividends;
        building.expenses.pop_dividends += collective_dividends;
        surplus -= collective_dividends;

        profit -= building.expenses.get_dividends();
    }
    building.budget += profit; // Pay the remainder profit to the building

    //! Based production not used!
    // auto base_production = 1.f;
    // // Capitalist nations require having a stake first; whereas non-capitalist nations do not
    // // require a pevious stake to exist at all to enforce their production scales
    // if(!nation.can_directly_control_factories()) {
    //     base_production = building.state_ownership * nation.commodity_production[output] * building.level;
    // } else {
    //     base_production = nation.commodity_production[output] * building.level;
    // }

    if(building_type.output_id.has_value()) {
        const auto& output = world.commodities[building_type.output_id.value()];
        auto& output_product = province.products[output];
        // Rescale production
        // This is used to set how much the of the maximum capacity the industry produce
        building.production_scale = glm::clamp(building.production_scale * glm::clamp(0.9f * building.get_operating_ratio(), 0.9f, 1.05f) * output_product.ds_ratio(), 0.05f, building.level);
    }
}

// Update the industry employment
static void update_factories_employment(const World& world, Province& province, std::vector<float>& new_workers) {
    auto unallocated_workers = province.pops[(int)PopGroup::LABORER].size;
    // Sort factories by their operating ratio, or profitability in regards to their expenses
    // eg: revenue / expenses = proftability ratio
    std::vector<std::pair<size_t, float>> factories_by_profitability;
    for(const auto& building_type : world.building_types)
        factories_by_profitability.emplace_back(
            building_type.get_id(),
            province.buildings[building_type].get_operating_ratio()
        );
    std::sort(factories_by_profitability.begin(), factories_by_profitability.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    float is_operating = province.controller_id == province.owner_id ? 1.f : 0.f;
    for(const auto& [industry_index, _] : factories_by_profitability) {
        const auto& building = province.buildings[industry_index];
        const auto& type = world.building_types[industry_index];
        if(building.level == 0.f) continue;
        const auto industry_workers = building.level * glm::max(1.f, building.production_scale) * type.num_req_workers;
        const auto allocated_workers = glm::max(glm::min(industry_workers, unallocated_workers), 0.f);
        // Average with how much the industry had before
        // Makes is more stable so everyone don't change workplace immediately
        constexpr auto hiring_delta_rate = 0.95f; // Change rate for hirings/firings
        new_workers[industry_index] = glm::clamp(hiring_delta_rate * building.workers + (1.f - hiring_delta_rate) * building.level * building.production_scale * type.num_req_workers, 0.f, unallocated_workers) * is_operating;
        unallocated_workers -= new_workers[industry_index];
    }
    assert(unallocated_workers >= 0.f);
}

/// @brief Calculate the budget that we spend on each needs
void update_pop_needs(World& world, Province& province, std::vector<PopNeed>& pop_needs, ProvinceEconomyInfo& info) {
    auto& nation = world.nations[province.controller_id];
    for(size_t i = 0; i < province.pops.size(); i++) {
        auto& pop_need = pop_needs[i];
        auto& pop = province.pops[i];
        const auto& needs_amounts = world.pop_types[pop.type_id].basic_needs_amount;
        if(pop.size < 1.f) return;
        
        if(pop_need.budget > 0.f) {
            const auto percentage_to_spend = 0.8f;
            const auto budget_alloc = pop_need.budget * percentage_to_spend;
            pop_need.budget -= budget_alloc;

            // If we are going to have value added taxes we should separate them from income taxes
            info.state_payment += budget_alloc * nation.current_policy.pop_tax;
            const auto budget_after_VAT = budget_alloc * (1.f - nation.current_policy.pop_tax);
            const auto budget_per_pop = budget_after_VAT / pop.size;

            auto total_factor = std::reduce(needs_amounts.begin(), needs_amounts.end());
            for(const auto& commodity : world.commodities) {
                if(needs_amounts[commodity] <= 0.f) continue;
                auto& product = province.products[commodity];
                const auto need_factor = needs_amounts[commodity] / total_factor;
                const auto maximum_demand = pop.size * need_factor;
                
                auto amount = 0.f;
                const auto payment = product.buy(maximum_demand, amount);
                pop.budget -= payment;
                pop_need.life_needs_met += (amount / pop.size) * need_factor;
            }
        }
        // Should be between -1 and 1
        pop_need.life_needs_met = glm::clamp(pop_need.life_needs_met, -1.f, 1.f);

        // Take a loan if this buying spree didn't satisfy us
        if(pop_need.life_needs_met < 0.f) {
            // Obtain total amount to borrow
            auto total_to_borrow = 0.f;
            for(const auto& commodity : world.commodities) {
                const auto& product = province.products[commodity];
                const auto need_factor = needs_amounts[commodity];
                total_to_borrow += pop.size * need_factor * product.price;
            }

            auto borrowed = 0.f;
            auto [public_debt, private_debt] = province.borrow_loan(total_to_borrow, borrowed);
            pop.public_debt += public_debt;
            pop.private_debt += private_debt;
            pop.budget += borrowed;
        } else {
            // Repay debts (public repay is prioritized)
            if(pop.public_debt > 0.f) {
                const auto repay_amount = glm::min(pop.public_debt, pop.budget);
                pop.public_debt -= repay_amount;
                info.state_payment += repay_amount;
            }
            if(pop.private_debt > 0.f) {
                const auto repay_amount = glm::min(pop.private_debt, pop.budget);
                pop.private_debt -= repay_amount;
                info.private_payment += repay_amount;
            }
        }
    }
}

std::vector<Economy::Market> init_markets(const World& world) {
    std::vector<Economy::Market> markets(world.commodities.size());
    for(const auto& commodity : world.commodities)
        markets[commodity].commodity = commodity.get_id();
    tbb::parallel_for(tbb::blocked_range(markets.begin(), markets.end()), [&world](const auto& markets_range) {
        for(auto& market : markets_range) {
            market.price.resize(world.provinces.size());
            market.supply.resize(world.provinces.size());
            market.demand.resize(world.provinces.size());
            market.global_demand = std::vector(world.provinces.size(), 0.f);
            for(const auto& province : world.provinces) {
                const auto& product = province.products[market.commodity];
                market.demand[province] = product.demand;
                market.price[province] = product.price;
                market.supply[province] = product.supply;
            }
        }
    });
    return markets;
}

void update_markets(const World& world, std::vector<Economy::Market> markets) {
    tbb::parallel_for(tbb::blocked_range(markets.begin(), markets.end()), [&world](const auto& markets_range) {
        for(auto& market : markets_range) {
            for(const auto& province : world.provinces) {
                const auto& product = province.products[market.commodity];
                market.demand[province] = product.demand;
                market.price[province] = product.price;
                market.supply[province] = product.supply;
            }
        }
    });
}

void Economy::do_tick(World& world, EconomyState& economy_state) {
    // Distrobute products accross
    world.profiler.start("E-init");

    auto& markets = economy_state.commodity_market;
    if(markets.empty())
        markets = init_markets(world);
    update_markets(world, markets);
    world.profiler.stop("E-init");

    world.profiler.start("E-trade");
    economy_state.trade.recalculate(world);
    auto& trade = economy_state.trade;

    tbb::parallel_for(tbb::blocked_range(markets.begin(), markets.end()), [&world, &trade](const auto& markets_range) {
        std::vector<float> values(world.provinces.size(), 0.f);
    for(auto& market : markets_range) {
        for(const auto province_id : trade.cost_eval) {
            auto& province = world.provinces[province_id];
            if(Nation::is_invalid(province.owner_id)) continue;
            auto& nation = world.nations[province.owner_id];
            auto sum_weightings = 0.f;
            for(const auto other_province_id : nation.owned_provinces) {
                auto price = market.price[other_province_id];
                auto apparent_price = price + 0.01f * trade.trade_costs[province_id][other_province_id];
                apparent_price += glm::epsilon<float>();
                values[other_province_id] = market.supply[other_province_id] / (apparent_price * apparent_price);
                sum_weightings += values[other_province_id];
            }
            if(sum_weightings == 0.f) continue;
            for(const auto other_province_id : nation.owned_provinces) {
                values[other_province_id] /= sum_weightings;
                auto demand = market.demand[other_province_id];
                market.global_demand[province_id] += demand * values[other_province_id];
            }
        }
        for(const auto province_id : trade.cost_eval) {
            const auto price_change_speed = 0.9f;
            const auto price_factor = market.global_demand[province_id] / (market.supply[province_id] + 0.01f);
            const auto new_price = market.price[province_id] * (1.f - price_change_speed) + (market.price[province_id] * price_factor) * price_change_speed;

            auto& province = world.provinces[province_id];
            if(Nation::is_invalid(province.owner_id)) continue;
            auto& product = province.products[market.commodity];
            //product.price = std::max(new_price, 0.01f);
            product.global_demand = market.global_demand[province_id];
        }
    }
    });
    world.profiler.stop("E-trade");

    world.profiler.start("E-big");
    tbb::combinable<std::vector<NewUnit>> province_new_units;
    tbb::combinable<std::vector<float>> paid_taxes;
    std::vector<std::vector<float>> buildings_new_worker(world.provinces.size());
    std::vector<std::vector<PopNeed>> pops_new_needs(world.provinces.size());

    tbb::parallel_for(static_cast<size_t>(0), world.provinces.size(), [&](const auto province_id) {
        auto& province = world.provinces[province_id];
        const auto& province_policy = world.nations[province.controller_id].current_policy;
        for(auto& product : province.products)
            product.close_market();

        auto& new_needs = pops_new_needs[province_id];
        new_needs.assign(province.pops.size(), PopNeed{});

        ProvinceEconomyInfo info{};
        info.produced.resize(world.commodities.size(), std::make_pair(0.f, 0.f));

        for(auto& building_type : world.building_types) {
            auto& building = province.buildings[building_type];
            update_industry_production(world, building, building_type, province, info);
        }

        for(size_t i = 0; i < province.pops.size(); i++) {
            const auto& pop = province.pops[i];
            new_needs[i].budget = pop.budget;
            new_needs[i].life_needs_met = glm::clamp(pop.life_needs_met - 0.1f, -1.f, 1.f);
        }

        // Bureaucracy employment and administration
        auto bureaucrats_amount = province.pops[(int)PopGroup::BUREAUCRAT].size;
        auto bureaucracy_pts = bureaucrats_amount
            * (1.f - province.pops[(int)PopGroup::BUREAUCRAT].militancy)
            * province.pops[(int)PopGroup::BUREAUCRAT].literacy;
        info.bureaucracy_eff = (province.total_pops() * province.average_militancy()) / bureaucracy_pts;
        info.admin_funds = province_policy.admin_funding * province_policy.min_wage;

        // Pops buying up stockpile from the province
        update_pop_needs(world, province, new_needs, info);

        // Factory employment for laborers, and artisans making independent products
        auto& new_workers = buildings_new_worker[province_id];
        new_workers.assign(world.building_types.size(), 0.f);
        update_factories_employment(world, province, new_workers);
        for(auto& building_type : world.building_types) {
            auto& building = province.buildings[building_type];
            update_industry_accounting(world, building, building_type, province, info);
        }

        // Payment to the pops, including soldier pop funds
        info.military_funds = province_policy.military_funding * province_policy.min_wage;
        new_needs[(int)PopGroup::LABORER].budget += info.laborers_payment;
        new_needs[(int)PopGroup::ARTISAN].budget += info.artisans_payment;
        new_needs[(int)PopGroup::BURGEOISE].budget += info.private_payment;
        new_needs[(int)PopGroup::BUREAUCRAT].budget += info.bureaucrats_payment;
        new_needs[(int)PopGroup::SOLDIER].budget += info.military_funds;

        paid_taxes.local().resize(world.nations.size());
        paid_taxes.local()[province.controller_id] = info.state_payment;
        for(auto& building : province.buildings) {
            // There must not be conflict ongoing otherwise they wont be able to build shit
            if(province.controller_id == province.owner_id && building.can_build_unit() && building.working_unit_type_id.has_value()) {
                auto& pop = province.pops[(int)PopGroup::SOLDIER];
                const auto final_size = glm::min(pop.size, 100.f);
                province_new_units.local().emplace_back(building.working_unit_type_id.value(), final_size, province, pop.type_id);
                building.working_unit_type_id.reset();
            }
        }
    });
    world.profiler.stop("E-big");

    world.profiler.start("E-mutex");
    // Collect list of nations that exist
    std::vector<Nation*> eval_nations;
    for(auto& nation : world.nations)
        if(nation.exists())
            eval_nations.push_back(&nation);

    // -------------------------- MUTEX PROTECTED WORLD CHANGES BELOW -------------------------------
    const std::scoped_lock lock(world.world_mutex);

    tbb::parallel_for(static_cast<size_t>(0), world.provinces.size(), [&](const auto province_id) {
        auto& province = world.provinces[province_id];
        if(Nation::is_invalid(province.controller_id)) return;
        const auto& new_needs = pops_new_needs[province_id];
        for(size_t i = 0; i < province.pops.size(); i++) {
            auto& pop = province.pops[i];
            pop.budget = new_needs[i].budget;
            pop.life_needs_met = new_needs[i].life_needs_met;
            const auto growth = pop.size * pop.life_needs_met * 0.0001f;
            pop.size = glm::max(pop.size + growth, 1.f);
            pop.militancy = glm::clamp(pop.militancy + 0.01f * -pop.life_needs_met, 0.f, 1.f);
        }
        const auto& new_workers = buildings_new_worker[province_id];
        for(const auto& building_type : world.building_types)
            province.buildings[building_type].workers = new_workers[building_type];
    });

    province_new_units.combine_each([&](auto& new_unit_list) {
        for(auto& new_unit : new_unit_list) { // Now commit the transaction of the new units into the main world area
            const auto& province = world.provinces[new_unit.province_id];
            const auto& nation = world.nations[province.controller_id];
            Unit unit{};
            unit.pop_id = PopId(size_t(new_unit.pop_id));
            unit.type_id = new_unit.type_id;
            unit.size = new_unit.size;
            unit.base = world.unit_types[unit.type_id].max_health;
            unit.set_owner(nation);
            world.unit_manager.add_unit(unit, province);

            Eng3D::Log::debug("economy", string_format("%s has built an unit %s", province.ref_name.c_str(), world.unit_types[unit.type_id].ref_name.c_str()));
        }
    });

    paid_taxes.combine_each([&world](auto& paid_taxes_list) {
        for(auto& nation : world.nations)
        nation.budget += paid_taxes_list[nation];
    });

    world.profiler.start("Emigration");
    //do_emigration(world);
    world.profiler.stop("Emigration");
    world.profiler.stop("E-mutex");
}
