#pragma once

#include <utility>
#include <vector>

#include "world.hpp"
#include "nation.hpp"

/**
 * This is not used by the server due to architectural reasons, however it may be useful for AI clients
 */
namespace Pathfind {
#if defined TILE_GRANULARITY
    std::vector<Tile *> unit_path(const World& world, const Nation& nation, Tile* start, Tile* end);
#endif
};