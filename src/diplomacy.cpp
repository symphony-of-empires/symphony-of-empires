#include "diplomacy.hpp"
#include "world.hpp"

namespace Diplomacy {
    bool is_friend(Nation* us, Nation* them) {
        const NationId idx = g_world->get_id(them);
        const NationRelation* relation = &us->relations[idx];

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

    bool is_foe(Nation* us, Nation* them) {
        return !is_friend(us, them);
    }
};

using namespace TreatyClause;
unsigned WarReparations::cost(void) {
    return (receiver->economy_score* (amount* days_duration)) / 100;
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
    return (receiver->prestige* (amount* days_duration)) / 100;
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
    for(const auto& province: provinces) {
        value += province->budget* province->total_pops();
    }
    return value* 0.00001f;
}
void LiberateNation::enforce(void) {
    // Reduce prestige due to lost lands
    sender->prestige += cost()* 0.0000025f;
    receiver->prestige -= cost()* 0.000005f;
    
    // Give provinces to this liberated nation
    for(auto& province: provinces) {
        province->owner = liberated;
    }
    
    // One-time clause
    done = true;
}
bool LiberateNation::in_effect(void) {
    return done;
}

unsigned ImposePolicies::cost(void) {
    return imposed.difference(receiver->current_policy);
}
void ImposePolicies::enforce(void) {
    memcpy(&receiver->current_policy, &imposed, sizeof(Policies));
    done = true;
}
bool ImposePolicies::in_effect(void) {
    return done;
}

unsigned AnexxProvince::cost(void) {
    size_t value = 0;
    for(const auto& province: provinces) {
        value += province->budget + province->total_pops();
    }
    return value* 0.000001f;
}
void AnexxProvince::enforce(void) {
    sender->prestige += cost()* 0.0000025f;
    receiver->prestige -= cost()* 0.000005f;
    
    // Give provinces to the winner
    for(auto& province: provinces) {
        province->owner = sender;
    }
    
    // One-time clause
    done = true;
}
bool AnexxProvince::in_effect(void) {
    return done;
}

unsigned Ceasefire::cost() {
    return receiver->military_score + receiver->naval_score;
}
void Ceasefire::enforce() {
    NationId receiver_id = g_world->get_id(receiver);
    NationId sender_id = g_world->get_id(sender);
    
    sender->relations[receiver_id].has_war = false;
    sender->relations[receiver_id].has_truce = true;
    
    receiver->relations[sender_id].has_war = false;
    receiver->relations[sender_id].has_truce = true;
    
    days_duration--;
}
bool Ceasefire::in_effect() {
    return (days_duration != 0);
}
