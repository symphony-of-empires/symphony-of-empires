#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <utility>
#include <vector>

#include "world.hpp"
#include "nation.hpp"

namespace Pathfind {
	/**
	 * Finds the path between the given start and end tiles with the given world configuration
	 * Implements the A* algorithm with euclidean distance as heuristic.
	 */
	std::vector<Tile *> unit_path(const World& world, const Nation& nation, Tile * start, Tile * end);
};

#endif