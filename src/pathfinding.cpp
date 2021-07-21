#include "pathfinding.hpp"

#include <queue>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

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
static std::vector<Tile *> generate_neighbors(const World& world, const Nation& nation, Tile* tile) {
    std::vector<Tile *> result;

    const NationId nation_id = world.get_id(&nation);
    
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            // Skip middle
            if(i == 0 && j == 0)
                continue;
            
            const size_t t_idx = world.get_id(tile);
            const size_t t_x = t_idx % world.width;
            const size_t t_y = t_idx / world.width;

            Tile* neighbour = &(world.get_tile(t_x + i, t_y + j));

            // Check that neighbour is above sea level
            if(neighbour->elevation > world.sea_level) {
                // Check that we can diplomatically pass thru their lands
                // Our own lands and lands without owners are automatically passable
                if(neighbour->owner_id != nation_id && neighbour->owner_id != (NationId)-1) {
                    const NationRelation& relation = world.nations[neighbour->owner_id]->relations[nation_id];
                    
                    // Does not have military acces AND does not have alliance AND is not at war
                    // Means the country is neutral towards us and we can't cross there
                    if(relation.has_military_access != true
                    && relation.has_alliance != true
                    && relation.has_war != true) {
                        continue;
                    }
                }

                // Otherwise we can pass!
                result.push_back(neighbour);
            }
        }
    }
    return result;
}

/**
 * Calculates the euclidean distance between the two tiles
 * considering only x and y (ignoring elevation)
 */
static inline float euclidean_distance(const World& world, Tile* t1, Tile* t2) {
    const size_t t1_idx = world.get_id(t1);
    const size_t t1_x = t1_idx % world.width;
    const size_t t1_y = t1_idx / world.width;

    const size_t t2_idx = world.get_id(t2);
    const size_t t2_x = t2_idx % world.width;
    const size_t t2_y = t2_idx / world.width;
    
    const int x_diff = t1_x - t2_x;
    const int y_diff = t1_y - t2_y;
    return std::sqrt(x_diff* x_diff + y_diff* y_diff);
}

/**
 * Calculates the cost accrued by moving from one tile to another, taking into
 * account elevation and infrastructure.
 */
static inline float tile_cost(const World& world, Tile* t1, Tile* t2) {
    const size_t t1_idx = world.get_id(t1);
    const size_t t1_x = t1_idx % world.width;
    const size_t t1_y = t1_idx / world.width;

    const size_t t2_idx = world.get_id(t2);
    const size_t t2_x = t2_idx % world.width;
    const size_t t2_y = t2_idx / world.width;

    const int x_diff = t1_x - t2_x;
    const int y_diff = t1_y - t2_y;
    
    // Maximum elevation difference accounts to same cost as one jump in x or y direction (1.0)
    const float elev_diff = ((int)t1->elevation - (int)t2->elevation) / 128.f;
    
    // Base distance is euclidean distance in x, y and elevation
    const float distance = std::sqrt(x_diff* x_diff + y_diff* y_diff + elev_diff* elev_diff);
    
    // Calculate average infrastructure level between the two tiles
    const float avg_infra = ((t1->infra_level + t2->infra_level) / 2.f);
    
    // Cost modifier from infrastructure scales linearly with infrastructure
    // with 1.0 cost modifier at max infra and 5.0 modifier at 0 infra
    // NOTE: Make sure that the infrastructure modifier is always larger than 1 for bad infra
    // (rather than <1 for good infra), or the heuristic will no longer be admissible
    // and A* will no longer be optimal
    const float infra_modifier = 1.f + 4.f* (1.f - avg_infra / 8.f); 
    
    // Rivers double the cost
    const float river_modifier = (t1->elevation == 127 || t2->elevation == 127) ? 2.f : 1.f;
    
    return river_modifier* infra_modifier* distance;
}

/**
 * Finds the path between the given start and end tiles with the given world configuration
 * Implements the A* algorithm with euclidean distance as heuristic.
 */
std::vector<Tile *> Pathfind::unit_path(const World& world, const Nation& nation, Tile* start, Tile* end) {
    // We can't go to sea
    if(start->elevation <= world.sea_level && end->elevation <= world.sea_level) {
        return std::vector<Tile *>();
    }
    
    // Keeps track of the costs so far
    std::unordered_map<Tile *, float> cost_map;
    
    // Keeps track of the previous tile for each tile
    std::unordered_map<Tile *, Tile *> prev_map;
    
    // Keeps track of which tiles have been visited
    std::unordered_set<Tile *> visited;
    
    // Priority queue based on cost
    std::priority_queue<
        std::pair<float, Tile *>,
        std::vector<std::pair<float, Tile *>>,
        std::greater<std::pair<float, Tile *>>
    > queue;
    
    cost_map[start] = 0.f;
    queue.push({0.f, start});

    while(!queue.empty()) {
        Tile* current = queue.top().second;
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
        for(const auto neighbor: generate_neighbors(world, nation, current)) {
            // If the neighbor is visited, we already have the optimal path to it
            if(visited.count(neighbor))
                continue;
            
            const float cost = cost_map[current] + tile_cost(world, current, neighbor);
            
            // If we found a new tile or a shorter path to a previously found tile
            if(!cost_map.count(neighbor) || cost < cost_map[neighbor]) {
                cost_map[neighbor] = cost;
                const float priority = cost + euclidean_distance(world, neighbor, end);
                queue.push({priority, neighbor});
                prev_map[neighbor] = current;
            }
        }
    }

    std::vector<Tile *> path;
    
    // Unwind path and reverse
    Tile* current = end;
    while(current != start) {
        path.push_back(current);
        current = prev_map[current];
    }
    path.push_back(start);
    
    std::reverse(path.begin(), path.end());
    return path;
}
