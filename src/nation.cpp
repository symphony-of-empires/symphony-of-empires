#include "nation.hpp"
#include "world.hpp"
#include "print.hpp"

// Declare war
void Nation::declare_war(Nation& nation) {
    World& world = World::get_instance();
    auto* war = new War();

    // Recollect offenders
    // - Those who are allied to us
    for(size_t i = 0; i < this->relations.size(); i++) {
        const auto& relation = this->relations[i];

        if(relation.has_alliance == true) {
            war->attackers.push_back(world.nations[i]);
        }
    }
    war->attackers.push_back(this);

    // Recollect defenders
    // - Those who are on a defensive pact with the target
    // - Those who are allied with the target
    for(size_t i = 0; i < nation.relations.size(); i++) {
        const auto& relation = nation.relations[i];

        if(relation.has_alliance == true
        || relation.has_defensive_pact == true) {
            war->defenders.push_back(world.nations[i]);
        }
    }
    war->defenders.push_back(&nation);

    war->name = "War by " + this->name + " against " + nation.name;
}

bool Nation::is_ally(const Nation& nation) {
    const World& world = World::get_instance();

    if(nation.relations[world.get_id(this)].has_war == true
        || this->relations[world.get_id(&nation)].has_war == true) {
        return false;
    }
    return true;
}

bool Nation::is_enemy(const Nation& nation) {
    const World& world = World::get_instance();

    if(nation.relations[world.get_id(this)].has_war == true
        || this->relations[world.get_id(&nation)].has_war == true) {
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
    return diplomatic_timer == 0;
}

void Nation::increase_relation(Nation& target) {
    if(!can_do_diplomacy())
        return;

    const World& world = World::get_instance();
    const Nation::Id t1_idx = world.get_id(&target);
    this->relations[t1_idx].relation += 5.f;
    const Nation::Id t2_idx = world.get_id(this);
    target.relations[t2_idx].relation += 5.f;

    print_info("%s increases relations with %s", name.c_str(), target.name.c_str());
    do_diplomacy();
}

void Nation::decrease_relation(Nation& target) {
    if(!can_do_diplomacy())
        return;

    const World& world = World::get_instance();
    const Nation::Id t1_idx = world.get_id(&target);
    this->relations[t1_idx].relation += 5.f;
    const Nation::Id t2_idx = world.get_id(this);
    target.relations[t2_idx].relation += 5.f;

    print_info("%s decreases relations with %s", name.c_str(), target.name.c_str());
    do_diplomacy();
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
        print_info("Parliament-less policy passed!");
        return;
    }

    float approvals = 0, disapprovals = 0;

    std::vector<Pop*> disapprovers, approvers;
    for(const auto& province : this->owned_provinces) {
        for(auto& pop : province->pops) {
            // Must have the minimum required social value
            // the min-social-value is taken from the new enacted policy
            if(pop.type->social_value < policies.min_sv_for_parliament) {
                continue;
            }

            const Policies& pop_policies = pop.ideology->policies;

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

        // All people who agreed gets happy
        for(auto& pop: approvers) {
            pop->militancy /= std::min(pop->con, 0.1f);
        }

        // All people who disagreed gets angered
        for(auto& pop: disapprovers) {
            pop->militancy *= std::min(pop->con, 0.1f);
        }
        print_info("New enacted policy passed parliament!");
    }
    // Legislation does not make it into the official law
    else {
        // All people who agreed gets angered
        for(auto& pop: approvers) {
            pop->militancy *= std::min(pop->con, 0.1f);
        }

        // All people who disagreed gets happy
        for(auto& pop: disapprovers) {
            pop->militancy /= std::min(pop->con, 0.1f);
        }
        print_info("New enacted policy did not made it into the parliament!");
    }
    return;
}

// Checks if a POP is part of one of our accepted cultures
bool Nation::is_accepted_culture(const Pop& pop) const {
    for(const auto& culture : accepted_cultures) {
        if(pop.culture == culture) {
            return true;
        }
    }
    return false;
}

// Same as above but with religion
bool Nation::is_accepted_religion(const Pop& pop) const {
    for(const auto& religion : accepted_religions) {
        if(pop.religion == religion) {
            return true;
        }
    }
    return false;
}

// Gets the total tax applied to a POP depending on their "wealth"
// (not exactly like that, more like by their type/status)
float Nation::get_tax(const Pop& pop) const {
    float base_tax = 1.f;

    // Advanced discrimination mechanics ;)

    // Only-accepted POPs policy should impose higher prices to non-accepted POPs
    // We are not an accepted-culture POP
    if(is_accepted_culture(pop) != true) {
        if(current_policy.treatment == TREATMENT_ONLY_ACCEPTED) {
            base_tax += 1.5f;
        }
        else if(current_policy.treatment == TREATMENT_EXTERMINATE) {
            // Fuck you
            base_tax += 1000.f;
        }
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
    Nation::Id nation_id = world.get_id(this);
    Province::Id province_id = world.get_id(&province);

    for(size_t i = province.min_x; i < province.max_x; i++) {
        for(size_t j = province.min_y; j < province.max_y; j++) {
            Tile& tile = world.get_tile(i, j);
            if(tile.province_id != province_id)
                continue;

            tile.owner_id = nation_id;
            world.nation_changed_tiles.push_back(&tile);
        }
    }

    world.nations[nation_id]->owned_provinces.insert(world.provinces[province_id]);
    world.provinces[province_id]->owner = world.nations[nation_id];
    return;
}

NationClientHint tmp_hint;
const NationClientHint& Nation::get_client_hint(void) const {
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
        tmp_hint.colour = rand();
        tmp_hint.alt_name = "No ClientHint defined";
        tmp_hint.ideology = World::get_instance().ideologies[0];
        return tmp_hint;
    }
    return client_hints[0];
}

/*float Nation::get_industry_output_rate() {
    for(const auto& p: techs) {

    }
}*/