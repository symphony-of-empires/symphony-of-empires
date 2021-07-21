#include "nation.hpp"
#include "world.hpp"

inline void Nation::do_diplomacy() {
    diplomatic_timer = (60* 48) - std::min(10.f* 48.f, prestige / 100.f);
}

inline bool Nation::can_do_diplomacy() {
    return diplomatic_timer == 0;
}

#include "print.hpp"
void Nation::increase_relation(const World& world, Nation* target) {
    const NationId t1_idx = world.get_id(target);
    this->relations[t1_idx].relation += 5.f;
    const NationId t2_idx = world.get_id(this);
    target->relations[t2_idx].relation += 5.f;

    print_info("%s increases relations with %s", name.c_str(), target->name.c_str());
}

void Nation::decrease_relation(const World& world, Nation* target) {
    const NationId t1_idx = world.get_id(target);
    this->relations[t1_idx].relation += 5.f;
    const NationId t2_idx = world.get_id(this);
    target->relations[t2_idx].relation += 5.f;

    print_info("%s decreases relations with %s", name.c_str(), target->name.c_str());
}

/**
* Automatically relocates the capital of a nation to another province
* Use this when a treaty makes a nation lose it's capital
 */
void Nation::auto_relocate_capital(void) {
    auto best_candidate = std::max_element(owned_provinces.begin(), owned_provinces.end(),
        [] (const auto* lhs, const auto* rhs) {
            return (lhs->total_pops() < rhs->total_pops());
        });
    capital = *best_candidate;
}

/**
* Enacts a policy on a nation
* @return false if policy draft failed to be applied, true if policy passed and is in-effect
 */
void Nation::set_policy(Policies& policies) {
    // TODO: Make parliament (aristocrat POPs) be able to reject policy changes
    // TODO: Increase militancy on non-agreeing POPs
    
    memcpy(&this->current_policy, &policies, sizeof(Policies));
    return;
}
