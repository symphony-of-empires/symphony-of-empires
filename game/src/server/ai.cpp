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
//      client/ai.cpp
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
#include "policy.hpp"
#include "province.hpp"
#include "server/economy.hpp"
#include "world.hpp"
#include "action.hpp"
#include "server/lua_api.hpp"
#include "server/server_network.hpp"

namespace AI {
    void init(World& world);
    void do_tick(World& world);
}

static std::vector<ProvinceId> g_water_provinces;
class AIManager {
    /// @brief Reshuffle weights of the AI
    void recalc_weights() {
        war_weight = static_cast<float>(rand() % 5000) / 100.f;
        unit_battle_weight = static_cast<float>(rand() % 5000) / 100.f;
        unit_exist_weight = static_cast<float>(rand() % 5000) / 100.f;
        coastal_weight = static_cast<float>(rand() % 5000) / 100.f;
        reconquer_weight = static_cast<float>(rand() % 5000) / 100.f;
    }
public:
    float war_weight = 1.f; // Base weight for war
    float unit_battle_weight = 1.f; // Attraction of units into entering on pre-existing battles
    float unit_exist_weight = 1.f; // Weight of an unit by just existing
    float coastal_weight = 1.f; // Importance given to coastal provinces
    float reconquer_weight = 1.f; // Weight to reconquer lost **home**land
    std::vector<float> nations_risk_factor;
    std::vector<float> potential_risk;
    std::vector<ProvinceId> eval_provinces;
    size_t last_constrolled_cnt = 0;
    size_t gains = 0;
    size_t losses = 0;

    /// @brief Recalculate weights iff losing territory
    void calc_weights(const Nation& nation) {
        auto new_controlled_cnt = nation.controlled_provinces.size();
        if(last_constrolled_cnt < new_controlled_cnt)
            gains += new_controlled_cnt - last_constrolled_cnt;
        else if(last_constrolled_cnt > new_controlled_cnt)
            losses += last_constrolled_cnt - new_controlled_cnt;
        
        if(losses >= gains) {
            losses -= gains;
            gains = 0;
            recalc_weights();
        }
        last_constrolled_cnt = new_controlled_cnt;
    }

    void collect_eval_provinces(const World& world, const Nation& nation) {
        // Provinces that can be evaluated in our AI
        eval_provinces = g_water_provinces;
        eval_provinces.reserve(world.provinces.size());
        eval_provinces.insert(eval_provinces.end(), nation.controlled_provinces.begin(), nation.controlled_provinces.end()); // Add our own nation's province ids
        for(const auto& other : world.nations) {
            if(&other != &nation) {
                const auto& relation = world.get_relation(nation, other);
                if(relation.has_landpass())
                    eval_provinces.insert(eval_provinces.end(), other.controlled_provinces.begin(), other.controlled_provinces.end());
            }
        }
    }

    float get_nation_risk(const World& world, const Nation& nation, const Nation& other) {
        const auto& relation = world.get_relation(nation, other);
        float factor = relation.has_war ? war_weight : -(relation.relation / 200.f);
        return factor;
    }

    void calc_nation_risk(const World& world, const Nation& nation) {
        nations_risk_factor.resize(world.nations.size(), 1.f);
        for(const auto& other : world.nations)
            if(&other != &nation)
                nations_risk_factor[other] = get_nation_risk(world, nation, other);
        nations_risk_factor[nation] = 0.f;
    }

    void calc_province_risk(const World& world, const Nation& nation) {
        // Calculate potential risk for every province
        potential_risk.resize(world.provinces.size(), 1.f);
        for(const auto province_id : eval_provinces) {
            const auto& province = world.provinces[province_id];
            for(const auto neighbour_id : province.neighbour_ids) {
                auto& neighbour = world.provinces[neighbour_id];
                if(world.terrain_types[neighbour.terrain_type_id].is_water_body) continue;
                auto draw_in_force = 1.f;
                // The "cooling" value which basically makes us ignore some provinces with lots of defenses
                // so we don't rack up deathstacks on a border with some micronation
                const auto& unit_ids = world.unit_manager.get_province_units(neighbour_id);
                for(const auto unit_id : unit_ids) {
                    auto& unit = world.unit_manager.units[unit_id];
                    auto& unit_owner = world.nations[unit.owner_id];
                    const auto unit_weight = unit_exist_weight * (unit.on_battle ? unit_battle_weight : 1.f);
                    draw_in_force += unit.get_strength() * unit_weight * nations_risk_factor[unit_owner];
                }
                // Try to recover our own lost provinces
                if(neighbour.owner_id == nation && neighbour.controller_id != nation)
                    draw_in_force *= reconquer_weight;
                if(province.is_coastal)
                    draw_in_force *= coastal_weight;
                if(Nation::is_valid(neighbour.controller_id)) // Only if neighbour has a controller
                    draw_in_force *= nations_risk_factor[neighbour.controller_id];
                potential_risk[province_id] += draw_in_force; // Spread out the heat
                potential_risk[neighbour_id] += potential_risk[province_id] / province.neighbour_ids.size();
            }
        }
    }

    const Province* get_highest_priority_province(const World& world, const Province* start, const Unit& unit) {
        // See which province has the most potential_risk so we cover it from potential threats
        const auto* highest_risk = start;
        for(const auto neighbour_id : start->neighbour_ids) {
            const auto& neighbour = world.provinces[neighbour_id];
            if(!world.unit_types[unit.type_id].is_naval && world.terrain_types[neighbour.terrain_type_id].is_water_body) continue;
            if(rand() % 2 == 0) continue;
            // Uncolonized land is unsteppable
            if(Nation::is_invalid(neighbour.controller_id) && !world.terrain_types[neighbour.terrain_type_id].is_water_body)
                continue;

            if(potential_risk[neighbour_id] > potential_risk[highest_risk->get_id()]) {
                if(Nation::is_valid(neighbour.controller_id) && neighbour.controller_id != unit.owner_id) {
                    const auto& relation = world.get_relation(neighbour.controller_id, unit.owner_id);
                    if(relation.has_landpass())
                        highest_risk = &neighbour;
                } else
                    highest_risk = &neighbour;
            }
        }
        return highest_risk;
    }
};
static std::vector<AIManager> ai_man;

void AI::init(World& world) {
    g_water_provinces.reserve(world.provinces.size());
    for(const auto& province : world.provinces)
        if(world.terrain_types[province.terrain_type_id].is_water_body)
            g_water_provinces.push_back(province);
    ai_man.resize(world.nations.size(), AIManager{});
}

void AI::do_tick(World& world) {
    // Do the AI turns in parallel
    tbb::parallel_for(tbb::blocked_range(world.nations.begin(), world.nations.end()), [&world](auto& nations_range) {
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
                        if(Province::is_valid(unit.get_target_province_id())) continue;
                        const auto& unit_province = world.provinces[unit.province_id()];
                        const auto* highest_risk = ai.get_highest_priority_province(world, &unit_province, unit);
                        // Above we made sure high_risk province is valid for us to step in
                        if(highest_risk == &unit_province || rand() % 32 == 0) {
                            auto it = highest_risk->neighbour_ids.begin();
                            std::advance(it, rand() % highest_risk->neighbour_ids.size());
                            highest_risk = &world.provinces[*it];
                            if(Nation::is_invalid(highest_risk->controller_id) && !world.terrain_types[highest_risk->terrain_type_id].is_water_body) continue;
                            if(Nation::is_valid(highest_risk->controller_id) && highest_risk->controller_id != unit.owner_id) {
                                const auto& relation = world.get_relation(highest_risk->controller_id, unit.owner_id);
                                if(!relation.has_landpass()) continue;
                            }
                            if(highest_risk == &unit_province) continue;
                        }
                        unit.set_target(*highest_risk);
                    }
                }
            }

            // Build units inside buildings that are not doing anything
            for(const auto province_id : nation.controlled_provinces) {
                auto& province = world.provinces[province_id];
                for(const auto& building_type : world.building_types) {
                    if(!building_type.can_build_military()) continue;
                    auto& building = province.buildings[static_cast<size_t>(building_type.get_id())];
                    if(UnitType::is_valid(building.working_unit_type_id) || !building.can_do_output()) continue;
                    /// @todo Actually produce something appropriate
                    auto& unit_type = world.unit_types[rand() % world.unit_types.size()];
                    building.working_unit_type_id = unit_type;
                    building.req_goods_for_unit = unit_type.req_goods;
                }
            }
        }
    });
}
