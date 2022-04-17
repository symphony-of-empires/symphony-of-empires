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
//      diplomacy.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/log.hpp"

#include "diplomacy.hpp"
#include "nation.hpp"
#include "world.hpp"

using namespace Diplomacy;

inline bool Diplomacy::is_friend(Nation& us, Nation& them) {
    const NationRelation* relation = &us.relations[g_world->get_id(them)];

    // A high relation means we are friendly <3
    if(relation->relation >= 50.f) {
        return true;
    } else {
        // Well, but maybe our interest is able to determine our friendliness towards them?
        if(relation->interest >= relation->relation) {
            // We cannot be friendly with negative relations
            if(relation->relation <= 0.f) {
                return false;
            }

            // Well, we need to be interested enough to like them
            if(relation->relation >= relation->interest / relation->relation) {
                return true;
            }
        }
        return false;
    }
}

inline bool Diplomacy::is_foe(Nation& us, Nation& them) {
    return !is_friend(us, them);
}

//
// WarReparations
//
unsigned TreatyClause::WarReparations::cost(void) {
    return (receiver->economy_score * (amount * days_duration)) / 100;
}

void TreatyClause::WarReparations::enforce(void) {
    sender->prestige += 0.0001f;
    receiver->prestige -= 0.0001f;
    sender->budget -= amount;
    receiver->budget += amount;
    days_duration--;
}

bool TreatyClause::WarReparations::in_effect(void) const {
    return (days_duration != 0);
}

//
// Humiliate
//
unsigned TreatyClause::Humiliate::cost(void) {
    return (receiver->prestige * (amount * days_duration)) / 100;
}

void TreatyClause::Humiliate::enforce(void) {
    sender->prestige += amount;
    receiver->prestige -= amount;
    days_duration--;
}

bool TreatyClause::Humiliate::in_effect(void) const {
    return (days_duration != 0);
}

//
// LiberateNation
//
unsigned TreatyClause::LiberateNation::cost(void) {
    return 0;
}

void TreatyClause::LiberateNation::enforce(void) {
    // Reduce prestige due to lost lands
    sender->prestige += cost() * 0.0000025f;
    receiver->prestige -= cost() * 0.000005f;

    // Give provinces to this liberated nation
    for(auto& province : provinces) {
        province->owner = liberated;
    }

    // One-time clause
    done = true;
}

bool TreatyClause::LiberateNation::in_effect(void) const {
    return !done;
}

//
// ImposePolicies
//
unsigned TreatyClause::ImposePolicies::cost(void) {
    return imposed.difference(receiver->current_policy);
}

void TreatyClause::ImposePolicies::enforce(void) {
    receiver->current_policy = imposed;
    done = true;
}

bool TreatyClause::ImposePolicies::in_effect(void) const {
    return !done;
}

//
// AnnexProvince
//
unsigned TreatyClause::AnnexProvince::cost(void) {
    return 0;
}

void TreatyClause::AnnexProvince::enforce(void) {
    sender->prestige += cost() * 0.0000025f;
    receiver->prestige -= cost() * 0.000005f;

    // Give provinces to the winner
    for(auto& province : provinces) {
        UnifiedRender::Log::debug("game", "Giving " + province->ref_name + " to " + sender->ref_name + " from " + receiver->ref_name);
        // Change ownership of provinces
        sender->give_province(*province);
    }

    // One-time clause
    done = true;
}

bool TreatyClause::AnnexProvince::in_effect(void) const {
    return !done;
}

//
// Ceasefire
//
unsigned TreatyClause::Ceasefire::cost() {
    return receiver->military_score + receiver->naval_score;
}

void TreatyClause::Ceasefire::enforce() {
    Nation::Id receiver_id = g_world->get_id(*receiver);
    Nation::Id sender_id = g_world->get_id(*sender);

    sender->relations[receiver_id].has_war = false;
    sender->relations[receiver_id].has_truce = true;

    receiver->relations[sender_id].has_war = false;
    receiver->relations[sender_id].has_truce = true;

    days_duration--;
}

bool TreatyClause::Ceasefire::in_effect() const {
    return (days_duration != 0);
}

//
// Puppet
//
unsigned TreatyClause::Puppet::cost() {
    return 0;
}

void TreatyClause::Puppet::enforce() {
    Nation::Id receiver_id = g_world->get_id(*receiver);
    Nation::Id sender_id = g_world->get_id(*sender);

    receiver->puppet_master = sender;

    done = true;
}

bool TreatyClause::Puppet::in_effect() const {
    return !done;
}

// Checks if the specified nations participates in the treaty
bool Treaty::does_participate(Nation& nation) {
    for(auto& status : this->approval_status) {
        if(status.first == &nation) {
            return true;
        }
    }
    return false;
}

// Checks if the treaty has any clause which may still make the treaty be in effect
bool Treaty::in_effect(void) const {
	bool on_effect = false;
	for(const auto& clause : this->clauses) {
		if(clause->type == TreatyClauseType::MONEY) {
			const auto* dyn_clause = static_cast<const TreatyClause::WarReparations*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::ANNEX_PROVINCES) {
			const auto* dyn_clause = static_cast<const TreatyClause::AnnexProvince*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::LIBERATE_NATION) {
			const auto* dyn_clause = static_cast<const TreatyClause::LiberateNation*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::HUMILIATE) {
			const auto* dyn_clause = static_cast<const TreatyClause::Humiliate*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::IMPOSE_POLICIES) {
			const auto* dyn_clause = static_cast<const TreatyClause::ImposePolicies*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::CEASEFIRE) {
			const auto* dyn_clause = static_cast<const TreatyClause::Ceasefire*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::PUPPET) {
			const auto* dyn_clause = static_cast<const TreatyClause::Puppet*>(clause);
			on_effect = dyn_clause->in_effect();
		}
		
		if(on_effect) {
            break;
        }
	}
	return on_effect;	
}

Battle::Battle(War& war, Province& province)
    : war{ &war },
    province{ &province }
{

}

bool War::is_involved(const Nation& nation) const {
    return (is_attacker(nation) || is_defender(nation));
}

bool War::is_attacker(const Nation& nation) const {
    return (std::find(attackers.begin(), attackers.end(), &nation) != attackers.end());
}

bool War::is_defender(const Nation& nation) const {
    return (std::find(defenders.begin(), defenders.end(), &nation) != defenders.end());
}