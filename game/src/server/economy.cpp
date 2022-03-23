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

#include "unified_render/log.hpp"
#include "unified_render/print.hpp"
#include "unified_render/serializer.hpp"
#include "unified_render/thread_pool.hpp"
#include "unified_render/rand.hpp"

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

void militancy_update(World& world, Nation* nation) {
    // Total anger in population (global)
    UnifiedRender::Decimal total_anger = 0.f;
    // Anger per ideology (how much we hate the current ideology)
    std::vector<UnifiedRender::Decimal> ideology_anger(world.ideologies.size(), 0.f);
    const UnifiedRender::Decimal coup_chances = 1000.f;
    auto rand = UnifiedRender::get_local_generator();
    for(const auto& province : nation->controlled_provinces) {
        for(const auto& pop : province->pops) {
            // TODO: Ok, look, the justification is that educated people
            // almost never do coups - in comparasion to uneducated
            // peseants, rich people don't need to protest!
            const UnifiedRender::Decimal anger = (std::max<UnifiedRender::Decimal>(pop.militancy * pop.con, 0.001f) / std::max<UnifiedRender::Decimal>(pop.literacy, 1.f) / std::max<UnifiedRender::Decimal>(pop.life_needs_met, 0.001f));
            total_anger += anger;
            for(const auto& ideology : world.ideologies) {
                ideology_anger[world.get_id(*ideology)] += (pop.ideology_approval[world.get_id(*ideology)] * anger) * (pop.size / 1000);
            }
        }
    }

    // Rebellions!
    // TODO: Broadcast this event to other people, maybe a REBEL_UPRISE action with a list of uprising provinces?
#if 0 // TODO: Fix so this works in parrallel
    if(!std::fmod(rand(), std::max<UnifiedRender::Decimal>(1, coup_chances - total_anger))) {
        // Compile list of uprising provinces
        std::vector<Province*> uprising_provinces;
        for(const auto& province : nation->owned_provinces) {
            UnifiedRender::Decimal province_anger = 0.f;
            UnifiedRender::Decimal province_threshold = 0.f;
            for(const auto& pop : province->pops) {
                province_anger += pop.militancy * pop.con;
                province_threshold += pop.literacy * pop.life_needs_met;
            }

            if(province_anger > province_threshold) {
                uprising_provinces.push_back(province);
            }
        }

        Nation* dup_nation = new Nation();
        dup_nation->name = nation->ref_name;
        dup_nation->ref_name = nation->ref_name;
        dup_nation->accepted_cultures = nation->accepted_cultures;
        dup_nation->accepted_religions = nation->accepted_religions;
        dup_nation->research = nation->research;
        dup_nation->relations = nation->relations;
        dup_nation->client_hints = nation->client_hints;
        // Rebel with the most popular ideology
        dup_nation->ideology = world.ideologies[std::distance(ideology_anger.begin(), std::max_element(ideology_anger.begin(), ideology_anger.end()))];
        world.insert(*dup_nation);
        for(auto& _nation : world.nations) {
            _nation->relations.resize(world.nations.size(), NationRelation{ 0.f, false, false, false, false, false, false, false, false, true, false });
        }

        // TODO: Tell the clients about this new nation
        g_server->broadcast(Action::NationAdd::form_packet(*dup_nation));

        // Make the most angry provinces revolt!
        std::vector<TreatyClause::BaseClause*> clauses;
        for(auto& province : uprising_provinces) {
            // TODO: We should make a copy of the `rebel` nation for every rebellion!!!
            // TODO: We should also give them an unique ideology!!!
            dup_nation->give_province(*province);
            for(auto& unit : province->get_units()) {
                unit->owner = dup_nation;
            }

            // Declare war seeking all provinces from the owner
            TreatyClause::AnexxProvince* cl = new TreatyClause::AnexxProvince();
            cl->provinces = uprising_provinces;
            cl->sender = dup_nation;
            cl->receiver = nation;
            cl->type = TreatyClauseType::ANEXX_PROVINCES;
            clauses.push_back(cl);
            print_info("Revolt on [%s]! [%s] has taken over!", province->ref_name.c_str(), dup_nation->ideology->ref_name.c_str());
        }
        dup_nation->declare_war(*nation, clauses);
    }
#endif

    // Roll a dice! (more probability with more anger!)
    if(!std::fmod(rand(), std::max(coup_chances, coup_chances - total_anger))) {
        // Choose the ideology with most "anger" (the one more probable to coup d'
        // etat) - amgry radicals will surely throw off the current administration
        // while peaceful people wonq't
        const int idx = std::distance(ideology_anger.begin(), std::max_element(ideology_anger.begin(), ideology_anger.end()));

        // Ideology_anger and ideologies are mapped 1:1 - so we just pick up the associated ideology
        // Apply the policies of the ideology
        nation->current_policy = world.ideologies[idx]->policies;

        // Switch ideologies of nation
        nation->ideology = world.ideologies[idx];

        // People who are aligned to the ideology are VERY happy now
        for(const auto& province : nation->owned_provinces) {
            for(auto& pop : province->pops) {
                pop.militancy = -50.f;
            }
        }

        print_info("Coup d' etat on [%s]! [%s] has taken over!", nation->ref_name.c_str(), nation->ideology->ref_name.c_str());
    }
}

constexpr float production_scaling_speed_factor = 0.5f;
constexpr float scale_speed(float v) {
    return 1.0f - production_scaling_speed_factor + production_scaling_speed_factor * v;
}
// Updates supply, demand, and set wages for workers
void update_factory_production(World& world,
    Building& building,
    BuildingType* building_type,
    Province* province,
    float& pop_payment) {

    if(!building_type->is_factory) return;

    if(building_type->outputs.size() == 0) {
        return;
    }

    // Multiple outputs ? Will we have that ?
    // TODO add output modifier
    // Calculate outputs
    auto output = building_type->outputs[0];
    auto output_product = province->products[world.get_id(*output)];
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
        Product& product = province->products[world.get_id(*good)];
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
        Product& product = province->products[world.get_id(*good)];
        product.demand += amount; // * price ?
    }

    if(profit <= 0) {
        if(output_value - inputs_cost > 0) {
            pop_payment += (output_value - inputs_cost);
        }
    }
    else {
        pop_payment += min_wage + profit * 0.2f * building.workers;
        // TODO pay profit to owner
    }

    // Rescale production
    // This is used to set how much the of the maximum capicity the factory produce
    building.production_scale = std::clamp<float>(building.production_scale * scale_speed((float)output_value / (min_wage + inputs_cost)), 0.05f, 1.f);
}

// Update the factory employment
void update_factories_employment(const World& world, Province* province) {
    float unallocated_workers;
    for(uint32_t i = 0; i < province->pops.size(); i++) {
        auto& pop = province->pops[i];
        if(pop.type->group == PopGroup::LABORER) {
            unallocated_workers += pop.size;
        }
    }
    // TODO set pop size to float ?

    // Sort factories by production scale, which is suppose to represent how profitable the factory is
    // Might be better to calculate how profitable it really is and use that instead
    std::vector<std::pair<uint32_t, float>> factories_by_profitability(province->buildings.size());
    for(uint32_t i = 0; i < province->buildings.size(); i++) {
        factories_by_profitability[i] = std::pair<uint32_t, float>(i, province->buildings[i].production_scale);
    }

    std::sort(std::begin(factories_by_profitability), std::end(factories_by_profitability), [](std::pair<uint32_t, float> const& a, std::pair<uint32_t, float> const& b) { return a.second > b.second; });

    // The most profitable factory gets to pick workers first
    for(uint32_t i = 0; i < province->buildings.size(); i++) {
        auto factory_index = factories_by_profitability[i].first;
        auto building = province->buildings[factory_index];
        auto type = world.building_types[factories_by_profitability[i].first];
        float factory_workers = building.level * type->num_req_workers * building.production_scale;
        float amount_needed = factory_workers;
        float allocated_workers = std::min(amount_needed, unallocated_workers);

        // Average with how much the factory had before
        // Makes is more stable so everyone don't change workplace immediately
        building.workers = (allocated_workers) / 16.0f + (building.workers * 15.0f) / 16.0f;
        unallocated_workers -= building.workers;
    }
}

void update_pop_needs(World& world, Province& province, Pop& pop) {
    // Deduct life needs met (to force pops to eat nom nom)
    pop.life_needs_met -= 0.01f;

    // Use 10% of our budget for buying uneeded commodities and shit
    // TODO: Should lower spending with higher literacy, and higher
    // TODO: Higher the fullfilment per unit with higher literacy

    auto type = *pop.type;

    // Do basic needs
    {
        float total_price = 0;
        for(size_t i = 0; i < world.goods.size(); i++) {
            auto price = province.products[i].price;
            total_price += type.basic_needs_amount[i] * price;
        }
        float buying_factor = std::min(1.f, (float)pop.budget / total_price);
        for(size_t i = 0; i < world.goods.size(); i++) {
            auto product = province.products[i];
            product.demand += type.basic_needs_amount[i] * buying_factor;
        }
        pop.life_needs_met += buying_factor;
        pop.budget = std::max(0.f, (float)pop.budget - total_price);
    }

    // Do luxury needs
    // TODO proper calulcation with pops trying to optimize satifcation
    {
        float total_price = 0;
        for(size_t i = 0; i < world.goods.size(); i++) {
            auto price = province.products[i].price;
            total_price += type.luxury_needs_satisfaction[i] * price;
        }
        float buying_factor = std::min(1.f, (float)pop.budget / total_price);
        for(size_t i = 0; i < world.goods.size(); i++) {
            auto product = province.products[i];
            product.demand += type.basic_needs_amount[i] * buying_factor;
        }
        pop.everyday_needs_met += buying_factor;
        pop.budget = std::max(0.f, (float)pop.budget - total_price);
    }

    // x2.5 life needs met modifier, that is the max allowed
    pop.life_needs_met = std::min<UnifiedRender::Decimal>(1.5f, std::max<UnifiedRender::Decimal>(pop.life_needs_met, -5.f));
    pop.everyday_needs_met = std::min<UnifiedRender::Decimal>(1.5f, std::max<UnifiedRender::Decimal>(pop.everyday_needs_met, -5.f));

    // Current liking of the party is influenced by the life_needs_met
    pop.ideology_approval[world.get_id(*province.owner->ideology)] += (pop.life_needs_met + 1.f) / 10.f;

    // NOTE: We used to have this thing where anything below 2.5 meant everyone dies
    // and this was removed because it's such an unescesary detail that consumes precious
    // CPU branching prediction... and we can't afford that!

    // More literacy means more educated persons with less children
    UnifiedRender::Decimal growth = pop.size / (pop.literacy + 1.f);
    growth *= pop.life_needs_met;
    growth = std::min<UnifiedRender::Decimal>(std::fmod(rand(), 10.f), growth);
    //growth *= (growth > 0.f) ? nation->get_reproduction_mod() : nation->get_death_mod();
    pop.size += static_cast<UnifiedRender::Number>((int)growth);

    // Met life needs means less militancy
    // For example, having 1.0 life needs means that we obtain -0.01 militancy per ecotick
    // and the opposite happens with negative life needs
    pop.militancy += 0.01f * (-pop.life_needs_met) * province.owner->get_militancy_mod();
    pop.con += 0.01f * (-pop.life_needs_met) * province.owner->get_militancy_mod();
}

// Phase 1 of economy: Delivers & Orders are sent from all factories in the world
void Economy::do_tick(World& world) {
    // Collect list of nations that exist
    std::vector<Nation*> eval_nations;
    for(const auto& nation : world.nations) {
        if(nation->exists()) {
            eval_nations.push_back(nation);
        }
    }

    std::vector<Unit*> new_units;
    std::mutex new_units_mutex;
    std::for_each(std::execution::par, eval_nations.begin(), eval_nations.end(), [&new_units, &new_units_mutex, &world](const auto& nation) {
        std::vector<Unit*> new_nation_units;

        // Minimal speedup but probably can keep our branch predictor happy ^_^
        auto rand = UnifiedRender::get_local_generator();
        for(const auto& province : nation->controlled_provinces) {
            float laborers_payment = 0.f;
            for(const auto& building_type : world.building_types) {
                auto& building = province->buildings[world.get_id(*building_type)];
                building.production_cost = 0.f;

                // Building not built does not exist
                if(!building.level) {
                    continue;
                }

                // Can't operate building without funds
                if(building.budget <= 0.f) {
#if 0
                    print_info("Building of [%s] in [%s] is closed due to lack of funds!", building_type->ref_name.c_str(), province->ref_name.c_str());
#endif
                    continue;
                }
                update_factory_production(world, building, building_type, province, laborers_payment);

                // Buildings who have fullfilled requirements to build stuff will spawn a unit
                if(building.working_unit_type != nullptr) {
                    bool can_build_unit = building.can_build_unit();

                    // Ratio of health:person is 25, thus making units very expensive
                    const UnifiedRender::Number army_size = building.working_unit_type->max_health + 100 * 25;
                    // TODO: Consume special soldier pops instead of farmers!!!
                    auto it = std::find_if(province->pops.begin(), province->pops.end(), [building, army_size](const auto& e) {
                        return (e.size >= army_size && e.type->group == PopGroup::FARMER);
                    });

                    if(it == province->pops.end()) {
                        can_build_unit = false;
                    }

                    can_build_unit = true;
                    if(can_build_unit) {
                        // TODO: Maybe delete if size becomes 0?
                        //UnifiedRender::Number final_size = std::min<UnifiedRender::Number>((*it).size, army_size);
                        //(*it).size -= final_size;
                        UnifiedRender::Number final_size = 100;

                        // Spawn a unit
                        Unit* unit = new Unit();
                        unit->set_province(*province);
                        unit->type = building.working_unit_type;
                        unit->owner = province->owner;
                        unit->budget = 5000.f;
                        unit->experience = 1.f;
                        unit->morale = 1.f;
                        unit->supply = 1.f;
                        unit->size = final_size;
                        unit->base = unit->type->max_health;
                        new_nation_units.push_back(unit);
                        building.working_unit_type = nullptr;
                        UnifiedRender::Log::debug("economy", "[" + province->ref_name + "]: Has built an unit of [" + unit->type->ref_name + "]");
                    }
                }

#if 0
                if(1) {
                    UnifiedRender::Log::debug("economy", "[%s]: Workers working on building of type [%s]", province->ref_name.c_str(), building_type->ref_name.c_str());
                    UnifiedRender::Log::debug("economy", "- %f farmers (%f needed)", available_farmers, needed_farmers);
                    UnifiedRender::Log::debug("economy", "- %f laborers (%f needed)", available_laborers, needed_laborers);
                    UnifiedRender::Log::debug("economy", "- %f entrepreneurs (%f needed)", available_entrepreneurs, needed_entrepreneurs);
                }
#endif
            }
            float amount_laborers;
            for(uint32_t i = 0; i < province->pops.size(); i++) {
                auto& pop = province->pops[i];
                if(pop.type->group == PopGroup::LABORER) {
                    amount_laborers += pop.size;
                }
            }
            for(uint32_t i = 0; i < province->pops.size(); i++) {
                auto& pop = province->pops[i];
                if(pop.type->group == PopGroup::LABORER) {
                    float ratio = pop.size / amount_laborers;
                    pop.budget += laborers_payment * ratio;
                }
            }

            update_factories_employment(world, province);

            // POPs now will proceed to buy products produced from factories & buying from the stockpile
            // of this province with the local market price
            for(size_t i = 0; i < province->pops.size(); i++) {
                Pop& pop = province->pops[i];

                update_pop_needs(world, *province, pop);
            }
        }

        // Do research on focused research
        if(nation->focus_tech != nullptr) {
            const UnifiedRender::Decimal research = nation->get_research_points() / nation->focus_tech->cost;
            nation->research[world.get_id(*nation->focus_tech)] += research;
        }

        militancy_update(world, nation);

        if(!new_nation_units.empty()) {
            // Add to new_units list
            std::scoped_lock lock(new_units_mutex);
            for(const auto& unit : new_nation_units) {
                new_units.push_back(unit);
            }
        }
    });

    if(!new_units.empty()) {
        // Lock for world is already acquired since the economy runs inside the world's do_tick which
        // should be lock guarded by the callee
        for(const auto& unit : new_units) {
            if(world.units.size() >= 10000) {
                continue;
            }

            // Now commit the transaction of the new units into the main world area
            world.insert(*unit);
            g_server->broadcast(Action::UnitAdd::form_packet(*unit));
        }
    }

    do_emigration(world);
}
