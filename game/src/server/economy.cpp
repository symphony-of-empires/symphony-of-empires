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

#include "unified_render/print.hpp"
#include "unified_render/serializer.hpp"
#include "unified_render/thread_pool.hpp"

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

// Structure that represents a person emigrating from a province to another

class Workers {
public:
    Pop& pop;
    size_t amount;
    Workers(Pop& _pop): pop{ _pop }, amount{ _pop.size } {};

    Workers& operator=(const Workers& workers) {
        pop = workers.pop;
        amount = workers.amount;
        return *this;
    }
};

class AvailableWorkers {
public:
    std::vector<Workers> entrepreneurs{};
    std::vector<Workers> farmers{};
    std::vector<Workers> laborers{};
};

// Phase 1 of economy: Delivers & Orders are sent from all factories in the world
void Economy::do_tick(World& world) {
    std::for_each(std::execution::par, world.nations.begin(), world.nations.end(), [&world](const auto& nation) {
        for(const auto& province : nation->owned_provinces) {
            AvailableWorkers province_workers{};
;            // POPs now will proceed to buy products produced from factories & buying from the stockpile
            // of this province with the local market price
            for(size_t i = 0; i < province->pops.size(); i++) {
                Pop& pop = province->pops[i];
                if(!pop.size) {
                    province->pops.erase(province->pops.begin() + i);
                    i--;
                    continue;
                }

                // Also add this POP to the AvailableWorkers array
                Workers workers{ pop };
                switch(pop.type->group) {
                case PopGroup::BURGEOISE:
                    province_workers.entrepreneurs.push_back(workers);
                    break;
                case PopGroup::FARMER:
                    province_workers.farmers.push_back(workers);
                    break;
                case PopGroup::LABORER:
                    province_workers.laborers.push_back(workers);
                    break;
                }

                // Deduct life needs met (to force pops to eat nom nom)
                pop.life_needs_met -= 0.01f;

                // Use 10% of our budget for buying uneeded commodities and shit
                // TODO: Should lower spending with higher literacy, and higher
                // TODO: Higher the fullfilment per unit with higher literacy
                // TODO: DO NOT MAKE POP BUY FROM STOCKPILE, INSTEAD MAKE THEM BUY FROM ORDERS
                for(size_t j = 0; j < province->products.size(); j++) {
                    // Only buy the available stuff
                    UnifiedRender::Number bought = std::min<UnifiedRender::Number>(std::rand() % pop.size, province->products[j].supply);

                    // Deduct from pop's budget the product * how many we bought
                    // NOTE: If we didn't bought anything it will simply invalidate this, plus if the supply is nil
                    // this also gets nullified
                    province->products[j].supply -= bought;
                    province->products[j].demand += bought;
                    pop.budget -= bought * province->products[j].price;
                    if(bought) {
                        if(world.goods[j]->is_edible) {
                            pop.life_needs_met += pop.size / bought;
                        }
                        else {
                            pop.everyday_needs_met += pop.size / bought;
                        }
                    }

                    if(pop.budget < 0.f) {
                        // TODO: Get a loan
                        // TODO: Get the life savings
                        break;
                    }
                }

                // x2.5 life needs met modifier, that is the max allowed
                pop.life_needs_met = std::min<UnifiedRender::Decimal>(1.5f, std::max<UnifiedRender::Decimal>(pop.life_needs_met, -5.f));
                pop.everyday_needs_met = std::min<UnifiedRender::Decimal>(1.5f, std::max<UnifiedRender::Decimal>(pop.everyday_needs_met, -5.f));

                // Current liking of the party is influenced by the life_needs_met
                pop.ideology_approval[world.get_id(province->controller->ideology)] += (pop.life_needs_met + 1.f) / 10.f;

                // NOTE: We used to have this thing where anything below 2.5 meant everyone dies
                // and this was removed because it's such an unescesary detail that consumes precious
                // CPU branching prediction... and we can't afford that!

                // Starvation in -1 or 0 or >1 are amortized by literacy
                UnifiedRender::Decimal growth = pop.life_needs_met / pop.literacy;
                //if(growth < 0 && (size_t)std::abs(growth) > pop.size) {
                //    growth = -pop.size;
                //}
                growth *= (growth > 0) ? province->controller->get_reproduction_mod() : province->controller->get_death_mod();
                pop.size += growth;

                // Add some RNG to shake things up and make gameplay more dynamic and less deterministic :)
                pop.size += std::fmod(std::rand(), std::min<UnifiedRender::Number>(5.f, std::max<UnifiedRender::Number>(1.f, pop.size / 10000)));
                pop.size = std::fabs(pop.size);

                // Met life needs means less militancy
                // For example, having 1.0 life needs means that we obtain -0.01 militancy per ecotick
                // and the opposite happens with negative life needs
                pop.militancy += 0.01f * (-pop.life_needs_met) * province->controller->get_militancy_mod();;
                pop.con += 0.01f * (-pop.life_needs_met) * province->controller->get_militancy_mod();;
            }

            for(size_t j = 0; j < world.building_types.size(); j++) {
                BuildingType* building_type = world.building_types[j];
                auto& building = province->buildings[j];
                building.production_cost = 0.f;

                // Can't operate building without funds
                if(building.budget < 0.f) {
                    print_info("Building of [%s] in [%s] is closed due to lack of funds!", building_type->ref_name.c_str(), province->ref_name.c_str());
                    continue;
                }

                UnifiedRender::Number needed_laborers = 0, available_laborers = 0;
                UnifiedRender::Number needed_farmers = 0, available_farmers = 0;
                UnifiedRender::Number needed_entrepreneurs = 0, available_entrepreneurs = 0;
                // Each output adds a required farmer or laborer depending on the type
                // of output, it also requires entrepreneurs to "manage" the operations
                // of the factory
                for(const auto& output : world.building_types[j]->outputs) {
                    const UnifiedRender::Number employed = 500;
                    if(output->is_edible) {
                        needed_farmers += employed;
                    } else {
                        needed_laborers += employed;
                    }
                    needed_entrepreneurs += employed / 100;
                }

                // And the inputs also employ people
                for(const auto& input : world.building_types[j]->inputs) {
                    const UnifiedRender::Number employed = 500;
                    if(input->is_edible) {
                        needed_farmers += employed;
                    } else {
                        needed_laborers += employed;
                    }
                    needed_entrepreneurs += employed / 100;
                }

                // Search through all the job requests
                // Industries require 2 (or 3) types of POPs to correctly function
                // - Laborers: They are needed to produce non-edible food
                // - Farmers: They are needed to produce edibles
                // - Burgeoise: They help "organize" the factory
                for(size_t i = 0; i < province_workers.farmers.size(); i++) {
                    if(available_farmers >= needed_farmers) {
                        break;
                    }

                    Workers& workers = province_workers.farmers[i];
                    const UnifiedRender::Number employed = std::min<UnifiedRender::Number>(needed_farmers - available_farmers, workers.amount);
                    available_farmers += employed;
                    // Give pay to the POP
                    const UnifiedRender::Decimal payment = employed * province->controller->current_policy.min_wage;
                    workers.pop.budget += payment * province->controller->get_salary_paid_mod();
                    building.budget -= payment;
                    workers.amount -= employed;
                    // Delete job request when it has 0 amount
                    if(!workers.amount) {
                        province_workers.farmers.erase(province_workers.farmers.begin() + i);
                        --i;
                        continue;
                    }
                }

                for(size_t i = 0; i < province_workers.laborers.size(); i++) {
                    if(available_laborers >= needed_laborers) {
                        break;
                    }

                    Workers& workers = province_workers.laborers[i];
                    const UnifiedRender::Number employed = std::min<UnifiedRender::Number>(needed_laborers - available_laborers, workers.amount);
                    available_laborers += employed;
                    // Give pay to the POP
                    const UnifiedRender::Decimal payment = employed * province->controller->current_policy.min_wage;
                    workers.pop.budget += payment * province->controller->get_salary_paid_mod();
                    building.budget -= payment;
                    workers.amount -= employed;
                    // Delete job request when it has 0 amount
                    if(!workers.amount) {
                        province_workers.laborers.erase(province_workers.laborers.begin() + i);
                        --i;
                        continue;
                    }
                }

                for(size_t i = 0; i < province_workers.entrepreneurs.size(); i++) {
                    if(available_entrepreneurs >= needed_entrepreneurs) {
                        break;
                    }

                    Workers& workers = province_workers.entrepreneurs[i];
                    const UnifiedRender::Number employed = std::min<UnifiedRender::Number>(needed_entrepreneurs - available_entrepreneurs, workers.amount);
                    available_entrepreneurs += employed;
                    // Give pay to the POP
                    const UnifiedRender::Decimal payment = employed * province->controller->current_policy.min_wage;
                    workers.pop.budget += payment * province->controller->get_salary_paid_mod();
                    building.budget -= payment;
                    workers.amount -= employed;
                    // Delete job request when it has 0 amount
                    if(!workers.amount) {
                        province_workers.entrepreneurs.erase(province_workers.entrepreneurs.begin() + i);
                        --i;
                        continue;
                    }
                }

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
                    
                    if(can_build_unit) {
                        // TODO: Maybe delete if size becomes 0?
                        (*it).size -= army_size;

                        // Spawn a unit
                        Unit* unit = new Unit();
                        unit->set_province(*province);
                        unit->type = building.working_unit_type;
                        unit->owner = province->controller;
                        unit->budget = 5000.f;
                        unit->experience = 1.f;
                        unit->morale = 1.f;
                        unit->supply = 1.f;
                        unit->size = unit->type->max_health;
                        unit->base = unit->size;

                        world.world_mutex.lock();
                        // Notify all clients of the server about this new unit
                        building.working_unit_type = nullptr;
                        world.insert(unit);
                        g_server->broadcast(Action::UnitAdd::form_packet(*unit));
                        world.world_mutex.unlock();
                        print_info("[%s]: Has built an unit of [%s]", province->ref_name.c_str(), unit->type->ref_name.c_str());
                    }
                }

                if(building_type->is_factory) {
                    print_info("[%s]: Workers working on building of type [%s]", province->ref_name.c_str(), building_type->ref_name.c_str());
                    print_info("- %f farmers (%f needed)", available_farmers, needed_farmers);
                    print_info("- %f laborers (%f needed)", available_laborers, needed_laborers);
                    print_info("- %f entrepreneurs (%f needed)", available_entrepreneurs, needed_entrepreneurs);

                    // Consume inputs needed to produce stuff (will decrease supplies and increase demand)
                    size_t k = 0;
                    for(const auto& input : world.building_types[j]->inputs) {
                        Product& product = province->products[world.get_id(input)];
                        // Farmers can only work with edibles and laborers can only work for edibles
                        UnifiedRender::Number quantity = 0;
                        if(input->is_edible) {
                            quantity = (available_farmers / needed_farmers) * 5000;
                        } else {
                            quantity = (available_laborers / needed_laborers) * 5000;
                        }
                        quantity = std::min<UnifiedRender::Number>(std::min<UnifiedRender::Number>(quantity, building.stockpile[k]), product.supply);
                        //quantity *= province->controller->get_industry_input_mod();
                        product.supply -= quantity;
                        product.demand += quantity;
                        building.stockpile[k] -= quantity;
                        k++;
                    }
                }

                // Building the building itself
                for(const auto& good : building.req_goods) {
                    UnifiedRender::Number quantity = good.second * province->controller->get_industry_input_mod();
                }

                // TODO: We should deduct and set willing payment from military spendings
                // Building an unit
                for(const auto& good : building.req_goods_for_unit) {
                    UnifiedRender::Number quantity = good.second * province->controller->get_industry_input_mod();
                }

                // Produce products (incrementing supply)
                if(building.can_do_output()) {
                    print_info("Can do output!!!");
                    for(const auto& output : world.building_types[j]->outputs) {
                        Product& product = province->products[world.get_id(output)];
                        // Farmers can only work with edibles and laborers can only work for edibles
                        UnifiedRender::Number quantity = 0;
                        if(output->is_edible) {
                            quantity = (available_farmers / needed_farmers) * 5000;
                        } else {
                            quantity = (available_laborers / needed_laborers) * 5000;
                        }
                        quantity = std::min<UnifiedRender::Number>(quantity, product.supply);
                        //quantity *= province->controller->get_industry_input_mod();
                        product.supply += quantity;
                    }
                    continue;
                }
            }

            // Close the local market of this province
            for(auto& product : province->products) {
                product.close_market();
                product.demand = 0;
            }
        }

        // Do research on focused research
        if(nation->focus_tech != nullptr) {
            const UnifiedRender::Decimal research = nation->get_research_points() / nation->focus_tech->cost;
            nation->research[world.get_id(nation->focus_tech)] += research;
        }

        // Total anger in population (global)
        UnifiedRender::Decimal total_anger = 0.f;
        // Anger per ideology (how much we hate the current ideology)
        std::vector<UnifiedRender::Decimal> ideology_anger(world.ideologies.size(), 0.f);
        const UnifiedRender::Decimal coup_chances = 1000.f;
        for(const auto& province : nation->owned_provinces) {
            for(const auto& pop : province->pops) {
                // TODO: Ok, look, the justification is that educated people
                // almost never do coups - in comparasion to uneducated
                // peseants, rich people don't need to protest!
                const UnifiedRender::Decimal anger = (std::max<UnifiedRender::Decimal>(pop.militancy * pop.con, 0.001f) / std::max<UnifiedRender::Decimal>(pop.literacy, 1.f) / std::max<UnifiedRender::Decimal>(pop.life_needs_met, 0.001f));
                total_anger += anger;
                for(const auto& ideology : world.ideologies) {
                    ideology_anger[world.get_id(ideology)] += (pop.ideology_approval[world.get_id(ideology)] * anger) * (pop.size / 1000);
                }
            }
        }

        // Rebellions!
        // TODO: Broadcast this event to other people, maybe a REBEL_UPRISE action with a list of uprising provinces?
        if(!std::fmod(std::rand(), std::max<UnifiedRender::Decimal>(1, coup_chances - total_anger))) {
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
            dup_nation->base_literacy = nation->base_literacy;
            dup_nation->research = nation->research;
            dup_nation->relations = nation->relations;
            dup_nation->client_hints = nation->client_hints;
            // Rebel with the most popular ideology
            dup_nation->ideology = world.ideologies[std::distance(ideology_anger.begin(), std::max_element(ideology_anger.begin(), ideology_anger.end()))];
            world.insert(dup_nation);
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
    });
    do_emigration(world);
}
