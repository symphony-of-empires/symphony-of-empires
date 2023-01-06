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
//      server/ai.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/combinable.h>

#include "eng3d/binary_image.hpp"
#include "eng3d/serializer.hpp"
#include "eng3d/log.hpp"
#include "eng3d/rand.hpp"

#include "diplomacy.hpp"
#include "indpobj.hpp"
#include "province.hpp"
#include "server/economy.hpp"
#include "world.hpp"
#include "action.hpp"
#include "server/lua_api.hpp"
#include "server/server_network.hpp"
#include "server/ai.hpp"

namespace AI {
    void init(World& world);
    void do_tick(World& world);
}

std::vector<ProvinceId> g_water_provinces;
std::vector<AIManager> ai_man;

void AI::init(World& world) {
    g_water_provinces.reserve(world.provinces.size());
    for(const auto& province : world.provinces)
        if(world.terrain_types[province.terrain_type_id].is_water_body)
            g_water_provinces.push_back(province);
    ai_man.resize(world.nations.size());
    for(auto& ai : ai_man)
        ai.recalc_weights();
}

void AI::do_tick(World& world) {
    // Calculate military strengths of each nation
    std::vector<float> nation_strengths(world.nations.size(), 0.f);
    for(const auto& province : world.provinces) {
        const auto& units = world.unit_manager.get_province_units(province.get_id());
        for(const auto unit_id : units) {
            const auto& unit = world.unit_manager.units[unit_id];
            nation_strengths[unit.owner_id] += unit.get_strength();
        }
    }
    for(const auto& nation : world.nations)
        ai_man[nation].military_strength = nation_strengths[nation];

    // Do the AI turns in parallel
    tbb::combinable<std::vector<std::pair<NationId, NationId>>> alliance_proposals;
    tbb::combinable<std::vector<std::pair<NationId, NationId>>> war_declarations;
    tbb::parallel_for(tbb::blocked_range(world.nations.begin(), world.nations.end()), [&alliance_proposals, &world](auto& nations_range) {
        for(auto& nation : nations_range) {
            auto& ai = ai_man[nation];
            if(!nation.exists()) { // Unconditionally surrender iff at war
                if(!(world.time % world.ticks_per_month) && nation.ai_controlled)
                    for(auto& treaty : world.treaties)
                        for(auto& [other_nation, approval] : treaty.approval_status)
                            if(other_nation == nation)
                                approval = TreatyApproval::ACCEPTED;
                continue;
            }

            // Diplomacy
            if(nation.ai_controlled) {
                // Ally other people also warring the people we're warring
                auto our_strength = ai.military_strength;
                auto enemy_strength = 0.f;
                std::vector<NationId> enemy_ids, ally_ids;
                for(const auto& other : world.nations) {
                    if(other.get_id() != nation.get_id()) {
                        const auto& relation = world.get_relation(nation, other);
                        if(relation.has_war) {
                            enemy_strength += ai_man[other].military_strength;
                            enemy_ids.push_back(other);
                        } else if(relation.is_allied()) {
                            our_strength += ai_man[other].military_strength;
                            ally_ids.push_back(other);
                        }
                    }
                }
                auto advantage = glm::max(our_strength, 1.f) / glm::max(enemy_strength, glm::epsilon<float>());
                if(advantage < ai.strength_threshold) {
                    // The enemy is bigger; so re-evaluate stances
                    for(const auto& other : world.nations) {
                        if(other.get_id() != nation.get_id()) {
                            const auto& relation = world.get_relation(nation, other);
                            if(!relation.has_war) {
                                // Propose an alliance iff we have mutual enemies
                                for(const auto enemy_id : enemy_ids) {
                                    const auto& enemy_rel = world.get_relation(other, enemy_id);
                                    if(enemy_rel.has_war)
                                        alliance_proposals.local().emplace_back(nation, other);
                                }
                            }
                        }
                    }
                }
            }

            // War/unit management
            if(nation.ai_do_cmd_troops) {
                ai.calc_weights(nation);
                ai.collect_eval_provinces(world, nation);
                ai.calc_nation_risk(world, nation);
                ai.calc_province_risk(world, nation);

                // Move units to provinces with highest risk
                for(const auto province_id : ai.eval_provinces) {
                    const auto& province = world.provinces[province_id];
                    const auto& unit_ids = world.unit_manager.get_province_units(province_id);
                    for(const auto unit_id : unit_ids) {
                        auto& unit = world.unit_manager.units[unit_id];
                        if(unit.owner_id != nation || !unit.can_move()) continue;

                        bool can_set_target = true;
                        if(unit.has_target_province())
                            can_set_target = ai.get_rand() > ai.override_threshold;

                        if(can_set_target) {
                            const auto& highest_risk = ai.get_highest_priority_province(world, province, unit);
                            // Above we made sure high_risk province is valid for us to step in
                            //if(!world.terrain_types[highest_risk->terrain_type_id].is_water_body) continue;
                            if(highest_risk.get_id() != province.get_id())
                                unit.set_target(highest_risk);
                        }
                    }
                }
            }

            // Build units inside buildings that are not doing anything
            for(const auto province_id : nation.controlled_provinces) {
                auto& province = world.provinces[province_id];
                for(const auto& building_type : world.building_types) {
                    if(!building_type.can_build_military()) continue;
                    auto& building = province.buildings[static_cast<size_t>(building_type.get_id())];
                    if(!building.can_do_output(province, building_type.input_ids))
                        continue;
                    /// @todo Actually produce something appropriate
                    auto& unit_type = world.unit_types[rand() % world.unit_types.size()];
                    building.work_on_unit(unit_type);
                }
            }

            // How do we know which factories we should be investing om? We first have to know
            // if we can invest them in the first place, which is what "can_directly_control_factories"
            // answers for us.
            if(nation.can_directly_control_factories()) {
                for(const auto province_id : nation.controlled_provinces) {
                    auto& province = world.provinces[province_id];

                    // Obtain list of products
                    struct ProductInfo {
                        decltype(province.products.begin()) product;
                        CommodityId id;
                    };
                    std::vector<ProductInfo> v;
                    for(auto it = province.products.begin(); it != province.products.end(); it++)
                        v.push_back(ProductInfo{ it, std::distance(province.products.begin(), it) });
                    // Sort by most important to fullfill (higher D/S ratio)
                    std::sort(v.begin(), v.end(), [&](const auto& a, const auto& b) {
                        return a.product->ds_ratio() > b.product->ds_ratio();
                    });

                    for (const auto& product_info : v) {
                        // We now have the most demanded product indice, we will now find an industry type
                        // we should invest on to make more of that product
                        const auto it = std::find_if(world.building_types.begin(), world.building_types.end(), [&](const auto& e) {
                            return e.output_id == product_info.id;
                        });

                        if(it != world.building_types.end()) {
                            // For now, investing 15% of the budget on this industry seems reasonable
                            const auto investment = nation.budget / 15.f;
                            nation.budget -= investment;
                            province.buildings[*it].estate_state.invest(investment);
                            break;
                        }
                    }
                }
            }
        }
    });

    /// @todo AI reject alliance proposals and so on; also allow the player to reject
    /// their alliance proposals too!
    alliance_proposals.combine_each([&world](const auto& alliance_proposals_range) {
        for(const auto& [nation_id, other_id] : alliance_proposals_range) {
            const auto& nation = world.nations[nation_id];
            const auto& other_nation = world.nations[other_id];
            world.fire_special_event("special_alliance", nation.ref_name.c_str(), other_nation.ref_name.c_str());
        }
    });
}
