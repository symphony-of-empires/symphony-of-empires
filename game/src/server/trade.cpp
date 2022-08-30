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
//      server/trade.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "server/trade.hpp"
#include "world.hpp"
#include "eng3d/pathfind.hpp"
#include <queue>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

using namespace Economy;

void Trade::recalculate(const World& world) {
    if(trade_cost.empty())
        this->initialize(world);

    for(Province::Id i = 0; i < cost_eval.size(); i++)
        std::fill(trade_cost[i].begin(), trade_cost[i].end(), std::numeric_limits<float>::max());

    glm::vec2 world_size{ world.width, world.height };

    // tbb::blocked_range<Province::Id> range(static_cast<Province::Id>(0), static_cast<Province::Id>(cost_eval.size()));
    tbb::parallel_for((Province::Id)0, (Province::Id)cost_eval.size(), [this, &world](Province::Id province_id) {
        if(world.provinces[province_id].is_coastal) return;
        Eng3D::Pathfind::from_source(province_id, this->neighbours, this->trade_cost[province_id]);
    }, tbb::auto_partitioner());
}

inline glm::vec3 Trade::get_sphere_coord(const Province& province, glm::vec2 world_size) {
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

inline float Trade::get_trade_cost(const Province& province1, const Province& province2, glm::vec2 world_size) {
    const auto sphere_coord1 = get_sphere_coord(province1, world_size);
    const auto sphere_coord2 = get_sphere_coord(province2, world_size);

    float cos_angle = glm::dot(sphere_coord1, sphere_coord2);
    float angle = glm::acos(cos_angle);
    const auto distance = angle / (2 * glm::pi<float>());
    // Cost to travel around the globe
    const auto trade_cost = 1000.f * g_world.terrain_types[province1.terrain_type_id].penalty * g_world.terrain_types[province2.terrain_type_id].penalty;
    return distance * trade_cost;
}

inline void Trade::initialize(const World& world) {
    // Construct a list of terrain types that are land
    std::vector<bool> is_land(world.terrain_types.size(), false);
    for(const auto& terrain_type : world.terrain_types)
        is_land[terrain_type.get_id()] = terrain_type.is_water_body;
    
    // Then proceed to construct the cost list so we only evaluate provinces
    // that will use the goods, this includes ignoring empty provinces from the
    // trading spider network
    cost_eval.reserve(world.provinces.size());
    for(const auto& province : world.provinces)
        if(is_land[province.terrain_type_id] && !province.pops.empty())
            cost_eval.emplace_back();

    trade_cost.reserve(cost_eval.size());
    for(Province::Id i = 0; i < cost_eval.size(); i++)
        trade_cost.emplace_back(std::vector<float>(cost_eval.size(), std::numeric_limits<float>::max()));

    glm::vec2 world_size{ world.width, world.height };
    neighbours.reserve(cost_eval.size());
    for(const auto province_id : cost_eval) {
        const auto& province = world.provinces[province_id];
        std::vector<Trade::Vertex> province_neighbours;
        province_neighbours.reserve(province.neighbour_ids.size());
        for(const auto neighbour_id : province.neighbour_ids) {
            const auto& neighbour = world.provinces[neighbour_id];
            const auto trade_cost = get_trade_cost(province, neighbour, world_size);
            province_neighbours.emplace_back(trade_cost, neighbour_id);
        }
        neighbours.emplace_back(province_neighbours);
    }
}
