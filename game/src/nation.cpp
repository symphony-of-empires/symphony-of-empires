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
#include "technology.hpp"
#include "province.hpp"
#include "ideology.hpp"
#include "world.hpp"

//
// Nation
//

/// @brief Declare war
/// @todo Make some form of "WarParticipationRequest" so we don't force allies to join
/// and we also make sure betrayals are possible
void Nation::declare_war(Nation& nation, std::vector<TreatyClause::BaseClause*> clauses) {
    auto& world = World::get_instance();
    auto* war = new War();

    //Eng3D::Log::debug("game", ref_name + " has declared war on " + nation.ref_name);
    war->wargoals = clauses;

    // Recollect offenders
    // - Those who are allied to us
    for(size_t i = 0; i < world.nations.size(); i++) {
        if(&world.nations[i] == this || &world.nations[i] == &nation) continue;
        const auto& relation = world.get_relation(i, *this);
        if(relation.is_allied() || world.nations[i].puppet_master_id == *this)
            war->attacker_ids.push_back(world.nations[i]);
    }
    war->attacker_ids.push_back(*this);

    // Recollect defender_ids
    // - Those who are on a defensive pact with the target
    // - Those who are allied with the target
    // - And those who aren't already attacking
    for(size_t i = 0; i < world.nations.size(); i++) {
        if(&world.nations[i] == this || &world.nations[i] == &nation) continue;
        if(std::find(war->attacker_ids.begin(), war->attacker_ids.end(), world.nations[i]) != war->attacker_ids.end()) continue;
        const auto& relation = world.get_relation(i, nation);
        if(relation.is_allied() || world.nations[i].puppet_master_id == nation)
            war->defender_ids.push_back(world.nations[i]);
    }
    war->defender_ids.push_back(nation);

    /// @todo We have to remove these since some are duplicated
    /// at the best case we should probably just put them on the attacking side?
    /// or also we could just make it an event where everyone is involved yknow
    for(auto attacker : war->attacker_ids) {
        for(size_t i = 0; i < war->defender_ids.size(); i++) {
            if(attacker == war->defender_ids[i]) {
                war->defender_ids.erase(war->defender_ids.begin() + i);
                i--;
                continue;
            }
        }
    }

    // Attackers are at war with the defender_ids
    for(auto attacker_id : war->attacker_ids) {
        for(auto defender_id : war->defender_ids) {
            assert(attacker_id != defender_id);
            auto& attacker = world.nations[attacker_id];
            auto& defender = world.nations[defender_id];
            if(attacker.puppet_master_id == defender_id)
                attacker.puppet_master_id = NationId(-1);
            else if(defender.puppet_master_id == attacker_id)
                defender.puppet_master_id = NationId(-1);
            
            auto& relation = world.get_relation(defender_id, attacker_id);
            relation.has_war = true; // Declare war
            relation.alliance = 0.f;
            relation.relation = -100.f;
        }
    }

    Eng3D::Log::debug("game", "Attackers");
    for(const auto& attacker_id : war->attacker_ids) {
        auto& attacker = world.nations[attacker_id];
        Eng3D::Log::debug("game", attacker.ref_name.get_string());
    }
    Eng3D::Log::debug("game", "Defenders");
    for(const auto& defender_id : war->defender_ids) {
        auto& defender = world.nations[defender_id];
        Eng3D::Log::debug("game", defender.ref_name.get_string());
    }
    war->name = translate_format("War of %s against %s", this->name.c_str(), nation.name.c_str());
    Eng3D::Log::debug("game", war->name.get_string());
    world.insert(*war);
}

bool Nation::is_ally(const Nation& nation) const {
    const auto& world = World::get_instance();
    const auto& relation = world.get_relation(*this, nation);
    if(relation.has_war) return false;
    return true;
}

bool Nation::is_enemy(const Nation& nation) const {
    const auto& world = World::get_instance();
    const auto& relation = world.get_relation(*this, nation);
    if(relation.has_war) return true;
    return false;
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
    // No parliament? No referendum
    if(current_policy.legislative_parliament != true) {
        this->current_policy = policies;
        Eng3D::Log::debug("game", "Parliament-less policy passed!");
    }

    auto rand = Eng3D::get_local_generator();
    unsigned int approvals = 0, disapprovals = 0;
    std::vector<Pop*> disapprovers, approvers;
    for(const auto province_id : owned_provinces) {
        auto& province = World::get_instance().provinces[province_id];
        for(auto& pop : province.pops) {
            const Policies& pop_policies = pop.get_ideology().policies;
            // To "cheese it up" we mix some ideologies of the people, randomly
            for(size_t i = 0; i < World::get_instance().ideologies.size(); i++) {
                pop.ideology_approval[i] += std::fmod(rand() / 1000.f, current_policy.difference(pop_policies));
                pop.ideology_approval[i] = glm::clamp<float>(pop.ideology_approval[i], -1.f, 1.f); // Clamp
            }
            
            // Disapproval of old (current) policy
            const int old_disapproval = current_policy.difference(pop_policies);
            // Dissaproval of new policy
            const int new_disapproval = policies.difference(pop_policies);
            if(new_disapproval < old_disapproval) {
                approvals += pop.size;
                disapprovers.push_back(&pop);
            } else {
                disapprovals += pop.size;
                approvers.push_back(&pop);
            }
        }
    }

    // Policy is enacted and passed parliament
    if(approvals > disapprovals) {
        // Set new policy
        this->current_policy = policies;
        // All who agreed are happy
        for(auto& pop : approvers)
            pop->militancy *= 0.8f;
        // All who disagreed are angered
        for(auto& pop : disapprovers) {
            pop->militancy += 0.2f;
            pop->militancy *= 1.2f;
        }
        Eng3D::Log::debug("game", "New enacted policy passed parliament!");
    }
    // Legislation does not make it into the official law
    else {
        // All people who agreed gets angered
        for(auto& pop : disapprovers)
            pop->militancy *= 0.8f;
        // All people who disagreed gets happy
        for(auto& pop : approvers) {
            pop->militancy += 0.2f;
            pop->militancy *= 1.2f;
        }
        Eng3D::Log::debug("game", "New enacted policy did not made it into the parliament!");
    }
}

/// @brief Checks if a LANGUAGE is part of one of our accepted languages
bool Nation::is_accepted_language(const Language& language) const {
    return language_discrim[World::get_instance().get_id(language)] >= 0.5f;
}

/// @brief Checks if a RELIGION is part of one of our accepted relgion
bool Nation::is_accepted_religion(const Religion& religion) const {
    return religion_discrim[World::get_instance().get_id(religion)] >= 0.5f;
}

/// @brief Gets the total tax applied to a POP depending on their "wealth"
/// (not exactly like that, more like by their type/status)
float Nation::get_tax(const Pop& pop) const {
    float base_tax = 1.f;
    if(World::get_instance().pop_types[pop.type_id].social_value <= 0.3f)
        return current_policy.poor_flat_tax * base_tax;
    // For the medium class
    else if(World::get_instance().pop_types[pop.type_id].social_value <= 0.6f)
        return current_policy.med_flat_tax * base_tax;
    // For the high class
    else if(World::get_instance().pop_types[pop.type_id].social_value <= 1.f)
        return current_policy.rich_flat_tax * base_tax;
    return base_tax;
}

#include "client/game_state.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"

/// @brief Gives this nation a specified province (for example on a treaty)
void Nation::give_province(Province& province) {
    auto& world = World::get_instance();
    if(province.owner_id == this->get_id()) return;
    if(Nation::is_valid(province.owner_id)) {
        auto& provinces = world.nations[province.owner_id].owned_provinces;
        std::erase(provinces, province);
    }
    this->owned_provinces.push_back(province);
    province.owner_id = this->get_id();
    this->control_province(province);
    // Update the province changed
    world.province_manager.mark_province_owner_changed(province);
}

void Nation::control_province(Province& province) {
    auto& world = World::get_instance();
    if(province.controller_id == this->get_id()) return;
    if(Nation::is_valid(province.controller_id)) {
        auto& provinces = world.nations[province.controller_id].controlled_provinces;
        std::erase(provinces, province);
    }
    this->controlled_provinces.push_back(province);
    province.controller_id = this->get_id();

    // Update the province changed
    world.province_manager.mark_province_control_changed(province);

    // Cancel the unit construction projects
    province.cancel_construction_project();
    if(province.controller_id != province.owner_id)
        for(auto& pop : province.pops)
            pop.militancy += 0.1f;
}

const NationClientHint& Nation::get_client_hint() const {
    return this->client_hints[this->ideology_id];
}

float Nation::get_research_points() const {
    float research = 0.f;
    for(const auto province_id : this->owned_provinces) {
        const auto& province = World::get_instance().provinces[province_id];
        for(const auto& pop : province.pops)
            research += pop.size * pop.literacy;
        if(research && !province.is_populated())
            research /= province.pops.size();
    }
    return research / 100.f;
}

bool Nation::can_research(const Technology& technology) const {
    // Only military/navy technologies can actually be researched
    // or not? wink, wink ;)
    //if(tech->type != TechnologyType::MILITARY && tech->type != TechnologyType::NAVY) return false;

    // All required technologies for this one must be researched
    for(const auto& req_tech_id : technology.req_technologies)
        if(research[req_tech_id] > 0.f)
            return false;
    return true;
}

void Nation::change_research_focus(const Technology& technology) {
    // Can't have already researched it (it would be dumb to re-research
    if(!this->research[World::get_instance().get_id(technology)]) return;
    // Must be able to research it
    if(!this->can_research(technology)) return;
    this->focus_tech_id = technology;
}

std::vector<NationId> Nation::get_allies(void) const {
    const auto& world = World::get_instance();

    std::vector<NationId> nation_ids;
    for(const auto& nation : world.nations) {
        const auto& relation = world.get_relation(*this, nation);
        if(relation.is_allied())
            nation_ids.push_back(nation);
    }
    return nation_ids;
}
