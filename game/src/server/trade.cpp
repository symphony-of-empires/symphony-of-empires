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
    if(trade_costs.empty())
        this->initialize(world);
    
    for(auto& trade_cost : this->trade_costs)
        std::fill(trade_cost.begin(), trade_cost.end(), std::numeric_limits<float>::max());

    glm::vec2 world_size{ world.width, world.height };
    tbb::parallel_for(static_cast<size_t>(0), cost_eval.size(), [this, &world](const auto province_id) {
        if(!world.provinces[province_id].is_coastal)
            Eng3D::Pathfind::from_source<ProvinceId>(ProvinceId(province_id), this->neighbours, this->trade_costs[province_id]);
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
    const auto distance = angle / (2.f * glm::pi<float>());

    // Dissuade trade with foreigners
    float foreign_penalty = 1.f;
    if(province1.controller_id != province2.controller_id)
    {
        foreign_penalty = 5.f;
        // Must be valid controller ids
        if(Nation::is_valid(province1.controller_id) || Nation::is_valid(province2.controller_id))
        {
            const auto& world = World::get_instance();
            // Must be in customs union if it's a foreigner
            const auto& relation = world.get_relation(province1.controller_id, province2.controller_id);
            if(relation.is_customs_union())
                foreign_penalty = 1.f;
        }
    }

    // Cost to travel around the globe
    const auto trade_cost = (g_world.terrain_types[province1.terrain_type_id].penalty + g_world.terrain_types[province2.terrain_type_id].penalty) * foreign_penalty;
    return distance * trade_cost;
}

inline void Trade::initialize(const World& world) {
    trade_costs.resize(world.provinces.size(), std::vector<float>(world.provinces.size(), std::numeric_limits<float>::max()));
    
    glm::vec2 world_size{ world.width, world.height };
    neighbours.reserve(world.provinces.size());
    for(const auto& province : world.provinces) {
        std::vector<Trade::Vertex> province_neighbours;
        province_neighbours.reserve(province.neighbour_ids.size());
        for(const auto neighbour_id : province.neighbour_ids) {
            const auto& neighbour = world.provinces[neighbour_id];
            const auto trade_cost = get_trade_cost(province, neighbour, world_size);
            province_neighbours.emplace_back(trade_cost, neighbour_id);
        }
        neighbours.push_back(province_neighbours);
    }
}
