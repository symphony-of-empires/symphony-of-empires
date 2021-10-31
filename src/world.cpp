#include "world.hpp"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <set>
#ifndef _MSC_VER
#	include <sys/cdefs.h>
#endif

#include "province.hpp"
#include "binary_image.hpp"
#include "path.hpp"
#include "print.hpp"
#include "serializer.hpp"
#include "server/server_network.hpp"
#include "diplomacy.hpp"
#include "policy.hpp"

const std::vector<const Tile*> Tile::get_neighbours(const World& world) const {
    std::vector<const Tile*> tiles;
    const size_t idx = world.get_id(this);

    // Up
    if(idx > world.width) {
        tiles.push_back(&world.tiles[idx - world.width]);
    }
    // Down
    if(idx < (world.width * world.height) - world.width) {
        tiles.push_back(&world.tiles[idx + world.width]);
    }
    // Left
    if(idx > 1) {
        tiles.push_back(&world.tiles[idx - 1]);
    }
    // Right
    if(idx < (world.width * world.height) - 1) {
        tiles.push_back(&world.tiles[idx + 1]);
    }
    return tiles;
}

World* g_world;

World& World::get_instance(void) {
    return *g_world;
}

// Obtains a tile from the world safely, and makes sure that it is in bounds
Tile& World::get_tile(size_t x, size_t y) const {
    if(x >= width || y >= height)
        throw std::runtime_error("Tile out of bounds");
    return tiles[x + y * width];
}

Tile& World::get_tile(size_t idx) const {
    if(idx >= width * height)
        throw std::runtime_error("Tile index exceeds boundaries");
    return tiles[idx];
}
