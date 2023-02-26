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
//      server/ai.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include "eng3d/entity.hpp"
#include "world.hpp"

extern std::vector<ProvinceId> g_water_provinces;

struct AIManager {
    float war_weight = 1.f; // Base weight for war
    float unit_battle_weight = 1.f; // Attraction of units into entering on pre-existing battles
    float unit_exist_weight = 1.f; // Weight of an unit by just existing
    float coastal_weight = 1.f; // Importance given to coastal provinces
    float reconquer_weight = 1.f; // Weight to reconquer lost **home**lan
    float erratic = 1.f; // How erratic we behave when doing military strategies
    float strength_threshold = 0.5f; // How much our enemy has to be in terms of strength
                                     // for us to revaluate our diplomatic stances
    float override_threshold = 1.f; // Threshold for overriding orders of units
    float conqueror_weight = 1.f; // How much this nation is going to conquer others for no reason
    std::vector<float> nations_risk_factor;
    std::vector<float> potential_risk;
    std::vector<ProvinceId> eval_provinces;
    size_t last_constrolled_cnt = 0;
    size_t gains = 0;
    size_t losses = 0;
    float military_strength = 0.f;

    AIManager() {
        potential_risk.resize(g_world.provinces.size(), 1.f);
    }

    float get_rand() const {
        return glm::max<float>(rand() % 100, 1.f) / 100.f;
    }

    /// @brief Reshuffle weights of the AI
    void recalc_weights() {
        war_weight = 1.f + 1.f * this->get_rand();
        unit_battle_weight = 1.f + 1.f * this->get_rand();
        unit_exist_weight = 1.f + 1.f * this->get_rand();
        coastal_weight = 1.f + 1.f * this->get_rand();
        reconquer_weight = 1.f + 1.f * this->get_rand();
        erratic = 1.f + 1.f * this->get_rand();
        strength_threshold = 1.f * this->get_rand();
        override_threshold = 1.f * this->get_rand();
        conqueror_weight = 1.f * this->get_rand();
    }

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
        float factor = relation.has_war ? war_weight : -relation.relation;
        return factor;
    }

    void calc_nation_risk(const World& world, const Nation& nation) {
        nations_risk_factor.resize(world.nations.size());
        for(const auto& other : world.nations)
            if(&other != &nation)
                nations_risk_factor[other] = get_nation_risk(world, nation, other);
        nations_risk_factor[nation] = -1.f;
    }

    void calc_province_risk(const World& world, const Nation& nation) {
        std::fill(potential_risk.begin(), potential_risk.end(), 1.f);
        // Calculate potential risk for every province
        for(const auto province_id : eval_provinces) {
            const auto& province = world.provinces[province_id];
            auto draw_in_force = 1.f;
            // The "cooling" value which basically makes us ignore some provinces with lots of defenses
            // so we don't rack up deathstacks on a border with some micronation
            const auto& unit_ids = world.unit_manager.get_province_units(province_id);
            for(const auto unit_id : unit_ids) {
                const auto& unit = world.unit_manager.units[unit_id];
                const auto& unit_owner = world.nations[unit.owner_id];
                const auto unit_weight = unit.on_battle ? unit_battle_weight : unit_exist_weight;
                draw_in_force += unit.get_strength() * unit_weight * nations_risk_factor[unit_owner];
            }

            if(province.is_coastal)
                draw_in_force *= coastal_weight;
            if(!world.terrain_types[province.terrain_type_id].is_water_body) {
                // Try to recover our own lost provinces
                if(province.owner_id == nation && province.controller_id != nation)
                    draw_in_force *= reconquer_weight;
                draw_in_force += nations_risk_factor[province.controller_id];
            }
            potential_risk[province_id] += draw_in_force;
        }

        // Spread out the heat
        for(const auto province_id : eval_provinces) {
            const auto& province = world.provinces[province_id];
            for(const auto neighbour_id : province.neighbour_ids)
                potential_risk[neighbour_id] += potential_risk[province_id] / province.neighbour_ids.size();
        }
    }

    const Province& get_highest_priority_province(const World& world, const Province& start, const Unit& unit) {
        // See which province has the most potential_risk so we cover it from potential threats
        const auto* highest_risk = &start;
        for(const auto neighbour_id : start.neighbour_ids) {
            const auto& neighbour = world.provinces[neighbour_id];
            // If going to water, must be a naval/amphibious/airplane unit
            //if(!world.unit_types[unit.type_id].is_naval && world.terrain_types[neighbour.terrain_type_id].is_water_body) continue;

            if(potential_risk[neighbour_id] >= potential_risk[highest_risk->get_id()]) {
                if(neighbour.controller_id != unit.owner_id) {
                    const auto& relation = world.get_relation(neighbour.controller_id, unit.owner_id);
                    if(relation.has_landpass())
                        highest_risk = &neighbour;
                } else {
                    highest_risk = &neighbour;
                }
            }
        }
        return *highest_risk;
    }
};

extern std::vector<AIManager> ai_man;
