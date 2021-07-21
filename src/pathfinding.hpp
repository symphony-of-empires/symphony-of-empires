#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <utility>
#include <vector>

#include "world.hpp"
#include "nation.hpp"

/**
 * This is not used by the server due to architectural reasons, however it may be useful for AI clients
 */
namespace Pathfind {
    std::vector<Tile *> unit_path(const World& world, const Nation& nation, Tile* start, Tile* end);
};

#endif
