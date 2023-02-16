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
//      nation.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cassert>

#include "eng3d/log.hpp"
#include "eng3d/rand.hpp"

#include "nation.hpp"
#include "world.hpp"
#include "province.hpp"
#include "world.hpp"
#include "world.hpp"

//
// Nation
//

/// @brief Declare war
/// @todo Make some form of "WarParticipationRequest" so we don't force allies to join
/// and we also make sure betrayals are possible
void Nation::declare_war(Nation& nation, std::vector<TreatyClause::BaseClause*> clauses) {
    auto& world = World::get_instance();
    //Eng3D::Log::debug("game", ref_name + " has declared war on " + nation.ref_name);

    // Recollect offenders
    // - Those who are allied to us
    std::vector<NationId> attacker_ids;
    for(auto& other_nation : world.nations) {
        if(&other_nation == this || &other_nation == &nation) continue;
        
        const auto& relation = world.get_relation(other_nation.get_id(), *this);
        if(relation.is_allied() || other_nation.is_puppeted_by(*this))
            attacker_ids.push_back(other_nation);
    }
    attacker_ids.push_back(*this);

    // Recollect defender_ids
    // - Those who are on a defensive pact with the target
    // - Those who are allied with the target
    // - And those who aren't already attacking
    std::vector<NationId> defender_ids;
    for(auto& other_nation : world.nations) {
        if(&other_nation == this || &other_nation == &nation) continue;

        auto it = std::find(attacker_ids.begin(), attacker_ids.end(), other_nation);
        if(it != attacker_ids.end()) continue;

        const auto& relation = world.get_relation(other_nation.get_id(), nation);
        if(relation.is_allied() || other_nation.is_puppeted_by(nation))
            defender_ids.push_back(other_nation);
    }
    defender_ids.push_back(nation);

    // Attackers are at war with the defender_ids
    for(auto attacker_id : attacker_ids) {
        for(auto defender_id : defender_ids) {
            assert(attacker_id != defender_id);

            auto& attacker = world.nations[attacker_id];
            auto& defender = world.nations[defender_id];
            if(attacker.puppet_master_id == defender_id)
                attacker.is_puppeted = false;
            else if(defender.puppet_master_id == attacker_id)
                defender.is_puppeted = false;
            
            // Declare war
            auto& relation = world.get_relation(defender_id, attacker_id);
            relation.has_war = true;
            relation.alliance = 0.f;
            relation.relation = -1.f;
        }
    }

    Eng3D::Log::debug("game", "Attackers");
    for(const auto& attacker_id : attacker_ids) {
        auto& attacker = world.nations[attacker_id];
        Eng3D::Log::debug("game", attacker.ref_name.get_string());
    }
    Eng3D::Log::debug("game", "Defenders");
    for(const auto& defender_id : defender_ids) {
        auto& defender = world.nations[defender_id];
        Eng3D::Log::debug("game", defender.ref_name.get_string());
    }
}

bool Nation::is_ally(const Nation& nation) const {
    const auto& relation = World::get_instance().get_relation(*this, nation);
    if(relation.has_war) return false;
    return relation.alliance > 0.f;
}

bool Nation::is_enemy(const Nation& nation) const {
    return World::get_instance().get_relation(*this, nation).has_war;
}

/// @brief Automatically relocates the capital of a nation to another province
/// Use this when a treaty makes a nation lose it's capital
void Nation::auto_relocate_capital() {
    const auto& world = World::get_instance();
    auto best_candidate = std::max_element(owned_provinces.cbegin(), owned_provinces.cend(), [&world](const auto& lhs, const auto& rhs) {
        return world.provinces[lhs].total_pops() < world.provinces[rhs].total_pops();
    });
    capital_id = *best_candidate;
}

/// @brief Enacts a policy on a nation
/// @return false if policy draft failed to be applied, true if policy passed and is in-effect
void Nation::set_policy(const Policies& policies) {
    // Set new policy
    this->current_policy = policies;
}

/// @brief Checks if a LANGUAGE is part of one of our accepted languages
bool Nation::is_accepted_language(const Language& language) const {
    return language_acceptance[language] >= 0.5f;
}

/// @brief Checks if a RELIGION is part of one of our accepted relgion
bool Nation::is_accepted_religion(const Religion& religion) const {
    return religion_discrim[religion] >= 0.5f;
}

/// @brief Gets the total tax applied to a POP depending on their "wealth"
/// (not exactly like that, more like by their type/status)
float Nation::get_tax(const Pop&) const {
    return this->current_policy.pop_tax;
}

#include "client/game_state.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"

/// @brief Gives this nation a specified province (for example on a treaty)
void Nation::give_province(Province& province) {
    auto& world = World::get_instance();
    if(province.owner_id == this->get_id()) return;
    
    auto& provinces = world.nations[province.owner_id].owned_provinces;
    std::erase(provinces, province);
    
    this->owned_provinces.push_back(province);
    province.owner_id = this->get_id();
    this->control_province(province);
    // Update the province changed
    world.province_manager.mark_province_owner_changed(province);
}

void Nation::control_province(Province& province) {
    auto& world = World::get_instance();
    if(province.controller_id == this->get_id()) return;

    auto& provinces = world.nations[province.controller_id].controlled_provinces;
    std::erase(provinces, province);

    this->controlled_provinces.push_back(province);
    province.controller_id = this->get_id();

    // Update the province changed
    world.province_manager.mark_province_control_changed(province);

    if (province.owner_id != this->get_id()) {
        // Cancel the unit construction projects
        province.cancel_construction_project();
        if(province.controller_id != province.owner_id)
            for(auto& pop : province.pops)
                pop.militancy += 0.1f;
        
        // All factories lose their money and scale down to 0
        for(auto& building : province.buildings) {
            building.budget = 0.f;
            building.production_scale = 0.f;
        }

        // And all pops lose their money too
        for(auto& pop : province.pops)
            pop.budget = 0.f;
    }
}

const Nation::ClientHint& Nation::get_client_hint() const {
    return this->client_hints[this->ideology_id];
}

float Nation::get_research_points() const {
    float new_research = 0.f;
    for(const auto province_id : this->owned_provinces) {
        const auto& province = World::get_instance().provinces[province_id];
        for(const auto& pop : province.pops)
            new_research += pop.size * pop.literacy;
        if(new_research && !province.is_populated())
            new_research /= province.pops.size();
    }
    return new_research / 100.f;
}

bool Nation::can_research(const Technology& technology) const {
    // All required technologies for this one must be researched
    for(const auto& req_tech_id : technology.req_technologies)
        if(research[req_tech_id] < 1.f)
            return false;
    return true;
}

void Nation::change_research_focus(const Technology& technology) {
    if(!this->research[technology] || !this->can_research(technology))
        return;
    this->focus_tech_id = technology;
}

void Nation::get_allies(std::function<void(const Nation&)> fn) const {
    const auto& world = World::get_instance();
    for(const auto& nation : world.nations)
        if(&nation != this && is_ally(nation))
            fn(nation);
}

const Ideology::Subideology& Nation::get_subideology() const {
    const auto& world = World::get_instance();
    return world.ideologies[this->ideology_id].subideologies[this->subideology_id];
}

bool Nation::can_directly_control_factories() const {
    const auto& subideology = this->get_subideology();
    return subideology.economic.capitalism >= -0.5f ? true : false;
}
