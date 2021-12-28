#include "diplomacy.hpp"
#include "policy.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <set>
#ifndef _MSC_VER
#	include <sys/cdefs.h>
#endif
#include <libintl.h>
#include <locale.h>

#include "province.hpp"
#include "server/economy.hpp"
#include "world.hpp"
#include "unified_render/binary_image.hpp"
#include "server/lua_api.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/serializer.hpp"
#include "io_impl.hpp"
#include "server/server_network.hpp"

// Obtain best potential good
Good* ai_get_potential_good(Nation* nation, World* world) {
    // We will randomly determine if we want to do a market analysis for secondary and tertiary products or if
    // we want to build primary-sector products (to kickstart chains)
    if(!(std::rand() % 5)) {
        // Analyze the market probability of sucess of a product this is determined by (demand * price), we calculate
        // this from the average of all products on our provinces we currently own

        // Obtain the average probability of profitability per good we will only however, take in account products
        // which are produced on provinces we own, hereafter we can obtain the average of the national good industry
        print_info("Market analysis strategy");

        // So our formula would be:
        // Sucess = Sum(Demand / (Supply + 1) * Price)
        std::vector<float> avg_prob = std::vector<float>(world->goods.size(), 0.f);
        for(const auto& product : world->products) {
            if(product->building == nullptr || product->building->get_owner() != nation) continue;
            avg_prob[world->get_id(product->good)] += product->demand / (product->supply + 1) * product->price;
        }

        // Now, we will take in account 2 factors:
        // - First, an industry which takes-in a good with high sucess and outputs something should have that something
        // have a sucess probability equal or higher to the taked good
        // - Second, we preferably want the A.I to complete the supply chain of all industries so doing the method described
        // above should increase the priority for filling out higher level industries
        for(const auto& building_type : world->building_types) {
            // Take in account all buildings for this
            for(const auto& input : building_type->inputs) {
                // Apply the higher-probability with outputs of this factory
                for(const auto& output : building_type->outputs) {
                    avg_prob[world->get_id(output)] += avg_prob[world->get_id(input)] + 1;
                }
            }
        }

        Good* target_good = world->goods.at(std::distance(avg_prob.begin(), std::max_element(avg_prob.begin(), avg_prob.end())));

        // The more buildings there are in the world the less we are wiling to construct one
        float saturation = std::max<size_t>(1, world->buildings.size()) / 100;
        if(fmod(std::rand(), saturation)) {
            print_info("Too much market saturation");
            return nullptr;
        }

        // Obtain the index of the highest element (the one with more sucess rate)
        return target_good;
    } else {
        // We will randomly pick any primary product which we are capable of producing
        // This is mostly useful for starting supply chains from zero
        print_info("Primary sector kickstart strategy");

        // The more buildings there are in the world the less we are wiling to construct one
        // (more intense with primary sector due to primary-industry spam)
        float saturation = std::max<size_t>(1, world->buildings.size()) / 50;
        if(fmod(std::rand(), saturation)) {
            print_info("Too much market saturation");
            return nullptr;
        }

        for(const auto& building_type : world->building_types) {
            // Only take in account RGOs (and buildings that have atleast 1 output)
            if(!building_type->inputs.empty() || !building_type->outputs.size()) continue;
            if(!building_type->is_factory) continue;

            // Randomness
            if(std::rand() % 5) continue;
            return building_type->outputs[std::rand() % building_type->outputs.size()];
        }
    }

    print_info("No suitable good");
    return nullptr;
}

// Reforms the policies of a nation taking in account several factors
// TODO: Take in account several factors
void ai_reform(Nation* nation) {
    Policies new_policy = nation->current_policy;

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

    nation->set_policy(new_policy);
}

// Update relations with another nation
void ai_update_relations(Nation* nation, Nation* other) {
    const World& world = World::get_instance();

    NationRelation& relation = nation->relations[world.get_id(other)];
    NationRelation& other_relation = other->relations[world.get_id(nation)];

    // Try to increase relations with our friend
    if(nation->is_ally(*other) && !(std::rand() % 250)) {
        nation->increase_relation(*other);

        // Propose an alliance
        if(relation.relation > 1.f && !other_relation.has_alliance) {
            relation.has_alliance = true;
            print_info("[%s] requested an alliance with [%s]!", nation->ref_name.c_str(), other->ref_name.c_str());
        }

        // If they want an alliance we won't hesitate to join (they are our friends after all)
        if(other_relation.has_alliance && !relation.has_alliance) {
            relation.has_alliance = true;
            print_info("Alliance [%s] <-> [%s] has been made!", nation->ref_name.c_str(), other->ref_name.c_str());
        }

        // Same with the defensive pact
        if(other_relation.has_defensive_pact && !relation.has_defensive_pact) {
            relation.has_defensive_pact = true;
            print_info("Defensive pact [%s] <-> [%s] has been made!", nation->ref_name.c_str(), other->ref_name.c_str());
        }
    }

    // Hate our enemies more
    if(nation->is_enemy(*other) && !(std::rand() % 250)) {
        nation->decrease_relation(*other);

        // Embargo them
        if(relation.relation < -1.f) {
            relation.has_embargo = true;
            print_info("[%s] has placed an embargo on [%s]!", nation->ref_name.c_str(), other->ref_name.c_str());
        }

        // We really hate our enemies, don't we?
        if(relation.relation < -50.f && !relation.has_war) {
            // TODO: Do not war if it's beyond our capabilities (i.e Liechestein vs. France, Prussia and UK)
            nation->declare_war(*other);
        }
    }

    // Randomness to spice stuff up
    if(!(std::rand() % 10)) {
        nation->increase_relation(*other);
    } else if(!(std::rand() % 100)) {
        nation->decrease_relation(*other);
    }
    
    if(1) {
        if(!relation.has_war && !relation.has_alliance && !relation.has_defensive_pact) {
            nation->declare_war(*other);
        } else {
            // Offer treaties
            if(!(std::rand() % 100)) {
                Treaty* treaty = new Treaty();

                {
                    auto* clause = new TreatyClause::AnexxProvince();
                    clause->sender = nation;
                    clause->receiver = other;
                    for(const auto& province : other->owned_provinces) {
                        if(province->controller == nation) {
                            if(!(std::rand() % 10)) {
                                clause->provinces.push_back(province);
                            }
                        }
                    }

                    if(!clause->provinces.empty()) {
                        treaty->clauses.push_back(clause);
                    }
                }

                {
                    auto* clause = new TreatyClause::LiberateNation();
                    clause->sender = nation;
                    clause->receiver = other;

                    clause->liberated = nullptr;
                    for(const auto& province : other->owned_provinces) {
                        for(const auto& other_nation : province->nuclei) {
                            if(other_nation != other) {
                                clause->liberated = other_nation;
                                break;
                            }
                        }

                        if(clause->liberated != nullptr) break;
                    }

                    if(clause->liberated != nullptr) {
                        treaty->clauses.push_back(clause);
                    }
                }

                {
                    auto* clause = new TreatyClause::Ceasefire();
                    clause->sender = nation;
                    clause->receiver = other;
                    treaty->clauses.push_back(clause);
                }

                ((World&)world).insert(treaty);
            }
        }
    }
}

void ai_build_commercial(Nation* nation, World* world) {
    Good* target_good;
    target_good = ai_get_potential_good(nation, world);
    if(target_good == nullptr) return;

    // Find an industry type which outputs this good
    BuildingType* type = nullptr;
    for(const auto& building_type : world->building_types) {
        if(!building_type->is_factory) continue;

        /*for(const auto& input : building_type->inputs) {
            if(input == target_good) {
                type = (BuildingType*)building_type;
                break;
            }
        }*/

        for(const auto& output : building_type->outputs) {
            if(output == target_good) {
                type = (BuildingType*)building_type;
                break;
            }
        }
    }

    print_info("[%s]: Good [%s] seems to be on a high-trend - building industry [%s] which makes that good", nation->ref_name.c_str(), target_good->ref_name.c_str(), type->ref_name.c_str());

    // Otherwise -- do not build anything since the highest valued good cannot be produced
    if(type == nullptr) return;

    auto it = std::begin(nation->owned_provinces);
    std::advance(it, std::rand() % nation->owned_provinces.size());
    
    Province* province = *it;
    if(province->min_x > world->width || province->min_y == world->height || province->max_x < province->min_x || province->max_y < province->min_y || !province->n_tiles) {
        print_error("Cant build buidling, province doesn't have any tiles");
    } else {
        // Now build the building
        Building* building = new Building();
        building->province = province;
        building->type = world->building_types[0];
        building->owner = nation;
        building->budget = 100.f;
        
        if(building->type->is_factory) {
            building->create_factory();
            for(const auto& product : building->output_products) {
                Packet packet = Packet();
                Archive ar = Archive();
                ActionType action = ActionType::PRODUCT_ADD;
                ::serialize(ar, &action);
                ::serialize(ar, product);
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);
            }
        }
        world->insert(building);

        // Broadcast the addition of the building to the clients
        g_server->broadcast(Action::BuildingAdd::form_packet(building));
        print_info("Building of [%s](%i), from [%s] built on [%s]", building->type->ref_name.c_str(), (int)world->get_id(building->type), nation->ref_name.c_str(), building->get_province()->ref_name.c_str());
    }
}

void ai_do_tick(Nation* nation, World* world) {
    if(!nation->exists() || !nation->owned_provinces.size()) return;

    if(!(world->time % world->ticks_per_month)) {
        if(nation->ai_do_policies) {
            // Do a policy reform every 6 months
            if(!((world->time / world->ticks_per_month) % 180)) {
                // Update our policies and laws
                ai_reform(nation);
            }
        }

        // Update relations with other nations
        if(nation->ai_do_diplomacy) {
            for(auto& other : world->nations) {
                if(!other->exists() || other == nation) continue;
                ai_update_relations(nation, other);
            }
        }

        // Research technologies
        if(nation->ai_do_research) {
            for(auto& tech : world->technologies) {
                // Do not research if already been completed
                if(!nation->research[world->get_id(tech)]) continue;

                // Must be able to research it
                if(!nation->can_research(tech)) continue;

                nation->change_research_focus(tech);
                print_info("[%s] now researching [%s] - %.2f research points (+%.2f)", nation->ref_name.c_str(), tech->ref_name.c_str(), nation->research[world->get_id(tech)], nation->get_research_points());
                break;
            }
        }

        // Accepting/rejecting treaties
        if(nation->ai_handle_treaties) {
            for(auto& treaty : world->treaties) {
                for(auto& part : treaty->approval_status) {
                    if(part.first == nation) {
                        if(part.second == TreatyApproval::ACCEPTED || part.second == TreatyApproval::DENIED) break;

                        if(std::rand() % 50 >= 25) {
                            print_info("We, [%s], deny the treaty of [%s]", nation->ref_name.c_str(), treaty->name.c_str());
                            part.second = TreatyApproval::DENIED;
                        } else {
                            print_info("We, [%s], accept the treaty of [%s]", nation->ref_name.c_str(), treaty->name.c_str());
                            part.second = TreatyApproval::ACCEPTED;
                        }
                    }
                }
            }
        }

        // Taking events
        if(nation->ai_handle_events) {
            for(const auto& event : nation->inbox) {
                event->take_descision(nation, &event->descisions[std::rand() % event->descisions.size()]);
            }
        }

        // Build a commercially related building
        if(!(std::rand() % 2) && nation->ai_do_build_production) {
            ai_build_commercial(nation, world);
        }

        if(nation->ai_do_unit_production) {
            // Risk of invasion
            unsigned int defense_factor = 1;
            for(const auto& building : g_world->buildings) {
                if(building->owner == nation) {
                    defense_factor += ((uint)building->type->defense_bonus + 1) * 10000;
                    continue;
                }
            }

            for(const auto& province : nation->owned_provinces) {
                defense_factor /= ((province->total_pops() + province->n_tiles) / 10000) + 1;
            }

            // Build defenses
            if(!(std::rand() % std::max<int>(500, defense_factor))) {
                auto it = std::begin(nation->owned_provinces);
                std::advance(it, std::rand() % nation->owned_provinces.size());
				
                Province* province = *it;
                if(province->min_x > world->width || province->min_y == world->height || province->max_x < province->min_x || province->max_y < province->min_y || !province->n_tiles) {
                    print_error("Cant build defense, province doesn't have any tiles");
                } else {
					Building* building = new Building();
                    building->province = province;
                    building->type = world->building_types[0];
                    building->owner = nation;
                    building->budget = 100.f;
                    world->insert(building);

                    // Broadcast the addition of the building to the clients
                    g_server->broadcast(Action::BuildingAdd::form_packet(building));
                    print_info("Building of %s(%i), from %s built on %s", building->type->name.c_str(), (int)world->get_id(building->type), nation->name.c_str(), building->get_province()->name.c_str());
                }
            }

            // Build units inside buildings that are not doing anything
            for(auto& building : g_world->buildings) {
                if(building->working_unit_type != nullptr || building->owner != nation) continue;
                Province* province = building->get_province();
                if(province == nullptr) continue;

                if(std::rand() % 100) continue;

                auto* unit_type = g_world->unit_types[std::rand() % g_world->unit_types.size()];
                //if(!unit_type->is_ground) continue;
                unit_type = g_world->unit_types[0];

                building->working_unit_type = unit_type;
                building->req_goods_for_unit = unit_type->req_goods;
                print_info("%s: Building unit [%s] in [%s]", nation->ref_name.c_str(), building->working_unit_type->ref_name.c_str(), province->ref_name.c_str());
            }
        }

        // Colonize a province
        if(!(std::rand() % 500)) {
            // Pair denoting the weight a province has, the more the better
            std::vector<std::pair<Province*, float>> colonial_value;
            for(const auto& province : world->provinces) {
                if(province->owner != nullptr) continue;
                colonial_value.push_back(std::make_pair(province, 0.f));
            }

            for(auto& prov : colonial_value) {
                // The more population the better
                prov.second += prov.first->total_pops() * 0.005f;

                // If it's a nucleus province it also gets a x100 multiplier to maximize priority
                // on the nuclei provinces
                if(prov.first->nuclei.find(nation) != prov.first->nuclei.end()) {
                    prov.second *= 100.f;
                }
            }

            // Found an appropriate colony!
            if(!colonial_value.empty()) {
                Province* target = (*std::max_element(colonial_value.begin(), colonial_value.end())).first;
                if(target->owner == nullptr) {
                    Packet packet = Packet();
                    Archive ar = Archive();
                    ActionType action = ActionType::PROVINCE_COLONIZE;
                    ::serialize(ar, &action);
                    ::serialize(ar, &target);
                    ::serialize(ar, target);
                    packet.data(ar.get_buffer(), ar.size());
                    g_server->broadcast(packet);

                    nation->give_province(*target);
                    print_info("Conquering %s for %s", target->name.c_str(), nation->name.c_str());
                }
            }
        }
    }

    // TODO: make a better algorithm
    if(nation->ai_do_cmd_troops) {
        std::vector<int> potential_risk(world->provinces.size(), 0);

        for(const auto& province : nation->owned_provinces) {
            for(const auto& neighbour : province->neighbours) {
                if(province->terrain_type->is_water_body) continue;

                if(neighbour->controller != nullptr && neighbour->controller != nation) {
                    NationRelation& relation = neighbour->controller->relations[world->get_id(province->owner)];

                    // Risk is augmentated when we border any non-ally nation
                    if(!relation.has_alliance) {
                        potential_risk[world->get_id(neighbour)] += 100;
                        if(relation.has_war) {
                            potential_risk[world->get_id(neighbour)] += 50000;
                        }
                    }
                }
            }

            float defense_strength = 0.f, attack_strength = 0.f;
            for(const auto& unit : province->get_units()) {
                defense_strength += unit->type->defense;
                attack_strength += unit->type->attack;
            }
            potential_risk[world->get_id(province)] -= defense_strength * attack_strength;
        }

        for(const auto& province : world->provinces) {
            for(const auto& neighbour : province->neighbours) {
                potential_risk[world->get_id(neighbour)] += (potential_risk[world->get_id(province)] + 1) / province->neighbours.size();
            }
        }

        for(auto& unit : g_world->units) {
            if(unit->province->neighbours.empty()) continue;
            if(unit->owner != nation) continue;

            // Do not override targets (temporal)
            // TODO: OVERRIDE TARGETS ON CRITICAL SITUATIONS
            //if(unit->target != nullptr) continue;

            // See which province has the most potential_risk so we cover it from potential threats
            Province* highest_risk = unit->province;
            for(const auto& province : unit->province->neighbours) {
                //if(!unit->type->is_naval && province->terrain_type->is_water_body) continue;
                if(province->terrain_type->is_water_body) continue;
                if(std::rand() % 2) continue;

                if(potential_risk[world->get_id(highest_risk)] < potential_risk[world->get_id(province)]) {
                    if(province->owner != nullptr) {
                        NationRelation& relation = province->owner->relations[world->get_id(unit->owner)];
                        if(relation.has_war || relation.has_alliance || province->owner == unit->owner) {
                            highest_risk = province;
                        }
                    }
                }
            }

            auto* province = highest_risk;
            if(province == unit->province || province == unit->target) continue;

            if(province->owner != nullptr) {
                // Can only go to a province if we have military accesss, they are our ally or if we are at war
                // also if it's ours we can move thru it
                NationRelation& relation = province->owner->relations[world->get_id(unit->owner)];
                if(province->owner == unit->owner || relation.has_alliance || relation.has_military_access || relation.has_war) {
                    unit->set_target(*province);
                }
            } else {
                unit->set_target(*province);
            }
        }
    }
}
