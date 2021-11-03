#include "pop.hpp"
#include "world.hpp"

Ideology* Pop::get_ideology(void) const {
    // Obtain ideology with highest approval
    auto v = g_world->ideologies[std::distance(
        this->ideology_approval.begin(),
        std::max_element(
            this->ideology_approval.begin(),
            this->ideology_approval.end()
        )
    )];
    return v;
}