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
#include <queue>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

using namespace Economy;

static inline void shortest_path_from_source(Province::Id source, const std::vector<std::vector<Trade::Vertex>>& neighbour_relations, std::vector<float>& costs) {
    auto cmp = [](Trade::Vertex const& a, Trade::Vertex const& b) { return a.cost < b.cost; };
    std::priority_queue<Trade::Vertex, std::vector<Trade::Vertex>, decltype(cmp)> heap;

    heap.emplace(Trade::Vertex{ 0, source });

    while(!heap.empty()) {
        auto vertex = heap.top();
        heap.pop();
        if(vertex.cost < costs[vertex.province_id]) {
            costs[vertex.province_id] = vertex.cost;
            const auto& neighbours = neighbour_relations[vertex.province_id];
            for(const auto& neighbour : neighbours) {
                if(neighbour.cost < costs[neighbour.province_id]) {
                    heap.emplace(neighbour.cost, neighbour.province_id);
                }
            }
        }
    }
}

void Trade::recalculate(const World& world) {
    if(trade_cost.empty())
        initialize(world);

    for(size_t i = 0; i < world.provinces.size(); i++) {
        for(size_t j = 0; j < world.provinces.size(); j++) {
            trade_cost[i][j] = std::numeric_limits<float>::max();
        }
    }

    glm::vec2 world_size{ world.width, world.height };

    // tbb::blocked_range<Province::Id> range(static_cast<Province::Id>(0), static_cast<Province::Id>(world.provinces.size()));
    tbb::parallel_for((Province::Id)0, (Province::Id)world.provinces.size(), [this, &world](Province::Id province_id) {
        if(world.provinces[province_id].is_coastal) return;
        auto& local_costs = this->trade_cost[province_id];
        shortest_path_from_source(province_id, this->neighbours, local_costs);
    }, tbb::auto_partitioner());
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

static inline float get_trade_cost(const Province& province1, const Province& province2, glm::vec2 world_size) {
    glm::vec3 sphere_coord1 = get_sphere_coord(province1, world_size);
    glm::vec3 sphere_coord2 = get_sphere_coord(province2, world_size);

    float cos_angle = glm::dot(sphere_coord1, sphere_coord2);
    float angle = glm::acos(cos_angle);
    float distance = angle / (2 * glm::pi<float>());
    // Cost to travel around the globe
    constexpr float trade_cost = 1000;
    return distance * trade_cost;
}

void Trade::initialize(const World& world) {
    trade_cost.reserve(world.provinces.size());
    for(Province::Id i = 0; i < world.provinces.size(); i++) {
        std::vector<float> prov_trade_cost(world.provinces.size(), std::numeric_limits<float>::max());
        trade_cost.push_back(prov_trade_cost);
    }

    glm::vec2 world_size{ world.width, world.height };
    neighbours.reserve(world.provinces.size());
    for(const auto& province : world.provinces) {
        std::vector<Trade::Vertex> prov_neighbours;
        prov_neighbours.reserve(prov_neighbours.size());
        for(const auto neighbour_id : province.neighbours) {
            const auto& neighbour = world.provinces[neighbour_id];
            const auto trade_cost = get_trade_cost(province, neighbour, world_size);
            prov_neighbours.emplace_back(trade_cost, neighbour_id);
        }
        neighbours.emplace_back(prov_neighbours);
    }
}
