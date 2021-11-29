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
#include "binary_image.hpp"
#include "server/lua_api.hpp"
#include "path.hpp"
#include "print.hpp"
#include "serializer.hpp"
#include "io_impl.hpp"
#include "server/server_network.hpp"

// Obtain best potential good
Good* ai_get_potential_good(Nation* nation, World* world) {
    // Analyze the market probability of sucess of a product
    // this is determined by (demand * price), we calculate
    // this from the average of all products on our provinces we currently own

    // Obtain the average probability of profitability per good
    // we will only however, take in account products which are produced on provinces
    // we own, hereafter we can obtain the average of the national good industry

    // So our formula would be:
    // Sucess = Sum(Demand / (Supply + 1) * Price)
    std::vector<float> avg_prob = std::vector<float>(world->goods.size(), 0.f);
    for(const auto& product : world->products) {
        if(product->origin->owner != nation) continue;

        avg_prob[world->get_id(product->good)] += product->demand / (product->supply + 1) * product->price;
    }

    // Now, we will take in account 2 factors:
    // - First, an industry which takes-in a good with high sucess and outputs something
    // should have that something have a sucess probability equal or higher to the taked
    // good
    // - Second, we preferably want the A.I to complete the supply chain of all industries
    // so doing the method described above should increase the priority for filling out higher
    // level industries
    for(const auto& building : world->buildings) {
        const Province* province = building->get_province();
        if(province == nullptr || province->owner != nation) continue;

        // We will only take in account industrial buildings
        // TODO: Take in account other buildings as well
        if(building->type->is_factory == false) continue;

        for(const auto& input : building->type->inputs) {
            // Apply the higher-probability with outputs of this factory
            for(const auto& output : building->type->outputs) {
                avg_prob[world->get_id(output)] += avg_prob[world->get_id(input)];
            }
        }
    }

    //for(size_t i = 0; i < world->goods.size(); i++) {
    //	print_info("%s: Good %s has %f probability", nation->name.c_str(), world->goods[i]->name.c_str(), avg_prob[i]);
    //}

    // Obtain the index of the highest element (the one with more sucess rate)
    return world->goods.at(std::distance(avg_prob.begin(), std::max_element(avg_prob.begin(), avg_prob.end())));
}

// Reforms the policies of a nation taking in account several factors
// TODO: Take in account several factors
void ai_reform(Nation* nation) {
    Policies new_policy = nation->current_policy;

    if(std::rand() % 100 > 50.f) {
        new_policy.import_tax += 0.1f * (rand() % 10);
    }
    else if(std::rand() % 100 > 50.f) {
        new_policy.import_tax -= 0.1f * (rand() % 10);
    }

    if(std::rand() % 100 > 50.f) {
        new_policy.export_tax += 0.1f * (rand() % 10);
    }
    else if(std::rand() % 100 > 50.f) {
        new_policy.export_tax -= 0.1f * (rand() % 10);
    }

    if(std::rand() % 100 > 50.f) {
        new_policy.domestic_export_tax += 0.1f * (rand() % 10);
    }
    else if(std::rand() % 100 > 50.f) {
        new_policy.domestic_export_tax -= 0.1f * (rand() % 10);
    }

    if(std::rand() % 100 > 50.f) {
        new_policy.domestic_import_tax += 0.1f * (rand() % 10);
    }
    else if(std::rand() % 100 > 50.f) {
        new_policy.domestic_import_tax -= 0.1f * (rand() % 10);
    }

    if(std::rand() % 100 > 50.f) {
        new_policy.industry_tax += 0.1f * (rand() % 10);
    }
    else if(std::rand() % 100 > 50.f) {
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
    }
    else if(std::rand() % 100 > 50.f) {
        new_policy.min_sv_for_parliament -= 0.1f * (rand() % 10);
    }

    nation->set_policy(new_policy);
}

// Update relations with another nation
void ai_update_relations(Nation* nation, Nation* other) {
    const World& world = World::get_instance();

    // Try to increase relations with our friend
    if(nation->is_ally(*other)) {
        nation->increase_relation(*other);

        // Propose an alliance
        if(nation->relations[world.get_id(other)].relation > 1.f && !other->relations[world.get_id(nation)].has_alliance) {
            nation->relations[world.get_id(other)].has_alliance = true;
            print_info("%s requested an alliance with %s!", nation->ref_name.c_str(), other->ref_name.c_str());
        }

        // If they want an alliance we won't hesitate to join (they are our friends after all)
        if(other->relations[world.get_id(nation)].has_alliance && !nation->relations[world.get_id(other)].has_alliance) {
            nation->relations[world.get_id(other)].has_alliance = true;
            print_info("Alliance %s <-> %s has been made!", nation->ref_name.c_str(), other->ref_name.c_str());
        }

        // Same with the defensive pact
        if(other->relations[world.get_id(nation)].has_defensive_pact && !nation->relations[world.get_id(other)].has_defensive_pact) {
            nation->relations[world.get_id(other)].has_defensive_pact = true;
            print_info("Defensive pact %s <-> %s has been made!", nation->ref_name.c_str(), other->ref_name.c_str());
        }
    }

    // Hate our enemies more
    if(nation->is_enemy(*other)) {
        nation->decrease_relation(*other);

        // Embargo them
        if(nation->relations[world.get_id(other)].relation < -1.f) {
            nation->relations[world.get_id(other)].has_embargo = true;
            print_info("%s has placed an embargo on %s!", nation->ref_name.c_str(), other->ref_name.c_str());
        }

        // We really hate our enemies, don't we?
        if(nation->relations[world.get_id(other)].has_embargo) {
            nation->declare_war(*other);
        }
    }

    // Randomness to spice stuff up
    if(std::rand() % 5000 == 0) {
        nation->increase_relation(*other);
    }
    else if(std::rand() % 5000 == 0) {
        nation->decrease_relation(*other);
    }
}

void ai_do_tick(Nation* nation, World* world) {
    if(!nation->exists()) return;
    if(nation->owned_provinces.size() == 0) return;

    if(nation->diplomatic_timer != 0) {
        nation->diplomatic_timer--;
    }

    if(world->time % 48 == 0) {
        // Do a policy reform every 6 months
        if((world->time / 48) % 180 == 0) {
            // Update our policies and laws
            ai_reform(nation);
        }

        // Update relations with other nations
        for(auto& other : world->nations) {
            if(!other->exists() || other == nation) continue;

            ai_update_relations(nation, other);
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
            if(prov.first->nucleuses.find(nation) != prov.first->nucleuses.end()) {
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

    // Accepting/rejecting treaties
    if(!(std::rand() % 50)) {
        for(auto& treaty : world->treaties) {
            for(auto& part : treaty->approval_status) {
                if(part.first == nation) {
                    if(part.second == TreatyApproval::ACCEPTED || part.second == TreatyApproval::DENIED) break;

                    if(std::rand() % 50 >= 25) {
                        print_info("We, %s, deny the treaty of %s", treaty->name.c_str());
                        part.second = TreatyApproval::DENIED;
                    }
                    else {
                        print_info("We, %s, accept the treaty of %s", treaty->name.c_str());
                        part.second = TreatyApproval::ACCEPTED;
                    }
                }
            }
        }
    }

    if(!(std::rand() % 50)) {
        for(const auto& event : nation->inbox) {
            event->take_descision(nation, &event->descisions[std::rand() % event->descisions.size()]);
        }
    }

    // TODO: make a better algorithm

    // Risk of invasion
    uint defense_factor = 1;
    for(const auto& building : g_world->buildings) {
        if(building->owner == nation) {
            defense_factor += ((uint)building->type->defense_bonus + 1) * 10000;
            continue;
        }
    }
    for(const auto& province : nation->owned_provinces) {
        defense_factor /= ((province->total_pops() + province->n_tiles) / 10000) + 1;
    }

    if(defense_factor < 5) {
        defense_factor = 5;
    }

    // Build defenses
    if(std::rand() % defense_factor == 0) {
        Building* building = new Building();
        building->owner = nation;

        auto it = std::begin(nation->owned_provinces);
        std::advance(it, std::rand() % nation->owned_provinces.size());
        Province* province = *it;
        //Province* province = g_world->provinces[std::rand() % g_world->provinces.size()];
        if(province->min_x > world->width || province->min_y == world->height ||
            province->max_x < province->min_x || province->max_y < province->min_y ||
            province->n_tiles == 0) {
            print_error("Cant build defense, province doesn't have any tiles");
        }
        else {

            // Randomly place in any part of the province
            glm::ivec2 coord = world->get_rand_province_coord(province);
            building->x = coord.x;
            building->y = coord.y;
            building->province = province;

            building->working_unit_type = nullptr;

            building->type = world->building_types[0];
            world->insert(building);

            // Broadcast the addition of the building to the clients
            {
                Packet packet = Packet();
                Archive ar = Archive();
                ActionType action = ActionType::BUILDING_ADD;
                ::serialize(ar, &action);
                ::serialize(ar, building);
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);
            }
            print_info("Building of %s(%i), from %s built on %s", building->type->name.c_str(), (int)world->get_id(building->type), nation->name.c_str(), building->get_province()->name.c_str());
        }
    }

    // Build units inside buildings that are not doing anything
    if(std::rand() % 50 == 0) {
        for(auto& building : g_world->buildings) {
            if(building->working_unit_type != nullptr || building->owner != nation) continue;

            while(building->working_unit_type == nullptr) {
                building->working_unit_type = g_world->unit_types[rand() % g_world->unit_types.size()];
            }
            building->req_goods_for_unit = building->working_unit_type->req_goods;
            print_info("DEPLOYING UNIT OF TYPE %s IN %s", building->working_unit_type->ref_name.c_str(), building->get_province()->ref_name.c_str());
        }
    }

    // Naval AI
    if(std::rand() % 100 == 0) {
        for(auto& unit : g_world->units) {
            if(unit->owner != nation) continue;

            auto it = std::begin(nation->owned_provinces);
            std::advance(it, std::rand() % nation->owned_provinces.size());
            Province* province = *it;

            if(province->min_x > world->width || province->min_y == world->height) continue;
            if(province->max_x < province->min_x || province->max_y < province->min_y) continue;
            if(province->n_tiles == 0) continue;
            glm::ivec2 coord = world->get_rand_province_coord(province);
            unit->tx = coord.x;
            unit->ty = coord.y;
            unit->x = std::min<float>(unit->x, g_world->width - 1);
            unit->y = std::min<float>(unit->y, g_world->height - 1);
        }
    }

    // Build a commercially related building
    if(std::rand() % 5000 == 0) {
        Good* target_good;
        target_good = ai_get_potential_good(nation, world);
        if(target_good == nullptr) return;

        print_info("Building a commercially related building");

        // Find an industry type which outputs this good
        BuildingType* type = nullptr;
        for(const auto& building_type : world->building_types) {
            if(!building_type->is_factory) continue;

            for(const auto& input : building_type->inputs) {
                if(input == target_good) {
                    type = (BuildingType*)building_type;
                    break;
                }
            }

            for(const auto& output : building_type->outputs) {
                if(output == target_good) {
                    type = (BuildingType*)building_type;
                    break;
                }
            }
        }

        print_info("%s: Good %s seems to be on a high-trend - building industry %s which makes that good", nation->name.c_str(), target_good->name.c_str(), type->name.c_str());

        // Otherwise -- do not build anything since the highest valued good cannot be produced
        if(type == nullptr) return;

        Province* province = g_world->provinces[std::rand() % g_world->provinces.size()];
        if(province->min_x > world->width || province->min_y == world->height ||
            province->max_x < province->min_x || province->max_y < province->min_y ||
            province->n_tiles == 0) {
            print_error("Cant build buidling, province doesn't have any tiles");
        }
        else {
            // Now build the building
            Building* building = new Building();
            building->owner = nation;

            // Randomly place in any part of the province
            glm::ivec2 coord = world->get_rand_province_coord(province);
            building->x = coord.x;
            building->y = coord.y;
            building->province = province;

            building->working_unit_type = nullptr;
            building->req_goods_for_unit = std::vector<std::pair<Good*, size_t>>();
            building->req_goods = std::vector<std::pair<Good*, size_t>>();
            building->type = type;
            if(building->type->is_factory) {
                building->budget = 100.f;
                building->corporate_owner = world->companies.at(std::rand() % world->companies.size());
                building->create_factory();
                building->corporate_owner->operating_provinces.insert(building->get_province());
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
            {
                Packet packet = Packet();
                Archive ar = Archive();
                ActionType action = ActionType::BUILDING_ADD;
                ::serialize(ar, &action);
                ::serialize(ar, building);
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);
            }
            print_info("Building of %s(%i), from %s built on %s", building->type->name.c_str(), (int)world->get_id(building->type), nation->name.c_str(), building->get_province()->name.c_str());
        }
    }
}
