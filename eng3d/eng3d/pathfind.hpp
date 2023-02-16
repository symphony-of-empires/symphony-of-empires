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
//      pathfind.hpp
//
// Abstract:
//      Generalized pathfinder algorithms
// ----------------------------------------------------------------------------

#pragma once

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <cmath>

namespace Eng3D::Pathfind {
    /// @brief Implements the A* algorithm with euclidean distance as heuristic. Returns a vector where the starting
    /// point is at the back and the end point at the front
    /// @param g The predicate generator returning a iterable container with the neioghbour of the given arguments
    /// @param h Heuristic function (cost + HeurFn())
    template<typename T>
    std::vector<T> get_path(T start, T end, std::function<std::vector<T>(T)> g, std::function<float(T, T)> h) {
        /// @brief Keeps track of the costs so far
        std::unordered_map<T, float> cost_map;
        /// @brief Keeps track of the previous tile for each tile
        std::unordered_map<T, T> prev_map;
        /// @brief Keeps track of which tiles have been visited
        std::unordered_set<T> visited;

        auto comp = [](const auto& a, const auto& b) { return a.first > b.first; };
        /// @brief Priority queue based on cost
        std::priority_queue<std::pair<float, T>,
            std::vector<std::pair<float, T>>,
            decltype(comp)
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
            for(const auto neighbor : g(current)) {
                // If the neighbor is visited, we already have the optimal path to it
                if(visited.count(neighbor)) continue;
                const float cost = cost_map[current] + h(current, neighbor);
                // If we found a new tile or a shorter path to a previously found tile
                if(!cost_map.count(neighbor) || cost < cost_map[neighbor]) {
                    cost_map[neighbor] = cost;
                    const float priority = cost + h(neighbor, end);
                    queue.push({ priority, neighbor });
                    prev_map[neighbor] = current;
                }
            }
        }

        // Unwind path and reverse
        std::vector<T> path;
        path.reserve(std::abs(static_cast<long>(start) - static_cast<long>(end)));
        auto current = end;
        while(current != start) {
            path.push_back(current);
            current = prev_map[current];
        }
        path.push_back(start);
        return path;
    }

    template<typename T>
    void from_source(const auto& source, const std::vector<std::vector<T>>& neighbour_rels, std::vector<float>& costs) {
        auto cmp = [](const T& a, const T& b) { return a.cost < b.cost; };
        std::priority_queue<T, std::vector<T>, decltype(cmp)> heap;
        heap.emplace(0.f, source);
        while(!heap.empty()) {
            auto vertex = heap.top();
            heap.pop();
            if(vertex.cost < costs[vertex.key]) {
                costs[vertex.key] = vertex.cost;
                const auto& neighbours = neighbour_rels[vertex.key];
                for(const auto& neighbour : neighbours)
                    if(neighbour.cost < costs[neighbour.key])
                        heap.emplace(neighbour.cost, neighbour.key);
            }
        }
    }
}
