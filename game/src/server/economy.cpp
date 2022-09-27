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
    float life_needs_met;
    float budget;
};

struct NewUnit {
    Unit unit;
    ProvinceId unit_province;
    NewUnit(Unit& _unit, ProvinceId _unit_province) 
        : unit{_unit},
        unit_province{ _unit_province }
    {

    }
};

constexpr float production_scaling_speed_factor = 0.5f;
constexpr float scale_speed(float v) {
    return 1.f - production_scaling_speed_factor + production_scaling_speed_factor * v;
}

// Updates supply, demand, and set wages for workers
static void update_factory_production(World& world, Building& building, const BuildingType& building_type, Province& province, float& pop_payment)
{
    if(Good::is_invalid(building_type.output_id) || building.can_do_output()) return;

    // TODO add output modifier
    // Calculate outputs
    auto& output = world.goods[building_type.output_id];
    auto& output_product = province.products[output];
    auto output_amount = building.level * 1000.f * building.production_scale;

    // TODO set min wages
    float min_wage = glm::max(1.f, 0.0001f);

    // TODO set output depending on amount of workers
    float total_worker_pop = building.workers;

    // TODO add input modifier
    auto inputs_cost = 0.f; // Buy the inputs for te factory
    for(const auto& input : building_type.req_goods)
        inputs_cost += province.products[input.first].buy(input.second * building.production_scale);
    auto output_value = output_product.produce(output_amount);
    auto profit = output_value - min_wage - inputs_cost;
    
    if(profit <= 0.f) {
        if(output_value - inputs_cost > 0.f)
            pop_payment += output_value - inputs_cost;
    } else {
        pop_payment += min_wage + profit * 0.2f * building.workers;
    }

    // Rescale production
    // This is used to set how much the of the maximum capicity the factory produce
    building.production_scale = glm::clamp(building.production_scale * scale_speed(output_value / (min_wage + inputs_cost)), 0.5f, 2.f);
}

// Update the factory employment
static void update_factories_employment(const World& world, Province& province, std::vector<float>& new_workers) {
    float unallocated_workers = 0.f;
    for(auto& pop : province.pops)
        if(world.pop_types[pop.type_id].group == PopGroup::LABORER)
            unallocated_workers += pop.size;

    // Sort factories by production scale, which is suppose to represent how profitable the factory is
    // Might be better to calculate how profitable it really is and use that instead
    std::vector<std::pair<size_t, float>> factories_by_profitability;
    for(size_t i = 0; i < province.buildings.size(); i++)
        factories_by_profitability.emplace_back(i, province.buildings[i].production_scale);
    std::sort(factories_by_profitability.begin(), factories_by_profitability.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    float is_operating = province.controller_id == province.owner_id ? 1.f : 0.f;
    for(const auto& factory_index : factories_by_profitability) {
        auto& building = province.buildings[factory_index.first];
        const auto& type = world.building_types[factory_index.first];
        auto factory_workers = building.level * type.num_req_workers * building.production_scale;
        auto allocated_workers = glm::min(factory_workers, unallocated_workers);
        // Average with how much the factory had before
        // Makes is more stable so everyone don't change workplace immediately
        new_workers[factory_index.first] = ((allocated_workers) / 16.0f + (building.workers * 15.0f) / 16.0f) * is_operating;
        unallocated_workers -= building.workers * is_operating;
    }
}

/// @brief Calculate the budget that we spend on each needs
void update_pop_needs(World& world, Province& province, std::vector<PopNeed>& pop_needs) {
    pop_needs.assign(province.pops.size(), PopNeed{});
    for(size_t i = 0; i < province.pops.size(); i++) {
        auto& pop_need = pop_needs[i];
        auto& pop = province.pops[i];
        const auto& type = world.pop_types[pop.type_id];
        pop_need.life_needs_met = -0.01f;
        // Do basic needs
        {
            auto total_price = 0.f;
            for(size_t j = 0; j < world.goods.size(); j++)
                total_price += glm::min(type.basic_needs_amount[j], province.products[j].supply) * province.products[j].price;
            auto buying_factor = glm::clamp(total_price / pop_need.budget, 0.1f, 1.f);
            for(size_t j = 0; j < world.goods.size(); j++)
                pop_need.budget -= province.products[j].buy(pop.size * type.basic_needs_amount[j] * buying_factor);
            pop_need.life_needs_met += buying_factor;
        }
        pop_need.budget = pop_need.budget < 0.f ? 0.f : pop_need.budget;
        pop_need.budget += pop.size * 1.1f;
    }
}

void Economy::do_tick(World& world, EconomyState& economy_state) {
    world.profiler.start("E-big");
    tbb::combinable<std::vector<NewUnit>> province_new_units;
    std::vector<std::vector<float>> buildings_new_worker(world.provinces.size());
    std::vector<std::vector<PopNeed>> pops_new_needs(world.provinces.size());
    tbb::parallel_for(0zu, world.provinces.size(), [&world, &buildings_new_worker, &province_new_units, &pops_new_needs](const auto province_id) {
        auto& province = world.provinces[province_id];
        if(Nation::is_invalid(province.controller_id)) return;
        for(auto& product : province.products)
            product.close_market();

        float laborers_payment = 1.f;
        for(auto& building_type : world.building_types) {
            auto& building = province.buildings[building_type];
            update_factory_production(world, building, building_type, province, laborers_payment);
        }

        auto& new_needs = pops_new_needs[province_id];
        new_needs.assign(province.pops.size(), PopNeed{});
        for(size_t i = 0; i < province.pops.size(); i++) {
            auto& pop = province.pops[i];
            new_needs[i].budget = pop.budget;
        }

        float laborers_amount = 0.f;
        for(auto& pop : province.pops)
            if(world.pop_types[pop.type_id].group == PopGroup::LABORER)
                laborers_amount += pop.size;
        for(size_t i = 0; i < province.pops.size(); i++) {
            const auto& pop = province.pops[i];
            if(world.pop_types[pop.type_id].group == PopGroup::LABORER)
                new_needs[i].budget += (pop.size / laborers_amount) * laborers_payment;
        }

        std::vector<float>& new_workers = buildings_new_worker[province_id];
        new_workers.assign(world.building_types.size(), 0.f);
        update_factories_employment(world, province, new_workers);
        update_pop_needs(world, province, new_needs);
        for(auto& building : province.buildings) {
            // There must not be conflict ongoing otherwise they wont be able to build shit
            if(province.controller_id == province.owner_id && UnitType::is_valid(building.working_unit_type_id) && building.can_build_unit()) {
                // Ratio of health:person is 25, thus making units very expensive
                const float army_size = 100;
                /// @todo Consume special soldier pops instead of farmers!!!
                auto it = std::find_if(province.pops.begin(), province.pops.end(), [&world, building, army_size](const auto& e) {
                    return (e.size >= army_size && world.pop_types[e.type_id].group == PopGroup::FARMER);
                });

                if(it != province.pops.end()) {
                    auto& nation = world.nations[province.owner_id];
                    const auto final_size = glm::min<float>((*it).size, army_size);
                    const auto given_money = final_size;
                    // Nation must have money to pay the units
                    if(given_money >= nation.budget) continue;
                    /// @todo Maybe delete if size becomes 0?
                    (*it).size -= final_size;
                    if(final_size) {
                        nation.budget -= given_money;
                        Unit unit{};
                        unit.type_id = building.working_unit_type_id;
                        unit.set_owner(nation);
                        unit.size = final_size;
                        unit.base = world.unit_types[unit.type_id].max_health;
                        province_new_units.local().emplace_back(unit, province);
                        building.working_unit_type_id = UnitTypeId(-1);
                        Eng3D::Log::debug("economy", "[" + province.ref_name + "]: Has built an unit of [" + world.unit_types[unit.type_id].ref_name + "]");
                    }
                }
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

    tbb::parallel_for(0zu, world.provinces.size(), [&world, &pops_new_needs, &buildings_new_worker](const auto province_id) {
        auto& province = world.provinces[province_id];
        if(Nation::is_invalid(province.controller_id)) return;
        const auto& new_needs = pops_new_needs[province_id];
        for(size_t i = 0; i < province.pops.size(); i++) {
            auto& pop = province.pops[i];
            pop.budget = new_needs[i].budget;
            pop.life_needs_met = new_needs[i].life_needs_met;

            float growth = pop.size * pop.life_needs_met * 0.001f;
            pop.size += static_cast<float>((int)growth);
            pop.militancy += 0.01f * -pop.life_needs_met;
            pop.ideology_approval[world.nations[province.owner_id].ideology_id] += pop.life_needs_met * 0.25f;
        }
        const auto& new_workers = buildings_new_worker[province_id];
        for(size_t i = 0; i < province.buildings.size(); i++)
            province.buildings[i].workers = new_workers[i];
    });

    province_new_units.combine_each([&world](auto& new_unit_list) {
        for(auto& new_unit : new_unit_list) // Now commit the transaction of the new units into the main world area
            world.unit_manager.add_unit(new_unit.unit, new_unit.unit_province);
    });

    world.profiler.start("Emigration");
    //do_emigration(world);
    world.profiler.stop("Emigration");
    world.profiler.stop("E-mutex");
}
