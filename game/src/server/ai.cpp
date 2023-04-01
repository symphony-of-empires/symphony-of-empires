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

#include "world.hpp"
#include "server/economy.hpp"
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
    for(auto& ai : ai_man) {
        ai.recalc_military_weights();
        ai.recalc_economic_weights();
    }
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
    struct UnitMove {
        NationId nation_id;
        UnitId unit_id;
        ProvinceId target_province_id;
    };
    tbb::combinable<std::vector<UnitMove>> unit_movements;
    struct BuildUnit {
        NationId nation_id;
        UnitTypeId unit_type_id;
        ProvinceId province_id;
        BuildingId building_id;
    };
    tbb::combinable<std::vector<BuildUnit>> build_units;
    struct BuildingInvestment {
        NationId nation_id;
        ProvinceId province_id;
        BuildingId building_id;
        float amount;
    };
    tbb::combinable<std::vector<BuildingInvestment>> building_investments;
    struct LoanPoolUpdate {
        NationId nation_id;
        float new_amount;
        float new_interest;
    };
    tbb::combinable<std::vector<LoanPoolUpdate>> loan_pool_updates;

    // --- UNITS MOVEMENT
    tbb::parallel_for(tbb::blocked_range(world.nations.begin(), world.nations.end()), [&](auto& nations_range) {
        for(auto& nation : nations_range) {
            auto& ai = ai_man[nation];
            if(!nation.ai_do_cmd_troops)
                continue;
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
                        if(highest_risk.get_id() != province.get_id()) {
                            UnitMove cmd{};
                            cmd.nation_id = nation.get_id();
                            cmd.unit_id = unit.get_id();
                            cmd.target_province_id = highest_risk.get_id();
                            unit_movements.local().push_back(cmd);
                        }
                    }
                }
            }
        }
    });

    // --- DIPLOMACY
    tbb::parallel_for(tbb::blocked_range(world.nations.begin(), world.nations.end()), [&](auto& nations_range) {
        for(auto& nation : nations_range) {
            auto& ai = ai_man[nation];
            if(!nation.ai_controlled)
                continue;
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
    });

    // --- WAR EFFORTS
    tbb::parallel_for(tbb::blocked_range(world.nations.begin(), world.nations.end()), [&](auto& nations_range) {
        for(auto& nation : nations_range) {
            auto& ai = ai_man[nation];
            if(!nation.ai_controlled)
                continue;
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

                    BuildUnit cmd{};
                    cmd.nation_id = nation.get_id();
                    cmd.province_id = province_id;
                    cmd.building_id = BuildingId(static_cast<size_t>(building_type.get_id()));
                    cmd.unit_type_id = unit_type.get_id();
                    //build_units.local().push_back(cmd);
                }
            }
        }
    });

    // -- ECONOMY INVESTMENTS
    tbb::parallel_for(tbb::blocked_range(world.nations.begin(), world.nations.end()), [&](auto& nations_range) {
        for(auto& nation : nations_range) {
            auto& ai = ai_man[nation];
            if(!(nation.ai_controlled && nation.can_directly_control_factories()))
                continue;
            // How do we know which factories we should be investing om? We first have to know
            // if we can invest them in the first place, which is what "can_directly_control_factories"
            // answers for us.
            for(const auto province_id : nation.controlled_provinces) {
                auto& province = world.provinces[province_id];

                // Obtain list of products
                std::vector<CommodityId> v;
                v.resize(world.commodities.size());
                for(const auto& commodity : world.commodities)
                    v[commodity] = commodity;
                // Sort by most important to fullfill (higher D/S ratio)
                std::sort(v.begin(), v.end(), [&](const auto& a, const auto& b) {
                    return province.products[a].sd_ratio() > province.products[b].sd_ratio();
                });

                const auto total_demand = std::accumulate(province.products.begin(), province.products.end(), 0.f,
                    [](const auto a, const auto& product) {
                    return a + product.demand;
                });

                /// @todo Dynamically allocate investment funds
                auto investment_alloc = nation.budget * ai.investment_aggressiveness;
                for(const auto& commodity_id : v) {
                    if(investment_alloc <= 0.f) break;

                    // We now have the most demanded product indice, we will now find an industry type
                    // we should invest on to make more of that product
                    const auto it = std::find_if(world.building_types.begin(), world.building_types.end(), [&](const auto& e) {
                        return e.output_id.has_value() && e.output_id.value() == commodity_id;
                    });

                    if(it == world.building_types.end()) continue;

                    // Do not invest in buildings that are not in need of money (eg. not in the red)
                    if(province.buildings[*it].budget > 0.f) continue;

                    const auto& product = province.products[commodity_id];
                    if(product.demand == 0.f) continue;
                    // Obtain the priority to invest here, the more demand this represents of the total
                    // demand on the given province, the more priority it would be given
                    const auto priority = product.demand / total_demand;
                    const auto investment = investment_alloc * priority;
                    investment_alloc -= investment;

                    BuildingInvestment cmd{};
                    cmd.nation_id = nation.get_id();
                    cmd.province_id = province_id;
                    cmd.building_id = BuildingId(static_cast<size_t>(it->get_id()));
                    cmd.amount = investment;
                    building_investments.local().push_back(cmd);
                }
            }
        }
    });

    // -- LOANS
    tbb::parallel_for(tbb::blocked_range(world.nations.begin(), world.nations.end()), [&](auto& nations_range) {
        for(auto& nation : nations_range) {
            auto& ai = ai_man[nation];
            if(!nation.ai_controlled)
                continue;
            /// @todo Dynamic-er interest rates and stuff
            LoanPoolUpdate cmd{};
            cmd.nation_id = nation.get_id();
            cmd.new_amount = 100.f; // 100 yen
            cmd.new_interest = 0.1f; // 10%
            loan_pool_updates.local().push_back(cmd);
        }
    });

    unit_movements.combine_each([&](const auto& list) {
        for(const auto& e : list) {
            const auto& target_province = world.provinces[e.target_province_id];
            auto& unit = world.unit_manager.units[e.unit_id];
            unit.set_target(target_province);
        }
    });

    build_units.combine_each([&](const auto& list) {
        for(const auto& e : list) {
            auto& province = world.provinces[e.province_id];
            const auto& unit_type = world.unit_types[e.unit_type_id];
            province.buildings[e.building_id].work_on_unit(unit_type);
        }
    });

    building_investments.combine_each([&](const auto& list) {
        for(const auto& e : list) {
            auto& nation = world.nations[e.nation_id];
            nation.budget -= e.amount;
            auto& province = world.provinces[e.province_id];
            if(province.controller_id == e.nation_id)
                province.buildings[e.building_id].estate_state.invest(e.amount);
            else
                province.buildings[e.building_id].estate_foreign[e.nation_id].invest(e.amount);
        }
    });

    loan_pool_updates.combine_each([&](const auto& list) {
        for(const auto& e : list) {
            auto& nation = world.nations[e.nation_id];
            const auto old_amount = nation.public_loan_interest;
            nation.budget -= e.new_amount - old_amount;
            nation.public_loan_pool = e.new_amount;
            nation.public_loan_interest = e.new_interest;
        }
    });

    /// @todo AI reject alliance proposals and so on; also allow the player to reject
    /// their alliance proposals too!
    alliance_proposals.combine_each([&](const auto& alliance_proposals_range) {
        for(const auto& [nation_id, other_id] : alliance_proposals_range) {
            const auto& nation = world.nations[nation_id];
            const auto& other_nation = world.nations[other_id];
            world.fire_special_event("special_alliance", nation.ref_name.data(), other_nation.ref_name.data());
        }
    });
}
