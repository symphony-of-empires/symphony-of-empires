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
//      pathfind.hpp
//
// Abstract:
//      Generalized pathfinder algorithms
// ----------------------------------------------------------------------------

#pragma once

#include <queue>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

namespace Eng3D::Pathfind {
    /// @brief Implements the A* algorithm with euclidean distance as heuristic.
    /// @tparam GenFn The predicate generator returning a iterable container with the neioghbour of the given arguments
    /// @tparam CostFn Cost calculator function
    /// @tparam HeurFn Heuristic function (cost + HeurFn())
    template<typename T, typename GenFn, typename HeurFn, typename... Targs>
    inline std::vector<T> get_path(T start, T end, GenFn g, HeurFn h, Targs... args) {
        /// @brief Keeps track of the costs so far
        std::unordered_map<T, float> cost_map;
        /// @brief Keeps track of the previous tile for each tile
        std::unordered_map<T, T> prev_map;
        /// @brief Keeps track of which tiles have been visited
        std::unordered_set<T> visited;
        /// @brief Priority queue based on cost
        std::priority_queue<
            std::pair<float, T>,
            std::vector<std::pair<float, T>>,
            std::greater<std::pair<float, T>>
        > queue;

        cost_map[start] = 0.f;
        queue.push({ 0.f, start });
        while(!queue.empty()) {
            auto current = queue.top().second;
            queue.pop();

            // If the current node has been previously visited, 
            // it's optimal cost has already been calculated, and we can skip it  
            if(visited.count(current)) continue;
            visited.insert(current);

            // We are done
            if(current == end) break;

            // Generate neighbours
            for(const auto neighbor : g(current, args...)) {
                // If the neighbor is visited, we already have the optimal path to it
                if(visited.count(neighbor)) continue;
                const float cost = cost_map[current] + h(current, neighbor, args...);
                // If we found a new tile or a shorter path to a previously found tile
                if(!cost_map.count(neighbor) || cost < cost_map[neighbor]) {
                    cost_map[neighbor] = cost;
                    const float priority = cost + h(neighbor, end, args...);
                    queue.push({ priority, neighbor });
                    prev_map[neighbor] = current;
                }
            }
        }

        // Unwind path and reverse
        std::vector<T> path;
        path.reserve(abs(start - end));
        auto current = end;
        while(current != start) {
            path.push_back(current);
            current = prev_map[current];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
        return path;
    }

    template<typename T, typename V>
    inline void from_source(T source, const std::vector<std::vector<V>>& neighbour_rels, std::vector<float>& costs) {
        auto cmp = [](const V& a, const V& b) { return a.cost < b.cost; };
        std::priority_queue<V, std::vector<V>, decltype(cmp)> heap;
        heap.emplace(0, source);
        while(!heap.empty()) {
            auto vertex = heap.top();
            heap.pop();
            if(vertex.cost < costs[vertex.key]) {
                costs[vertex.key] = vertex.cost;
                const auto& neighbours = neighbour_rels[vertex.key];
                for(const auto& neighbour : neighbours) {
                    if(neighbour.cost < costs[neighbour.key])
                        heap.emplace(neighbour.cost, neighbour.key);
                }
            }
        }
    }
}
