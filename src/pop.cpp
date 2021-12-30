#include "pop.hpp"
#include "world.hpp"

bool Pop::operator==(const Pop& rhs) const {
    return (this->culture == rhs.culture && this->religion == rhs.religion && this->type == rhs.type);
}

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