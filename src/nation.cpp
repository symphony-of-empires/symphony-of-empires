#include "nation.hpp"
#include "world.hpp"
#include "print.hpp"
#include "technology.hpp"

// Declare war
// TODO: Make some form of "WarParticipationRequest" so we don't force allies to join
// and we also make sure betrayals are possible
void Nation::declare_war(Nation& nation, std::vector<TreatyClause::BaseClause*> clauses) {
    World& world = World::get_instance();
    auto* war = new War();

    print_info("%s has declared war on %s!", this->ref_name.c_str(), nation.ref_name.c_str());

    war->wargoals = clauses;

    // Recollect offenders
    // - Those who are allied to us
    for(uint i = 0; i < this->relations.size(); i++) {
        const auto& relation = this->relations[i];

        if(relation.has_alliance) {
            war->attackers.push_back(world.nations[i]);
        }
    }
    war->attackers.push_back(this);

    print_info("Attackers");
    for(const auto& attacker : war->attackers) {
        print_info("[%s]", attacker->ref_name.c_str());
    }

    // Recollect defenders
    // - Those who are on a defensive pact with the target
    // - Those who are allied with the target
    for(uint i = 0; i < nation.relations.size(); i++) {
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
            defender->relations[world.get_id(attacker)].has_war = true;
        }
    }

    print_info("Defenders");
    for(const auto& defender : war->defenders) {
        print_info("[%s]", defender->ref_name.c_str());
    }

    war->name = "War of " + this->name + " against " + nation.name;
    print_info("War!, [%s]", war->name.c_str());
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

    print_info("[%s] increases relations with [%s]", ref_name.c_str(), target.ref_name.c_str());
    this->do_diplomacy();
}

void Nation::decrease_relation(Nation& target) {
    //if(!can_do_diplomacy()) return;

    const World& world = World::get_instance();
    this->relations[world.get_id(&target)].relation -= 5.f;
    target.relations[world.get_id(this)].relation -= 5.f;

    print_info("[%s] decreases relations with [%s]", ref_name.c_str(), target.ref_name.c_str());
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

            const Policies& pop_policies = pop.get_ideology()->policies;

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
        if(pop.culture == culture) return true;
    }
    return false;
}

// Same as above but with religion
bool Nation::is_accepted_religion(const Pop& pop) const {
    for(const auto& religion : accepted_religions) {
        if(pop.religion == religion) return true;
    }
    return false;
}

// Gets the total tax applied to a POP depending on their "wealth"
// (not exactly like that, more like by their type/status)
float Nation::get_tax(const Pop& pop) const {
    float base_tax = 1.f;

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

    const std::lock_guard lock(world.nation_changed_tiles_mutex);

    uint count = 0;
    for(uint i = province.min_x; i <= province.max_x; i++) {
        for(uint j = province.min_y; j <= province.max_y; j++) {
            Tile& tile = world.get_tile(i, j);
            if(tile.province_id != province_id) continue;

            tile.owner_id = nation_id;
            world.nation_changed_tiles.push_back(&tile);
            count++;
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
        if(hint.ideology == ideology) return hint;
    }

    // 2nd search: Find a hint that is fallback
    for(const auto& hint : client_hints) {
        if(hint.ideology == nullptr) return hint;
    }

    if(client_hints.empty()) {
        tmp_hint.colour = rand();
        tmp_hint.alt_name = this->ref_name + "_MISSING_CLIENTHINT";
        tmp_hint.ideology = World::get_instance().ideologies[0];
        return tmp_hint;
    }
    return client_hints[0];
}

float Nation::get_research_points(void) const {
    float research = 0.f;
    for(const auto& province : this->owned_provinces) {
        for(const auto& pop : province->pops) {
            research += pop.size * pop.literacy;
        }
        research /= province->pops.size();
    }
    return research;
}

bool Nation::can_research(Technology* tech) const {
    // Only military/navy technologies can actually be researched
    if(tech->type != TechnologyType::MILITARY && tech->type != TechnologyType::NAVY) return false;

    // All required technologies for this one must be researched
    for(const auto& req_tech : tech->req_technologies) {
        if(research[World::get_instance().get_id(req_tech)] > 0.f) return false;
    }
    return true;
}

void Nation::change_research_focus(Technology* tech) {
    // Can't have already researched it (it would be dumb to re-research
    if(!research[World::get_instance().get_id(tech)]) return;
    // Must be able to research it
    if(!can_research(tech)) return;

    focus_tech = tech;
}

/*float Nation::get_industry_output_rate() {
    for(const auto& p: techs) {

    }
}*/