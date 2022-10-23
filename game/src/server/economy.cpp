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

constexpr float production_scaling_speed_factor = 0.5f;
constexpr float scale_speed(float v) {
    return 1.f - production_scaling_speed_factor + production_scaling_speed_factor * v;
}

// Updates supply, demand, and set wages for workers
static void update_factory_production(World& world, Building& building, const BuildingType& building_type, Province& province, float& pop_payment, float& artisans_amount, float& artisan_payment)
{
    // Barracks and so on
    if(Good::is_invalid(building_type.output_id)) return;
    
    constexpr auto artisan_production_rate = 1.f;
    constexpr auto factory_production_rate = 10.f;
    auto& output = world.goods[building_type.output_id];
    auto& output_product = province.products[output];
    if(!building.can_do_output(province)) { // Artisans take place of factory
        const auto output_amount = artisans_amount * artisan_production_rate; // Reduced rate of production
        artisan_payment += output_product.produce(output_amount);
        return; 
    }

    // TODO add output modifier
    // Calculate outputs

    // TODO set min wages
    float min_wage = glm::max(1.f, 0.0001f);

    // TODO set output depending on amount of workers
    float total_worker_pop = building.workers;
    auto output_amount = building.production_scale * total_worker_pop * factory_production_rate;

    // TODO add input modifier
    auto inputs_cost = 0.f; // Buy the inputs for the factory
    for(const auto& [product_id, amount] : building_type.req_goods)
        inputs_cost += province.products[product_id].buy(amount * building.production_scale);
    auto output_value = output_product.produce(output_amount);
    auto profit = output_value - min_wage - inputs_cost;
    
    if(profit <= 0.f) {
        if(output_value - inputs_cost > 0.f)
            pop_payment += output_value - inputs_cost;
    } else {
        pop_payment += min_wage + profit * 0.2f * building.workers;
    }
    building.budget += profit;
    // TODO: Make building inoperate for the legnth of the upgrade (need to acquire materials)
    auto upgrade_cost = 1000.f * building.level;
    if(building.budget >= upgrade_cost) {
        building.budget -= upgrade_cost;
        building.level += 1.f;
    } else if(building.budget <= upgrade_cost) {
        building.budget += upgrade_cost;
        building.level -= 1.f;
    }

    // Rescale production
    // This is used to set how much the of the maximum capacity the factory produce
    building.production_scale = building.level;//glm::clamp(building.production_scale * scale_speed(output_value / (min_wage + inputs_cost)), 0.f, 1.f);
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
    for(const auto& [factory_index, _] : factories_by_profitability) {
        auto& building = province.buildings[factory_index];
        const auto& type = world.building_types[factory_index];
        auto factory_workers = building.level * type.num_req_workers * building.production_scale;
        auto allocated_workers = glm::max(glm::min(factory_workers, unallocated_workers), 0.f);
        // Average with how much the factory had before
        // Makes is more stable so everyone don't change workplace immediately
        new_workers[factory_index] = (allocated_workers / 16.0f + (building.workers * 15.0f) / 16.0f) * is_operating;
        unallocated_workers -= building.workers * is_operating;
    }
}

/// @brief Calculate the budget that we spend on each needs
void update_pop_needs(World& world, Province& province, std::vector<PopNeed>& pop_needs) {
    for(size_t i = 0; i < province.pops.size(); i++) {
        auto& pop_need = pop_needs[i];
        auto& pop = province.pops[i];
        const auto& type = world.pop_types[pop.type_id];

        // Do basic needs
        if(pop_need.budget == 0.f) return;
        auto used_budget = 0.f;
        for(size_t j = 0; j < world.goods.size(); j++) {
            if(pop.size == 0.f) continue;
            if(type.basic_needs_amount[j] == 0.f) continue;
            const auto budget_alloc = pop_need.budget * 0.8f;
            if(budget_alloc == 0.f) continue;

            const auto needed_amount = pop.size * type.basic_needs_amount[j];
            if(needed_amount == 0.f) continue;
            const auto amount = glm::clamp(type.basic_needs_amount[j] * budget_alloc / province.products[j].price / world.goods.size(), 0.f, needed_amount);
            //= glm::min(type.basic_needs_amount[j], province.products[j].supply) / budget_alloc;

            pop_need.life_needs_met += amount / needed_amount;
            used_budget += province.products[j].buy(amount);
        }
        pop_need.budget = glm::max(pop_need.budget - used_budget, 0.f);
        pop_need.budget += pop.size * 1.5f;
    }
}

void Economy::do_tick(World& world, EconomyState& economy_state) {
    world.profiler.start("E-big");
    tbb::combinable<std::vector<NewUnit>> province_new_units;
    std::vector<std::vector<float>> buildings_new_worker(world.provinces.size());
    std::vector<std::vector<PopNeed>> pops_new_needs(world.provinces.size());

    tbb::parallel_for(static_cast<size_t>(0), world.provinces.size(), [&world, &buildings_new_worker, &province_new_units, &pops_new_needs](const auto province_id) {
        auto& province = world.provinces[province_id];
        if(Nation::is_invalid(province.controller_id)) return;
        for(auto& product : province.products)
            product.close_market();

        auto artisans_amount = 0.f;
        for(auto& pop : province.pops) {
            if(world.pop_types[pop.type_id].group == PopGroup::ARTISAN)
                artisans_amount += pop.size;
        }

        auto laborers_payment = 1.f, artisans_payment = 1.f;
        for(auto& building_type : world.building_types) {
            auto& building = province.buildings[building_type];
            update_factory_production(world, building, building_type, province, laborers_payment, artisans_amount, artisans_payment);
        }

        auto& new_needs = pops_new_needs[province_id];
        new_needs.assign(province.pops.size(), PopNeed{});
        for(size_t i = 0; i < province.pops.size(); i++) {
            const auto& pop = province.pops[i];
            new_needs[i].budget = pop.budget;
            new_needs[i].life_needs_met = pop.life_needs_met - 0.25f;
        }

        auto laborers_amount = 0.f;
        for(auto& pop : province.pops) {
            if(world.pop_types[pop.type_id].group == PopGroup::LABORER)
                laborers_amount += pop.size;
        }
        for(size_t i = 0; i < province.pops.size(); i++) {
            const auto& pop = province.pops[i];
            if(world.pop_types[pop.type_id].group == PopGroup::LABORER)
                new_needs[i].budget += (pop.size / laborers_amount) * laborers_payment;
            else if(world.pop_types[pop.type_id].group == PopGroup::ARTISAN)
                new_needs[i].budget += (pop.size / artisans_amount) * artisans_payment;
        }

        auto& new_workers = buildings_new_worker[province_id];
        new_workers.assign(world.building_types.size(), 0.f);
        update_factories_employment(world, province, new_workers);
        update_pop_needs(world, province, new_needs);
        for(auto& building : province.buildings) {
            // There must not be conflict ongoing otherwise they wont be able to build shit
            if(province.controller_id == province.owner_id && UnitType::is_valid(building.working_unit_type_id) && building.can_build_unit()) {
                auto& pop = province.get_soldier_pop();
                const auto final_size = glm::min(pop.size, 100.f);
                province_new_units.local().emplace_back(building.working_unit_type_id, final_size, province, pop.type_id);
                building.working_unit_type_id = UnitTypeId(-1);
            }
        }
    });
    world.profiler.stop("E-big");

    // Distrobute products accross
    world.profiler.start("E-Distrobute");
    for(const auto& province : world.provinces)
        for(const auto& good : world.goods)
            for(const auto neighbour_id : province.neighbour_ids)
                world.provinces[neighbour_id].products[good].supply += glm::min(0.1f, province.products[good].supply * 0.1f);
    world.profiler.stop("E-Distrobute");

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
        const auto& nation = world.nations[province.controller_id];
        const auto& new_needs = pops_new_needs[province_id];
        for(size_t i = 0; i < province.pops.size(); i++) {
            auto& pop = province.pops[i];
            pop.budget = new_needs[i].budget;
            pop.life_needs_met = new_needs[i].life_needs_met;
            const auto growth = glm::clamp(pop.size * pop.life_needs_met * 0.1f, -100.f, 100.f);
            pop.size = glm::max(pop.size + growth, 1.f);
            pop.militancy += 0.01f * -pop.life_needs_met;
            pop.ideology_approval[nation.ideology_id] += pop.life_needs_met * 0.25f;
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

    world.profiler.start("Emigration");
    //do_emigration(world);
    world.profiler.stop("Emigration");
    world.profiler.stop("E-mutex");
}
