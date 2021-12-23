#include "province.hpp"
#include "world.hpp"
#include "product.hpp"
#include "good.hpp"
#include "company.hpp"
#include "unit.hpp"
#include "building.hpp"

// Obtains the country that currently has a larger number of
// tiles controlled from this province
Nation& Province::get_occupation_controller(void) const {
    const World& world = World::get_instance();
    return (*controller);
}

// Calculates the total number of POPs in this province (total population)
size_t Province::total_pops(void) const {
    size_t total = 0;
    for(const auto& pop : pops) {
        total += pop.size;
    }
    return total;
}

// Create a vector containing a list of all products available on this province
std::vector<Product*> Province::get_products(void) const {
    return products;
}

float Province::get_attractiveness(const Pop& pop) const {
    float attractive = this->base_attractive;
    
    if(!this->owner->is_accepted_culture(pop) && !this->owner->is_accepted_religion(pop)) {
        // Linearized version, instead of using if-else trees we just
        // multiply the attractive by the scale; EXTERMINATE = 3, so 3 - 3 is 0 which nullifies the attractivenes
        // and the more open the borders are the more lenient the "scale" becomes
        const int scale = 3 - this->owner->current_policy.treatment;
        attractive *= scale;
    }

    // Account for literacy difference
    attractive -= this->owner->base_literacy;

    // Account for GDP difference
    attractive -= this->owner->gdp * 0.0001f;

    // A social value between 0 and 1 is for poor people, the value for medium class
    // is between 1 and 2, for the rich is above 2

    // For the lower class, lower taxes is good, and so on for other POPs
    if(pop.type->social_value >= 0.f && pop.type->social_value <= 1.f) {
        attractive += -(this->owner->current_policy.poor_flat_tax);
    }
    // For the medium class
    else if(pop.type->social_value >= 1.f && pop.type->social_value <= 2.f) {
        attractive += -(this->owner->current_policy.med_flat_tax);
    }
    // For the high class
    else if(pop.type->social_value >= 2.f) {
        attractive += -(this->owner->current_policy.rich_flat_tax);
    }
    return attractive;
}

std::pair<float, float> Province::get_pos(void) const {
    return std::make_pair(min_x + ((max_x - min_x) / 2.f), min_y + ((max_y - min_y) / 2.f));
}

std::vector<Unit*> Province::get_units(void) const {
    std::vector<Unit*> temp;
    for(const auto& unit : World::get_instance().units) {
        if(unit->province != this) continue;
        temp.push_back(unit);
    }
    return temp;
}

std::vector<Building*> Province::get_buildings(void) const {
    std::vector<Building*> temp;
    for(const auto& building : World::get_instance().buildings) {
        if(building->get_province() != this) continue;
        temp.push_back(building);
    }
    return temp;
}