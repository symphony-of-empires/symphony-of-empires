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
#include "eng3d/thread_pool.hpp"
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

// Updates supply, demand, and set wages for workers
static void update_industry_production(World& world, Building& building, const BuildingType& building_type, Province& province, float& artisans_amount, float& artisan_payment)
{
    constexpr auto artisan_production_rate = 0.01f;
    auto& output = world.commodities[building_type.output_id];
    auto& output_product = province.products[output];
    if(!building.can_do_output(province, building_type.input_ids) || building.level == 0.f) { // Artisans take place of industry
        if(output_product.supply < output_product.demand) { // Artisans only produce iff suitable so
            const auto output_amount = artisans_amount * artisan_production_rate; // Reduced rate of production
            artisan_payment += output_product.produce(output_amount);
        }
        return; 
    }

    // TODO add input modifier
    building.expenses.inputs_cost = 0.f; // Buy the inputs for the industry
    for(const auto& [product_id, amount] : building_type.req_goods)
        building.expenses.inputs_cost += province.products[product_id].buy(amount * building.production_scale);
    output_product.produce(building.get_output_amount());
}

static void update_industry_accounting(World& world, Building& building, const BuildingType& building_type, Province& province, float& pop_payment, float& state_payment, float& private_payment)
{
    // Barracks and so on
    if(Commodity::is_invalid(building_type.output_id)) return;
    const auto& nation = world.nations[province.controller_id];

    // TODO: Make it so burgeoise aren't duplicating money out of thin air
    const auto private_investment = private_payment * 0.8 * nation.current_policy.private_ownership;
    building.budget += private_investment;
    
    auto& output = world.commodities[building_type.output_id];
    auto& output_product = province.products[output];
    if(!building.can_do_output(province, building_type.input_ids) || building.level == 0.f) // Artisans take place of industry
        return;

    // TODO add output modifier
    // Calculate outputs

    // TODO set min wages
    float min_wage = glm::max(nation.current_policy.min_wage, glm::epsilon<float>());

    building.expenses.wages = min_wage * building.workers;
    pop_payment += building.expenses.wages;
    auto profit = building.get_profit();

    // Taxation occurs even without a surplus
    building.expenses.state_taxes = 0.f;
    if(profit > 0.f) {
        building.expenses.state_taxes = profit * nation.current_policy.industry_profit_tax;
        state_payment += building.expenses.state_taxes;
        profit -= building.expenses.state_taxes;
    }

    // Dividends that will be paid to the shareholders
    building.expenses.state_dividends = building.expenses.pop_dividends = building.expenses.private_dividends = 0.f; // Dividends are paid after calculating the surplus
    auto surplus = profit - building.expenses.get_total();
    if(surplus > 0.f) {
        // Disperse profits to holders
        // TODO: Should surplus be decremented between each payout?
        state_payment += building.get_state_payment(surplus);
        building.expenses.state_dividends += building.get_state_payment(surplus);
        surplus -= building.get_state_payment(surplus);

        private_payment += building.get_private_payment(surplus);
        building.expenses.private_dividends += building.get_private_payment(surplus);
        surplus -= building.get_private_payment(surplus);

        pop_payment += building.get_collective_payment(surplus);
        building.expenses.pop_dividends += building.get_collective_payment(surplus);
        surplus -= building.get_collective_payment(surplus);

        profit -= building.expenses.get_dividends();
    }

    building.budget += profit; // Pay the remainder profit to the building
    // TODO: Make building inoperate for the legnth of the upgrade (need to acquire materials)
    const auto upgrade_cost = building.get_upgrade_cost();
    if(building.budget >= upgrade_cost) {
        building.budget -= upgrade_cost;
        building.level += 1.f;
    } else if(building.budget <= -(upgrade_cost * 0.5f)) {
        building.budget += upgrade_cost;
        building.level -= 1.f;
    }

    //! Based production not used!
    // auto base_production = 1.f;
    // // Capitalist nations require having a stake first; whereas non-capitalist nations do not
    // // require a pevious stake to exist at all to enforce their production scales
    // if(!nation.can_directly_control_factories()) {
    //     base_production = building.state_ownership * nation.commodity_production[output] * building.level;
    // } else {
    //     base_production = nation.commodity_production[output] * building.level;
    // }
    
    // Rescale production
    // This is used to set how much the of the maximum capacity the industry produce
    const auto max_revenue = output_product.price * building.get_max_output_amount(building_type.num_req_workers);
    if(max_revenue == 0.f) {
        building.production_scale = scale_speed(building.production_scale, 0.f);
    } else if(building.expenses.get_total() == 0.f) {
        building.production_scale = scale_speed(building.production_scale, building.level);
    } else {
        building.production_scale = scale_speed(building.production_scale, building.level * glm::clamp(max_revenue / building.expenses.get_total(), 0.f, 1.f));
    }
}

// Update the industry employment
static void update_factories_employment(const World& world, Province& province, std::vector<float>& new_workers) {
    float unallocated_workers = 0.f;
    for(auto& pop : province.pops)
        if(world.pop_types[pop.type_id].group == PopGroup::LABORER)
            unallocated_workers += pop.size;

    // Sort factories by production scale, which is suppose to represent how profitable the industry is
    // Might be better to calculate how profitable it really is and use that instead
    std::vector<std::pair<size_t, float>> factories_by_profitability;
    for(size_t i = 0; i < province.buildings.size(); i++)
        factories_by_profitability.emplace_back(i, province.buildings[i].production_scale);
    std::sort(factories_by_profitability.begin(), factories_by_profitability.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    float is_operating = province.controller_id == province.owner_id ? 1.f : 0.f;
    for(const auto& [industry_index, _] : factories_by_profitability) {
        auto& building = province.buildings[industry_index];
        const auto& type = world.building_types[industry_index];
        auto industry_workers = building.production_scale * type.num_req_workers;
        auto allocated_workers = glm::max(glm::min(industry_workers, unallocated_workers), 0.f);
        // Average with how much the industry had before
        // Makes is more stable so everyone don't change workplace immediately
        new_workers[industry_index] = (allocated_workers / 16.0f + (building.workers * 15.0f) / 16.0f) * is_operating;
        unallocated_workers -= building.workers * is_operating;
    }
}

/// @brief Calculate the budget that we spend on each needs
void update_pop_needs(World& world, Province& province, std::vector<PopNeed>& pop_needs, float& state_payment) {
    auto& nation = world.nations[province.controller_id];

    std::vector<float> goods_payment(world.commodities.size(), 0.f);
    for(size_t i = 0; i < province.pops.size(); i++) {
        auto& pop_need = pop_needs[i];
        auto& pop = province.pops[i];
        const auto& needs_amounts = world.pop_types[pop.type_id].basic_needs_amount;
        if(pop.size == 0.f || pop_need.budget == 0.f) return;

        const auto percentage_to_spend = 0.8f;
        const auto budget_alloc = pop_need.budget * percentage_to_spend;
        pop_need.budget -= budget_alloc;

        // If we are going to have value added taxes we should separate them from income taxes
        state_payment += budget_alloc * nation.current_policy.pop_tax;
        const auto budget_after_VAT = budget_alloc * (1.f - nation.current_policy.pop_tax);
        const auto budget_per_pop = budget_after_VAT / pop.size;

        auto total_factor = std::reduce(needs_amounts.begin(), needs_amounts.end());
        for(const auto& commodity : world.commodities) {
            if(needs_amounts[commodity] == 0.f) continue;

            const auto need_factor = needs_amounts[commodity] / total_factor;
            const auto amount = (budget_per_pop * need_factor / province.products[commodity].price);

            pop_need.life_needs_met += amount * need_factor;
            // const auto payment = province.products[commodity].buy(amount);
            goods_payment[commodity] += amount * province.products[commodity].price;
            province.products[commodity].demand += amount * pop.size;
        }
    }
    for(const auto& building_type : world.building_types)
        province.buildings[building_type].revenue.outputs += goods_payment[building_type.output_id];
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
    if (markets.empty())
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
                product.price = std::max(new_price, 0.01f);
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

    tbb::parallel_for(static_cast<size_t>(0), world.provinces.size(), [&world, &buildings_new_worker, &province_new_units, &pops_new_needs, &paid_taxes](const auto province_id) {
        auto& province = world.provinces[province_id];
        if(Nation::is_invalid(province.controller_id)) return;
        for(auto& product : province.products)
            product.close_market();

        auto& new_needs = pops_new_needs[province_id];
        new_needs.assign(province.pops.size(), PopNeed{});

        auto laborers_payment = 0.f, artisans_payment = 0.f, state_payment = 0.f, private_payment = 0.f, bureaucrats_payment = 0.f;
        auto artisans_amount = 0.f, burgeoise_amount = 0.f, laborers_amount = 0.f, bureaucrats_amount = 0.f;
        auto bureaucracy_pts = 0.f;
        for(auto& pop : province.pops) {
            if(world.pop_types[pop.type_id].group == PopGroup::ARTISAN)
                artisans_amount += pop.size;
            else if(world.pop_types[pop.type_id].group == PopGroup::LABORER)
                laborers_amount += pop.size;
            else if(world.pop_types[pop.type_id].group == PopGroup::BURGEOISE)
                burgeoise_amount += pop.size;
            else if(world.pop_types[pop.type_id].group == PopGroup::BUREAUCRAT) {
                bureaucrats_amount += pop.size;
                bureaucracy_pts += pop.size * (1.f - pop.militancy) * pop.literacy;
            }
        }

        for(auto& building_type : world.building_types) {
            auto& building = province.buildings[building_type];
            building.revenue.outputs = 0.f;
            update_industry_production(world, building, building_type, province, artisans_amount, artisans_payment);
        }

        for(size_t i = 0; i < province.pops.size(); i++) {
            const auto& pop = province.pops[i];
            new_needs[i].budget = pop.budget;
            new_needs[i].life_needs_met = glm::clamp(pop.life_needs_met - 0.1f, -1.f, 1.f);
        }

        // Bureaucracy points
        auto bureaucracy_eff = (province.total_pops() * province.average_militancy()) / bureaucracy_pts;

        auto& new_workers = buildings_new_worker[province_id];
        new_workers.assign(world.building_types.size(), 0.f);
        update_factories_employment(world, province, new_workers);
        for(auto& building_type : world.building_types) {
            auto& building = province.buildings[building_type];
            update_industry_accounting(world, building, building_type, province, laborers_payment, state_payment, private_payment);
        }

        for(size_t i = 0; i < province.pops.size(); i++) {
            const auto& pop = province.pops[i];
            if(world.pop_types[pop.type_id].group == PopGroup::LABORER)
                new_needs[i].budget += (pop.size / laborers_amount) * laborers_payment;
            else if(world.pop_types[pop.type_id].group == PopGroup::ARTISAN)
                new_needs[i].budget += (pop.size / artisans_amount) * artisans_payment;
            else if(world.pop_types[pop.type_id].group == PopGroup::BURGEOISE)
                new_needs[i].budget += (pop.size / burgeoise_amount) * private_payment;
            else if(world.pop_types[pop.type_id].group == PopGroup::BUREAUCRAT)
                new_needs[i].budget += (pop.size / bureaucrats_amount) * bureaucrats_payment;
        }
        update_pop_needs(world, province, new_needs, state_payment);

        paid_taxes.local().resize(world.nations.size());
        paid_taxes.local()[province.controller_id] = state_payment;
        for(auto& building : province.buildings) {
            // There must not be conflict ongoing otherwise they wont be able to build shit
            if(province.controller_id == province.owner_id && building.can_build_unit() && building.is_working_on_unit()) {
                auto& pop = province.get_soldier_pop();
                const auto final_size = glm::min(pop.size, 100.f);
                province_new_units.local().emplace_back(building.working_unit_type_id, final_size, province, pop.type_id);
                building.stop_working_on_unit();
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

    tbb::parallel_for(static_cast<size_t>(0), world.provinces.size(), [&world, &pops_new_needs, &buildings_new_worker](const auto province_id) {
        auto& province = world.provinces[province_id];
        if(Nation::is_invalid(province.controller_id)) return;
        const auto& new_needs = pops_new_needs[province_id];
        for(size_t i = 0; i < province.pops.size(); i++) {
            auto& pop = province.pops[i];
            pop.budget = new_needs[i].budget;
            pop.life_needs_met = new_needs[i].life_needs_met;
            const auto growth = glm::clamp(pop.size * pop.life_needs_met * 0.1f, -100.f, 100.f);
            pop.size = glm::max(pop.size + growth, 1.f);
            pop.militancy = glm::clamp(pop.militancy + 0.01f * -pop.life_needs_met, 0.f, 1.f);
        }
        const auto& new_workers = buildings_new_worker[province_id];
        for(size_t i = 0; i < province.buildings.size(); i++)
            province.buildings[i].workers = new_workers[i];
    });

    province_new_units.combine_each([&world](auto& new_unit_list) {
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
