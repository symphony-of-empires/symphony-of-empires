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
void AI::init(World& world) {
    g_water_provinces.reserve(world.provinces.size());
    for(const auto& province : world.provinces)
        if(world.terrain_types[province.terrain_type_id].is_water_body)
            g_water_provinces.push_back(province);
}

void AI::do_tick(World& world) {
    // Do the AI turns in parallel
    tbb::parallel_for(tbb::blocked_range(world.nations.begin(), world.nations.end()), [&world](auto& nations_range) {
        for(auto& nation : nations_range) {
            if(!nation.exists()) {
                // Automatically unconditionally surrender once we stop existing
                if(!(world.time % world.ticks_per_month) && nation.ai_controlled)
                    for(auto& treaty : world.treaties)
                        for(auto& [other_nation, approval] : treaty.approval_status)
                            if(other_nation == nation)
                                approval = TreatyApproval::ACCEPTED;
                continue;
            }

            if(nation.ai_do_cmd_troops) {
                auto r = static_cast<float>(rand() % 100) / 100.f;
                auto base_weight = 10.f * r;
                r = static_cast<float>(rand() % 100) / 100.f;
                auto war_weight = 90.f * r; // Weight of war
                r = static_cast<float>(rand() % 100) / 100.f;
                auto unit_battle_weight = 100.5f * r; // Attraction of units into entering on pre-existing battles
                r = static_cast<float>(rand() % 100) / 100.f;
                auto unit_exist_weight = 50.f * r; // Weight of an unit by just existing
                r = static_cast<float>(rand() % 100) / 100.f;
                auto coastal_weight = 150.f * r; // Importance given to coastal provinces
                r = static_cast<float>(rand() % 100) / 100.f;
                auto nation_weight = 100.f * r; // Nations weight
                r = static_cast<float>(rand() % 100) / 100.f;
                auto reconquer_weight = 400.f * r; // Weight to reconquer lost **home**land
                r = static_cast<float>(rand() % 100) / 100.f;
                auto homeland_weight = 100.f * r; // Homeland protection

                std::vector<double> nations_risk_factor(world.nations.size(), 1.f);
                // Provinces that can be evaluated for war
                std::vector<ProvinceId> eval_provinces = g_water_provinces;
                eval_provinces.reserve(g_world.provinces.size());
                eval_provinces.insert(eval_provinces.end(), nation.controlled_provinces.begin(), nation.controlled_provinces.end()); // Add our own nation's province ids

                for(const auto& other : world.nations) {
                    if(&other == &nation) continue;
                    // Here we calculate the risk factor of each nation and then we put it on a lookup table
                    // because we can't afford to calculate this for EVERY FUCKING province
                    const auto& relation = world.get_relation(nation, other);
                    // And add if they're allied with us or let us pass thru
                    if(relation.has_landpass())
                        eval_provinces.insert(eval_provinces.end(), other.controlled_provinces.begin(), other.controlled_provinces.end());

                    constexpr auto relation_max = 100.f;
                    constexpr auto relation_range = 200.f; // Range of relations, the max-min difference
                    // Risk is augmentated when we border any non-ally nation
                    if(!relation.is_allied()) // Makes sure so 200 relations results on 0.0 attraction, while 1 or 0 relations result on 3.9 and 4.0
                        nations_risk_factor[other] = ((relation_range - (relation.relation + relation_max)) / relation_max) * nation_weight;
                    if(relation.has_war)
                        nations_risk_factor[other] = war_weight * nation_weight;
                }
                nations_risk_factor[nation] = homeland_weight * nation_weight;

                std::vector<double> potential_risk(world.provinces.size(), 1.f);
                for(const auto province_id : eval_provinces) {
                    const auto& province = world.provinces[province_id];
                    for(const auto neighbour_id : province.neighbour_ids) {
                        auto& neighbour = g_world.provinces[neighbour_id];
                        if(g_world.terrain_types[neighbour.terrain_type_id].is_water_body) continue;
                        auto draw_in_force = base_weight;
                        // The "cooling" value which basically makes us ignore some provinces with lots of defenses
                        // so we don't rack up deathstacks on a border with some micronation
                        const auto& unit_ids = world.unit_manager.get_province_units(neighbour_id);
                        for(const auto unit_id : unit_ids) {
                            auto& unit = g_world.unit_manager.units[unit_id];
                            // Only account this for units that are of our nation
                            // because enemy units will require us to give more importance to it
                            double unit_strength = (g_world.unit_types[unit.type_id].attack * g_world.unit_types[unit.type_id].defense * unit.size) / 100.f;
                            unit_strength = unit.on_battle ? std::abs(unit_strength) : unit_strength; // Allow stacking on battles
                            // This works because nations which are threatening to us have positive values, so they
                            // basically make the draw_in_force negative, which in turns does not draw away but rather
                            // draw in even more units
                            draw_in_force += unit_strength * (unit.on_battle ? unit_battle_weight : unit_exist_weight);
                        }
                        if(Nation::is_valid(neighbour.controller_id)) // Only if neighbour has a controller
                            draw_in_force *= nations_risk_factor[neighbour.controller_id];
                        // Try to recover our own lost provinces
                        if(neighbour.owner_id == nation && neighbour.controller_id != nation)
                            draw_in_force *= reconquer_weight;
                        potential_risk[province_id] += draw_in_force; // Spread out the heat
                        if(neighbour.is_coastal)
                            potential_risk[province_id] *= coastal_weight;
                        potential_risk[neighbour_id] += potential_risk[province_id] / province.neighbour_ids.size();
                    }
                }

                for(const auto province_id : eval_provinces) {
                    const auto& province = world.provinces[province_id];
                    const auto& unit_ids = world.unit_manager.get_province_units(province_id);
                    for(const auto unit_id : unit_ids) {
                        auto& unit = g_world.unit_manager.units[unit_id];
                        if(unit.owner_id != nation || !unit.can_move()) continue;
                        if(Province::is_valid(unit.get_target_province_id())) continue;
                        
                        const auto& unit_province = world.provinces[unit.province_id()];
                        // See which province has the most potential_risk so we cover it from potential threats
                        const auto* highest_risk = &unit_province;
                        for(const auto neighbour_id : unit_province.neighbour_ids) {
                            const auto& neighbour = g_world.provinces[neighbour_id];
                            if(!world.unit_types[unit.type_id].is_naval && world.terrain_types[neighbour.terrain_type_id].is_water_body) continue;
                            if(rand() % 2 == 0) continue;
                            // Uncolonized land is unsteppable
                            if(Nation::is_invalid(neighbour.controller_id) && !g_world.terrain_types[neighbour.terrain_type_id].is_water_body)
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

                        // Above we made sure high_risk province is valid for us to step in
                        if(highest_risk == &unit_province || rand() % 32 == 0) {
                            auto it = highest_risk->neighbour_ids.begin();
                            std::advance(it, rand() % highest_risk->neighbour_ids.size());
                            highest_risk = &g_world.provinces[*it];
                            if(Nation::is_invalid(highest_risk->controller_id) && !g_world.terrain_types[highest_risk->terrain_type_id].is_water_body) continue;
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
