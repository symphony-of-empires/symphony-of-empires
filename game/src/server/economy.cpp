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
//      server/economy.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <execution>
#include <cstdio>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/combinable.h>
#include "eng3d/log.hpp"
#include "eng3d/serializer.hpp"
#include "eng3d/thread_pool.hpp"
#include "eng3d/rand.hpp"

#include "action.hpp"
#include "server/economy.hpp"
#include "world.hpp"
#include "io_impl.hpp"
#include "server/server_network.hpp"
#include "product.hpp"
#include "good.hpp"
#include "emigration.hpp"

#undef min
#undef max

// Visual Studio does not define ssize_t because it's a POSIX-only type
#ifdef _MSC_VER
typedef signed int ssize_t;
#endif

struct Market {
    Good::Id good;
    std::vector<float> prices;
    std::vector<float> supply;
    std::vector<float> demand;
};

struct PopNeed {
    float life_needs_met;
    float everyday_needs_met;
    float budget;
};

void militancy_update(World& world, Nation& nation) {
    for(const auto& province_id : nation.controlled_provinces) {
        auto& province = world.provinces[province_id];
        for(auto& pop : province.pops) {
            // More literacy means more educated persons with less children
            Eng3D::Decimal growth = pop.size / (pop.literacy + 1.f);
            growth *= pop.life_needs_met;
            growth = std::min<Eng3D::Decimal>(std::fmod(rand(), 10.f), growth);
            //growth *= (growth > 0.f) ? nation->get_reproduction_mod() : nation->get_death_mod();
            pop.size += static_cast<Eng3D::Number>((int)growth);
            // Met life needs means less militancy
            // For example, having 1.0 life needs means that we obtain -0.01 militancy per ecotick
            // and the opposite happens with negative life needs
            pop.militancy += 0.01f * (-pop.life_needs_met) * province.owner->get_militancy_mod();
            // Current liking of the party is influenced by the life_needs_met
            pop.ideology_approval[world.get_id(*province.owner->ideology)] += (pop.life_needs_met + 1.f) / 10.f;
        }
    }

    // Total anger in population (global)
    Eng3D::Decimal total_anger = 0.f;
    // Anger per ideology (how much we hate the current ideology)
    std::vector<Eng3D::Decimal> ideology_anger(world.ideologies.size(), 0.f);
    const Eng3D::Decimal coup_chances = 1000.f;
    auto rand = Eng3D::get_local_generator();
    for(const auto& province_id : nation.controlled_provinces) {
        const auto& province = world.provinces[province_id];
        for(const auto& pop : province.pops) {
            /// @todo Ok, look, the justification is that educated people
            // almost never do coups - in comparasion to uneducated
            // peseants, rich people don't need to protest!
            const Eng3D::Decimal anger = (std::max<Eng3D::Decimal>(pop.militancy, 0.001f) / std::max<Eng3D::Decimal>(pop.literacy, 1.f) / std::max<Eng3D::Decimal>(pop.life_needs_met, 0.001f));
            total_anger += anger;
            for(const auto& ideology : world.ideologies)
                ideology_anger[world.get_id(ideology)] += (pop.ideology_approval[world.get_id(ideology)] * anger) * (pop.size / 1000);
        }
    }

    // Rebellions!
    /// @todo Broadcast this event to other people, maybe a REBEL_UPRISE action with a list of uprising provinces?
    if(!std::fmod(rand(), std::max<Eng3D::Decimal>(1, coup_chances - total_anger))) {
#if 0 /// @todo Fix so this works in parrallel
        // Compile list of uprising provinces
        std::vector<Province*> uprising_provinces;
        for(const auto& province_id : nation->owned_provinces) {
            const auto& province = world.provinces[province_id];
            Eng3D::Decimal province_anger = 0.f;
            Eng3D::Decimal province_threshold = 0.f;
            for(const auto& pop : province->pops) {
                province_anger += pop.militancy;
                province_threshold += pop.literacy * pop.life_needs_met;
            }

            if(province_anger > province_threshold)
                uprising_provinces.push_back(province);
        }

        // Nation 0 is always the rebel nation
        Nation* rebel_nation = g_world->nations[0];
        // Make the most angry provinces revolt!
        std::vector<TreatyClause::BaseClause*> clauses;
        for(auto& province : uprising_provinces) {
            /// @todo We should make a copy of the `rebel` nation for every rebellion!!!
            /// @todo We should also give them an unique ideology!!!
            rebel_nation->give_province(*province);
            for(auto& unit : province->get_units()) {
                unit->owner = rebel_nation;
            }

            // Declare war seeking all provinces from the owner
            TreatyClause::AnnexProvince* cl = new TreatyClause::AnnexProvince();
            cl->provinces = uprising_provinces;
            cl->sender = rebel_nation;
            cl->receiver = nation;
            cl->type = TreatyClauseType::ANNEX_PROVINCES;
            clauses.push_back(cl);
            Eng3D::Log::debug("game", "Revolt on " + province->ref_name + " by " + rebel_nation->ideology->ref_name);
        }
        rebel_nation->declare_war(*nation, clauses);
#endif
    }

    // Roll a dice! (more probability with more anger!)
    if(!std::fmod(rand(), std::max(coup_chances, coup_chances - total_anger))) {
        // Choose the ideology with most "anger" (the one more probable to coup d'
        // etat) - amgry radicals will surely throw off the current administration
        // while peaceful people wonq't
        const int idx = std::distance(ideology_anger.begin(), std::max_element(ideology_anger.begin(), ideology_anger.end()));
        // Ideology_anger and ideologies are mapped 1:1 - so we just pick up the associated ideology
        // Apply the policies of the ideology
        nation.current_policy = world.ideologies[idx].policies;
        // Switch ideologies of nation
        nation.ideology = &world.ideologies[idx];
        // People who are aligned to the ideology are VERY happy now
        for(const auto& province_id : nation.owned_provinces) {
            auto& province = world.provinces[province_id];
            for(auto& pop : province.pops)
                pop.militancy = -50.f;
        }
        Eng3D::Log::debug("game", "Coup d' etat on " + nation.ref_name + " by " + nation.ideology->ref_name);
    }
}

constexpr int32_t price_update_delay = 2;
constexpr float price_change_rate = 0.025f;
constexpr float base_price = 1.f;
constexpr float purchasing_change_rate = 1.f;
void economy_single_good_tick(World& world, Good& good) {
    auto good_id = world.get_id(good);
    auto province_size = world.provinces.size();
    // determine new prices
    for(uint32_t i = 0; i < province_size; i++) {
        auto province = world.provinces[i];
        auto& product = province.products[good_id];
        auto demand = product.demand;
        auto supply = product.supply;
        auto demand_in_state = std::max<double>(demand, 0.001f);
        auto current_price = product.price;
        float final_dot_product = current_price * demand_in_state / (supply + 0.0001f);
        auto new_price = std::clamp<float>(final_dot_product, 0.01f, base_price * 10.0f);
        auto state_price_delta = ((current_price * (1.0f - price_change_rate) + new_price * price_change_rate) - current_price) / float(price_update_delay);
        product.price += state_price_delta;
    }
}

constexpr float production_scaling_speed_factor = 0.5f;
constexpr float scale_speed(float v) {
    return 1.f - production_scaling_speed_factor + production_scaling_speed_factor * v;
}

// Updates supply, demand, and set wages for workers
void update_factory_production(World& world, Building& building, BuildingType* building_type, Province& province, float& pop_payment)
{
    if(building_type->output == nullptr) return;

    // TODO add output modifier
    // Calculate outputs
    auto output = building_type->output;
    Product& output_product = province.products[world.get_id(*output)];
    auto output_price = output_product.price;
    auto output_amount = 1.f * building.production_scale;

    // TODO set min wages
    float min_wage = 0;
    min_wage = 1.f;

    // TODO set output depending on amount of workers
    float total_worker_pop = building.workers;
    min_wage = std::max(min_wage, 0.0001f);

    // TODO add input modifier
    float inputs_cost = 0.f;
    for(uint32_t i = 0; i < building_type->req_goods.size(); i++) {
        auto input = building_type->req_goods[i];
        auto good = input.first;
        auto amount = input.second;
        Product& product = province.products[world.get_id(*good)];
        auto price = product.price;
        inputs_cost += price * amount;
    }
    inputs_cost *= building.production_scale;
    auto output_value = output_amount * output_price;
    auto profit = output_value - min_wage - inputs_cost;

    output_product.supply += output_amount;

    for(uint32_t i = 0; i < building_type->req_goods.size(); i++) {
        auto input = building_type->req_goods[i];
        auto good = input.first;
        auto amount = input.second;
        Product& product = province.products[world.get_id(*good)];
        product.demand += amount; // * price ?
    }

    if(profit <= 0) {
        if(output_value - inputs_cost > 0)
            pop_payment += (output_value - inputs_cost);
    } else {
        pop_payment += min_wage + profit * 0.2f * building.workers;
        // TODO pay profit to owner
    }

    // Rescale production
    // This is used to set how much the of the maximum capicity the factory produce
    building.production_scale = std::clamp<float>(building.production_scale * scale_speed((float)output_value / (min_wage + inputs_cost)), 0.05f, 1.f);
}

// Update the factory employment
void update_factories_employment(const World& world, Province& province, std::vector<float>& new_workers) {
    float unallocated_workers;
    for(uint32_t i = 0; i < province.pops.size(); i++) {
        auto& pop = province.pops[i];
        if(pop.type->group == PopGroup::LABORER)
            unallocated_workers += pop.size;
    }
    // TODO set pop size to float ?

    // Sort factories by production scale, which is suppose to represent how profitable the factory is
    // Might be better to calculate how profitable it really is and use that instead
    std::vector<std::pair<uint32_t, float>> factories_by_profitability(province.buildings.size());
    for(uint32_t i = 0; i < province.buildings.size(); i++)
        factories_by_profitability[i] = std::pair<uint32_t, float>(i, province.buildings[i].production_scale);
    std::sort(std::begin(factories_by_profitability), std::end(factories_by_profitability), [](std::pair<uint32_t, float> const& a, std::pair<uint32_t, float> const& b) { return a.second > b.second; });

    // The most profitable factory gets to pick workers first
    for(uint32_t i = 0; i < province.buildings.size(); i++) {
        auto factory_index = factories_by_profitability[i].first;
        auto& building = province.buildings[factory_index];
        const auto& type = world.building_types[factory_index];
        float factory_workers = building.level * type.num_req_workers * building.production_scale;
        float amount_needed = factory_workers;
        float allocated_workers = std::min(amount_needed, unallocated_workers);

        // Average with how much the factory had before
        // Makes is more stable so everyone don't change workplace immediately
        new_workers[factory_index] = (allocated_workers) / 16.0f + (building.workers * 15.0f) / 16.0f;
        unallocated_workers -= building.workers;
    }
}

void update_pop_needs(World& world, Province& province, std::vector<PopNeed>& pop_needs) {
    pop_needs.assign(province.pops.size(), PopNeed{});
    for(size_t i = 0; i < province.pops.size(); i++) {
        PopNeed& pop_need = pop_needs[i];
        Pop& pop = province.pops[i];
        const PopType& type = *pop.type;

        pop_need.life_needs_met = 0.f;
        // Do basic needs
        {
            float total_price = 0;
            for(size_t j = 0; j < world.goods.size(); j++) {
                auto price = province.products[j].price;
                total_price += type.basic_needs_amount[j] * price;
            }
            float buying_factor = std::min(1.f, (float)pop_need.budget / total_price);
            for(size_t j = 0; j < world.goods.size(); j++) {
                Product& product = province.products[j];
                product.demand += type.basic_needs_amount[j] * buying_factor;
            }
            pop_need.life_needs_met = buying_factor;
            pop_need.budget = std::max(0.f, (float)pop_need.budget - total_price);
        }

        pop_need.everyday_needs_met = 0.f;
        // Do luxury needs
        // TODO proper calulcation with pops trying to optimize satifcation
        {
            float total_price = 0;
            for(size_t j = 0; j < world.goods.size(); j++) {
                auto price = province.products[j].price;
                total_price += type.luxury_needs_satisfaction[j] * price;
            }
            float buying_factor = std::min(1.f, (float)pop_need.budget / total_price);
            for(size_t j = 0; j < world.goods.size(); j++) {
                Product& product = province.products[j];
                product.demand += type.basic_needs_amount[j] * buying_factor;
            }
            pop_need.everyday_needs_met = buying_factor;
            pop_need.budget = std::max(0.f, (float)pop_need.budget - total_price);
        }
    }
}

// Buildings who have fullfilled requirements to build stuff will spawn a unit
static inline Unit* build_unit(Building& building, Province& province) {
    bool can_build_unit = building.can_build_unit();

    // Ratio of health:person is 25, thus making units very expensive
    const Eng3D::Number army_size = 100;
    /// @todo Consume special soldier pops instead of farmers!!!
    auto it = std::find_if(province.pops.begin(), province.pops.end(), [building, army_size](const auto& e) {
        return (e.size >= army_size && e.type->group == PopGroup::FARMER);
    });
    if(it == province.pops.end())
        can_build_unit = false;

    /// @todo Maybe delete if size becomes 0?
    const Eng3D::Number final_size = std::min<Eng3D::Number>((*it).size, army_size);
    (*it).size -= final_size;
    if(can_build_unit && final_size) {
        // Spawn a unit
        Unit* unit = new Unit();
        unit->set_province(province);
        unit->type = building.working_unit_type;
        unit->owner = province.owner;
        unit->budget = 5000.f;
        unit->experience = 1.f;
        unit->morale = 1.f;
        unit->supply = 1.f;
        unit->size = final_size;
        unit->base = unit->type->max_health;

        building.working_unit_type = nullptr;
        Eng3D::Log::debug("economy", "[" + province.ref_name + "]: Has built an unit of [" + unit->type->ref_name + "]");
        return unit;
    } else {
        return nullptr;
    }
}

void Economy::do_tick(World& world) {
    world.profiler.start("E-init");
    std::vector<Market> markets(world.goods.size());

    const size_t provinces_size = world.provinces.size();
    const size_t goods_size = world.goods.size();

    for(Good::Id good_id = 0; good_id < goods_size; good_id++) {
        Market& market = markets[good_id];
        market.good = good_id;
    }
    std::for_each(std::execution::par, markets.begin(), markets.end(), [&world, provinces_size](auto& market) {
        market.prices.reserve(provinces_size);
        market.supply.reserve(provinces_size);
        market.demand.reserve(provinces_size);

        for(size_t i = 0; i < world.provinces.size(); i++) {
            Province& province = world.provinces[i];
            Product& product = province.products[market.good];
            market.demand[i] = 0.f;
            if(province.owner) {
                market.prices[i] = product.price;
                market.supply[i] = product.supply;
            } else {
                market.prices[i] = product.price;
                market.supply[i] = product.supply;
            }

        }
    });
    world.profiler.stop("E-init");

    world.profiler.start("E-trade");
    std::for_each(std::execution::par, markets.begin(), markets.end(), [&world, provinces_size](auto& market) {
        for(size_t i = 0; i < provinces_size; i++) {
            Province& province = world.provinces[i];
            const size_t province_neighbours_size = province.neighbours.size();
            Product& product = province.products[market.good];
            if(product.supply <= 0.f) continue;
            for(auto neighbour : province.neighbours) {
                if(neighbour->owner) continue;
                const size_t neighbour_neighbours_size = neighbour->neighbours.size();
                Product& other_product = neighbour->products[market.good];

                // transfer goods
                if(other_product.price > product.price) {
                    float amount = product.supply / province_neighbours_size;
                    market.supply[i] -= amount;
                    market.demand[i] += amount;
                } else {
                    float other_amount = other_product.supply / neighbour_neighbours_size;
                    market.supply[i] += other_amount;
                }
            }
        }
    });
    world.profiler.stop("E-trade");

    int coastal = 0;
    for(size_t i = 0; i < world.provinces.size(); i++) {
        Province& province = world.provinces[i];
        for(auto neighbour : province.neighbours) {
            if(neighbour->terrain_type->is_water_body) {
                coastal++;
                break;
            }
        }
    }

    world.profiler.start("E-good");
    std::for_each(std::execution::par, markets.begin(), markets.end(), [&world](const auto& market) {
        economy_single_good_tick(world, world.goods[market.good]);
    });
    world.profiler.stop("E-good");

    world.profiler.start("E-big");
    std::vector<Province::Id> province_ids(provinces_size);
    Province::Id last_id = 0;
    for(Province::Id id = 0; id < provinces_size; id++) {
        if(world.provinces[id].owner == nullptr) continue;
        province_ids[last_id++] = id;
    }
    province_ids.resize(last_id);

    tbb::combinable<tbb::concurrent_vector<Unit*>> province_new_units;
    std::vector<std::vector<float>> buildings_new_worker(provinces_size);
    std::vector<std::vector<PopNeed>> pops_new_needs(provinces_size);
    tbb::parallel_for(tbb::blocked_range(province_ids.begin(), province_ids.end()), [&world, &buildings_new_worker, &province_new_units, &pops_new_needs, provinces_size](const auto& province_ids_ranges) {
        for(const auto& province_id : province_ids_ranges) {
            Province& province = world.provinces[province_id];
            float laborers_payment = 0.f;
            for(auto& building_type : world.building_types) {
                auto& building = province.buildings[world.get_id(building_type)];
                update_factory_production(world, building, &building_type, province, laborers_payment);
            }
            std::vector<PopNeed>& new_needs = pops_new_needs[province_id];
            new_needs.assign(province.pops.size(), PopNeed());

            const size_t pops_size = province.pops.size();
            for(uint32_t i = 0; i < pops_size; i++) {
                auto& pop = province.pops[i];
                new_needs[i].budget = pop.budget;
            }
            float laborers_amount = 0.f;
            for(uint32_t i = 0; i < pops_size; i++) {
                auto& pop = province.pops[i];
                if(pop.type->group == PopGroup::LABORER)
                    laborers_amount += pop.size;
            }
            for(uint32_t i = 0; i < pops_size; i++) {
                auto& pop = province.pops[i];
                if(pop.type->group == PopGroup::LABORER) {
                    float ratio = pop.size / laborers_amount;
                    new_needs[i].budget += laborers_payment * ratio;
                }
            }

            std::vector<float>& new_workers = buildings_new_worker[province_id];
            new_workers.assign(world.building_types.size(), 0.f);
            update_factories_employment(world, province, new_workers);
            update_pop_needs(world, province, new_needs);
            for(size_t i = 0; i < province.buildings.size(); i++) {
                Building& building = province.buildings[i];
                if(building.working_unit_type != nullptr) {
                    Unit* unit = build_unit(building, province);
                    if(unit != nullptr)
                        province_new_units.local().push_back(unit);
                }
            }
        }
    });
    world.profiler.stop("E-big");

    world.profiler.start("E-mutex");
    // Collect list of nations that exist
    std::vector<Nation*> eval_nations;
    for(auto& nation : world.nations) {
        if(nation.exists())
            eval_nations.push_back(&nation);
    }

    // -------------------------- MUTEX PROTECTED WORLD CHANGES BELOW -------------------------------
    std::scoped_lock lock(world.world_mutex);

    std::for_each(std::execution::par, province_ids.begin(), province_ids.end(), [&world, &pops_new_needs, &buildings_new_worker](const auto& province_id) {
        Province& province = world.provinces[province_id];
        std::vector<PopNeed>& new_needs = pops_new_needs[province_id];
        for(uint32_t i = 0; i < province.pops.size(); i++) {
            auto& pop = province.pops[i];
            pop.budget = new_needs[i].budget;
            pop.life_needs_met = new_needs[i].life_needs_met;
            pop.everyday_needs_met = new_needs[i].everyday_needs_met;
        }
        std::vector<float>& new_workers = buildings_new_worker[province_id];
        for(uint32_t i = 0; i < province.buildings.size(); i++) {
            auto& building = province.buildings[i];
            building.workers = new_workers[i];
        }
    });

    std::for_each(std::execution::par, eval_nations.begin(), eval_nations.end(), [&world](const auto& nation) {
        std::vector<Unit*> new_nation_units;
        // Do research on focused research
        if(nation->focus_tech != nullptr) {
            const Eng3D::Decimal research = nation->get_research_points() / nation->focus_tech->cost;
            nation->research[world.get_id(*nation->focus_tech)] += research;
        }
        militancy_update(world, *nation);
    });

    // Lock for world is already acquired since the economy runs inside the world's do_tick which
    // should be lock guarded by the callee
    province_new_units.combine_each([&world](const auto& unit_list) {
        for(const auto& unit : unit_list) {
            // Now commit the transaction of the new units into the main world area
            world.insert(*unit);
            g_server->broadcast(Action::UnitAdd::form_packet(*unit));
        }
    });
    world.profiler.stop("E-mutex");
    // do_emigration(world);
}
