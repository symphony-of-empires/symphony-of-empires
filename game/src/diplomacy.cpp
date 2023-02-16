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

#include "world.hpp"
#include "world.hpp"
#include "world.hpp"

using namespace Diplomacy;

TreatyClause::BaseClause::BaseClause(const Nation& _sender, const Nation& _receiver) {
    sender_id = _sender;
    receiver_id = _receiver;
}

inline bool Diplomacy::is_friend(Nation& us, Nation& them) {
    const auto& relation = g_world.get_relation(us, them);
    // High enough relation with the threshold means it is friendly
    return relation.relation >= 0.5f;
}

inline bool Diplomacy::is_foe(Nation& us, Nation& them) {
    return !is_friend(us, them);
}

//
// Payment
//
unsigned TreatyClause::Payment::cost() {
    return amount * days_duration;
}

void TreatyClause::Payment::enforce() {
    auto& world = World::get_instance();
    world.nations[sender_id].prestige += 0.0001f;
    world.nations[receiver_id].prestige -= 0.0001f;
    world.nations[sender_id].budget -= amount;
    world.nations[receiver_id].budget += amount;
    days_duration--;
}

bool TreatyClause::Payment::in_effect() const {
    return days_duration != 0;
}

//
// Humiliate
//
unsigned TreatyClause::Humiliate::cost() {
    const auto& world = World::get_instance();
    return (world.nations[receiver_id].prestige * (amount * days_duration)) / 100;
}

void TreatyClause::Humiliate::enforce() {
    auto& world = World::get_instance();
    world.nations[sender_id].prestige += amount;
    world.nations[receiver_id].prestige -= amount;
    days_duration--;
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
    auto& world = World::get_instance();
    world.nations[sender_id].prestige += cost() * 0.025f;
    world.nations[receiver_id].prestige -= cost() * 0.05f;
    for(const auto province_id : province_ids)
        world.nations[liberated_id].give_province(world.provinces[province_id]);
    done = true;
}

bool TreatyClause::LiberateNation::in_effect() const {
    return !done;
}

//
// ImposePolicies
//
unsigned TreatyClause::ImposePolicies::cost() {
    const auto& world = World::get_instance();
    return imposed.difference(world.nations[receiver_id].current_policy);
}

void TreatyClause::ImposePolicies::enforce() {
    auto& world = World::get_instance();
    world.nations[receiver_id].set_policy(imposed);
    done = true;
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
    auto& world = World::get_instance();
    world.nations[sender_id].prestige += cost() * 0.025f;
    world.nations[receiver_id].prestige -= cost() * 0.05f;
    for(const auto province_id : province_ids)
        world.nations[sender_id].give_province(world.provinces[province_id]);
    done = true;
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
    auto& world = World::get_instance();
    world.nations[receiver_id].make_puppet(world.nations[sender_id]);
    done = true;
}

bool TreatyClause::Puppet::in_effect() const {
    return !done;
}

// Checks if the specified nations participates in the treaty
bool Treaty::does_participate(const Nation& nation) const {
    for(auto& [other_nation, _] : this->approval_status)
        if(other_nation == nation)
            return true;
    return false;
}

// Checks if the treaty has any clause which may still make the treaty be in effect
bool Treaty::in_effect() const {
    bool on_effect = std::find_if(this->approval_status.begin(), this->approval_status.end(), [](const auto& status) { return status.second != TreatyApproval::ACCEPTED; }) == this->approval_status.end();
    if(!on_effect)
        return false;
    
    for(const auto& clause : this->clauses) {
        if(clause->type == TreatyClauseType::PAYMENT) {
            const auto* dyn_clause = static_cast<const TreatyClause::Payment*>(clause);
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
