// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/pathfinding.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <queue>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

#include "eng3d/pathfind.hpp"
#include "pathfinding.hpp"
#include "province.hpp"

/// @brief Calculates the neighbors for a given Tile. The neighbors are the 8 tiles around
/// it, while taking into account the map bounds.
static std::vector<Province::Id> generate_neighbors(Province::Id province_id, const World& world, const Nation& nation) {
    const auto& province = world.provinces[province_id];
    std::vector<Province::Id> result;
    result.reserve(province.neighbour_ids.size());
    for(const auto neighbour_id : province.neighbour_ids)
        result.push_back(neighbour_id);
    return result;
}

static inline glm::vec3 get_sphere_coord(const Province& province, glm::vec2 world_size) {
    const glm::vec2 normalized_pos = province.get_pos() / world_size;
    glm::vec2 radiance_pos;
    radiance_pos.x = normalized_pos.x * 2.f * glm::pi<float>();
    radiance_pos.y = normalized_pos.y * glm::pi<float>();

    glm::vec3 sphere_position;
    sphere_position.x = glm::cos(radiance_pos.x) * glm::sin(radiance_pos.y);
    sphere_position.y = glm::sin(radiance_pos.x) * glm::sin(radiance_pos.y);
    sphere_position.z = glm::cos(radiance_pos.y);
    return sphere_position;
}

static inline float euclidean_distance(Province::Id province1_id, Province::Id province2_id, const World& world, const Nation& nation) {
    const auto& province1 = world.provinces[province1_id];
    const auto& province2 = world.provinces[province2_id];
    const glm::vec2 world_size{ world.width, world.height };
    const auto sphere_coord1 = get_sphere_coord(province1, world_size);
    const auto sphere_coord2 = get_sphere_coord(province2, world_size);
    float cos_angle = glm::dot(sphere_coord1, sphere_coord2);
    float angle = glm::acos(cos_angle);
    float distance = angle / (2 * glm::pi<float>());
    return distance;
}

// Call pathfinder like this:
// Eng3D::Pathfind::get_path<Province::Id>(generate_neighbors, euclidean_distance, world, nation);
