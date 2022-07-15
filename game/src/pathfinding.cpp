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

#include "pathfinding.hpp"

#include "province.hpp"
#include <queue>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

/**
 * Checks whether the given coordinates are within bounds for the given world
 */
static constexpr bool coord_in_bounds(const World& world, int x, int y) {
    return y >= 0 && (size_t)y < world.height;
}

/**
 * Calculates the neighbors for a given Tile. The neighbors are the 8 tiles around
 * it, while taking into account the map bounds.
 */
static std::vector<Province::Id> generate_neighbors(const World& world, const Nation& nation, Province::Id province_id) {
    const Province& province = world.provinces[province_id];

    std::vector<Province::Id> result;
    result.reserve(province.neighbours.size());
    for(const auto& neighbour_id : province.neighbours) {
        result.push_back(neighbour_id);

        const Province& neighbour = world.provinces[neighbour_id];

        // // Check that we can diplomatically pass thru their lands
        // // Our own lands and lands without owners are automatically passable
        // if(neighbour.owner_id != nation.get_id() && neighbour.owner_id != (Nation::Id)-1) {
        //     const NationRelation& relation = world.nations[neighbour->owner_id]->relations[nation_id];

        //     // Does not have military acces AND does not have alliance AND is not at war
        //     // Means the country is neutral towards us and we can't cross there
        //     if(relation.has_military_access != true && relation.has_alliance != true && relation.has_war != true) {
        //         continue;
        //     }
        // }

        // // Otherwise we can pass!
        // result.push_back(neighbour);
    };
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

static inline float euclidean_distance(const Province& province1, const Province& province2, glm::vec2 world_size) {
    glm::vec3 sphere_coord1 = get_sphere_coord(province1, world_size);
    glm::vec3 sphere_coord2 = get_sphere_coord(province2, world_size);

    float cos_angle = glm::dot(sphere_coord1, sphere_coord2);
    float angle = glm::acos(cos_angle);
    float distance = angle / (2 * glm::pi<float>());
    return distance;
}

/**
 * Calculates the cost accrued by moving from one tile to another, taking into
 * account elevation and infrastructure.
 */
static inline float tile_cost(const World& world, Province::Id t1, Province::Id t2) {
    const Province& prov_1 = world.provinces[t1];
    const Province& prov_2 = world.provinces[t2];
    const glm::vec2 world_size{ world.width, world.height };
    const float distance = euclidean_distance(prov_1, prov_2, world_size);

    return distance;
}

/**
 * Finds the path between the given start and end tiles with the given world configuration
 * Implements the A* algorithm with euclidean distance as heuristic.
 */
std::vector<Province::Id> Pathfind::unit_path(const World& world, const Nation& nation, Province::Id start, Province::Id end) {
    // We can't go to sea
    //if(start->elevation <= world.sea_level && end->elevation <= world.sea_level) {
    //    return std::vector<Tile *>();
    //}

    // Keeps track of the costs so far
    std::unordered_map<Province::Id, float> cost_map;

    // Keeps track of the previous tile for each tile
    std::unordered_map<Province::Id, Province::Id> prev_map;

    // Keeps track of which tiles have been visited
    std::unordered_set<Province::Id> visited;

    // Priority queue based on cost
    std::priority_queue<
        std::pair<float, Province::Id>,
        std::vector<std::pair<float, Province::Id>>,
        std::greater<std::pair<float, Province::Id>>
    > queue;

    cost_map[start] = 0.f;
    queue.push({ 0.f, start });

    const glm::vec2 world_size{ world.width, world.height };

    while(!queue.empty()) {
        Province::Id current = queue.top().second;
        queue.pop();

        // If the current node has been previously visited, 
        // it's optimal cost has already been calculated, and we can skip it  
        if(visited.count(current))
            continue;

        visited.insert(current);

        // We are done
        if(current == end)
            break;

        // Generate neighbours
        for(const auto neighbor : generate_neighbors(world, nation, current)) {
            // If the neighbor is visited, we already have the optimal path to it
            if(visited.count(neighbor))
                continue;

            const float cost = cost_map[current] + tile_cost(world, current, neighbor);

            // If we found a new tile or a shorter path to a previously found tile
            if(!cost_map.count(neighbor) || cost < cost_map[neighbor]) {
                cost_map[neighbor] = cost;
                const Province& neighbor_prov = world.provinces[neighbor];
                const Province& end_prov = world.provinces[end];
                const float priority = cost + euclidean_distance(neighbor_prov, end_prov, world_size);
                queue.push({ priority, neighbor });
                prev_map[neighbor] = current;
            }
        }
    }

    std::vector<Province::Id> path;

    // Unwind path and reverse
    Province::Id current = end;
    while(current != start) {
        path.push_back(current);
        current = prev_map[current];
    }
    path.push_back(start);

    std::reverse(path.begin(), path.end());
    return path;
}
