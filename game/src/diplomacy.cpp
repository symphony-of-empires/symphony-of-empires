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
//      diplomacy.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/log.hpp"

#include "diplomacy.hpp"
#include "nation.hpp"
#include "world.hpp"

using namespace Diplomacy;

TreatyClause::BaseClause::BaseClause(const Nation& _sender, const Nation& _receiver) {
    sender_id = _sender;
    receiver_id = _receiver;
}

inline bool Diplomacy::is_friend(Nation& us, Nation& them) {
    const auto& relation = g_world.get_relation(us, them);
    // High enough relation with the threshold means it is friendly
    return relation.relation >= 50;
}

inline bool Diplomacy::is_foe(Nation& us, Nation& them) {
    return !is_friend(us, them);
}

//
// WarReparations
//
unsigned TreatyClause::WarReparations::cost() {
    // return (amount * days_duration) / 100;
    return 0;
}

void TreatyClause::WarReparations::enforce() {
    // sender->prestige += 0.0001f;
    // receiver->prestige -= 0.0001f;
    // sender->budget -= amount;
    // receiver->budget += amount;
    // days_duration--;
}

bool TreatyClause::WarReparations::in_effect() const {
    return days_duration != 0;
}

//
// Humiliate
//
unsigned TreatyClause::Humiliate::cost() {
    // return (receiver->prestige * (amount * days_duration)) / 100;
    return 0;
}

void TreatyClause::Humiliate::enforce() {
    // sender->prestige += amount;
    // receiver->prestige -= amount;
    // days_duration--;
}

bool TreatyClause::Humiliate::in_effect() const {
    return days_duration != 0;
}

//
// LiberateNation
//
unsigned TreatyClause::LiberateNation::cost() {
    return 0;
}

void TreatyClause::LiberateNation::enforce() {
    // // Reduce prestige due to lost lands
    // sender->prestige += cost() * 0.0000025f;
    // receiver->prestige -= cost() * 0.000005f;
    // // Give provinces to this liberated nation
    // for(auto& province : provinces)
    //     province->owner_id = g_world.get_id(*liberated);
    // // One-time clause
    // done = true;
}

bool TreatyClause::LiberateNation::in_effect() const {
    return !done;
}

//
// ImposePolicies
//
unsigned TreatyClause::ImposePolicies::cost() {
    // return imposed.difference(receiver->current_policy);
    return 0;
}

void TreatyClause::ImposePolicies::enforce() {
    // receiver->set_policy(imposed);
    // done = true;
}

bool TreatyClause::ImposePolicies::in_effect() const {
    return !done;
}

//
// AnnexProvince
//
unsigned TreatyClause::AnnexProvince::cost() {
    return 0;
}

void TreatyClause::AnnexProvince::enforce() {
    // sender->prestige += cost() * 0.0000025f;
    // receiver->prestige -= cost() * 0.000005f;
    // // Give provinces to the winner
    // for(auto& province : provinces) {
    //     Eng3D::Log::debug("game", "Giving " + province->ref_name + " to " + sender->ref_name + " from " + receiver->ref_name);
    //     // Change ownership of provinces
    //     sender->give_province(*province);
    // }
    // // One-time clause
    // done = true;
}

bool TreatyClause::AnnexProvince::in_effect() const {
    return !done;
}

//
// Puppet
//
unsigned TreatyClause::Puppet::cost() {
    return 0;
}

void TreatyClause::Puppet::enforce() {
    // receiver->puppet_master_id = *sender;
    // done = true;
}

bool TreatyClause::Puppet::in_effect() const {
    return !done;
}

// Checks if the specified nations participates in the treaty
bool Treaty::does_participate(Nation& nation) {
    for(auto& [other_nation, _] : this->approval_status)
        if(other_nation == nation)
            return true;
    return false;
}

// Checks if the treaty has any clause which may still make the treaty be in effect
bool Treaty::in_effect() const {
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
        } else if(clause->type == TreatyClauseType::PUPPET) {
            const auto* dyn_clause = static_cast<const TreatyClause::Puppet*>(clause);
            on_effect = dyn_clause->in_effect();
        }
        if(on_effect) break;
    }
    return on_effect;
}

bool War::is_involved(const Nation& nation) const {
    return is_attacker(nation) || is_defender(nation);
}

bool War::is_attacker(const Nation& nation) const {
    return std::find(attacker_ids.begin(), attacker_ids.end(), nation) != attacker_ids.end();
}

bool War::is_defender(const Nation& nation) const {
    return std::find(defender_ids.begin(), defender_ids.end(), nation) != defender_ids.end();
}