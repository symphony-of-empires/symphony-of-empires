#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <utility>
#include <vector>

#include "world.hpp"

/**
 * Finds the path between the given start and end tiles with the given world configuration
 * Implements the A* algorithm with euclidean distance as heuristic.
 */
std::vector<Tile*> find_path(const World& world, Tile* start, Tile* end);


#endif