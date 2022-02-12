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
//      nation.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "nation.hpp"
#include "world.hpp"
#include "technology.hpp"

#include "unified_render/log.hpp"

//
// Nation
//

/*
Nation::Nation(void) {

}

Nation::~Nation(void) {
    
}
//*/

// Declare war
// TODO: Make some form of "WarParticipationRequest" so we don't force allies to join
// and we also make sure betrayals are possible
void Nation::declare_war(Nation& nation, std::vector<TreatyClause::BaseClause*> clauses) {
    World& world = World::get_instance();
    auto* war = new War();

    UnifiedRender::Log::debug("game", ref_name + " has declared war on " + nation.ref_name);

    war->wargoals = clauses;

    // Recollect offenders
    // - Those who are allied to us
    for(unsigned int i = 0; i < this->relations.size(); i++) {
        const auto& relation = this->relations[i];

        if(relation.has_alliance) {
            war->attackers.push_back(world.nations[i]);
        }
    }
    war->attackers.push_back(this);

    UnifiedRender::Log::debug("game", "Attackers");
    for(const auto& attacker : war->attackers) {
        UnifiedRender::Log::debug("game", attacker->ref_name);
    }

    // Recollect defenders
    // - Those who are on a defensive pact with the target
    // - Those who are allied with the target
    for(unsigned int i = 0; i < nation.relations.size(); i++) {
        const auto& relation = nation.relations[i];

        if(relation.has_alliance || relation.has_defensive_pact) {
            war->defenders.push_back(world.nations[i]);
        }
    }
    war->defenders.push_back(&nation);

    // Attackers are at war with the defenders
    for(auto& attacker : war->attackers) {
        for(auto& defender : war->defenders) {
            // Bilateral war
            attacker->relations[world.get_id(defender)].has_war = true;
            attacker->relations[world.get_id(defender)].has_alliance = false;
            attacker->relations[world.get_id(defender)].has_defensive_pact = false;

            defender->relations[world.get_id(attacker)].has_war = true;
            defender->relations[world.get_id(attacker)].has_alliance = false;
            defender->relations[world.get_id(attacker)].has_defensive_pact = false;
        }
    }

    UnifiedRender::Log::debug("game", "Defenders");
    for(const auto& defender : war->defenders) {
        UnifiedRender::Log::debug("game", defender->ref_name);
    }

    war->name = "War of " + this->name + " against " + nation.name;
    UnifiedRender::Log::debug("game", war->name);

    world.insert(war);
}

bool Nation::is_ally(const Nation& nation) {
    const World& world = World::get_instance();

    if(nation.relations[world.get_id(this)].has_war || this->relations[world.get_id(&nation)].has_war) {
        return false;
    }
    return true;
}

bool Nation::is_enemy(const Nation& nation) {
    const World& world = World::get_instance();

    if(nation.relations[world.get_id(this)].has_war || this->relations[world.get_id(&nation)].has_war) {
        return true;
    }
    return false;
}

// Whetever the nation exists at all - we cannot add nations in-game so we just check
// if the nation "exists" at all, this means that it has a presence and a goverment
// must own atleast 1 province
bool Nation::exists(void) {
    return owned_provinces.size() > 0;
}

inline void Nation::do_diplomacy() {
    // TODO: Fix this formula which is currently broken
    //diplomatic_timer = std::max((60 * 48) - std::min(10.f * 48.f, prestige / 100.f), 4.f);
    diplomatic_timer = 48;
}

inline bool Nation::can_do_diplomacy() {
    return (diplomatic_timer == 0);
}

void Nation::increase_relation(Nation& target) {
    //if(!can_do_diplomacy()) return;

    const World& world = World::get_instance();
    this->relations[world.get_id(&target)].relation += 5.f;
    target.relations[world.get_id(this)].relation += 5.f;

    UnifiedRender::Log::debug("game", ref_name + " increases relations with " + target.ref_name);
    this->do_diplomacy();
}

void Nation::decrease_relation(Nation& target) {
    //if(!can_do_diplomacy()) return;

    const World& world = World::get_instance();
    this->relations[world.get_id(&target)].relation -= 5.f;
    target.relations[world.get_id(this)].relation -= 5.f;

    UnifiedRender::Log::debug("game", ref_name + " decreases relations with " + target.ref_name);
    this->do_diplomacy();
}

// Automatically relocates the capital of a nation to another province
// Use this when a treaty makes a nation lose it's capital
void Nation::auto_relocate_capital(void) {
    auto best_candidate = std::max_element(owned_provinces.begin(), owned_provinces.end(),
        [](const auto* lhs, const auto* rhs) {
        return (lhs->total_pops() < rhs->total_pops());
    });
    capital = *best_candidate;
}

// Enacts a policy on a nation
// @return false if policy draft failed to be applied, true if policy passed and is in-effect
void Nation::set_policy(Policies& policies) {
    // No parliament? No referendum
    if(current_policy.legislative_parliament != true) {
        this->current_policy = policies;
        UnifiedRender::Log::debug("game", "Parliament-less policy passed!");
        return;
    }

    unsigned int approvals = 0, disapprovals = 0;
    std::vector<Pop*> disapprovers, approvers;
    for(const auto& province : this->owned_provinces) {
        for(auto& pop : province->pops) {
            // Must have the minimum required social value
            // the min-social-value is taken from the new enacted policy
            if(pop.type->social_value < policies.min_sv_for_parliament) {
                continue;
            }

            const Policies& pop_policies = pop.get_ideology()->policies;

            // Disapproval of old (current) policy
            const int old_disapproval = current_policy.difference(pop_policies);

            // Dissaproval of new policy
            const int new_disapproval = policies.difference(pop_policies);

            if(new_disapproval < old_disapproval) {
                approvals += pop.size;
                disapprovers.push_back(&pop);
            }
            else {
                disapprovals += pop.size;
                approvers.push_back(&pop);
            }
        }
    }

    // Policy is enacted and passed parliament
    if(approvals > disapprovals) {
        // Set new policy
        this->current_policy = policies;

        // All people who agreed gets happy
        for(auto& pop : approvers) {
            pop->militancy /= std::min<UnifiedRender::Decimal>(pop->con, 0.1f);
        }

        // All people who disagreed gets angered
        for(auto& pop : disapprovers) {
            pop->militancy *= std::min<UnifiedRender::Decimal>(pop->con, 0.1f);
        }
        UnifiedRender::Log::debug("game", "New enacted policy passed parliament!");
    }
    // Legislation does not make it into the official law
    else {
        // All people who agreed gets angered
        for(auto& pop : approvers) {
            pop->militancy *= std::min<UnifiedRender::Decimal>(pop->con, 0.1f);
        }

        // All people who disagreed gets happy
        for(auto& pop : disapprovers) {
            pop->militancy /= std::min<UnifiedRender::Decimal>(pop->con, 0.1f);
        }
        UnifiedRender::Log::debug("game", "New enacted policy did not made it into the parliament!");
    }
    return;
}

// Checks if a POP is part of one of our accepted cultures
bool Nation::is_accepted_culture(const Pop& pop) const {
    return accepted_cultures.find(pop.culture) != accepted_cultures.end();
}
// Checks if a CULTURE is part of one of our accepted cultures
bool Nation::is_accepted_culture(const Culture& culture) const {
    return accepted_cultures.find((Culture*)&culture) != accepted_cultures.end();
}

// Checks if a POP is part of one of our accepted religion
bool Nation::is_accepted_religion(const Pop& pop) const {
    return accepted_religions.find(pop.religion) != accepted_religions.end();
}
// Checks if a RELIGION is part of one of our accepted relgion
bool Nation::is_accepted_religion(const Religion& religion) const {
    return accepted_religions.find((Religion*)&religion) != accepted_religions.end();
}

// Gets the total tax applied to a POP depending on their "wealth"
// (not exactly like that, more like by their type/status)
UnifiedRender::Decimal Nation::get_tax(const Pop& pop) const {
    UnifiedRender::Decimal base_tax = 1.f;

    if(!is_accepted_culture(pop) && !is_accepted_religion(pop)) {
        // Exterminate imposes a scale of 3(+1), which will be enough to drive off most POPs
        const int scale = 1 + current_policy.treatment;
        base_tax *= 2 * scale;
    }

    if(pop.type->social_value <= 1.f) {
        return current_policy.poor_flat_tax * base_tax;
    }
    // For the medium class
    else if(pop.type->social_value <= 2.f) {
        return current_policy.med_flat_tax * base_tax;
    }
    // For the high class
    else if(pop.type->social_value <= 3.f) {
        return current_policy.rich_flat_tax * base_tax;
    }
    return base_tax;
}

// Gives this nation a specified province (for example on a treaty)
void Nation::give_province(Province& province) {
    World& world = World::get_instance();

    const Nation::Id nation_id = world.get_id(this);
    const Province::Id province_id = world.get_id(&province);

    //std::scoped_lock lock(world.nation_changed_tiles_mutex);

    world.nations[nation_id]->owned_provinces.insert(world.provinces[province_id]);
    world.provinces[province_id]->owner = world.nations[nation_id];
    world.provinces[province_id]->controller = world.nations[nation_id];
    return;
}

const NationClientHint& Nation::get_client_hint(void) {
    // Find match
    for(const auto& hint : client_hints) {
        if(hint.ideology == ideology) {
            return hint;
        }
    }

    // 2nd search: Find a hint that is fallback
    for(const auto& hint : client_hints) {
        if(hint.ideology == nullptr) {
            return hint;
        }
    }

    if(client_hints.empty()) {
        NationClientHint tmp_hint;
        tmp_hint.color = rand();
        tmp_hint.alt_name = this->ref_name + "_MISSING_CLIENTHINT";
        tmp_hint.ideology = World::get_instance().ideologies[0];
        client_hints.push_back(tmp_hint);
        return client_hints[0];
    }
    return client_hints[0];
}

UnifiedRender::Decimal Nation::get_research_points(void) const {
    UnifiedRender::Decimal research = 0.f;
    for(const auto& province : this->owned_provinces) {
        for(const auto& pop : province->pops) {
            research += pop.size * pop.literacy;
        }

        if(research && !province->pops.empty()) {
            research /= province->pops.size();
        }
    }
    return research / 100.f;
}

bool Nation::can_research(const Technology* tech) const {
    // Only military/navy technologies can actually be researched
    // or not? wink, wink ;)
    //if(tech->type != TechnologyType::MILITARY && tech->type != TechnologyType::NAVY) return false;

    // All required technologies for this one must be researched
    for(const auto& req_tech : tech->req_technologies) {
        if(research[World::get_instance().get_id(req_tech)] > 0.f) {
            return false;
        }
    }
    return true;
}

void Nation::change_research_focus(Technology* tech) {
    // Can't have already researched it (it would be dumb to re-research
    if(!research[World::get_instance().get_id(tech)]) {
        return;
    }

    // Must be able to research it
    if(!can_research(tech)) {
        return;
    }

    focus_tech = tech;
}

std::vector<Nation*> Nation::get_allies(void) {
    World& world = World::get_instance();

    std::vector<Nation*> list;
    for(const auto& nation : world.nations) {
        if(relations[world.get_id(nation)].has_alliance) {
            list.push_back(nation);
        }
    }
    return list;
}

/*float Nation::get_industry_output_rate() {
    for(const auto& p: techs) {

    }
}*/

UnifiedRender::Decimal Nation::get_industry_output_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->industry_input_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_industry_input_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->industry_input_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_workers_needed_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->workers_needed_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_salary_paid_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->salary_paid_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_delivery_cost_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->delivery_cost_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_literacy_learn_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->literacy_learn_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_reproduction_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->reproduction_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_death_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->death_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_militancy_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->militancy_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_con_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->con_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_life_needs_met_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->life_needs_met_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_everyday_needs_met_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->everyday_needs_met_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_luxury_needs_met_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->luxury_needs_met_mod;
    }
    return c;
}

UnifiedRender::Decimal Nation::get_immigration_attraction_mod(void) {
    UnifiedRender::Decimal c = 1.f;
    for(const auto& mod : modifiers) {
        c += mod->immigration_attraction;
    }
    return c;
}
