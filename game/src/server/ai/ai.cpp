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
//      client/ai.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <set>

#include "eng3d/binary_image.hpp"
#include "eng3d/path.hpp"
#include "eng3d/serializer.hpp"
#include "eng3d/log.hpp"

#include "diplomacy.hpp"
#include "policy.hpp"
#include "province.hpp"
#include "server/economy.hpp"
#include "world.hpp"
#include "server/lua_api.hpp"
#include "io_impl.hpp"
#include "server/server_network.hpp"

enum AiMode {
    AI_IDLE,
    AI_ACTIVE,
};

class AiData {
public:
    enum AiMode mode;
    std::vector<double> nations_risk_factor;
};

static std::vector<AiData> g_ai_data;
static std::vector<Province::Id> g_water_provinces;

void ai_init(World& world) {
    g_ai_data.resize(world.nations.size());
    for(auto& ai_data : g_ai_data)
        ai_data.nations_risk_factor.resize(world.nations.size());

    g_water_provinces.reserve(world.provinces.size());
    for(const auto& province : world.provinces)
        if(province.terrain_type->is_water_body)
            g_water_provinces.push_back(province.get_id());
}

// Obtain best potential good
static inline Good* ai_get_potential_good(Nation& nation) {
    World& world = World::get_instance();
    // We will randomly determine if we want to do a market analysis for secondary and tertiary products or if
    // we want to build primary-sector products (to kickstart chains)
    if(!(std::rand() % 5)) {
        // Analyze the market probability of sucess of a product this is determined by (demand * price), we calculate
        // this from the average of all products on our provinces we currently own

        // Obtain the average probability of profitability per good we will only however, take in account products
        // which are produced on provinces we own, hereafter we can obtain the average of the national good industry
        Eng3D::Log::debug("ai", "Market analysis strategy");

        // So our formula would be:
        // Sucess = Sum(Demand / (Supply + 1) * Price)
        std::vector<float> avg_prob(world.goods.size(), 0.f);
        for(const auto province_id : nation.owned_provinces) {
            const auto& province = world.provinces[province_id];
            for(const auto& good : world.goods) {
                const Product& product = province.products[world.get_id(good)];
                avg_prob[world.get_id(good)] += product.demand / (product.supply + 1) * product.price;
            }
        }

        // Now, we will take in account 2 factors:
        // - First, an industry which takes-in a good with high sucess and outputs something should have that something
        // have a sucess probability equal or higher to the taked good
        // - Second, we preferably want the A.I to complete the supply chain of all industries so doing the method described
        // above should increase the priority for filling out higher level industries
        for(const auto& building_type : world.building_types) {
            // Take in account all buildings for this
            for(const auto& input : building_type.inputs) {
                // Apply the higher-probability with outputs of this factory
                avg_prob[world.get_id(*building_type.output)] += avg_prob[world.get_id(*input)] + 1;
            }
        }

        Good* target_good = &world.goods.at(std::distance(avg_prob.begin(), std::max_element(avg_prob.begin(), avg_prob.end())));

        // The more buildings there are in the world the less we are wiling to construct one
        //float saturation = std::max<size_t>(1, world.buildings.size()) / 100;
        float saturation = 1.f;
        if(std::fmod(std::rand(), saturation)) {
            Eng3D::Log::debug("ai", "Too much market saturation");
            return nullptr;
        }

        // Obtain the index of the highest element (the one with more sucess rate)
        return target_good;
    } else {
        // We will randomly pick any primary product which we are capable of producing
        // This is mostly useful for starting supply chains from zero
        Eng3D::Log::debug("ai", "Primary sector kickstart strategy");

        // The more buildings there are in the world the less we are wiling to construct one
        // (more intense with primary sector due to primary-industry spam)
        //float saturation = std::max<size_t>(1, world.buildings.size()) / 50;
        float saturation = 1.f;
        if(fmod(std::rand(), saturation)) {
            Eng3D::Log::debug("ai", "Too much market saturation");
            return nullptr;
        }

        for(const auto& building_type : world.building_types) {
            // Only take in account RGOs (and buildings that have an output)
            if(!building_type.inputs.empty() || building_type.output == nullptr) {
                continue;
            }

            // Randomness
            if(std::rand() % 5) {
                continue;
            }
            return building_type.output;
        }
    }

    Eng3D::Log::debug("ai", "No suitable good");
    return nullptr;
}

// Reforms the policies of a nation taking in account several factors
/// @todo Take in account several factors
static inline void ai_reform(Nation& nation) {
    World& world = World::get_instance();
    Policies new_policy = nation.current_policy;

    if(std::rand() % 100 > 50.f) {
        new_policy.import_tax += 0.1f * (rand() % 10);
    } else if(std::rand() % 100 > 50.f) {
        new_policy.import_tax -= 0.1f * (rand() % 10);
    }

    if(std::rand() % 100 > 50.f) {
        new_policy.export_tax += 0.1f * (rand() % 10);
    } else if(std::rand() % 100 > 50.f) {
        new_policy.export_tax -= 0.1f * (rand() % 10);
    }

    if(std::rand() % 100 > 50.f) {
        new_policy.domestic_export_tax += 0.1f * (rand() % 10);
    } else if(std::rand() % 100 > 50.f) {
        new_policy.domestic_export_tax -= 0.1f * (rand() % 10);
    }

    if(std::rand() % 100 > 50.f) {
        new_policy.domestic_import_tax += 0.1f * (rand() % 10);
    } else if(std::rand() % 100 > 50.f) {
        new_policy.domestic_import_tax -= 0.1f * (rand() % 10);
    }

    if(std::rand() % 100 > 50.f) {
        new_policy.industry_tax += 0.1f * (rand() % 10);
    } else if(std::rand() % 100 > 50.f) {
        new_policy.industry_tax -= 0.1f * (rand() % 10);
    }

    new_policy.public_healthcare = (std::rand() % 100 > 50.f) ? true : false;
    new_policy.public_education = (std::rand() % 100 > 50.f) ? true : false;
    new_policy.private_property = (std::rand() % 100 > 50.f) ? true : false;
    new_policy.executive_parliament = (std::rand() % 100 > 50.f) ? true : false;
    new_policy.legislative_parliament = (std::rand() % 100 > 50.f) ? true : false;
    new_policy.foreign_trade = (std::rand() % 100 > 50.f) ? true : false;

    if(std::rand() % 100 > 50.f) {
        new_policy.min_sv_for_parliament += 0.1f * (rand() % 10);
    } else if(std::rand() % 100 > 50.f) {
        new_policy.min_sv_for_parliament -= 0.1f * (rand() % 10);
    }

    nation.set_policy(new_policy);
}

// Update relations with another nation
static inline void ai_update_relations(Nation& nation, Nation& other) {
    auto& world = World::get_instance();
    auto& relation = world.get_relation(world.get_id(nation), world.get_id(other));

    // Try to increase relations with our friend
    if(nation.is_ally(other) && !(std::rand() % 250)) {
        nation.increase_relation(other);

        // Propose an alliance
        if(relation.relation > 1 && !relation.has_alliance) {
            relation.has_alliance = true;
            relation.relation = 100;
            Eng3D::Log::debug("ai", nation.ref_name + " requested an alliance with " + other.ref_name);
        }

        // If they want an alliance we won't hesitate to join (they are our friends after all)
        if(!relation.has_alliance) {
            relation.has_alliance = true;
            relation.relation = 100;
            Eng3D::Log::debug("ai", nation.ref_name + " did an alliance with " + other.ref_name);
        }

        // Same with the defensive pact
        if(!relation.has_defensive_pact) {
            relation.has_defensive_pact = true;
            Eng3D::Log::debug("ai", nation.ref_name + " defensive pact with " + other.ref_name);
        }
    }

    // Hate our enemies more
    if(nation.is_enemy(other) && !(std::rand() % 500)) {
        nation.decrease_relation(other);

        // Embargo them
        if(relation.relation < -15) {
            relation.has_embargo = true;
            Eng3D::Log::debug("ai", nation.ref_name + " has placed an embargo on " + other.ref_name);
        }

        // We really hate our enemies, don't we?
        if(relation.relation < -90 && !relation.has_war) {
            /// @todo Do not war if it's beyond our capabilities (i.e Liechestein vs. France, Prussia and UK)
            nation.declare_war(other);
        }
    }

    // Randomness to spice stuff up
    if(!(std::rand() % 50)) {
        nation.increase_relation(other);
    } else if(!(std::rand() % 500)) {
        nation.decrease_relation(other);
    }

    // Check if we even border them
    bool is_border = false;
    for(const auto province_id : nation.controlled_provinces) {
        const auto& province = world.provinces[province_id];
        for(const auto neighbour_id : province.neighbours) {
            auto& neighbour = g_world.provinces[neighbour_id];
            if(neighbour.controller == &other) {
                is_border = true;
                break;
            }
        }
    }

    // Our strength as attackers
    float our_power = 1.f;
    for(const auto& ally_nation : nation.get_allies()) {
        for(const auto province_id : ally_nation->owned_provinces) {
            const auto& unit_ids = world.unit_manager.get_province_units(province_id);
            for(const auto unit_id : unit_ids) {
                auto& unit = g_world.unit_manager.units[unit_id];
                our_power += unit.type->attack * unit.size;
                our_power += unit.type->defense * unit.size;
            }
        }
    }

    // The strength of the defenders
    float other_power = 1.f;
    for(const auto& ally_nation : other.get_allies()) {
        for(const auto province_id : ally_nation->owned_provinces) {
            const auto& unit_ids = world.unit_manager.get_province_units(province_id);
            for(const auto unit_id : unit_ids) {
                auto& unit = g_world.unit_manager.units[unit_id];
                other_power += unit.type->attack * unit.size;
                other_power += unit.type->defense * unit.size;
            }
        }
    }

    // Hating a nation a lot will make us reconsider logic military actions and go "purely by instinct"
    // Calculate the times the other nation has our power, multiply that by a factor of 1,000,000
    // If the relation is negative then we divide by the positive sum of it
    if(relation.relation < -1) {
        const float force_dist = 10.f * ((1.f + other_power) / (1.f + our_power));
        const int chance = std::max<float>(0, force_dist - -relation.relation);
        if(std::rand() % (100 + (chance * 10)) == 0) {
            if(!relation.has_war) {
                // Check we border said nation
                bool has_border = false;
                bool has_units = false;
                for(const auto province_id : nation.controlled_provinces) {
                    const auto& province = world.provinces[province_id];
                    // Check for borders
                    for(const auto neighbour_id : province.neighbours) {
                        auto& neighbour = g_world.provinces[neighbour_id];
                        if(neighbour.controller == &other) {
                            has_border = true;
                            break;
                        }
                    }

                    // Check for units
                    has_units = !g_world.unit_manager.get_province_units(province_id).empty();
                    if(has_border && has_units) break;
                }
                if(has_border) nation.declare_war(other);
            }
        }
    }

    if(relation.has_war) {
        // Offer treaties
        if(!(std::rand() % 100)) {
            Treaty treaty = Treaty();
            {
                auto* clause = new TreatyClause::AnnexProvince();
                clause->sender = &nation;
                clause->receiver = &other;
                for(const auto province_id : other.owned_provinces) {
                    auto& province = world.provinces[province_id];
                    if(province.controller == &nation) {
                        if(!(std::rand() % 10))
                            clause->provinces.push_back(&province);
                    }
                }

                if(!clause->provinces.empty()) {
                    treaty.clauses.push_back(clause);
                }
            }

            {
                auto* clause = new TreatyClause::LiberateNation();
                clause->sender = &nation;
                clause->receiver = &other;

                clause->liberated = nullptr;
                for(const auto province_id : other.owned_provinces) {
                    const auto& province = world.provinces[province_id];
                    for(const auto& nucleus_id : province.nuclei) {
                        auto& nucleus = g_world.nations[nucleus_id];
                        if(&nucleus != &other) {
                            clause->liberated = &nucleus;
                            break;
                        }
                    }

                    if(clause->liberated != nullptr) break;
                }

                if(clause->liberated != nullptr)
                    treaty.clauses.push_back(clause);
            }

            {
                auto* clause = new TreatyClause::Ceasefire();
                clause->sender = &nation;
                clause->receiver = &other;
                treaty.clauses.push_back(clause);
            }

            ((World&)world).insert(treaty);
        }
    }
}

static inline void ai_build_commercial(Nation& nation) {
    if(nation.owned_provinces.empty()) return;

    World& world = World::get_instance();
    Good* target_good;
    target_good = ai_get_potential_good(nation);
    if(target_good == nullptr) return;

    // Find an industry type which outputs this good
    BuildingType* type = nullptr;
    for(auto& building_type : world.building_types) {
        if(building_type.output == target_good) {
            type = &building_type;
            break;
        }
    }

    // Otherwise -- do not build anything since the highest valued good cannot be produced
    if(type == nullptr) return;
    Eng3D::Log::debug("ai", nation.ref_name + " Good " + target_good->ref_name + " seems to be on a high-trend - building industry " + type->ref_name + " which makes that good");

    auto it = std::begin(nation.owned_provinces);
    std::advance(it, std::rand() % nation.owned_provinces.size());

    auto& province = world.provinces[*it];
    
    // Now build the building
    const BuildingType& building_type = world.building_types.at(0);
    province.add_building(building_type);
    // Broadcast the addition of the building to the clients
    g_server->broadcast(Action::BuildingAdd::form_packet(province, building_type));
    Eng3D::Log::debug("ai", "Building of " + building_type.ref_name + ", from " + nation.ref_name + " built on " + province.ref_name);
}

void ai_do_tick(Nation& nation) {
    auto& world = World::get_instance();
    if(!nation.exists()) return;
    auto& ai_data = g_ai_data[world.get_id(nation)];

    /// @todo make a better algorithm
    if(nation.ai_do_cmd_troops) {
        constexpr auto base_weight = 10.f;
        constexpr auto war_weight = 90.f; // Weight of war
        constexpr auto unit_battle_weight = 100.5f; // Attraction of units into entering on pre-existing battles
        constexpr auto unit_exist_weight = 50.f; // Weight of an unit by just existing
        constexpr auto coastal_weight = 150.f; // Importance given to coastal provinces
        constexpr auto nation_weight = 100.f; // Nations weight

        std::fill(ai_data.nations_risk_factor.begin(), ai_data.nations_risk_factor.end(), 1.f);
        std::vector<Province::Id> eval_provinces; // Provinces that can be evaluated for war
        eval_provinces.reserve(world.provinces.size());

        for(const auto province_id : g_water_provinces)
            eval_provinces.push_back(province_id);

        for(const auto& other : world.nations) {
            if(&other == &nation) {
                // Add our own nation's province ids
                for(const auto province_id : nation.controlled_provinces)
                    eval_provinces.push_back(province_id);
                continue;
            }
            // Here we calculate the risk factor of each nation and then we put it on a lookup table
            // because we can't afford to calculate this for EVERY FUCKING province
            const auto& relation = world.get_relation(nation.get_id(), other.get_id());
            // And add if they're allied with us or let us pass thru
            if(relation.has_alliance || relation.has_military_access || relation.has_war) {
                for(const auto province_id : other.controlled_provinces)
                    eval_provinces.push_back(province_id);
            }

            constexpr auto relation_max = 100.f;
            constexpr auto relation_range = 200.f; // Range of relations, the max-min difference
            // Risk is augmentated when we border any non-ally nation
            if(!relation.has_alliance)
                // Makes sure so 200 relations results on 0.0 attraction, while 1 or 0 relations result on 3.9 and 4.0
                ai_data.nations_risk_factor[other.get_id()] = ((relation_range - (relation.relation + relation_max)) / relation_max) * nation_weight;
            if(relation.has_war)
                ai_data.nations_risk_factor[other.get_id()] = war_weight * nation_weight;
        }
        ai_data.nations_risk_factor[nation.get_id()] = -1.f * nation_weight;

        std::vector<double> potential_risk(world.provinces.size(), 1.f);
        for(const auto province_id : eval_provinces) {
            const auto& province = world.provinces[province_id];
            for(const auto neighbour_id : province.neighbours) {
                auto& neighbour = g_world.provinces[neighbour_id];
                if(neighbour.terrain_type->is_water_body) continue;
                auto draw_in_force = base_weight;
                // The "cooling" value which basically makes us ignore some provinces with lots of defenses
                // so we don't rack up deathstacks on a border with some micronation
                const auto& unit_ids = world.unit_manager.get_province_units(neighbour_id);
                for(const auto unit_id : unit_ids) {
                    auto& unit = g_world.unit_manager.units[unit_id];
                    // Only account this for units that are of our nation
                    // because enemy units will require us to give more importance to it
                    double unit_strength = (unit.type->attack * unit.type->defense * unit.size) / 100.f;
                    unit_strength = unit.on_battle ? std::abs(unit_strength) : unit_strength; // Allow stacking on battles
                    // This works because nations which are threatening to us have positive values, so they
                    // basically make the draw_in_force negative, which in turns does not draw away but rather
                    // draw in even more units
                    draw_in_force += unit_strength * (unit.on_battle ? unit_battle_weight : unit_exist_weight);
                }
                // Only if neighbour has a controller
                if(neighbour.controller != nullptr)
                    draw_in_force *= ai_data.nations_risk_factor[neighbour.controller->get_id()];
                // Spread out the heat
                potential_risk[province_id] += draw_in_force;
                if(neighbour.is_coastal)
                    potential_risk[province_id] *= coastal_weight;
                potential_risk[neighbour_id] += potential_risk[province_id] / province.neighbours.size();
            }
        }

        for(const auto province_id : eval_provinces) {
            const auto& province = world.provinces[province_id];
            const auto& unit_ids = world.unit_manager.get_province_units(province_id);
            for(const auto unit_id : unit_ids) {
                auto& unit = g_world.unit_manager.units[unit_id];
                if(unit.owner_id != nation.get_id() || !unit.can_move()) continue;
                // Do not change targets
                /// @todo Change targets when urgent
                if(Province::is_valid(unit.get_target_province_id())) continue;
                
                const auto& unit_province = world.provinces[unit.province_id()];
                // See which province has the most potential_risk so we cover it from potential threats
                const auto* highest_risk = &unit_province;
                for(const auto neighbour_id : unit_province.neighbours) {
                    const auto& neighbour = g_world.provinces[neighbour_id];
                    if(!unit.type->is_naval && neighbour.terrain_type->is_water_body) continue;
                    if(std::rand() % 2 == 0) continue;

                    // Uncolonized land is unsteppable
                    if(neighbour.controller == nullptr && !neighbour.terrain_type->is_water_body)
                        continue;

                    if(potential_risk[neighbour_id] > potential_risk[highest_risk->get_id()]) {
                        if(neighbour.controller != nullptr && neighbour.controller->get_id() != unit.owner_id) {
                            const auto& relation = world.get_relation(neighbour.controller->get_id(), unit.owner_id);
                            if(relation.has_war || relation.has_alliance || relation.has_military_access)
                                highest_risk = &neighbour;
                        } else {
                            highest_risk = &neighbour;
                        }
                    }
                }

                // Above we made sure high_risk province is valid for us to step in
                if(highest_risk == &unit_province || std::rand() % 32 == 0) {
                    auto it = highest_risk->neighbours.begin();
                    std::advance(it, std::rand() % highest_risk->neighbours.size());
                    highest_risk = &g_world.provinces[*it];
                    if(highest_risk->controller == nullptr && !highest_risk->terrain_type->is_water_body) continue;
                    if(highest_risk->controller != nullptr && highest_risk->controller->get_id() != unit.owner_id) {
                        const auto& relation = world.get_relation(highest_risk->controller->get_id(), unit.owner_id);
                        if(!(relation.has_war || relation.has_alliance || relation.has_military_access)) continue;
                    }
                    if(highest_risk == &unit_province) continue;
                }
                
                unit.set_target(*highest_risk);
            }
        }
    }

    // Once we hit a economical tick & we are controlled by AI
    if(!(world.time % world.ticks_per_month) && nation.ai_controlled) {
        // Do a policy reform every 6 months
        if(!((world.time / world.ticks_per_month) % 180)) {
            // Update our policies and laws
            ai_reform(nation);
        }

        // Update relations with other nations
        for(auto& other : world.nations) {
            if(!other.exists() || &other == &nation) continue;
            ai_update_relations(nation, other);
        }

        // Colonize a province
        if(0) {
            // Pair denoting the weight a province has, the more the better
            std::vector<std::pair<Province*, float>> colonial_value;
            for(auto& province : world.provinces) {
                if(province.terrain_type->is_water_body || Province::is_valid(province.owner_id)) continue;
                colonial_value.push_back(std::make_pair(&province, 0.f));
            }

            for(auto& prov : colonial_value) {
                // The more population the better
                prov.second += prov.first->total_pops() * 0.005f;
                // Bordering a province of ours means we are **to** colonize it
                for(const auto neighbour_id : prov.first->neighbours) {
                    auto& neighbour = g_world.provinces[neighbour_id];
                    if(neighbour.controller == &nation)
                        prov.second *= 100.f;
                }
                // If it's a nucleus province it also gets a x100 multiplier to maximize priority
                // on the nuclei provinces
                if(prov.first->nuclei.find(world.get_id(nation)) != prov.first->nuclei.end())
                    prov.second *= 100.f;
            }

            // Found an appropriate colony!
            if(!colonial_value.empty()) {
                Province* target = (*std::max_element(colonial_value.begin(), colonial_value.end())).first;
                if(target != nullptr && Nation::is_invalid(target->owner_id)) {
                    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
                    Archive ar = Archive();
                    ActionType action = ActionType::PROVINCE_COLONIZE;
                    ::serialize(ar, &action);
                    ::serialize(ar, &target);
                    ::serialize(ar, target);
                    packet.data(ar.get_buffer(), ar.size());
                    g_server->broadcast(packet);
                    nation.give_province(*target);
                    Eng3D::Log::debug("ai", "Conquering " + target->ref_name + " for " + nation.ref_name);
                }
            }
        }

        // Research technologies
        for(auto& technology : world.technologies) {
            // Do not research if already been completed and be able to research it
            if(!nation.research[world.get_id(technology)] || !nation.can_research(technology))
                continue;

            nation.change_research_focus(technology);
            Eng3D::Log::debug("ai", "[" + nation.ref_name + "] now researching [" + technology.ref_name + "] - " + std::to_string(nation.research[world.get_id(technology)]) + " research points (" + std::to_string(nation.get_research_points()) + ")");
            break;
        }

        // Accepting/rejecting treaties
        for(auto& treaty : world.treaties) {
            for(auto& part : treaty.approval_status) {
                if(part.first == &nation) {
                    if(part.second == TreatyApproval::ACCEPTED || part.second == TreatyApproval::DENIED) break;
                    // We can only deny treaties if NOT ALL of our country is sieged
                    // also we can surrender if our capital is taken, but only if it's the sender of the treaty itself
                    if(nation.controlled_provinces.empty() || world.provinces[nation.capital_id].controller == treaty.sender) {
                        Eng3D::Log::debug("ai", "We, [" + nation.ref_name + "], surrender & accept the treaty of [" + treaty.name + "]");
                        part.second = TreatyApproval::ACCEPTED;
                    } else {
                        if(std::rand() % 10) {
                            Eng3D::Log::debug("ai", "We, [" + nation.ref_name + "], deny the treaty of [" + treaty.name + "]");
                            part.second = TreatyApproval::DENIED;
                        } else {
                            Eng3D::Log::debug("ai", "We, [" + nation.ref_name + "], accept the treaty of [" + treaty.name + "]");
                            part.second = TreatyApproval::ACCEPTED;
                        }
                    }
                }
            }
        }

        // Taking events
        for(auto& event : nation.inbox) {
            event.take_decision(nation, event.decisions[std::rand() % event.decisions.size()]);
        }

        // Build a factory/non-militar building
        ai_build_commercial(nation);

        // Risk of invasion
        int defense_factor = 1;
        for(const auto& other : g_world.nations) {
            if(&other == &nation) continue;
            const auto& relation = world.get_relation(world.get_id(other), world.get_id(nation));
            if(relation.has_war) defense_factor++;
        }
        defense_factor = std::min<float>(defense_factor, 100);

        const int base_reluctance = 100;
        
        // Build defenses
        if(std::rand() % (base_reluctance / defense_factor) == 0 && !nation.owned_provinces.empty()) {
            auto it = std::begin(nation.owned_provinces);
            std::advance(it, std::rand() % nation.owned_provinces.size());
            Province& province = world.provinces[*it];
            
            const BuildingType& building_type = world.building_types[0];
            province.add_building(building_type);
            // Broadcast the addition of the building to the clients
            g_server->broadcast(Action::BuildingAdd::form_packet(province, building_type));
            Eng3D::Log::debug("ai", "Construction of building " + building_type.name + " from " + nation.name + " built on " + province.name);
        }

        if(std::rand() % (base_reluctance / defense_factor) == 0) {
            // Build units inside buildings that are not doing anything
            for(const auto province_id : nation.controlled_provinces) {
                if(std::rand() % (base_reluctance / defense_factor)) continue;

                auto& province = world.provinces[province_id];
                for(size_t i = 0; i < world.building_types.size(); i++) {
                    const auto* building_type = &world.building_types[i];
                    if(!building_type->can_build_military()) {
                        //continue;
                    }

                    auto& building = province.buildings[i];
                    if(building.level == 0 || building.working_unit_type != nullptr || !building.can_do_output())
                        continue;
                    /// @todo Actually produce something appropriate
                    auto* unit_type = &world.unit_types[std::rand() % world.unit_types.size()];
                    building.working_unit_type = unit_type;
                    building.req_goods_for_unit = unit_type->req_goods;
                    Eng3D::Log::debug("ai", "Building of unit " + unit_type->name + " from " + nation.name + " built on " + province.name);
                }
            }
        }
    }
}
