#include "diplomacy.hpp"
#include "nation.hpp"
#include "world.hpp"

using namespace Diplomacy;

inline bool Diplomacy::is_friend(Nation& us, Nation& them) {
    const NationRelation* relation = &us.relations[g_world->get_id(&them)];

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

using namespace TreatyClause;
unsigned WarReparations::cost(void) {
    return (receiver->economy_score * (amount * days_duration)) / 100;
}

void WarReparations::enforce(void) {
    sender->prestige += 0.0001f;
    receiver->prestige -= 0.0001f;
    sender->budget -= amount;
    receiver->budget += amount;
    days_duration--;
}

bool WarReparations::in_effect(void) {
    return (days_duration != 0);
}

unsigned Humiliate::cost(void) {
    return (receiver->prestige * (amount * days_duration)) / 100;
}

void Humiliate::enforce(void) {
    sender->prestige += amount;
    receiver->prestige -= amount;
    days_duration--;
}

bool Humiliate::in_effect(void) {
    return (days_duration != 0);
}

unsigned LiberateNation::cost(void) {
    size_t value = 0;
    for(const auto& province : provinces) {
        value += province->budget * province->total_pops();
    }
    return value * 0.00001f;
}

void LiberateNation::enforce(void) {
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

bool LiberateNation::in_effect(void) {
    return !done;
}

unsigned ImposePolicies::cost(void) {
    return imposed.difference(receiver->current_policy);
}

void ImposePolicies::enforce(void) {
    receiver->current_policy = imposed;
    done = true;
}

bool ImposePolicies::in_effect(void) {
    return !done;
}

#include "unified_render/print.hpp"
unsigned AnexxProvince::cost(void) {
    size_t value = 0;
    for(const auto& province : provinces) {
        value += province->budget + province->total_pops();
    }
    return value * 0.000001f;
}

void AnexxProvince::enforce(void) {
    sender->prestige += cost() * 0.0000025f;
    receiver->prestige -= cost() * 0.000005f;

    // Give provinces to the winner
    for(auto& province : provinces) {
        print_info("Giving [%s] to [%s] (originally from [%s])", province->ref_name.c_str(), sender->ref_name.c_str(), receiver->ref_name.c_str());

        province->owner = sender;

        // Add to sender's owned provinces list and remove from
        // original owner
        sender->owned_provinces.insert(province);
        const auto it = receiver->owned_provinces.find(province);
        if(it != receiver->owned_provinces.end()) {
            receiver->owned_provinces.erase(it);
        }
    }

    // One-time clause
    done = true;
}

bool AnexxProvince::in_effect(void) {
    return !done;
}

unsigned Ceasefire::cost() {
    return receiver->military_score + receiver->naval_score;
}

void Ceasefire::enforce() {
    Nation::Id receiver_id = g_world->get_id(receiver);
    Nation::Id sender_id = g_world->get_id(sender);

    sender->relations[receiver_id].has_war = false;
    sender->relations[receiver_id].has_truce = true;

    receiver->relations[sender_id].has_war = false;
    receiver->relations[sender_id].has_truce = true;

    days_duration--;
}

bool Ceasefire::in_effect() {
    return (days_duration != 0);
}

/** Checks if the specified nations participates in the treaty */
bool Treaty::does_participate(Nation& nation) {
    for(auto& status : this->approval_status) {
        if(status.first == &nation) {
            return true;
        }
    }
    return false;
}

/** Checks if the treaty has any clause which may still make the treaty be in effect */
bool Treaty::in_effect(void) const {
	bool on_effect = false;
	for(const auto& clause : this->clauses) {
		if(clause->type == TreatyClauseType::WAR_REPARATIONS) {
			auto dyn_clause = static_cast<TreatyClause::WarReparations*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::ANEXX_PROVINCES) {
			auto dyn_clause = static_cast<TreatyClause::AnexxProvince*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::LIBERATE_NATION) {
			auto dyn_clause = static_cast<TreatyClause::LiberateNation*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::HUMILIATE) {
			auto dyn_clause = static_cast<TreatyClause::Humiliate*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::IMPOSE_POLICIES) {
			auto dyn_clause = static_cast<TreatyClause::ImposePolicies*>(clause);
			on_effect = dyn_clause->in_effect();
		} else if(clause->type == TreatyClauseType::CEASEFIRE) {
			auto dyn_clause = static_cast<TreatyClause::Ceasefire*>(clause);
			on_effect = dyn_clause->in_effect();
		}
		
		if(on_effect) {
            break;
        }
	}
	return on_effect;	
}