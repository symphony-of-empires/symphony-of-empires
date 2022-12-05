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
    
    //! Do this for now
    if (cost_eval.empty()) {
        tbb::parallel_for(static_cast<size_t>(0), world.provinces.size(), [this, &world](const auto province_id) {
            glm::vec2 world_size{ world.width, world.height};
            const auto& province = world.provinces[province_id];
            for(size_t i = 0; i < world.provinces.size(); i++) {
                const auto& other_province = world.provinces[i];
                this->trade_costs[province_id][i] = 
                    get_trade_cost(province, other_province, world_size);
            }
        });
    }

    if (cost_eval.empty()) {
        for(size_t i = 0; i < world.provinces.size(); i++) {
            if(!world.provinces[i].is_coastal)
                cost_eval.push_back(i);
        }
    }

    //! Do costly right now
    // for(auto& trade_cost : this->trade_costs)
    //     std::fill(trade_cost.begin(), trade_cost.end(), std::numeric_limits<float>::max());
    // glm::vec2 world_size{ world.width, world.height };
    // tbb::parallel_for(tbb::blocked_range(cost_eval.begin(), cost_eval.end()), [this, &world](const auto& province_range) {
    //     for(auto& province_id : province_range) {
    //         Eng3D::Pathfind::from_source<ProvinceId>(ProvinceId(province_id), this->neighbours, this->trade_costs[province_id]);
    //     }
    // }, tbb::auto_partitioner());
}

float Trade::get_trade_cost(const Province& province1, const Province& province2, glm::vec2 world_size) const {
    const auto radius = 100.f;
    const auto distance = province1.euclidean_distance(province2, world_size, radius);

    // Dissuade trade with foreigners
    auto foreign_penalty = 1.f;
    if(province1.controller_id != province2.controller_id) {
        foreign_penalty = 5.f;
        // Must be valid controller ids
        const auto& world = World::get_instance();
        // Must be in customs union if it's a foreigner
        const auto& relation = world.get_relation(province1.controller_id, province2.controller_id);
        if(relation.is_customs_union())
            foreign_penalty = 1.f;
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
