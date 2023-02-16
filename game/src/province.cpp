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
//      province.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "province.hpp"
#include "world.hpp"
#include "product.hpp"
#include "unit.hpp"
#include "building.hpp"
#include "world.hpp"
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

#include "eng3d/utils.hpp"

//
// Province
//
// Calculates the total number of POPs in this province (total population)
float Province::get_attractiveness(const Pop& pop) const {
    float attractive = this->base_attractive;
    const auto& owner = g_world.nations[this->owner_id];
    return attractive;
}

void Province::add_building(const BuildingType& building_type) {
    // Now build the building
    this->buildings[building_type].level += 1.f;
    this->buildings[building_type].req_goods = building_type.req_goods;
}

void Province::cancel_construction_project() {
    for(auto& building : buildings)
        building.working_unit_type_id.reset();
}

bool Province::is_neighbour(const Province& province) const {
    return std::find(this->neighbour_ids.begin(), this->neighbour_ids.end(), province) != this->neighbour_ids.end();
}

/// @brief Borrow a loan
/// @param amount Amount to borrow
/// @param borrowed Borrowed amount (in total)
/// @return std::pair<float, float> Pair of amount in debt, first is public debt and second is private
std::pair<float, float> Province::borrow_loan(float amount, float& borrowed) {
    const auto& nation = g_world.nations[this->owner_id];
    // First try the one with lower interest
    if(nation.public_loan_interest <= this->private_loan_interest || amount > this->private_loan_pool) {
        borrowed = glm::min(amount, nation.public_loan_pool);
        return std::make_pair(borrowed + borrowed * nation.public_loan_interest, 0.f);
    }
    borrowed = glm::min(amount, this->private_loan_pool);
    return std::make_pair(0.f, borrowed + borrowed * this->private_loan_interest);
}

std::vector<UnitId> Province::Battle::get_attacker_unit_ids() const {
    const auto& units = g_world.unit_manager.units;
    std::vector<UnitId> v;
    for(const auto nation_id : this->attacker_nations_ids)
        for(const auto unit_id : this->unit_ids)
            if(units[unit_id].owner_id == nation_id)
                v.push_back(unit_id);
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
    return v;
}

std::vector<UnitId> Province::Battle::get_defender_unit_ids() const {
    const auto& units = g_world.unit_manager.units;
    std::vector<UnitId> v;
    for(const auto nation_id : this->defender_nations_ids)
        for(const auto unit_id : this->unit_ids)
            if(units[unit_id].owner_id == nation_id)
                v.push_back(unit_id);
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
    return v;
}
