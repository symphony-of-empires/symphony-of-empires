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
#include "pop.hpp"
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
    attractive += -(owner.get_tax(pop) * 100.f); // Less tax = better
    return attractive;
}

void Province::add_building(const BuildingType& building_type) {
    // Now build the building
    this->buildings[building_type].level += 1.f;
    this->buildings[building_type].req_goods = building_type.req_goods;
}

void Province::cancel_construction_project() {
    for(auto& building : buildings)
        building.working_unit_type_id = UnitTypeId(-1);
}

glm::vec3 Province::get_sphere_coord(glm::vec2 world_size, float radius) const {
    const auto normalized_pos = this->get_pos() / world_size;
    glm::vec2 radiance_pos;
    radiance_pos.x = normalized_pos.x * 2.f * glm::pi<float>();
    radiance_pos.y = normalized_pos.y * glm::pi<float>();

    glm::vec3 sphere_position;
    sphere_position.x = glm::cos(radiance_pos.x) * glm::sin(radiance_pos.y);
    sphere_position.y = glm::sin(radiance_pos.x) * glm::sin(radiance_pos.y);
    sphere_position.z = glm::cos(radiance_pos.y);
    sphere_position *= radius;
    return sphere_position;
}

float Province::euclidean_distance(const Province& other_province, glm::vec2 world_size, float radius) const {
    const auto sphere_coord1 = get_sphere_coord(world_size, radius);
    const auto sphere_coord2 = other_province.get_sphere_coord(world_size, radius);
    const auto cos_angle = glm::dot(sphere_coord1, sphere_coord2) / (radius * radius);
    const auto angle = glm::acos(cos_angle);
    const auto distance = angle * radius;
    return distance;
}

bool Province::is_neighbour(const Province& province) const {
    return std::find(this->neighbour_ids.begin(), this->neighbour_ids.end(), province) != this->neighbour_ids.end();
}

Pop& Province::get_soldier_pop() {
    const auto& world = World::get_instance();
    auto it = std::find_if(pops.begin(), pops.end(), [&world](const auto& e) {
        return world.pop_types[e.type_id].group == PopGroup::SOLDIER;
    });
    return *it;
}
